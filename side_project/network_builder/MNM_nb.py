import os
import numpy as np
import networkx as nx

DLINK_ENUM = ['CTM', 'LQ', 'LTM', 'PQ']
DNODE_ENUM = ['FWJ', 'GRJ', 'DMOND', 'DMDND']


class MNM_dlink():
  def __init__(self):
    self.ID = None
    self.len = None
    self.typ = None
    self.ffs = None
    self.cap = None
    self.rhoj = None
    self.lanes = None

  def __init__(self, ID, length, typ, ffs, cap, rhoj, lanes):
    self.ID = ID
    self.length = length  #mile
    self.typ = typ        #type
    self.ffs = ffs        #mile/h
    self.cap = cap        #v/hour
    self.rhoj = rhoj      #v/miles
    self.lanes = lanes    #num of lanes

  def is_ok(self, unit_time = 5):
    assert(self.length > 0.0)
    assert(self.ffs > 0.0)
    assert(self.cap > 0.0)
    assert(self.rhoj > 0.0)
    assert(self.lanes > 0)
    assert(self.typ in DLINK_ENUM)
    assert(self.cap / self.ffs < self.rhoj)
    assert(unit_time * self.ffs / 3600 >= self.length)

class MNM_dnode():
  def __init__(self):
    self.ID = None
    self.typ = None
  def __init__(self, ID, typ):
    self.ID = ID
    self.typ = typ
  def is_ok(self):
    assert(self.typ in DNODE_ENUM)

class MNM_demand():
  def __init__(self):
    self.demand_dict = dict()

  def add_demand(self, O, D, demand, overwriting = False):
    assert(type(demand) is np.ndarray)
    assert(len(demand.shape) == 1)
    if O not in self.demand_dict.keys():
      self.demand_dict[O] = dict()
    if (not overwriting) and (D in self.demand_dict[O].keys()):
      raise("Error, exists OD demand already") 
    else:
      self.demand_dict[O][D] = demand


class MNM_od():
  def __init__(self):
    self.O_dict = dict()
    self.D_dict = dict()

  def add_origin(self, O, Onode_ID, overwriting = False):
    if (not overwriting) and (O in self.O_dict.keys()):
      raise("Error, exists origin node already")
    else:
      self.O_dict[O] = Onode_ID

  def add_destination(self, D, Dnode_ID, overwriting = False):
    if (not overwriting) and (D in self.D_dict.keys()):
      raise("Error, exists destination node already")
    else:
      self.D_dict[D] = Dnode_ID

class MNM_graph():
  def __init__(self):
    self.graph = nx.DiGraph()
    self.edgeID_dict = nx.DiGraph()

  def add_edge(self, s, e, ID, create_node = False, overwriting = False):
    if (not overwriting) and ((s,e) in self.graph.edges()):
      raise("Error, exists edge in graph")
    elif (not create_node) and s in self.graph.nodes():
      raise("Error, exists start node of edge in graph")
    elif (not create_node) and e in self.graph.nodes():
      raige("Error, exists end node of edge in graph")
    else:
      self.graph.add_edge(s, e, ID = ID)
      self.edgeID_dict[ID] = (s, e)

  def add_node(self, node, overwriting = True):
    if (not overwriting) and node in self.graph.nodes():
      raise("Error, exists node in graph")
    else:
      self.graph.add_node(node)


class MNM_routing():
  def __init__(self):
    print "MNM_routing"

class MNM_routing_fixed(MNM_routing):
  def __init__(self):
    super(MNM_routing_fixed, self).__init__()
    self.required_items = ['num_path', 'choice_portion', 'route_frq', 'path_table']    

class MNM_routing_adaptive(MNM_routing):
  """docstring for MNM_routing_adaptive"""
  def __init__(self):
    super(MNM_routing_adaptive, self).__init__()
    self.required_items = ['route_frq']

class MNM_routing_hybrid(MNM_routing):
  """docstring for MNM_routing_hybrid"""
  def __init__(self):
    super(MNM_routing_hybrid, self).__init__()
    self.required_items = []
    
    

class MNM_config():
  def __init__(self):
    print "MNM_config"


class MNM_network_builder():
  def __init__(self):
    print "Test"