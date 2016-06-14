#include "shortest_path.h"
#include "limits.h"




int MNM_Shortest_Path::one_to_one(TInt origin_node_ID, TInt dest_node_ID, 
                      PNEGraph graph, std::map<TInt, TFlt>& cost_map,
                      std::vector<TInt> &output_array)
{
  return 0;
}


int MNM_Shortest_Path::all_to_one(TInt dest_node_ID, 
                      PNEGraph graph, std::map<TInt, TFlt>& cost_map,
                      std::map<TInt, TInt> &output_map)
{
  if (output_map.size() != 0){
    printf("Output map already exist, clear!\n");
    output_map.clear();
  }

  std::map<TInt, TFlt> _dist = std::map<TInt, TFlt>();
  _dist.insert(std::pair<TInt, TFlt>(dest_node_ID, TFlt(0)));
  
  std::deque<MNM_Cost*> m_Q;
  std::map<TInt, MNM_Cost*> m_Q_support;
  MNM_Cost *_cost = new MNM_Cost(dest_node_ID, TFlt(0));
  m_Q.push_back(_cost);
  m_Q_support.insert(std::pair<TInt, MNM_Cost*>(dest_node_ID, _cost));

  TInt _node_ID;
  for (auto _node_it = graph->BegNI(); _node_it < graph->EndNI(); _node_it++) {
    // printf("node id %d with out-degree %d and in-degree %d\n",
      // __node_it.GetId(), __node_it.GetOutDeg(), __node_it.GetInDeg());
    _node_ID = _node_it.GetId();
    if (_node_ID != dest_node_ID){
      _dist.insert(std::pair<TInt, TFlt>(_node_ID, DBL_MAX));
      output_map.insert(std::pair<TInt, TInt>(_node_ID, -1));
      _cost = new MNM_Cost(_node_ID, DBL_MAX);
      m_Q.push_back(_cost);
      m_Q_support.insert(std::pair<TInt, MNM_Cost*>(_node_ID, _cost));
    }
  }

  MNM_Cost *_min_cost;
  TInt _in_node_ID, _tmp_ID, _in_link_ID;
  TFlt _alt, _tmp_dist;
  while (m_Q.size() != 0){
    auto _pv = std::min_element(m_Q.begin(), m_Q.end(), CompareCostDecendSort);
    _min_cost = *(_pv);
    _tmp_ID = _min_cost -> m_ID;
    auto _node_it = graph -> GetNI(_tmp_ID);
    _tmp_dist = _dist.find(_tmp_ID) -> second;
    for (int e = 0; e < _node_it.GetInDeg(); ++e){
      _in_node_ID = _node_it.GetInNId(e);
      _in_link_ID = graph -> GetEI(_in_node_ID, _min_cost->m_ID).GetId();
      _alt = _tmp_dist + cost_map.find(_in_link_ID) -> second;
      if (_alt < _dist.find(_in_node_ID) -> second){
        _dist.find(_in_node_ID) -> second = _alt;
        output_map.find(_in_node_ID) -> second = _in_link_ID;
        m_Q_support.find(_in_node_ID) -> second -> m_cost = _alt;
      }
    }
    m_Q.erase(_pv);
  }

  for (auto _it = m_Q.begin(); _it != m_Q.end(); _it++){
    free (*_it);
  }
  m_Q.clear();
  m_Q_support.clear();
  return 0;
}

bool CompareCostDecendSort(MNM_Cost *lhs, MNM_Cost *rhs)
{
  return lhs->m_cost < rhs->m_cost;
}