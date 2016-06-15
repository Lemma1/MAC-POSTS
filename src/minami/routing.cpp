#include "routing.h"

MNM_Routing::MNM_Routing(PNEGraph &graph, MNM_Statistics* statistics,
                          MNM_OD_Factory *od_factory, MNM_Node_Factory *node_factory, MNM_Link_Factory *link_factory)
{
  m_graph = graph;
  m_statistics = statistics;
  m_od_factory = od_factory;
  m_node_factory = node_factory;
  m_link_factory = link_factory;

}

MNM_Routing::~MNM_Routing()
{

}

/**************************************************************************
                          Random rouing
**************************************************************************/
/* assign each vehicle a random link ahead of it, only used for testing */

MNM_Routing_Random::MNM_Routing_Random(PNEGraph &graph, MNM_Statistics* statistics, MNM_OD_Factory *od_factory, 
                                          MNM_Node_Factory *node_factory, MNM_Link_Factory *link_factory)
  : MNM_Routing::MNM_Routing(graph, statistics, od_factory, node_factory, link_factory)
{

}


MNM_Routing_Random::~MNM_Routing_Random()
{

}

int MNM_Routing_Random::init_routing()
{
  return 0;
}


int MNM_Routing_Random::update_routing(TInt timestamp)
{
  MNM_Origin *__origin;
  MNM_DMOND *__origin_node;
  TInt __node_ID;
  TNEGraph::TNodeI __node_I;
  TInt __out_ID;
  MNM_Dlink *__next_link;
  MNM_Dlink* __link;
  // printf("MNM_Routing: route the origin vehciles.\n");
  /* route the vehicle in Origin nodes */
  for (auto __origin_it = m_od_factory->m_origin_map.begin(); __origin_it != m_od_factory->m_origin_map.end(); __origin_it++){
    __origin = __origin_it -> second;
    __origin_node = __origin -> m_origin_node;
    __node_ID = __origin_node -> m_node_ID;
    for (auto __veh_it = __origin_node -> m_in_veh_queue.begin(); __veh_it!=__origin_node -> m_in_veh_queue.end(); __veh_it++){
      __node_I = m_graph -> GetNI(__node_ID);
      __out_ID = __node_I.GetOutNId(MNM_Ults::mod(rand(), __node_I.GetOutDeg()));
      __next_link = m_link_factory -> get_link(m_graph -> GetEI(__node_ID, __out_ID).GetId());
      (*__veh_it) -> set_next_link(__next_link);
    }
  }
  // printf("MNM_Routing: route the link vehciles.\n");
  TInt __link_ID;
  /* route the vehicles at the end of each link */
  for (auto __link_it = m_link_factory -> m_link_map.begin(); __link_it != m_link_factory -> m_link_map.end(); __link_it ++){
    __link = __link_it -> second;
    __node_ID = __link -> m_to_node -> m_node_ID;
    for (auto __veh_it = __link -> m_finished_array.begin(); __veh_it!=__link -> m_finished_array.end(); __veh_it++){
      __node_I = m_graph -> GetNI(__node_ID);
      if (__node_I.GetOutDeg() > 0){
        __link_ID = __node_I.GetOutEId(MNM_Ults::mod(rand(), __node_I.GetOutDeg()));
        __next_link = m_link_factory -> get_link(__link_ID);
        (*__veh_it) -> set_next_link(__next_link);
      }
      else{
        (*__veh_it) -> set_next_link(NULL);
      }
    }
  }
  // printf("MNM_Routing: Finished.\n");
  return 0;
}

/**************************************************************************
                          Hybrid rouing
**************************************************************************/

MNM_Routing_Hybrid::MNM_Routing_Hybrid(std::string file_folder, PNEGraph &graph, MNM_Statistics* statistics, 
                  MNM_OD_Factory *od_factory, MNM_Node_Factory *node_factory, MNM_Link_Factory *link_factory)
  : MNM_Routing::MNM_Routing(graph, statistics, od_factory, node_factory, link_factory)
{
  m_self_config = new MNM_ConfReader(file_folder + "/config.conf", "HYBRID");
  m_routing_freq = m_self_config -> get_int("route_frq");
  m_table = new Routing_Table();
}

MNM_Routing_Hybrid::~MNM_Routing_Hybrid()
{
  for (auto _it = m_od_factory -> m_destination_map.begin(); _it != m_od_factory -> m_destination_map.end(); _it++){
    m_table -> find(_it -> second) -> second -> clear();
    free(m_table -> find(_it -> second) -> second);
  }
  m_table -> clear();
  free(m_table);
}

int MNM_Routing_Hybrid::init_routing()
{
  std::map<TInt, TInt> *_shortest_path_tree;
  for (auto _it = m_od_factory -> m_destination_map.begin(); _it != m_od_factory -> m_destination_map.end(); _it++){
    _shortest_path_tree = new std::map<TInt, TInt>();
    m_table -> insert(std::pair<MNM_Destination*, std::map<TInt, TInt>*>(_it -> second, _shortest_path_tree));
    // for (auto _node_it = m_node_factory -> m_node_map.begin(); _node_it != m_node_factory -> m_node_map.end(); _node_it++){
    //   _shortest_path_tree -> insert(std::pair<TInt, TInt>(_node_it -> first, -1));
    // }
  }
  return 0;
}

int MNM_Routing_Hybrid::update_routing(TInt timestamp)
{
  MNM_Destination *_dest;
  TInt _dest_node_ID;
  std::map<TInt, TInt> *_shortest_path_tree;
  if ((timestamp) % m_routing_freq  == 0 || timestamp == 0) {
    printf("Calculating the shortest path trees!\n");
    for (auto _it = m_od_factory -> m_destination_map.begin(); _it != m_od_factory -> m_destination_map.end(); _it++){
      _dest = _it -> second;
      _dest_node_ID = _dest -> m_dest_node -> m_node_ID;
      // printf("Destination ID: %d\n", (int) _dest_node_ID);
      _shortest_path_tree = m_table -> find(_dest) -> second;
      MNM_Shortest_Path::all_to_one_FIFO(_dest_node_ID, m_graph, m_statistics -> m_record_interval_tt, *_shortest_path_tree);
  //     for (auto _it = _shortest_path_tree -> begin(); _it!=_shortest_path_tree -> end(); _it++){
  //     // printf("For node %d, it should head to %d\n", _it -> first, _it -> second);
  // }
    }
  }

  /* route the vehicle in Origin nodes */
  printf("Routing the vehicle!\n");
  MNM_Origin *_origin;
  MNM_DMOND *_origin_node;
  TInt _node_ID, _next_link_ID;
  MNM_Dlink *_next_link;
  MNM_Veh *_veh;
  for (auto _origin_it = m_od_factory->m_origin_map.begin(); _origin_it != m_od_factory->m_origin_map.end(); _origin_it++){
    _origin = _origin_it -> second;
    _origin_node = _origin -> m_origin_node;
    _node_ID = _origin_node -> m_node_ID;
    for (auto _veh_it = _origin_node -> m_in_veh_queue.begin(); _veh_it!=_origin_node -> m_in_veh_queue.end(); _veh_it++){
      _veh = *_veh_it;
      _next_link_ID = m_table -> find(_veh -> get_destionation()) -> second -> find(_node_ID) -> second;
      if (_next_link_ID < 0){
        printf("Something wrong in routing, wrong next link 1\n");
        exit(-1);
      }
      _next_link = m_link_factory -> get_link(_next_link_ID);
      _veh -> set_next_link(_next_link);
    }
  }  

  MNM_Destination *_veh_dest;
  MNM_Dlink *_link;
  for (auto _link_it = m_link_factory -> m_link_map.begin(); _link_it != m_link_factory -> m_link_map.end(); _link_it ++){
    _link = _link_it -> second;
    _node_ID = _link -> m_to_node -> m_node_ID;
    for (auto _veh_it = _link -> m_finished_array.begin(); _veh_it!=_link -> m_finished_array.end(); _veh_it++){
      _veh = *_veh_it;
      _veh_dest = _veh -> get_destionation();
      if (_veh_dest -> m_dest_node -> m_node_ID == _node_ID){
        _veh -> set_next_link(NULL);
      }
      else{
        _next_link_ID = m_table -> find(_veh -> get_destionation()) -> second -> find(_node_ID) -> second;
        if (_next_link_ID == -1){
          printf("Something wrong in routing, wrong next link 2\n");
          printf("The node is %d, the vehicle should head to %d\n", (int)_node_ID, (int)_veh_dest -> m_dest_node -> m_node_ID);
          exit(-1);
        }
        _next_link = m_link_factory -> get_link(_next_link_ID);
        _veh -> set_next_link(_next_link);
      }
    }
  }
  printf("Finished Routing\n");
  return 0;
}