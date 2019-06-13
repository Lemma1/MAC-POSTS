import os
import numpy as np
import networkx as nx
import scipy

from scipy.sparse import coo_matrix

class OD_cov():
  def __init__(self, O_dim, D_dim):
    self.O_dim = O_dim
    self.D_dim = D_dim
    self.O_cov = cov_tree()
    self.D_cov = cov_tree()


class cov_tree():
  def __init__(self, source_list, node_list):
    self.source_list = source_list
    self.node_list = node_list
    assert (len(node_list) == 2 * len(source_list) - 1)
    self.dim = len(source_list)
    self.var_2parts = None
    self.A = None

  def init_paras(self):
    self.var_2parts = np.random.rand(self.dim - 1 + self.dim)

  def construct_A(self, root):
    G = get_digraph_from_tree(root)
    G_rev = get_path_G(G)
    path_dict = get_path_dict(G_rev, self.source_list)
    self.A = get_path_to_node_matrix(self.source_list, self.node_list, path_dict)
    return self.A

  def get_cov(self):
    return self.A.dot(scipy.sparse.diags(self.var_2parts)).dot(self.A.T)


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



