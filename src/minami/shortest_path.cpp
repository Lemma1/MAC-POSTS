#include "shortest_path.h"
#include "limits.h"

#include <queue>


int MNM_Shortest_Path::one_to_one(TInt origin_node_ID, TInt dest_node_ID, 
                      PNEGraph graph, std::unordered_map<TInt, TFlt>& cost_map,
                      std::vector<TInt> &output_array)
{
  return 0;
}


int MNM_Shortest_Path::all_to_one_Dijkstra(TInt destination_ID, 
                        PNEGraph graph, std::unordered_map<TInt, TFlt> &cost_map,
                        std::unordered_map<TInt, TInt> &output_map)
{
  std::priority_queue<MNM_Cost*, std::vector<MNM_Cost*>, LessThanByCost> m_Q \
  = std::priority_queue<MNM_Cost*, std::vector<MNM_Cost*>, LessThanByCost>();
  MNM_Cost *dest_cost = new MNM_Cost(destination_ID, TFlt(0));
  m_Q.push(dest_cost);

  std::unordered_map<TInt, TFlt> dist_to_dest = std::unordered_map<TInt, TFlt>();

  for (auto _node_it = graph->BegNI(); _node_it < graph->EndNI(); _node_it++){
    TInt _node_id = _node_it.GetId();
    if (_node_id != destination_ID){
      dist_to_dest.insert({_node_id, TFlt(std::numeric_limits<double>::max())});
      output_map.insert({_node_id, -1});  // If the destination is not accessible the output remains -1
    }
  }
  dist_to_dest[destination_ID] = TFlt(0);

  // Initializaiton above. Dijkstra with binary min-heap (std::prioitiry_queue) below:

  // NOTE: Since C++ std::priority_queue does not have decrease_key() function, 
  // we insert [pointer to new MNM_cost object] to the min-heap every time when
  // the dist_to_dest[] changes for some node. So there could be duplicated elements
  // in the min-heap for the same nodes with different distance values. But the duplication
  // doesn't affect the correctness of algorithm. (visited label for eliminating the 
  // duplication is also tested, but slower than not using it, kind of weird.)
  while (m_Q.size() != 0){
    MNM_Cost *_min_cost = m_Q.top();
    m_Q.pop();
    TInt _node_id = _min_cost->m_ID;
    auto _node_it = graph->GetNI(_node_id);
    TFlt _tmp_dist = dist_to_dest[_node_id]; 
      for (int e = 0; e < _node_it.GetInDeg(); e++){
        TInt _in_node_id = _node_it.GetInNId(e);
        TInt _in_link_id = graph->GetEI(_in_node_id, _node_id).GetId();
        TFlt _alt = _tmp_dist + cost_map[_in_link_id];
        if (_alt < dist_to_dest[_in_node_id]){
          dist_to_dest[_in_node_id] = _alt;
          m_Q.push(new MNM_Cost(_in_node_id, _alt));
          output_map[_in_node_id] = _in_link_id;
        }
      }
    delete _min_cost;
  }

  return 0;
}


int MNM_Shortest_Path::all_to_one_Dijkstra_deprecated(TInt dest_node_ID, 
                      PNEGraph graph, std::unordered_map<TInt, TFlt>& cost_map,
                      std::unordered_map<TInt, TInt> &output_map)
{
  // if (output_map.size() != 0){
  //   // printf("Output map already exist, clear!\n");
  //   output_map.clear();
  // }
  // printf("1\n");
  std::unordered_map<TInt, TFlt> _dist = std::unordered_map<TInt, TFlt>();
  _dist.insert(std::pair<TInt, TFlt>(dest_node_ID, TFlt(0)));
  
  // std::deque<MNM_Cost*> m_Q = std::deque<MNM_Cost*>();
  std::priority_queue<MNM_Cost*, std::vector<MNM_Cost*>, LessThanByCost> m_Q = std::priority_queue<MNM_Cost*, std::vector<MNM_Cost*>, LessThanByCost>();
  std::unordered_map<TInt, MNM_Cost*> m_Q_support = std::unordered_map<TInt, MNM_Cost*>();
  MNM_Cost *_cost = new MNM_Cost(dest_node_ID, TFlt(0));
  // m_Q.push_back(_cost);
  m_Q.push(_cost);
  m_Q_support.insert(std::pair<TInt, MNM_Cost*>(dest_node_ID, _cost));

  TInt _node_ID;
  for (auto _node_it = graph->BegNI(); _node_it < graph->EndNI(); _node_it++) {
    // printf("node id %d with out-degree %d and in-degree %d\n",
      // _node_it.GetId(), _node_it.GetOutDeg(), _node_it.GetInDeg());
    _node_ID = _node_it.GetId();
    if (_node_ID != dest_node_ID){
      _dist.insert(std::pair<TInt, TFlt>(_node_ID, DBL_MAX));
      output_map.insert(std::pair<TInt, TInt>(_node_ID, -1));
      _cost = new MNM_Cost(_node_ID, DBL_MAX);
      // m_Q.push_back(_cost);
      m_Q.push(_cost);
      m_Q_support.insert(std::pair<TInt, MNM_Cost*>(_node_ID, _cost));
    }
  }
  // printf("2\n");
  MNM_Cost *_min_cost;
  TInt _in_node_ID, _tmp_ID, _in_link_ID;
  TFlt _alt, _tmp_dist;
  while (m_Q.size() != 0){
    // _min_cost = m_Q.front();
    // m_Q.pop_front();
    // auto _pv = std::min_element(m_Q.begin(), m_Q.end(), CompareCostDecendSort);
    _min_cost = m_Q.top();
    // _min_cost = *(_pv);
    _tmp_ID = _min_cost -> m_ID;
    auto _node_it = graph -> GetNI(_tmp_ID);
    _tmp_dist = _dist.find(_tmp_ID) -> second;
    for (int e = 0; e < _node_it.GetInDeg(); ++e){
      _in_node_ID = _node_it.GetInNId(e);
      // m_Q.push_back(m_Q_support.find(_in_node_ID) -> second);
      _in_link_ID = graph -> GetEI(_in_node_ID, _min_cost->m_ID).GetId();
      _alt = _tmp_dist + cost_map.find(_in_link_ID) -> second;
      if (_alt < _dist.find(_in_node_ID) -> second){
        // m_Q.push_back(m_Q_support.find(_in_node_ID) -> second);
        m_Q.push(m_Q_support.find(_in_node_ID) -> second);
        _dist.find(_in_node_ID) -> second = _alt;
        output_map.find(_in_node_ID) -> second = _in_link_ID;
        m_Q_support.find(_in_node_ID) -> second -> m_cost = _alt;
      }
    }
    // m_Q.erase(_pv);
    m_Q.pop();
  }

  // for (auto _it : m_Q){
  //   free (*_it);
  // }
  // m_Q.clear();
  m_Q_support.clear();
  _dist.clear();
  return 0;
}

int MNM_Shortest_Path::all_to_one_FIFO(TInt dest_node_ID, 
                      PNEGraph graph, const std::unordered_map<TInt, TFlt>& cost_map,
                      std::unordered_map<TInt, TInt> &output_map)
{
  // if (output_map.size() != 0){
  //   // printf("Output map already exist, clear!\n");
  //   output_map.clear();
  // }

  // for (auto _map_it : cost_map){
  //   printf("For link ID %d, it's cost is %.4f\n", _map_it.first(), _map_it.second());
  // }

  std::unordered_map<TInt, TFlt> _dist = std::unordered_map<TInt, TFlt>();
  _dist.insert(std::pair<TInt, TFlt>(dest_node_ID, TFlt(0)));
  
  std::deque<TInt> m_Q = std::deque<TInt>();
  std::unordered_map<TInt, bool> m_Q_support = std::unordered_map<TInt, bool>();

  m_Q.push_back(dest_node_ID);
  m_Q_support.insert(std::pair<TInt, bool>(dest_node_ID, true));

  TInt _node_ID;
  for (auto _node_it = graph->BegNI(); _node_it < graph->EndNI(); _node_it++) {
    // printf("node id %d with out-degree %d and in-degree %d\n",
      // _node_it.GetId(), _node_it.GetOutDeg(), _node_it.GetInDeg());
    _node_ID = _node_it.GetId();
    if (_node_ID != dest_node_ID){
      _dist.insert(std::pair<TInt, TFlt>(_node_ID, DBL_MAX));
      if (output_map.find(_node_ID) != output_map.end()){
        output_map.find(_node_ID) -> second = -1;
      }
      else{
        output_map.insert(std::pair<TInt, TInt>(_node_ID, -1));
      }
      m_Q_support.insert(std::pair<TInt, bool>(_node_ID, false));
    }
  }
  TInt _in_node_ID, _tmp_ID, _in_link_ID;
  TFlt _alt, _tmp_dist;
  while (m_Q.size() != 0){
    // printf("current m_Q size is %d\n", m_Q.size());
    _tmp_ID = m_Q.front();
    m_Q.pop_front();
    m_Q_support.find(_tmp_ID) -> second = false;
    auto _node_it = graph -> GetNI(_tmp_ID);
    _tmp_dist = _dist.find(_tmp_ID) -> second;
    for (int e = 0; e < _node_it.GetInDeg(); ++e){
      _in_node_ID = _node_it.GetInNId(e);
      // m_Q.push_back(m_Q_support.find(_in_node_ID) -> second);
      _in_link_ID = graph -> GetEI(_in_node_ID, _tmp_ID).GetId();
      _alt = _tmp_dist + cost_map.find(_in_link_ID) -> second;
      // printf("Current alternative distance is %.4f\n", _alt());
      if (_alt < _dist.find(_in_node_ID) -> second){
        // m_Q.push_back(m_Q_support.find(_in_node_ID) -> second);
        _dist.find(_in_node_ID) -> second = _alt;
        output_map.find(_in_node_ID) -> second = _in_link_ID;
        if (!m_Q_support.find(_in_node_ID) -> second){
          m_Q.push_back(_in_node_ID);
          m_Q_support.find(_in_node_ID) -> second = true;
        }
      }
    }
  }

  m_Q.clear();
  m_Q_support.clear();
  _dist.clear();
  return 0;
}


int MNM_Shortest_Path::all_to_one_LIFO(TInt dest_node_ID, 
                      PNEGraph graph, std::unordered_map<TInt, TFlt>& cost_map,
                      std::unordered_map<TInt, TInt> &output_map)
{
  // if (output_map.size() != 0){
  //   // printf("Output map already exist, clear!\n");
  //   output_map.clear();
  // }

  // for (auto _map_it : cost_map){
  //   printf("For link ID %d, it's cost is %.4f\n", _map_it.first(), _map_it.second());
  // }

  std::unordered_map<TInt, TFlt> _dist = std::unordered_map<TInt, TFlt>();
  _dist.insert(std::pair<TInt, TFlt>(dest_node_ID, TFlt(0)));
  
  std::deque<TInt> m_Q = std::deque<TInt>();
  std::unordered_map<TInt, bool> m_Q_support = std::unordered_map<TInt, bool>();

  m_Q.push_front(dest_node_ID);
  m_Q_support.insert(std::pair<TInt, bool>(dest_node_ID, true));

  TInt _node_ID;
  for (auto _node_it = graph->BegNI(); _node_it < graph->EndNI(); _node_it++) {
    // printf("node id %d with out-degree %d and in-degree %d\n",
      // _node_it.GetId(), _node_it.GetOutDeg(), _node_it.GetInDeg());
    _node_ID = _node_it.GetId();
    if (_node_ID != dest_node_ID){
      _dist.insert(std::pair<TInt, TFlt>(_node_ID, DBL_MAX));
      if (output_map.find(_node_ID) != output_map.end()){
        output_map.find(_node_ID) -> second = -1;
      }
      else{
        output_map.insert(std::pair<TInt, TInt>(_node_ID, -1));
      }
      m_Q_support.insert(std::pair<TInt, bool>(_node_ID, false));
    }
  }
  TInt _in_node_ID, _tmp_ID, _in_link_ID;
  TFlt _alt, _tmp_dist;
  while (m_Q.size() != 0){
    // printf("current m_Q size is %d\n", m_Q.size());
    _tmp_ID = m_Q.front();
    m_Q.pop_front();
    m_Q_support.find(_tmp_ID) -> second = false;
    auto _node_it = graph -> GetNI(_tmp_ID);
    _tmp_dist = _dist.find(_tmp_ID) -> second;
    for (int e = 0; e < _node_it.GetInDeg(); ++e){
      _in_node_ID = _node_it.GetInNId(e);
      // m_Q.push_back(m_Q_support.find(_in_node_ID) -> second);
      _in_link_ID = graph -> GetEI(_in_node_ID, _tmp_ID).GetId();
      _alt = _tmp_dist + cost_map.find(_in_link_ID) -> second;
      // printf("Current alternative distance is %.4f\n", _alt());
      if (_alt < _dist.find(_in_node_ID) -> second){
        // m_Q.push_back(m_Q_support.find(_in_node_ID) -> second);
        _dist.find(_in_node_ID) -> second = _alt;
        output_map.find(_in_node_ID) -> second = _in_link_ID;
        if (!m_Q_support.find(_in_node_ID) -> second){
          m_Q.push_front(_in_node_ID);
          m_Q_support.find(_in_node_ID) -> second = true;
        }
      }
    }
  }

  m_Q.clear();
  m_Q_support.clear();
  _dist.clear();
  return 0;
}

bool CompareCostDecendSort(MNM_Cost *lhs, MNM_Cost *rhs)
{
  return lhs->m_cost < rhs->m_cost;
}


/*------------------------------------------------------------
                          TDSP
-------------------------------------------------------------*/
int  MNM_Shortest_Path::all_to_one_TDSP(TInt dest_node_ID, 
                        PNEGraph graph, std::unordered_map<TInt, TFlt*>& cost_map,
                        std::unordered_map<TInt, TInt*> &output_map, TInt num_interval)
{
  return 0;
}

bool MNM_Shortest_Path::is_FIFO(PNEGraph graph, std::unordered_map<TInt, TFlt*>& cost_map, 
                            TInt num_interval, TFlt unit_time)
{
  TInt _edge_ID;
  TFlt *_cost_list;
  for (auto _edge_it = graph->BegEI(); _edge_it < graph->EndEI(); _edge_it++) {
    _edge_ID = _edge_it.GetId();
    _cost_list = cost_map.find(_edge_ID) -> second;
    for (int i=0; i<num_interval - 1; ++i){
      if (_cost_list[i] > unit_time + _cost_list[i+1]){
        return false;
      }
    }
  }
  return true;
}





