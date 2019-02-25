import os
import numpy as np
import networkx as nx
from bidict import bidict
from collections import OrderedDict

DLINK_ENUM = ['CTM', 'LQ', 'PQ']
DNODE_ENUM = ['FWJ', 'DMOND', 'DMDND']


class MNM_dlink():
  def __init__(self):
    self.ID = None
    self.length = None
    self.typ = None
    self.ffs_car = None
    self.cap_car = None
    self.rhoj_car = None
    self.lanes = None
    self.ffs_truck = None
    self.cap_truck = None
    self.convert_factor = None

  def __init__(self, ID, length, typ, ffs_car, cap_car, rhoj_car, lanes, 
                  ffs_truck, cap_truck, rhoj_truck, convert_factor):
    self.ID = ID
    self.length = length  #mile
    self.typ = typ        #type
    self.ffs_car = ffs_car        #mile/h
    self.cap_car = cap_car        #v/hour
    self.rhoj_car = rhoj_car      #v/miles
    self.lanes = lanes    #num of lanes
    self.ffs_truck = ffs_truck        #mile/h
    self.cap_truck = cap_truck        #v/hour
    self.rhoj_truck = rhoj_truck      #v/miles
    self.convert_factor = convert_factor


  def get_car_fft(self):
    return self.length / self.ffs_car * 3600

  def get_truck_fft(self):
    return self.length / self.ffs_truck * 3600

  def is_ok(self, unit_time = 5):
    assert(self.length > 0.0)
    assert(self.ffs_car > 0.0)
    assert(self.cap_car > 0.0)
    assert(self.rhoj_car > 0.0)
    assert(self.ffs_truck > 0.0)
    assert(self.cap_truck > 0.0)
    assert(self.rhoj_truck > 0.0)
    assert(self.lanes > 0)
    assert(self.convert_factor >= 1)
    assert(self.typ in DLINK_ENUM)
    assert(self.cap_car / self.ffs_car < self.rhoj_car)
    assert(self.cap_truck / self.ffs_truck < self.rhoj_truck)
    # if self.ffs < 9999:
    #   assert(unit_time * self.ffs / 3600 <= self.length)

  def __str__(self):
    return ("MNM_dlink, ID: {}, type: {}, length: {} miles, ffs_car: {} mi/h, ffs_truck: {}".format(self.ID, 
                self.typ, self.length, self.ffs_car, self.ffs_truck))

  def __repr__(self):
    return self.__str__()

  def generate_text(self):
    return ' '.join([str(e) for e in [self.ID, self.typ, 
            self.length, self.ffs_car, self.cap_car, self.rhoj_car, self.lanes,
            self.ffs_truck, self.cap_truck, self.rhoj_truck, self.convert_factor]])

class MNM_dnode():
  def __init__(self):
    self.ID = None
    self.typ = None
    self.convert_factor = None
  def __init__(self, ID, typ, convert_factor):
    self.ID = ID
    self.typ = typ
    self.convert_factor = convert_factor

  def is_ok(self):
    assert(self.typ in DNODE_ENUM)

  def __str__(self):
    return "MNM_dnode, ID: {}, type: {}, convert_factor: {}".format(self.ID, self.typ, self.convert_factor)

  def __repr__(self):
    return self.__str__()

  def generate_text(self):
    return ' '.join([str(e) for e in [self.ID, self.typ, self.convert_factor]])

class MNM_demand():
  def __init__(self):
    self.demand_dict = dict()

  def add_demand(self, O, D, car_demand, truck_demand, overwriting = False):
    assert(type(car_demand) is np.ndarray)
    assert(len(car_demand.shape) == 1)
    assert(type(truck_demand) is np.ndarray)
    assert(len(truck_demand.shape) == 1)
    if O not in self.demand_dict.keys():
      self.demand_dict[O] = dict()
    if (not overwriting) and (D in self.demand_dict[O].keys()):
      raise("Error, exists OD demand already") 
    else:
      self.demand_dict[O][D] = [car_demand, truck_demand]

  def build_from_file(self, file_name):
    f = file(file_name)
    log = f.readlines()[1:]
    f.close()
    for i in range(len(log)):
      tmp_str = log[i].strip()
      if tmp_str == '':
        continue
      words = tmp_str.split()
      O_ID = np.int(words[0])
      D_ID = np.int(words[1])
      demand = np.array(words[2:]).astype(np.float)
      total_l = len(demand)
      assert (total_l % 2 == 0)
      self.add_demand(O_ID, D_ID, demand[0: total_l /2], demand[- total_l /2:])

  def __str__(self):
    return "MNM_demand, number of O: {}".format(len(self.demand_dict))

  def __repr__(self):
    return self.__str__()

  def generate_text(self):
    tmp_str = '#Origin_ID Destination_ID <car demand by interval> <truck demand by interval>\n'
    for O in self.demand_dict.keys():
      for D in self.demand_dict[O].keys():
        tmp_str += ' '.join([str(e) for e in [O, D] + 
                  self.demand_dict[O][D][0].tolist() + self.demand_dict[O][D][1].tolist()] ) + '\n'
    return tmp_str


class MNM_od():
  def __init__(self):
    self.O_dict = bidict()
    self.D_dict = bidict()

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

  def build_from_file(self, file_name):
    self.O_dict = bidict()
    self.D_dict = bidict()
    flip = False
    f = file(file_name)
    log = f.readlines()[1:]
    f.close()
    for i in range(len(log)):
      tmp_str = log[i].strip()
      if tmp_str == '':
        continue
      if tmp_str.startswith('#'):
        flip = True
        continue
      words = tmp_str.split()
      od = np.int(words[0])
      node = np.int(words[1])
      if not flip:
        self.add_origin(od, node)
      else:
        self.add_destination(od, node)

  def generate_text(self):
    tmp_str = '#Origin_ID <-> node_ID\n'
    for O_ID, node_ID in self.O_dict.iteritems():
      tmp_str += ' '.join([str(e) for e in [O_ID, node_ID]]) + '\n'
    tmp_str += '#Dest_ID <-> node_ID\n'
    for D_ID, node_ID in self.D_dict.iteritems():
      tmp_str += ' '.join([str(e) for e in [D_ID, node_ID]]) + '\n'
    return tmp_str

  def __str__(self):
    return "MNM_od, number of O:" + str(len(self.O_dict)) + ", number of D:" + str(len(self.D_dict))

  def __repr__(self):
    return self.__str__()

class MNM_graph():
  def __init__(self):
    self.G = nx.DiGraph()
    self.edgeID_dict = OrderedDict()

  def add_edge(self, s, e, ID, create_node = False, overwriting = False):
    if (not overwriting) and ((s,e) in self.G.edges()):
      raise("Error, exists edge in graph")
    elif (not create_node) and s in self.G.nodes():
      raise("Error, exists start node of edge in graph")
    elif (not create_node) and e in self.G.nodes():
      raige("Error, exists end node of edge in graph")
    else:
      self.G.add_edge(s, e, ID = ID)
      self.edgeID_dict[ID] = (s, e)

  def add_node(self, node, overwriting = True):
    if (not overwriting) and node in self.G.nodes():
      raise("Error, exists node in graph")
    else:
      self.G.add_node(node)

  def build_from_file(self, file_name):
    self.G = nx.DiGraph()
    self.edgeID_dict = OrderedDict()
    f = file(file_name)
    log = f.readlines()[1:]
    f.close()
    for i in range(len(log)):
      tmp_str = log[i].strip()
      if tmp_str == '':
        continue
      words = tmp_str.split()
      assert(len(words) == 3)
      edge_id = np.int(words[0])
      from_id = np.int(words[1])
      to_id = np.int(words[2])
      self.add_edge(from_id, to_id, edge_id, create_node = True, overwriting = False)

  def __str__(self):
    return "MNM_graph, number of node:" + str(self.G.number_of_nodes()) + ", number of edges:" + str(self.G.number_of_edges())

  def __repr__(self):
    return self.__str__()

  def generate_text(self):
    tmp_str = '# EdgeId FromNodeId  ToNodeId\n'
    for edge_id, (from_id, to_id) in self.edgeID_dict.iteritems():
      tmp_str += ' '.join([str(e) for e in [edge_id, from_id, to_id]]) + '\n'
    return tmp_str

class MNM_path():
  def __init__(self):
    # print "MNM_path"
    self.path_ID = None
    self.origin_node = None
    self.destination_node = None
    self.node_list = list()
    self.route_portions = None
    self.truck_route_portions = None

  def __init__(self, node_list, path_ID):
    self.path_ID = path_ID
    self.origin_node = node_list[0]
    self.destination_node = node_list[-1]
    self.node_list = node_list
    self.route_portions = None

  def __eq__(self, other):
    if ((self.origin_node is None) or (self.destination_node is None) or 
         (other.origin_node is None) or (other.destination_node is None)):
      return False 
    if (other.origin_node != self.origin_node):
      return False
    if (other.destination_node != self.destination_node):
      return False   
    if (len(self.node_list) != len(other.node_list)):
      return False
    for i in range(len(self.node_list)):
      if self.node_list[i] != other.node_list[i]:
        return False
    return True

  def create_route_choice_portions(self, num_intervals):
    self.route_portions = np.zeros(num_intervals)
    self.truck_route_portions = np.zeros(num_intervals)

  def attach_route_choice_portions(self, portions):
    self.route_portions = portions

  def attach_route_choice_portions_truck(self, portions):
    self.truck_route_portions = portions

  def __ne__(self, other):
    return not self.__eq__(other)

  def __str__(self):
    return "MNM_path, path ID {}, O node: {}, D node {}".format(self.path_ID, self.origin_node, self.destination_node)

  def __repr__(self):
    return self.__str__()

  def generate_node_list_text(self):
    return ' '.join([str(e) for e in self.node_list])

  def generate_portion_text(self):
    assert(self.route_portions is not None)
    assert(self.truck_route_portions is not None)
    return ' '.join([str(e) for e in np.append(self.route_portions, self.truck_route_portions)])

class MNM_pathset():
  def __init__(self):
    self.origin_node = None
    self.destination_node = None
    self.path_list = list()

  def add_path(self, path, overwriting = False):
    assert(path.origin_node == self.origin_node)
    assert(path.destination_node == self.destination_node)
    if (not overwriting) and (path in self.path_list):
      raise ("Error, path in path set")
    else:
      self.path_list.append(path)

  def normalize_route_portions(self, sum_to_OD = False):
    for i in range(len(self.path_list) - 1):
      assert(self.path_list[i].route_portions.shape == self.path_list[i + 1].route_portions.shape)
    tmp_sum = np.zeros(self.path_list[0].route_portions.shape)
    for i in range(len(self.path_list)):
      tmp_sum += self.path_list[i].route_portions
    for i in range(len(self.path_list)):
      self.path_list[i].route_portions = self.path_list[i].route_portions / np.maximum(tmp_sum, 1e-6)
    if sum_to_OD:
      return tmp_sum

  def normalize_truck_route_portions(self, sum_to_OD = False):
    for i in range(len(self.path_list) - 1):
      assert(self.path_list[i].truck_route_portions.shape == self.path_list[i + 1].truck_route_portions.shape)
    tmp_sum = np.zeros(self.path_list[0].truck_route_portions.shape)
    for i in range(len(self.path_list)):
      tmp_sum += self.path_list[i].truck_route_portions
    for i in range(len(self.path_list)):
      self.path_list[i].truck_route_portions = self.path_list[i].truck_route_portions / np.maximum(tmp_sum, 1e-6)
    if sum_to_OD:
      return tmp_sum

  def __str__(self):
    return "MNM_pathset, O node: {}, D node: {}, number_of_paths: {}".format(self.origin_node, self.destination_node, len(self.path_list))

  def __repr__(self):
    return self.__str__()

class MNM_pathtable():
  def __init__(self):
    # print "MNM_pathtable"
    self.path_dict = dict()
    self.ID2path = OrderedDict()

  def add_pathset(self, pathset, overwriting = False):
    if pathset.origin_node not in self.path_dict.keys():
      self.path_dict[pathset.origin_node] = dict()
    if (not overwriting) and (pathset.destination_node in 
              self.path_dict[pathset.origin_node]):
      raise ("Error: exists pathset in the pathtable")
    else:
      self.path_dict[pathset.origin_node][pathset.destination_node] = pathset

  def build_from_file(self, file_name, w_ID = False):
    if w_ID:
      raise ("Error, path table build_from_file no implemented")
    self.path_dict = dict()
    self.ID2path = OrderedDict()
    f = file(file_name)
    log = f.readlines()
    f.close()
    for i in range(len(log)):
      tmp_str = log[i]
      if tmp_str == '':
        continue
      words = tmp_str.split()
      origin_node = np.int(words[0])
      destination_node = np.int(words[-1])
      if origin_node not in self.path_dict.keys():
        self.path_dict[origin_node] = dict()
      if destination_node not in self.path_dict[origin_node].keys():
        tmp_path_set = MNM_pathset()
        tmp_path_set.origin_node = origin_node
        tmp_path_set.destination_node = destination_node
        self.add_pathset(tmp_path_set)
      tmp_node_list = list(map(lambda x : np.int(x), words))
      tmp_path = MNM_path(tmp_node_list, i)
      self.path_dict[origin_node][destination_node].add_path(tmp_path)
      self.ID2path[i] = tmp_path

  def load_route_choice_from_file(self, file_name, w_ID = False, buffer_length = None):
    if w_ID:
      raise ("Error, pathtable load_route_choice_from_file not implemented")
    f = file(file_name)
    log = list(filter(lambda x: not x.strip() == '', f.readlines()))
    f.close()
    assert(len(log) == len(self.ID2path))
    for i in range(len(log)):
      tmp_portions = np.array(log[i].strip().split()).astype(np.float)
      if buffer_length is not None:
        assert(len(tmp_portions) == buffer_length * 2)
        self.ID2path[i].attach_route_choice_portions(tmp_portions[:buffer_length])
        self.ID2path[i].attach_route_choice_portions_truck(tmp_portions[buffer_length:])
      else:
        raise("deprecated")

  def __str__(self):
    return "MNM_pathtable, number of paths:" + str(len(self.ID2path)) 

  def __repr__(self):
    return self.__str__()

  def generate_table_text(self):
    tmp_str = ""
    for path_ID, path in self.ID2path.iteritems():
      tmp_str += path.generate_node_list_text() + '\n'
    return tmp_str

  def generate_portion_text(self):
    tmp_str = ""
    for path_ID, path in self.ID2path.iteritems():
      tmp_str += path.generate_portion_text() + '\n'
    return tmp_str    

# class MNM_routing():
#   def __init__(self):
#     print "MNM_routing"

# class MNM_routing_fixed(MNM_routing):
#   def __init__(self):
#     super(MNM_routing_fixed, self).__init__()
#     self.required_items = ['num_path', 'choice_portion', 'route_frq', 'path_table']

# class MNM_routing_adaptive(MNM_routing):
#   """docstring for MNM_routing_adaptive"""
#   def __init__(self):
#     super(MNM_routing_adaptive, self).__init__()
#     self.required_items = ['route_frq']

# class MNM_routing_hybrid(MNM_routing):
#   """docstring for MNM_routing_hybrid"""
#   def __init__(self):
#     super(MNM_routing_hybrid, self).__init__()
#     self.required_items = []
    
    

class MNM_config():
  def __init__(self):
    # print "MNM_config"
    self.config_dict = OrderedDict()
    self.type_dict = {'network_name' : str, 'unit_time': np.int, 'total_interval': np.int,
                      'assign_frq' : np.int, 'start_assign_interval': np.int, 'max_interval': np.int,
                      'flow_scalar': np.int, 'num_of_link': np.int, 'num_of_node': np.int, 
                      'num_of_O': np.int, 'num_of_D': np.int, 'OD_pair': np.int,
                      'routing_type' : str, 'rec_mode': str, 'rec_mode_para': str, 'rec_folder': str,
                      'rec_volume': np.int, 'volume_load_automatic_rec': np.int, 'volume_record_automatic_rec': np.int,
                      'rec_tt': np.int, 'tt_load_automatic_rec':np.int, 'tt_record_automatic_rec':np.int,
                      'route_frq': np.int, 'path_file_name': str, 'num_path': np.int,
                      'choice_portion': str, 'route_frq': np.int, 'adaptive_ratio':np.float,
                      'buffer_length':np.int, 'adaptive_ratio_car': np.float, 
                      'adaptive_ratio_truck': np.float}

  def build_from_file(self, file_name):
    self.config_dict = OrderedDict()
    f = file(file_name)
    log = f.readlines()
    f.close()
    for i in range(len(log)):
      tmp_str = log[i]
      # print tmp_str
      if tmp_str == '' or tmp_str.startswith('#') or tmp_str.strip() == '':
        continue
      if tmp_str.startswith('['):
        new_item = tmp_str.strip().strip('[]')
        if new_item in self.config_dict.keys():
          raise ("Error, MNM_config, multiple items", new_item)
        self.config_dict[new_item] = OrderedDict()
        continue
      words = tmp_str.split('=')
      name = words[0].strip()
      self.config_dict[new_item][name] = self.type_dict[name](words[1].strip())

  def __str__(self):
    tmp_str = ''
    tmp_str += '[DTA]\n'
    for name, value in self.config_dict['DTA'].iteritems():
      tmp_str += "{} = {}\n".format(str(name), str(value))
    tmp_str += '\n[STAT]\n'
    for name, value in self.config_dict['STAT'].iteritems():
      tmp_str += "{} = {}\n".format(str(name), str(value))    
    if (self.config_dict['DTA']['routing_type'] == 'Fixed' or self.config_dict['DTA']['routing_type'] == 'Hybrid'
        or self.config_dict['DTA']['routing_type'] == 'Biclass_Hybrid'):
      tmp_str += '\n[FIXED]\n'
      for name, value in self.config_dict['FIXED'].iteritems():
        tmp_str += "{} = {}\n".format(str(name), str(value))  
    if (self.config_dict['DTA']['routing_type'] == 'Adaptive' or self.config_dict['DTA']['routing_type'] == 'Hybrid'
        or self.config_dict['DTA']['routing_type'] == 'Biclass_Hybrid'):
      tmp_str += '\n[ADAPTIVE]\n'
      for name, value in self.config_dict['ADAPTIVE'].iteritems():
        tmp_str += "{} = {}\n".format(str(name), str(value))  
    return tmp_str

  def __repr__(self):
    return self.__str__()

class MNM_network_builder():
  def __init__(self):
    self.config = MNM_config()
    self.network_name = None
    self.link_list = list()
    self.node_list = list()
    self.graph = MNM_graph()
    self.od = MNM_od()
    self.demand = MNM_demand()
    self.path_table = MNM_pathtable()
    self.route_choice_flag = False


  def get_link(self, ID):
    for link in self.link_list:
      if link.ID == ID:
        return link
    return None

  def load_from_folder(self, path, config_file_name = 'config.conf',
                                    link_file_name = 'MNM_input_link', node_file_name = 'MNM_input_node',
                                    graph_file_name = 'Snap_graph', od_file_name = 'MNM_input_od',
                                    pathtable_file_name = 'path_table', path_p_file_name = 'path_table_buffer',
                                    demand_file_name = 'MNM_input_demand'):
    if os.path.isfile(os.path.join(path, config_file_name)):
      self.config.build_from_file(os.path.join(path, config_file_name))
    else:
      print "No config file"
    if os.path.isfile(os.path.join(path, link_file_name)):
      self.link_list = self.read_link_input(os.path.join(path, link_file_name))
    else:
      print "No link input"
    if os.path.isfile(os.path.join(path, node_file_name)):
      self.node_list = self.read_node_input(os.path.join(path, node_file_name))
    else:
      print "No node input"
    if os.path.isfile(os.path.join(path, graph_file_name)):
      self.graph.build_from_file(os.path.join(path, graph_file_name))
    else:
      print "No graph input"

    if os.path.isfile(os.path.join(path, od_file_name)):
      self.od.build_from_file(os.path.join(path, od_file_name))
    else:
      print "No OD input"

    if os.path.isfile(os.path.join(path, demand_file_name)):
      self.demand.build_from_file(os.path.join(path, demand_file_name))
    else:
      # print "No demand input"
      pass

    if os.path.isfile(os.path.join(path, pathtable_file_name)):
      self.path_table.build_from_file(os.path.join(path, pathtable_file_name))
      if os.path.isfile(os.path.join(path, path_p_file_name)):
        self.path_table.load_route_choice_from_file(os.path.join(path, path_p_file_name), 
                    buffer_length = self.config.config_dict['FIXED']['buffer_length'])
        self.route_choice_flag = True
      else:
        self.route_choice_flag = False
        # print "No route choice portition for path table"

    else:
      print "No path table input"


  def dump_to_folder(self, path, config_file_name = 'config.conf',
                                    link_file_name = 'MNM_input_link', node_file_name = 'MNM_input_node',
                                    graph_file_name = 'Snap_graph', od_file_name = 'MNM_input_od',
                                    pathtable_file_name = 'path_table', path_p_file_name = 'path_table_buffer',
                                    demand_file_name = 'MNM_input_demand'):
    if not os.path.isdir(path):
      os.makedirs(path)
    f = open(os.path.join(path, link_file_name), 'wb')
    f.write(self.generate_link_text())
    f.close()

    f = open(os.path.join(path, node_file_name), 'wb')
    f.write(self.generate_node_text())
    f.close()

    f = open(os.path.join(path, config_file_name), 'wb')
    f.write(str(self.config))
    f.close()

    f = open(os.path.join(path, od_file_name), 'wb')
    f.write(self.od.generate_text())
    f.close()

    f = open(os.path.join(path, demand_file_name), 'wb')
    f.write(self.demand.generate_text())
    f.close()

    f = open(os.path.join(path, graph_file_name), 'wb')
    f.write(self.graph.generate_text())
    f.close()

    f = open(os.path.join(path, pathtable_file_name), 'wb')
    f.write(self.path_table.generate_table_text())
    f.close()

    if self.route_choice_flag:
      f = open(os.path.join(path, path_p_file_name), 'wb')
      f.write(self.path_table.generate_portion_text())
      f.close()

  def read_link_input(self, file_name):
    link_list = list()
    f = file(file_name)
    log = f.readlines()[1:]
    f.close()
    for i in range(len(log)):
      tmp_str = log[i].strip()
      if tmp_str == '':
        continue
      words = tmp_str.split()
      ID = np.int(words[0])
      typ = words[1]
      length = np.float(words[2])
      ffs_car = np.float(words[3])
      cap_car = np.float(words[4])
      rhoj_car = np.float(words[5])
      lanes = np.int(words[6])
      ffs_truck = np.float(words[7])
      cap_truck = np.float(words[8])
      rhoj_truck = np.float(words[9])
      convert_factor = np.float(words[10])
      l = MNM_dlink(ID, length, typ, ffs_car, cap_car, rhoj_car, lanes, ffs_truck, cap_truck, rhoj_truck, convert_factor)
      l.is_ok()
      link_list.append(l)
    return link_list

  def read_node_input(self, file_name):
    node_list = list()
    f = file(file_name)
    log = f.readlines()[1:]
    f.close()
    for i in range(len(log)):
      tmp_str = log[i].strip()
      if tmp_str == '':
        continue
      words = tmp_str.split()
      ID = np.int(words[0])
      typ = words[1]
      convert_factor = np.float(words[2])
      n = MNM_dnode(ID, typ, convert_factor)
      n.is_ok()
      node_list.append(n)
    return node_list

  def generate_link_text(self):
    tmp_str = '''#ID Type LEN(mile) FFS_car(mile/h) Cap_car(v/hour) RHOJ_car(v/miles) Lane FFS_truck(mile/h) Cap_truck(v/hour) RHOJ_truck(v/miles) Convert_factor(1)\n'''
    for link in self.link_list:
      tmp_str += link.generate_text() + '\n'
    return tmp_str

  def generate_node_text(self):
    tmp_str = '#ID Type Convert_factor(only for Inout node)\n'
    for node in self.node_list:
      tmp_str += node.generate_text() + '\n'
    return tmp_str

  def set_network_nake(self, name):
    self.network_name = name

  # def update_demand_path(self, car_flow, truck_flow, choice_dict):
  #   assert (len(car_flow) == len(self.path_table.ID2path) * self.config.config_dict['DTA']['max_interval'])
  #   assert (len(truck_flow) == len(self.path_table.ID2path) * self.config.config_dict['DTA']['max_interval'])
  #   max_interval = self.config.config_dict['DTA']['max_interval']
  #   car_flow = car_flow.reshape(self.config.config_dict['DTA']['max_interval'], len(self.path_table.ID2path))
  #   truck_flow = truck_flow.reshape(self.config.config_dict['DTA']['max_interval'], len(self.path_table.ID2path))
  #   for i, path_ID in enumerate(self.path_table.ID2path.keys()):
  #     path = self.path_table.ID2path[path_ID]
  #     path.attach_route_choice_portions(car_flow[:, i] + truck_flow[:, i])
  #   self.demand.demand_dict = dict()
  #   for O_node in self.path_table.path_dict.keys():
  #     O = self.od.O_dict.inv[O_node]
  #     for D_node in self.path_table.path_dict[O_node].keys():
  #       D = self.od.D_dict.inv[D_node]
  #       demand = self.path_table.path_dict[O_node][D_node].normalize_route_portions(sum_to_OD = True)
  #       if choice_dict[O][D] == 0:
  #         self.demand.add_demand(O, D, demand, np.zeros(max_interval), overwriting = True)
  #       elif choice_dict[O][D] == 1:
  #         self.demand.add_demand(O, D, np.zeros(max_interval), demand, overwriting = True)
  #       else:
  #         raise("Error, wrong truck flow or car flow in update_demand_path") 
  #   self.route_choice_flag = True



  def update_demand_path2(self, car_flow, truck_flow):
    assert (len(car_flow) == len(self.path_table.ID2path) * self.config.config_dict['DTA']['max_interval'])
    assert (len(truck_flow) == len(self.path_table.ID2path) * self.config.config_dict['DTA']['max_interval'])
    max_interval = self.config.config_dict['DTA']['max_interval']
    car_flow = car_flow.reshape(self.config.config_dict['DTA']['max_interval'], len(self.path_table.ID2path))
    truck_flow = truck_flow.reshape(self.config.config_dict['DTA']['max_interval'], len(self.path_table.ID2path))
    for i, path_ID in enumerate(self.path_table.ID2path.keys()):
      path = self.path_table.ID2path[path_ID]
      path.attach_route_choice_portions(car_flow[:, i])
      path.attach_route_choice_portions_truck(truck_flow[:, i])
    self.demand.demand_dict = dict()
    for O_node in self.path_table.path_dict.keys():
      O = self.od.O_dict.inv[O_node]
      for D_node in self.path_table.path_dict[O_node].keys():
        D = self.od.D_dict.inv[D_node]
        demand = self.path_table.path_dict[O_node][D_node].normalize_route_portions(sum_to_OD = True)
        truck_demand = self.path_table.path_dict[O_node][D_node].normalize_truck_route_portions(sum_to_OD = True)
        self.demand.add_demand(O, D, demand, truck_demand, overwriting = True)
    self.route_choice_flag = True

  # def get_path_flow(self):
  #   f = np.zeros((self.config.config_dict['DTA']['max_interval'], len(self.path_table.ID2path)))
  #   f_truck = np.zeros((self.config.config_dict['DTA']['max_interval'], len(self.path_table.ID2path)))
  #   for i, ID in enumerate(self.path_table.ID2path.keys()):
  #     path = self.path_table.ID2path[ID]
  #     # print path.route_portions
  #     f[:, i] = path.route_portions * self.demand.demand_dict[self.od.O_dict.inv[path.origin_node]][self.od.D_dict.inv[path.destination_node]][0]
  #     f_truck[:, i] = path.truck_route_portions  * self.demand.demand_dict[self.od.O_dict.inv[path.origin_node]][self.od.D_dict.inv[path.destination_node]][1]
  #   return f.flatten(), f_truck.flatten()