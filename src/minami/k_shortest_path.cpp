#include "k_shortest_path.h"

#include <functional>
#include <queue>

// double get_cost_from_cost(MNM_Cost c){return c.m_cost();}

// double get_cost_from_three_heap(KSP_Heap th){return th.m_top_cost.m_cost();}

KSP_Heap::KSP_Heap()
{
  m_top_cost = NULL;
}

KSP_Heap::~KSP_Heap()
{
}

MNM_KSP_Tree::MNM_KSP_Tree(TInt dest_node_ID, PNEGraph graph)
{
  m_dest_node_ID = dest_node_ID;
  m_graph = graph;
  m_dist_to_dest_map = std::unordered_map<TInt, TFlt>();
  m_sp_tree_map = std::unordered_map<TInt, TInt>();
  m_link_ID_heap_map = std::unordered_map<TInt, heap<MNM_Cost*>::node*>();
  m_node_ID_ksp_heap_map = std::unordered_map<TInt, heap<KSP_Heap*>::node*>();
}

MNM_KSP_Tree::~MNM_KSP_Tree()
{
  m_dist_to_dest_map.clear();
  m_sp_tree_map.clear();
}

int MNM_KSP_Tree::initialize()
{
  return 0;
}

int MNM_KSP_Tree::update_tree(std::unordered_map<TInt, TFlt>& cost_map)
{
  MNM_Shortest_Path::all_to_one_Dijkstra(m_dest_node_ID, m_graph, m_dist_to_dest_map, cost_map, m_sp_tree_map);
  update_DG(cost_map);

  return 0;
}

int MNM_KSP_Tree::get_ksp(TInt src_node_ID, TInt k, MNM_Pathset* path_set)
{
  // std::deque<TInt> _Q = std::deque<TInt>();
  std::priority_queue<MNM_Path*, std::vector<MNM_Path*>, LessByPathP> _Q \
  = std::priority_queue<MNM_Path*, std::vector<MNM_Path*>, LessByPathP>();
  // _Q.push(m_DG.minimal() -> m_top_cost -> m_ID);
  return 0;
}

int MNM_KSP_Tree::update_DG(std::unordered_map<TInt, TFlt>& cost_map)
{
  // construct D(G)
  KSP_Heap *_temp_ksp_heap;
  TInt _temp_node_ID, _temp_link_ID;
  MNM_Cost *_temp_cost;
  heap<MNM_Cost*>::node *_temp_node;
  heap<KSP_Heap*>::node *_temp_ksp_node;
  for (auto _map_it = m_sp_tree_map.begin(); _map_it != m_sp_tree_map.end(); ++_map_it){
    _temp_ksp_heap = new KSP_Heap();
    _temp_node_ID = _map_it -> first;
    TNEGraph::TNodeI _node_it = m_graph -> GetNI(_temp_node_ID);
    for (int e = 0; e < m_graph -> GetNI(_temp_node_ID).GetOutDeg(); ++e) {
      _temp_link_ID = _node_it.GetOutEId(e);
      if (_temp_link_ID != _map_it -> second){
        _temp_cost = new MNM_Cost(_temp_link_ID, delta(_temp_link_ID, cost_map));
        _temp_node = _temp_ksp_heap -> m_binary_heap_cost.insert(_temp_cost);
        m_link_ID_heap_map.insert({_temp_link_ID, _temp_node});
      }
    }
    _temp_ksp_heap -> m_top_cost = _temp_ksp_heap -> m_binary_heap_cost.minimal();

    
    // _temp_ksp_node = m_DG.insert(_temp_ksp_heap);
    // m_node_ID_ksp_heap_map.insert({_temp_node_ID,_temp_ksp_node});
  }
  
  return 0;
}


TFlt MNM_KSP_Tree::delta(TInt link_ID, std::unordered_map<TInt, TFlt>& cost_map)
{
  auto _link_it = m_graph -> GetEI(link_ID);
  return cost_map[link_ID] + m_dist_to_dest_map[_link_it.GetDstNId()] - m_dist_to_dest_map[_link_it.GetSrcNId()];
}

MNM_Path *MNM_KSP_Tree::build_path_with_one_more_sidetrack(MNM_Path *path)
{
  MNM_Cost *_temp_cost;
  TInt _last_link_ID = path -> m_link_vec.back();
  TInt _head_node_ID = m_graph -> GetEI(_last_link_ID).GetDstNId();
  if (m_node_ID_ksp_heap_map.find(_head_node_ID) != m_node_ID_ksp_heap_map.end()){
    MNM_Path *_new_path = new MNM_Path();
    _new_path -> m_link_vec = path -> m_link_vec;
    _temp_cost = m_node_ID_ksp_heap_map[_head_node_ID] -> value -> m_top_cost;
    _new_path -> m_link_vec.push_back(_temp_cost -> m_ID);
    _new_path -> m_p = path -> m_p + _temp_cost -> m_cost;
    return _new_path;
  }
  else{
    return NULL;
  }
}

std::vector<MNM_Path*> MNM_KSP_Tree::build_path_with_except_last_sidetrack(MNM_Path *path)
{
  std::vector<MNM_Path*> _path_set = std::vector<MNM_Path*>();
  TInt _last_link_ID = path -> m_link_vec.back();
  // TInt _head_node_ID = m_graph -> GetEI(_last_link_ID).GetSrcNId();
  heap<MNM_Cost*>::node *_temp_cost_node =  m_link_ID_heap_map[_last_link_ID];
  TFlt _delta_e = _temp_cost_node -> value -> m_cost;
  MNM_Cost *_temp_cost;
  for (auto _temp_cost_child : _temp_cost_node -> children){
    if (_temp_cost_child != NULL){
      _temp_cost = _temp_cost_child -> value;
      MNM_Path *_new_path = new MNM_Path();
      _new_path -> m_link_vec = path -> m_link_vec;
      _new_path -> m_link_vec.pop_back();
      _new_path -> m_link_vec.push_back(_temp_cost -> m_ID);
      _new_path -> m_p = path -> m_p - _delta_e + _temp_cost -> m_cost;
      _path_set.push_back(_new_path);
    }
  }
  return _path_set;
}
