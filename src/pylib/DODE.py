import os
import numpy as np
import pandas as pd
import hashlib
import time
import shutil
from scipy.sparse import coo_matrix
import multiprocessing as mp

import MNMAPI


class DODE(object):
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
    self.paths_list = self.config['paths_list']
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

  def _run_simulation(self, f, counter):
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
    a.register_links(self.observed_links)
    a.register_paths(self.paths_list)
    a.install_cc()
    a.install_cc_tree()
    a.run_whole()
    return a

  def get_dar(self, dta, f):
    raw_dar = dta.get_dar_matrix(np.arange(0, self.num_loading_interval, self.ass_freq), 
                np.arange(0, self.num_loading_interval, self.ass_freq) + self.ass_freq)
    dar = self._massage_raw_dar(raw_dar, self.ass_freq, f, self.num_assign_interval)
    return dar

  def get_dar2(self, dta, f):
    dar = dta.get_complete_dar_matrix(np.arange(0, self.num_loading_interval, self.ass_freq), 
                np.arange(0, self.num_loading_interval, self.ass_freq) + self.ass_freq, 
                self.num_assign_interval, f)
    return dar

  def _massage_raw_dar(self, raw_dar, ass_freq, f, num_assign_interval):
    num_e_path = len(self.paths_list)
    num_e_link = len(self.observed_links)
    link_seq = (np.array(map(lambda x: self.observed_links.index(x), raw_dar[:, 2].astype(np.int)))
                + raw_dar[:, 3] * num_e_link / ass_freq).astype((np.int))
    path_seq = (raw_dar[:, 0] + raw_dar[:, 1] * num_e_path).astype(np.int)
    p = raw_dar[:, 4] / f[path_seq]

    mat = coo_matrix((p, (link_seq, path_seq)), 
                   shape=(num_assign_interval * num_e_link, num_assign_interval * num_e_path)).tocsr()
    return mat

  def init_path_flow(self, init_scale):
    return np.random.rand(self.num_assign_interval * self.num_path) * init_scale

  def compute_path_flow_grad_and_loss(self, one_data_dict, f, counter = 0):
    # print "Running simulation"
    dta = self._run_simulation(f, counter)
    # print "Getting DAR"
    dar = self.get_dar2(dta, f)
    # print "Evaluating grad"
    grad = np.zeros(len(self.observed_links) * self.num_assign_interval)
    if self.config['use_link_flow']:
      grad += self.config['link_flow_weight'] * self._compute_grad_on_link_flow(dta, one_data_dict['link_flow'])
    if self.config['use_link_tt']:
      grad += self.config['link_tt_weight'] * self._compute_grad_on_link_tt(dta, one_data_dict['link_tt'])
    # print "Getting Loss"
    loss = self._get_loss(one_data_dict, dta)
    return  dar.T.dot(grad), loss


  def compute_path_flow_grad_and_loss_mpwrapper(self, one_data_dict, f, j, output):
    grad, tmp_loss = self.compute_path_flow_grad_and_loss(one_data_dict, f, counter = j)
    # print "finished original grad loss"
    output.put((grad, tmp_loss))
    # output.put(grad)
    # print "finished put"
    return

  def _compute_grad_on_link_flow(self, dta, link_flow_array):
    x_e = dta.get_link_inflow(np.arange(0, self.num_loading_interval, self.ass_freq), 
                  np.arange(0, self.num_loading_interval, self.ass_freq) + self.ass_freq).flatten(order = 'F')
    grad = -np.nan_to_num(link_flow_array - x_e)
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
      loss += self.config['link_flow_weight'] * np.linalg.norm(np.nan_to_num(x_e - one_data_dict['link_flow']))
    return loss



  def estimate_path_flow(self, init_scale = 0.1, step_size = 0.1, max_epoch = 100, adagrad = False):
    # print "Init"
    f_e = self.init_path_flow(init_scale)
    # print "Start loop"
    for i in range(max_epoch):
      if adagrad:
        sum_g_square = 1e-6
      seq = np.random.permutation(self.num_data)
      loss = np.float(0)
      for j in seq:
        # print j
        one_data_dict = self._get_one_data(j)
        grad, tmp_loss = self.compute_path_flow_grad_and_loss(one_data_dict, f_e)
        if adagrad:
          sum_g_square = sum_g_square + np.power(grad, 2)
          f_e -= step_size * grad / np.sqrt(sum_g_square)
        else:
          f_e -= grad * step_size / np.sqrt(i+1)
        f_e = np.maximum(f_e, 1e-3)
        loss += tmp_loss
      print "Epoch:", i, "Loss:", loss / np.float(self.num_data)
    return f_e

  def estimate_path_flow_mp(self, init_scale = 0.1, step_size = 0.1, 
                              max_epoch = 100, adagrad = False, n_process = 4):
    # print "Init"
    f_e = self.init_path_flow(init_scale)
    # print "Start loop"
    for i in range(max_epoch):
      if adagrad:
        sum_g_square = 1e-6
      seq = np.random.permutation(self.num_data)
      split_seq = np.array_split(seq, np.maximum(1, int(self.num_data/n_process)))
      loss = np.float(0)
      for part_seq in split_seq:
        output = mp.Queue()
        processes = [mp.Process(target=self.compute_path_flow_grad_and_loss_mpwrapper, args=(self._get_one_data(j), f_e, j, output)) for j in part_seq]
        for p in processes:
          p.start()
        results = list()
        while 1:
          running = any(p.is_alive() for p in processes)
          while not output.empty():
             results.append(output.get())
          if not running:
              break
        for p in processes:
          p.join()
        # results = [output.get() for p in processes]
        for res in results:
          loss += res[1]
          grad = res[0]
          if adagrad:
            sum_g_square = sum_g_square + np.power(grad, 2)
            f_e -= step_size * grad / np.sqrt(sum_g_square)
          else:
            f_e -= grad * step_size / np.sqrt(i+1)          
          f_e = np.maximum(f_e, 1e-3)
      print "Epoch:", i, "Loss:", loss / np.float(self.num_data)
    return f_e

  def generate_route_choice(self):
    pass


class PDODE(DODE, object):
  def __init__(self, nb, config):
    super(PDODE, self).__init__(nb, config)
    # self.k = config['k']


  def init_path_distribution(self, flow_mean_scale = 1, flow_variance_scale = 1):
    f_u = np.random.rand(self.num_assign_interval * self.num_path) * flow_mean_scale
    f_sigma = np.random.rand(self.num_assign_interval * self.num_path) * flow_variance_scale
    return (f_u, f_sigma)


  def generate_standard_normal(self):
    return np.random.randn(self.num_assign_interval * self.num_path)


  def dod_forward(self, stdnorm, f_u, f_sigma):
    return stdnorm * f_sigma + f_u

  def dod_backward(self, stdnorm, f_u, f_sigma, g):
    return (g, stdnorm * g)


  def estimate_path_flow(self, init_scale = 0.1, step_size = 0.1, max_epoch = 100, adagrad = False):
    # print "Init"
    (f_u, f_sigma) = self.init_path_distribution(init_scale)
    # print "Start loop"
    for i in range(max_epoch):
      if adagrad:
        sum_g_u_square = 1e-6
        sum_g_sigma_square = 1e-6
      seq = np.random.permutation(self.num_data)
      loss = np.float(0)
      for j in seq:
        # print j
        stdnorm = self.generate_standard_normal()
        f_e_one = self.dod_forward(stdnorm, f_u, f_sigma)
        f_e_one = np.maximum(f_e_one, 1e-3)
        one_data_dict = self._get_one_data(j)
        grad, tmp_loss = self.compute_path_flow_grad_and_loss(one_data_dict, f_e_one)
        (g_u, g_sigma) = self.dod_backward(stdnorm, f_u, f_sigma, grad)
        if adagrad:
          sum_g_u_square = sum_g_u_square + np.power(g_u, 2)
          sum_g_sigma_square = sum_g_sigma_square + np.power(g_sigma, 2)
          f_u -= step_size * g_u / np.sqrt(sum_g_u_square)
          f_sigma -= step_size * g_sigma / np.sqrt(sum_g_sigma_square)
        else:
          f_u -= g_u * step_size / np.sqrt(i+1)
          f_sigma -= g_sigma * step_size / np.sqrt(i+1)
        f_u = np.maximum(f_u, 1e-3)
        f_sigma = np.maximum(f_sigma, 1e-3)
        loss += tmp_loss
      print "Epoch:", i, "Loss:", loss / np.float(self.num_data)
    return f_u, f_sigma