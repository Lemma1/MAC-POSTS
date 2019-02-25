import numpy as np
import pandas as pd
import sys
import datetime
import os
import matplotlib.pyplot as plt
import matplotlib
import networkx as nx
import pickle
from collections import OrderedDict
import copy
from scipy.sparse import csr_matrix
from scipy import io
import seaborn as sns
import joblib
# from base import *
from joblib import Parallel, delayed
import random
import time


MNM_nb_folder = os.path.join('..', '..', '..', 'side_project', 'network_builder')
sys.path.append(MNM_nb_folder)
python_lib_folder = os.path.join('..', '..', 'pylib')
sys.path.append(python_lib_folder)




from MNMAPI import *
from MNM_mcnb import *
from mcDODE import *


# In[5]:


origin_spc_folder = os.path.join('/home/lemma/Documents/MAC-POSTS/data/input_files_MckeesRocks_SPC_new')
cache_folder = os.path.join('cahce')


# In[6]:


nb = MNM_network_builder()




nb.load_from_folder(origin_spc_folder)




[m_car, L_car, m_truck, L_truck, m_spd_car, m_spd_truck, observed_link_list] = pickle.load(open('mc_data.pickle', 'rb'))


# In[17]:


data_dict = dict()
data_dict['car_count_agg_L_list'] = [L_car]
data_dict['truck_count_agg_L_list'] = [L_truck]
data_dict['car_link_flow'] = [m_car]
data_dict['truck_link_flow'] = [m_truck]
data_dict['car_link_tt'] = [m_spd_car]
data_dict['truck_link_tt'] = [m_spd_truck]


# In[18]:


config = dict()
config['use_car_link_flow'] = False
config['use_car_link_tt'] = False
config['use_truck_link_flow'] = True
config['use_truck_link_tt'] = False
config['car_count_agg'] = True
config['truck_count_agg'] = True
config['link_car_flow_weight'] = 1
config['link_truck_flow_weight'] = 1
config['link_car_tt_weight'] = 0.001
config['link_truck_tt_weight'] = 0.001
config['num_data'] = 1
config['observed_links'] = observed_link_list
config['paths_list'] = range(nb.config.config_dict['FIXED']['num_path'])


config['compute_car_link_flow_loss'] = True
config['compute_truck_link_flow_loss'] = True
config['compute_car_link_tt_loss'] = True
config['compute_truck_link_tt_loss'] = True
# In[35]:


dode = MCDODE(nb, config)


# In[36]:


dode.add_data(data_dict)


# In[ ]:


dode.estimate_path_flow(max_epoch = 100, car_step_size = 1e-5, truck_step_size = 3e-6,
                       car_init_scale = 0.01, truck_init_scale = 0.001,
                       store_folder = 'new_mcpath_storage',
                       use_file_as_init = '11iteration.pickle')

