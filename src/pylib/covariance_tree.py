import os
import numpy as np
import networkx as nx
import scipy

from scipy.sparse import coo_matrix
from scipy.cluster import hierarchy

class OD_parameter_server():
  def __init__(self, OD_list, num_intervals):
    self.OD_list = OD_list
    self.num_intervals = num_intervals
    self.O_dim = None
    self.D_dim = None
    self.O_list = None
    self.D_list = None
    self.O2OD = None
    self.D2OD = None
    self.demand_mean_list = None
    self.demand_std_list = None
    self.O_cov_list = None
    self.D_cov_list = None
    self.OD2idx_dict = None

    self.demand_mean_gradsum_list = None
    self.demand_std_gradsum_list = None
    self.O_cov_para_gradsum_list = None
    self.D_cov_para_gradsum_list = None

  def construct(self, O_dist, D_dist):
    self.OD2idx_dict = dict()
    self.O_list = list()
    self.D_list = list()
    for odidx, (O,D) in enumerate(self.OD_list):
      self.OD2idx_dict[(O,D)] = odidx
      self.O_list.append(O)
      self.D_list.append(D)
    self.O_list = list(set(self.O_list))
    self.D_list = list(set(self.D_list))
    self.O_dim = len(self.O_list)
    self.D_dim = len(self.D_list)
    self.O_cov_list = list()
    self.D_cov_list = list()
    # O_dist = np.random.rand(self.O_dim,5)
    # D_dist = np.random.rand(self.D_dim,5)
    O_tree = hierarchy.to_tree(hierarchy.linkage(O_dist, 'ward'))
    D_tree = hierarchy.to_tree(hierarchy.linkage(D_dist, 'ward'))
    for i in range(self.num_intervals):
      self.O_cov_list.append(cov_tree(list(range(self.O_dim)), 
                                        list(range(self.O_dim * 2 - 1))))
      self.O_cov_list[i].construct_A(O_tree)
      # self.O_cov_list[i].A = np.eye(1)
      self.D_cov_list.append(cov_tree(list(range(self.D_dim)), 
                                        list(range(self.D_dim * 2 - 1))))
      self.D_cov_list[i].construct_A(D_tree)
      # self.D_cov_list[i].A = np.eye(1)
    self.O2OD = self._construct_X2XY(self.O_list, self.OD_list, 0)
    self.D2OD = self._construct_X2XY(self.D_list, self.OD_list, 1)


  def _construct_X2XY(self, X_list, XY_list, item_idx):
    col = list()
    row = list()
    val = list()
    
    x2idx_dict = {x:idx for idx, x in enumerate(X_list)}

    for XY_idx, XY in enumerate(XY_list):
      X = XY[item_idx]
      x_idx = x2idx_dict[X]
      row.append(XY_idx)
      col.append(x_idx)
      val.append(1.0)
    A = coo_matrix((val, (row, col)), shape=(len(XY_list), len(X_list))).tocsr()
    return A


  def initialize(self, mean_scale = 0.1, std_scale= 0.01, O_cov_scale = 0.1, D_cov_scale = 0.1):
    self.demand_mean_list = list()
    self.demand_std_list = list()
    for i in range(self.num_intervals):
      self.demand_mean_list.append(
          np.random.rand(len(self.OD_list)) * mean_scale)
      self.demand_std_list.append(
          np.random.rand(len(self.OD_list)) * std_scale)
      self.O_cov_list[i].init_paras(O_cov_scale)
      self.D_cov_list[i].init_paras(D_cov_scale)
    self.demand_mean_gradsum_list = [1e-6 for i in range(self.num_intervals)]
    self.demand_std_gradsum_list = [1e-6 for i in range(self.num_intervals)]
    self.O_cov_para_gradsum_list = [1e-6 for i in range(self.num_intervals)]
    self.D_cov_para_gradsum_list = [1e-6 for i in range(self.num_intervals)]


  def forward(self):
    O_cov_random_list = list()
    D_cov_random_list = list()
    demand_mean_random_list = list()
    tmp_q_list = list()
    for i in range(self.num_intervals):
      O_cov_random = np.random.randn(self.O_dim * 2 - 1)
      D_cov_random = np.random.randn(self.D_dim * 2 - 1)
      demand_mean_random = np.random.randn(len(self.OD_list))
      O_cov = self.O_cov_list[i]
      D_cov = self.D_cov_list[i]
      tmp_q = (self.demand_mean_list[i]                                               # mean
                + demand_mean_random * self.demand_std_list[i]                       # variance per OD
                + self.O2OD.dot(O_cov.A.dot(O_cov_random * O_cov.std_2parts))        # O-related variance
                + self.D2OD.dot(D_cov.A.dot(D_cov_random * D_cov.std_2parts)))       # D-related variance
      tmp_q = np.maximum(tmp_q, 1e-6)
      tmp_q_list.append(tmp_q)
      O_cov_random_list.append(O_cov_random)
      D_cov_random_list.append(D_cov_random)
      demand_mean_random_list.append(demand_mean_random)
    q = np.concatenate(tmp_q_list)
    return q, [O_cov_random_list, D_cov_random_list, demand_mean_random_list]

  def backward(self, cur_iter, step_size, q_grad, random_list, adagrad = False):
    [O_cov_random_list, D_cov_random_list, demand_mean_random_list] = random_list
    q_grad_reshaped = q_grad.reshape((self.num_intervals, len(self.OD_list)))
    # print q_grad_reshaped
    for i in range(self.num_intervals):
      q_e = self.demand_mean_list[i]
      q_std_e = self.demand_std_list[i]
      O_cov_v = self.O_cov_list[i].std_2parts
      D_cov_v = self.D_cov_list[i].std_2parts
      if adagrad:
        # update demand mean
        self.demand_mean_gradsum_list[i] +=  np.power(q_grad_reshaped[i], 2)
        q_e -= q_grad_reshaped[i] * step_size / np.sqrt(self.demand_mean_gradsum_list[i])
        # print (self.demand_mean_gradsum_list[i])
        q_e = np.maximum(q_e, 1e-6)
        self.demand_mean_list[i] = q_e
        # update demand std
        q_std_grad = q_grad_reshaped[i] * demand_mean_random_list[i]
        self.demand_std_gradsum_list[i] += np.power(q_std_grad, 2)
        q_std_e -=   q_std_grad * step_size / np.sqrt(self.demand_std_gradsum_list[i])
        q_std_e = np.maximum(q_std_e, 1e-6)
        self.demand_std_list[i] = q_std_e
        # update O cov
        O_cov = self.O_cov_list[i]
        O_cov_grad = O_cov.A.T.dot(self.O2OD.T.dot(q_grad_reshaped[i])) * O_cov_random_list[i]
        self.O_cov_para_gradsum_list[i] += np.power(O_cov_grad, 2)
        O_cov_v -= O_cov_grad * step_size / np.sqrt(self.O_cov_para_gradsum_list[i])
        O_cov_v = np.maximum(O_cov_v, 1e-6)
        self.O_cov_list[i].std_2parts = O_cov_v
        # update D cov
        D_cov = self.D_cov_list[i]
        D_cov_grad = D_cov.A.T.dot(self.D2OD.T.dot(q_grad_reshaped[i])) * D_cov_random_list[i]
        self.D_cov_para_gradsum_list[i] += np.power(D_cov_grad, 2)
        D_cov_v -= D_cov_grad * step_size / np.sqrt(self.D_cov_para_gradsum_list[i])
        D_cov_v = np.maximum(D_cov_v, 1e-6)
        self.D_cov_list[i].std_2parts = D_cov_v
      else:
        # update demand mean
        q_e -= q_grad_reshaped[i] * step_size / np.sqrt(cur_iter+1)
        q_e = np.maximum(q_e, 1e-6)
        self.demand_mean_list[i] = q_e
        # update demand std
        q_std_grad = q_grad_reshaped[i] * demand_mean_random_list[i]
        q_std_e -=   q_std_grad * step_size / np.sqrt(cur_iter+1)
        q_std_e = np.maximum(q_std_e, 1e-6)
        self.demand_std_list[i] = q_std_e
        # update O cov
        O_cov = self.O_cov_list[i]
        O_cov_grad = O_cov.A.T.dot(self.O2OD.T.dot(q_grad_reshaped[i])) * O_cov_random_list[i]
        O_cov_v -= O_cov_grad * step_size / np.sqrt(cur_iter+1)
        O_cov_v = np.maximum(O_cov_v, 1e-6)
        self.O_cov_list[i].std_2parts = O_cov_v
        # update D cov
        D_cov = self.D_cov_list[i]
        D_cov_grad = D_cov.A.T.dot(self.D2OD.T.dot(q_grad_reshaped[i])) * D_cov_random_list[i]
        D_cov_v -= D_cov_grad * step_size / np.sqrt(cur_iter+1)
        D_cov_v = np.maximum(D_cov_v, 1e-6)
        self.D_cov_list[i].std_2parts = D_cov_v


class cov_tree():
  def __init__(self, source_list, node_list):
    self.source_list = source_list
    self.node_list = node_list
    assert (len(node_list) == 2 * len(source_list) - 1)
    self.dim = len(source_list)
    self.std_2parts = None
    self.A = None

  def init_paras(self, scale = 0.1):
    self.std_2parts = np.random.rand(self.dim - 1 + self.dim) * scale

  def construct_A(self, root):
    G = get_digraph_from_tree(root)
    G_rev = get_path_G(G)
    path_dict = get_path_dict(G_rev, self.source_list)
    self.A = get_path_to_node_matrix(self.source_list, self.node_list, path_dict)
    return self.A

  def get_cov(self):
    return self.A.dot(scipy.sparse.diags(self.std_2parts)).dot(self.A.T)


def get_digraph_from_tree(root):
  G = nx.DiGraph()
  to_be_visit_list = [root]
  assert (not root.is_leaf())
  while (len(to_be_visit_list) > 0):
    node = to_be_visit_list.pop()
    left_node = node.get_left()
    G.add_edge(node.id, left_node.id)
    if not left_node.is_leaf():
      to_be_visit_list.append(left_node)
    right_node = node.get_right()
    G.add_edge(node.id, right_node.id)
    if not right_node.is_leaf():
      to_be_visit_list.append(right_node)
  return G

def get_path_G(G):
  G_rev = G.reverse()
  return G_rev


def get_path_dict(G, source_list):
  path_dict = dict()
  for source_node in source_list:
    tmp_list = list()
    cur_node = source_node
    while(len(G[cur_node]) > 0):
      tmp_list.append(cur_node)
      assert (len(G[cur_node]) == 1)
      cur_node = G[cur_node].keys()[0]
    tmp_list.append(cur_node)
    path_dict[source_node] = tmp_list
  return path_dict


def get_path_to_node_matrix(source_list, node_list, path_dict):
  row = list()
  col = list()
  val = list()
  node_idx2node_dict = {node:idx for idx, node in enumerate(node_list)}

  for source_idx, source_node in enumerate(source_list):
    tmp_path = path_dict[source_node]
    for node in tmp_path:
      row.append(source_idx)
      col.append(node_idx2node_dict[node])
      val.append(1.0)

  A = coo_matrix((val, (row, col)), shape=(len(source_list), len(node_list))).tocsr()
  return A



