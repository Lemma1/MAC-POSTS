import os
import numpy as np
import pandas as pd
import hashlib
import time
import shutil
from scipy.sparse import coo_matrix
import multiprocessing as mp

import MNMAPI
from DODE import *
from covariance_tree import *

class SDODE():
  def __init__(self, nb, config):
    self.config = config
    self.nb = nb
    self.num_assign_interval = nb.config.config_dict['DTA']['max_interval']
    self.ass_freq = nb.config.config_dict['DTA']['assign_frq']
    self.num_link = nb.config.config_dict['DTA']['num_of_link']
    self.num_path = nb.config.config_dict['FIXED']['num_path']
    self.num_loading_interval =  self.num_assign_interval * self.ass_freq
    self.data_dict = dict()
    self.num_data = self.config['num_data']
    self.observed_links = self.config['observed_links']
    self.all_links = list(map(lambda x: x.ID, self.nb.link_list))
    self.observed_links_idxs = list(map(lambda x: self.all_links.index(x), self.observed_links))
    self.paths_list = self.config['paths_list']
    self.demand_list = self.nb.demand.demand_list
    assert (len(self.paths_list) == self.num_path)

  def _add_link_flow_data(self, link_flow_df_list):
    assert (self.config['use_link_flow'])
    assert (self.num_data == len(link_flow_df_list))
    assert (len(self.observed_links) == len(link_flow_df_list[0].columns))
    for i in range(self.num_data):
      assert (len(self.observed_links) == len(link_flow_df_list[i].columns))
      for j in range(len(self.observed_links)):
        assert (self.observed_links[j] == link_flow_df_list[i].columns[j])
    self.data_dict['link_flow'] = link_flow_df_list

  def _add_link_tt_data(self, link_spd_df_list):
    assert (self.config['use_link_tt'])
    assert (self.num_data == len(link_spd_df_list))
    for i in range(self.num_data):
      assert (len(self.observed_links) == len(link_spd_df_list[i].columns))
      for j in range(len(self.observed_links)):
        assert (self.observed_links[j] == link_spd_df_list[i].columns[j])
    self.data_dict['link_tt'] = link_spd_df_list

  def add_data(self, data_dict):
    if self.config['use_link_flow']:
      self._add_link_flow_data(data_dict['link_flow'])
    if self.config['use_link_tt']:
      self._add_link_tt_data(data_dict['link_tt'])

  def _run_simulation(self, f, counter = 0):
    # print "RUN"
    hash1 = hashlib.sha1()
    hash1.update(str(time.time()) + str(counter))
    new_folder = str(hash1.hexdigest())
    self.nb.update_demand_path(f)
    self.nb.config.config_dict['DTA']['total_interval'] = self.num_loading_interval
    self.nb.dump_to_folder(new_folder)
    a = MNMAPI.dta_api()
    a.initialize(new_folder)
    shutil.rmtree(new_folder)
    a.register_links(self.all_links)
    a.register_paths(self.paths_list)
    a.install_cc()
    a.install_cc_tree()
    a.run_whole()
    return a

  def get_full_dar(self, dta, f):
    dar = dta.get_complete_dar_matrix(np.arange(0, self.num_loading_interval, self.ass_freq), 
                np.arange(0, self.num_loading_interval, self.ass_freq) + self.ass_freq, 
                self.num_assign_interval, f)
    return dar


  def get_path_tt_dict(self, dta):
    path_tt_array = dta.get_path_tt(np.arange(0, self.num_loading_interval, self.ass_freq))
    path_ID2tt = dict()
    for path_ID_idx, path_ID in enumerate(self.paths_list):
      path_ID2tt[path_ID] = path_tt_array[path_ID_idx, :]
    return path_ID2tt

  def assign_route_portions(self, path_ID2tt, theta = 0.1):
    for O_node in self.nb.path_table.path_dict.keys():
      for D_node in self.nb.path_table.path_dict[O_node].keys():
        tmp_path_set = self.nb.path_table.path_dict[O_node][D_node]
        cost_array = np.zeros((len(tmp_path_set.path_list),self.num_assign_interval))
        for tmp_path_idx, tmp_path in enumerate(tmp_path_set.path_list):
          cost_array[tmp_path_idx, :] = path_ID2tt[tmp_path.path_ID]
        p_array = generate_portion_array(cost_array, theta = theta)
        for tmp_path_idx, tmp_path in enumerate(tmp_path_set.path_list):
          tmp_path.attach_route_choice_portions(p_array[tmp_path_idx])

  def init_demand_flow(self, init_scale = 0.1):
    return np.random.rand(self.num_assign_interval * len(self.demand_list)) * init_scale

  def compute_path_flow_grad_and_loss(self, one_data_dict, f, counter = 0):
    # print "Running simulation"
    dta = self._run_simulation(f, counter)
    # print "Getting DAR"
    dar = self.get_full_dar(dta, f)[self.get_full_observed_link_index(), :]
    # print "Evaluating grad"
    grad = np.zeros(len(self.observed_links) * self.num_assign_interval)
    if self.config['use_link_flow']:
      grad += self.config['link_flow_weight'] * self._compute_grad_on_link_flow(dta, one_data_dict['link_flow'])
    if self.config['use_link_tt']:
      grad += self.config['link_tt_weight'] * self._compute_grad_on_link_tt(dta, one_data_dict['link_tt'])
    # print "Getting Loss"
    loss = self._get_loss(one_data_dict, dta)
    new_path_cost_array = dta.get_path_tt(np.arange(0, self.num_loading_interval, self.ass_freq))
    return  dar.T.dot(grad), loss, new_path_cost_array

  def _compute_grad_on_link_flow(self, dta, link_flow_array):
    x_e = dta.get_link_inflow(np.arange(0, self.num_loading_interval, self.ass_freq), 
                  np.arange(0, self.num_loading_interval, self.ass_freq) + self.ass_freq).flatten(order = 'F')
    grad = -np.nan_to_num(link_flow_array - x_e[self.get_full_observed_link_index()])
    return grad

  def _compute_grad_on_link_tt(self, dta, link_flow_array):
    tt_e = dta.get_link_tt(np.arange(0, self.num_loading_interval, self.ass_freq))
    tt_free = list(map(lambda x: self.nb.get_link(x).get_fft(), self.observed_links))
    for i in range(tt_e.shape[0]):
      pass
    return 0  

  def _get_one_data(self, j):
    assert (self.num_data > j)
    one_data_dict = dict()
    if self.config['use_link_flow']:
      one_data_dict['link_flow'] = self.data_dict['link_flow'][j].values.flatten()
    if self.config['use_link_tt']:
      one_data_dict['link_tt'] = self.data_dict['link_tt'][j].values.flatten()
    return one_data_dict

  def _get_loss(self, one_data_dict, dta):
    loss = np.float(0)
    if self.config['use_link_flow']:
      x_e = dta.get_link_inflow(np.arange(0, self.num_loading_interval, self.ass_freq), 
                  np.arange(0, self.num_loading_interval, self.ass_freq) + self.ass_freq).flatten(order = 'F')
      loss += self.config['link_flow_weight'] * np.linalg.norm(
                np.nan_to_num(x_e[self.get_full_observed_link_index()] - one_data_dict['link_flow']))
    return loss


  def get_full_observed_link_index(self):
    link_list = list()
    for i in range(self.num_assign_interval):
      for idx in self.observed_links_idxs:
        link_list.append(idx + i * len(self.all_links))
    return link_list


  def estimate_demand(self, init_scale = 0.1, step_size = 0.1, 
                      max_epoch = 100, adagrad = False,
                      theta = 0.1):
    q_e = self.init_demand_flow(init_scale = init_scale)
    for i in range(max_epoch):
      seq = np.random.permutation(self.num_data)
      loss = np.float(0)
      sum_g_square = 1e-6
      for j in seq:
        one_data_dict = self._get_one_data(j)
        P = self.nb.get_route_portion_matrix()
        f_e = P.dot(q_e)
        f_grad, tmp_loss, path_cost = self.compute_path_flow_grad_and_loss(one_data_dict, f_e)
        q_grad = P.T.dot(f_grad)
        sum_g_square = sum_g_square + np.power(q_grad, 2)
        q_e -= q_grad * step_size / np.sqrt(sum_g_square)
        q_e = np.maximum(q_e, 1e-6)
        loss += tmp_loss
        self.assign_route_portions(path_cost, theta = theta)
      print "Epoch:", i, "Loss:", loss / np.float(self.num_data)
    return q_e



  def estimate_demand_cov(self, O_dist, D_dist, init_mean_scale = 0.1, 
                      init_std_scale = 0.01, init_O_cov_scale = 0.1,
                      init_D_cov_scale = 0.1, step_size = 0.1, 
                      max_epoch = 100, adagrad = False,
                      theta = 0.1):
    q_para = OD_parameter_server(self.demand_list, self.num_assign_interval)
    q_para.construct(O_dist, D_dist)
    q_para.initialize(mean_scale = init_mean_scale, std_scale= init_std_scale, 
                        O_cov_scale = init_O_cov_scale, D_cov_scale = init_D_cov_scale)
    iter_counter = 0
    for i in range(max_epoch):
      seq = np.random.permutation(self.num_data)
      loss = np.float(0)
      for j in seq:
        one_data_dict = self._get_one_data(j)
        q_e, random_list = q_para.forward()
        P = self.nb.get_route_portion_matrix()
        # print q_e, q_e.shape
        f_e = P.dot(q_e)
        f_grad, tmp_loss, path_cost = self.compute_path_flow_grad_and_loss(one_data_dict, f_e)
        q_grad = P.T.dot(f_grad)
        q_para.backward(iter_counter, step_size, q_grad, random_list, adagrad = adagrad)
        iter_counter += 1
        loss += tmp_loss
        self.assign_route_portions(path_cost, theta = theta)
      print "Epoch:", i, "Loss:", loss / np.float(self.num_data)
    return q_para


###  Behavior related function
def generate_portion_array(cost_array, theta = 0.1):
  p_array = np.zeros(cost_array.shape)
  for i in range(cost_array.shape[1]):
    p_array[:, i] = logit_fn(cost_array[:,i], theta)
  return p_array

def logit_fn(cost, theta, max_cut = True):
  scale_cost = - theta * cost
  if max_cut:
    e_x = np.exp(scale_cost - np.max(scale_cost))
  else:
    e_x = np.exp(scale_cost)
  return e_x / e_x.sum()
