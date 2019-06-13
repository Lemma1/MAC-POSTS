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
  
  