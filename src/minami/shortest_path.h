#ifndef SHORTEST_PATH_H
#define SHORTEST_PATH_H

#include "Snap.h"
#include "path.h"
#include "ults.h"

#include <vector>
#include <unordered_map>
#include <deque>
#include <algorithm>

class MNM_Link_Cost;
class MNM_Path;

class MNM_Shortest_Path
{
public:
  int static one_to_one(TInt origin_node_ID, TInt dest_node_ID, 
                        PNEGraph graph, std::unordered_map<TInt, TFlt>& cost_map,
                        std::vector<TInt> &output_array);
  int static all_to_one_Dijkstra(TInt dest_node_ID, 
                        PNEGraph graph, std::unordered_map<TInt, TFlt>& cost_map,
                        std::unordered_map<TInt, TInt> &output_map);
  int static all_to_one_Dijkstra(TInt destination_ID, 
                        PNEGraph graph, 
                        std::unordered_map<TInt, TFlt> &dist_to_dest,
                        std::unordered_map<TInt, TFlt> &cost_map,
                        std::unordered_map<TInt, TInt> &output_map);
  int static all_to_one_Dijkstra(TInt destination_ID, 
                        PNEGraph graph, std::unordered_map<TInt, TFlt*> &cost_map,
                        std::unordered_map<TInt, TFlt*> &dist_to_dest,
                        std::unordered_map<TInt, TInt*> &output_map,
                        TInt cost_position, TInt dist_position, 
                        TInt output_position);
  int static all_to_one_Dijkstra_deprecated(TInt dest_node_ID, 
                        PNEGraph graph, std::unordered_map<TInt, TFlt>& cost_map,
                        std::unordered_map<TInt, TInt> &output_map);
  int static all_to_one_FIFO(TInt dest_node_ID, 
                        PNEGraph graph, const std::unordered_map<TInt, TFlt>& cost_map,
                        std::unordered_map<TInt, TInt> &output_map);
  int static all_to_one_LIFO(TInt dest_node_ID, 
                        PNEGraph graph, std::unordered_map<TInt, TFlt>& cost_map,
                        std::unordered_map<TInt, TInt> &output_map);
/*------------------------------------------------------------
                          TDSP
-------------------------------------------------------------*/
  int static all_to_one_TDSP(TInt dest_node_ID, 
                        PNEGraph graph, std::unordered_map<TInt, TFlt*>& cost_map,
                        std::unordered_map<TInt, TInt*> &output_map, TInt num_interval);
  bool static is_FIFO(PNEGraph graph, std::unordered_map<TInt, TFlt*>& cost_map, TInt num_interval,
                      TFlt unit_time);
};

/*------------------------------------------------------------
                  shortest path one destination tree
-------------------------------------------------------------*/
// class MNM_SP_Tree
// {
//   MNM_SP_Tree(TInt dest_node_ID, PNEGraph graph);
//   ~MNM_SP_Tree();

//   int initialize();
//   int update_tree(std::unordered_map<TInt, TFlt>& cost_map);
//   TFlt get_distance_to_destination(TInt node_ID);
//   int get_next_link_ID(TInt node_ID);

// };


/*------------------------------------------------------------
                  TDSP  one destination tree
-------------------------------------------------------------*/
class MNM_TDSP_Tree
{
public:
  MNM_TDSP_Tree(TInt dest_node_ID, PNEGraph graph, TInt max_interval);
  ~MNM_TDSP_Tree();

  int initialize();
  int update_tree(std::unordered_map<TInt, TFlt*>& cost_map);
  TFlt get_distance_to_destination(TInt node_ID, TFlt time_stamp);
  int get_tdsp(TInt src_node_ID, TInt time, MNM_Path* path);
  std::unordered_map<TInt, TFlt*> m_dist;
  std::unordered_map<TInt, TInt*> m_tree;
  TInt m_dest_node_ID;
  PNEGraph m_graph;
  TInt m_max_interval;
};



/*------------------------------------------------------------
                          misc
-------------------------------------------------------------*/
class MNM_Cost
{
public:
  MNM_Cost(){;};
  MNM_Cost(TInt ID, TFlt cost){m_ID = ID; m_cost = cost;};
  TInt m_ID;
  TFlt m_cost;
  // bool operator <(const MNM_Cost& d){return m_cost >= d.m_cost;};
  // bool operator >(const MNM_Cost& d){return m_cost > d.m_cost;};
  // bool operator <=(const MNM_Cost& d){return m_cost <= d.m_cost;};
  // bool operator >=(const MNM_Cost& d){return m_cost >= d.m_cost;};
};

struct LessThanByCost
{
  bool operator()(const MNM_Cost* lhs, const  MNM_Cost* rhs) const
  {
    return lhs -> m_cost >= rhs -> m_cost;
  }
};

bool CompareCostDecendSort(MNM_Cost *lhs, MNM_Cost *rhs);

#endif
