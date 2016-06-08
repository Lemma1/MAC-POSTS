#include "routing.h"

MNM_Routing::MNM_Routing(PNEGraph &graph, MNM_Statistics* statistics,
                          MNM_OD_Factory *od_factory, MNM_Link_Factory *link_factory)
{
  m_graph = graph;
  m_statistics = statistics;
  m_od_factory = od_factory;
  m_link_factory = link_factory;

}

MNM_Routing::~MNM_Routing()
{

}

/**************************************************************************
                          Random rouing
**************************************************************************/
/* assign each vehicle a random link ahead of it, only used for testing */

MNM_Routing_Random::MNM_Routing_Random(PNEGraph &graph, MNM_Statistics* statistics, MNM_OD_Factory *od_factory, MNM_Link_Factory *link_factory)
  : MNM_Routing::MNM_Routing(graph, statistics, od_factory, link_factory)
{

}


MNM_Routing_Random::~MNM_Routing_Random()
{

}

int MNM_Routing_Random::init_routing()
{
  return 0;
}


int MNM_Routing_Random::update_routing()
{
  MNM_Origin *__origin;
  MNM_DMOND *__origin_node;
  TInt __node_ID;
  TNEGraph::TNodeI __node_I;
  TInt __out_ID;
  MNM_Dlink *__next_link;
  MNM_Dlink* __link;
  printf("MNM_Routing: route the origin vehciles.\n");
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
  printf("MNM_Routing: route the link vehciles.\n");
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
  printf("MNM_Routing: Finished.\n");
  return 0;
}

