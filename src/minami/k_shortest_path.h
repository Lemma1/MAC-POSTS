#ifndef K_SHORTEST_PATH_H
#define K_SHORTEST_PATH_H

#include "Snap.h"
#include "path.h"
#include "shortest_path.h"
#include "d_array_heap.h"

#include <unordered_map>



/* tailed-heap used in the ksp algorithm */
class KSP_Heap
{ 
public:
  KSP_Heap();
  ~KSP_Heap();
  MNM_Cost *m_top_cost;
  heap<MNM_Cost*> m_binary_heap_cost = heap<MNM_Cost*>([](MNM_Cost *c) {return c -> m_cost();});  
};



/*------------------------------------------------------------
                  K-shortest path one destination tree
-------------------------------------------------------------*/
class MNM_KSP_Tree
{
public:
  MNM_KSP_Tree(TInt dest_node_ID, PNEGraph graph);
  ~MNM_KSP_Tree();

  int initialize();
  int update_tree(std::unordered_map<TInt, TFlt>& cost_map);
  int update_heap();
  int get_ksp(TInt src_node_ID, MNM_Pathset* path_set);


  TInt m_dest_node_ID;
  std::unordered_map<TInt, TFlt> m_dist_to_dest_map;
  std::unordered_map<TInt, TInt> m_sp_tree_map;
  heap<KSP_Heap*> m_DG = heap<KSP_Heap*>([](KSP_Heap *th){return th -> m_top_cost -> m_cost();});
  std::unordered_map<TInt, heap<KSP_Heap>::node*> m_node_ID_heap_map;
  PNEGraph m_graph;
};



#endif
