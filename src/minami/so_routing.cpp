#include "routing.h"

/**************************************************************************
                          Pre determined Routing
**************************************************************************/

MNM_Routing_Predetermined::MNM_Routing_Predetermined(PNEGraph &graph,
              MNM_OD_Factory *od_factory, MNM_Node_Factory *node_factory, MNM_Link_Factory *link_factory
              ,Path_Table *p_table, MNM_Pre_Routing *pre_routing)
  : MNM_Routing::MNM_Routing(graph, od_factory, node_factory, link_factory){
  	m_tracker = std::unordered_map<MNM_Veh*, std::deque<TInt>*>();
  	m_path_table = p_table;
  	m_pre_routing = pre_routing;

  }

MNM_Routing_Predetermined::~MNM_Routing_Predetermined()
{
  for (auto _map_it : m_tracker){
    _map_it.second -> clear();
    delete _map_it.second;
  }
  m_tracker.clear();
}

int MNM_Routing_Predetermined::update_routing(TInt timestamp){
  // question: the releasing frequency of origin is not every assignment interval?
  TInt _release_freq = m_od_factory -> m_origin_map.begin() -> second -> m_frequency;
  MNM_Origin *_origin;
  MNM_DMOND *_origin_node;
  TInt _node_ID, _next_link_ID;
  MNM_Dlink *_next_link;
  MNM_Veh *_veh;
  if (timestamp % _release_freq ==0){
    // need to register vehicles to m_tracker
  }

  // step 1: register vehilces in the Origin nodes to m_tracker, update their next link

  // step 2: update the next link of all vehicles in the last cell of links
  MNM_Destination *_veh_dest;
  MNM_Dlink *_link;
  for (auto _link_it = m_link_factory -> m_link_map.begin(); _link_it != m_link_factory -> m_link_map.end(); _link_it ++){
    _link = _link_it -> second;
    _node_ID = _link -> m_to_node -> m_node_ID;
    // printf("2.1\n");
    for (auto _veh_it = _link -> m_finished_array.begin(); _veh_it!=_link -> m_finished_array.end(); _veh_it++){
      _veh = *_veh_it;
      _veh_dest = _veh -> get_destination();
      // printf("2.2\n");
      if (_veh_dest -> m_dest_node -> m_node_ID == _node_ID){
        if (m_tracker.find(_veh) -> second -> size() != 0){
          printf("Something wrong in fixed routing!\n");
          exit(-1);
        }
        _veh -> set_next_link(NULL);
        // m_tracker.erase(m_tracker.find(_veh));
      }
      else{
        // printf("2.3\n");
        if (m_tracker.find(_veh) == m_tracker.end()){
          printf("Vehicle not registered in link, impossible!\n");
          exit(-1);
        }
        if(_veh -> get_current_link() == _veh -> get_next_link()){
          _next_link_ID = m_tracker.find(_veh) -> second -> front();
          if (_next_link_ID == -1){
            printf("Something wrong in routing, wrong next link 2\n");
            printf("The node is %d, the vehicle should head to %d\n", (int)_node_ID, (int)_veh_dest -> m_dest_node -> m_node_ID);
            exit(-1);
          }
          _next_link = m_link_factory -> get_link(_next_link_ID);
          _veh -> set_next_link(_next_link);
          m_tracker.find(_veh) -> second -> pop_front();
        }
      }
    }
  }




	return 0;
}


