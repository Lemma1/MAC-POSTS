#include "k_shortest_path.h"

#include <functional>

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
  m_node_ID_heap_map = std::unordered_map<TInt, heap<KSP_Heap>::node*>();
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
  return 0;
}

int MNM_KSP_Tree::get_ksp(TInt src_node_ID, MNM_Pathset* path_set)
{
  return 0;
}

int MNM_KSP_Tree::update_heap()
{
  // construct tailed heap for each node in SP tree

  return 0;
}