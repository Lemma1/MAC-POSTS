#include "gridlock_checker.h"


MNM_Gridlock_Checker::MNM_Gridlock_Checker(PNEGraph graph, MNM_Link_Factory *link_factory) 
{
  m_full_graph = graph;
  m_link_factory = link_factory;
  m_link_veh_map = std::unordered_map<TInt, MNM_Veh*>();
  m_gridlock_graph = PNEGraph::TObj::New();
}

MNM_Gridlock_Checker::~MNM_Gridlock_Checker() 
{
  m_link_veh_map.clear();
  m_gridlock_graph.Clr();
}

int MNM_Gridlock_Checker::initialize()
{
  for (auto _map_it : m_link_factory -> m_link_map){
    m_link_veh_map.insert({_map_it.first, get_last_veh(_map_it.second)});
  }  
  return 0;
}

MNM_Veh* MNM_Gridlock_Checker::get_last_veh(MNM_Dlink *link)
{
  if (link -> m_finished_array.size() == 0){
    return NULL;
  }
  return link -> m_finished_array.front();
}

bool MNM_Gridlock_Checker::is_gridlocked()
{
  return false;
}