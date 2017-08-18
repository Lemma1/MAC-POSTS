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
	return 0;
}


