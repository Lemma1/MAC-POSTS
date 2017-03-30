#include "gridlock_checker.h"

#include <deque>
#include <set>

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

bool MNM_Gridlock_Checker::has_cycle(PNEGraph graph)
{
  std::deque<TInt> _link_queue = std::deque<TInt>();
  std::set<TInt> _remained_link_set = std::set<TInt>();
  for (auto _link_it = graph->BegEI(); _link_it < graph->EndEI(); _link_it++){
    _remained_link_set.insert(_link_it.GetId());
  }
  TInt _temp_link_ID, _temp_link_ID2;
  TInt _temp_dest_node_ID;
  while (_remained_link_set.size() != 0){
    _link_queue.push_back(*(_remained_link_set.begin()));
    while (_link_queue.size() != 0){
      _temp_link_ID = _link_queue.front();
      _remained_link_set.erase(_temp_link_ID);
      _link_queue.pop_front();
      _temp_dest_node_ID = graph -> GetEI(_temp_link_ID).GetDstNId();
      TNEGraph::TNodeI _node_it = graph -> GetNI(_temp_dest_node_ID);
      for (int e = 0; e < graph -> GetNI(_temp_dest_node_ID).GetOutDeg(); ++e) {
        // printf("Out: edge (%d %d)\n", _node_it.GetId(), _node_it.GetOutNId(e));
        _temp_link_ID2 = _node_it.GetOutEId(e);
        if (_remained_link_set.find(_temp_link_ID2) != _remained_link_set.end()){
          _link_queue.push_back(_temp_link_ID2);
        }
        else{
          return false;
        }
      }
    }    
  }
  return true;
}

bool MNM_Gridlock_Checker::is_gridlocked()
{
  return false;
}

