#ifndef PATH_H
#define PATH_H

#include "Snap.h"
#include "factory.h"
#include "shortest_path.h"
#include "path.h"

#include <vector>
#include <deque>
#include <unordered_map>
#include <fstream>
#include <string>

class MNM_Path
{
public:
  MNM_Path();
  ~MNM_Path();
  std::string node_vec_to_string();
  std::string buffer_to_string();
  std::deque<TInt> m_link_vec;
  std::deque<TInt> m_node_vec;
  TInt m_path_ID;
  TFlt m_p;
  TFlt *m_buffer;
  TInt m_buffer_length;
  int allocate_buffer(TInt length);
  inline bool operator==(const MNM_Path& rhs)
  {if (m_link_vec.size() != rhs.m_link_vec.size()) return false; 
  for (size_t i=0; i<rhs.m_link_vec.size(); ++i){if (rhs.m_link_vec[i] != m_link_vec[i]) return false;} return true;}
};

struct LessByPathP
{
  bool operator()(const MNM_Path* lhs, const  MNM_Path* rhs) const
  {
    return lhs -> m_p >= rhs -> m_p;
  }
};

class MNM_Pathset
{
public:
  MNM_Pathset();
  ~MNM_Pathset();
  std::vector<MNM_Path*> m_path_vec;
  int normalize_p();
  bool is_in(MNM_Path* path);
};

typedef std::unordered_map<TInt, std::unordered_map<TInt, MNM_Pathset*>*> Path_Table;

namespace MNM {
  MNM_Path *extract_path(TInt origin_ID, TInt dest_ID, std::map<TInt, TInt> &output_map, PNEGraph &graph);
  Path_Table *build_pathset(PNEGraph &graph, MNM_OD_Factory *od_factory, MNM_Link_Factory *link_factory);
  int save_path_table(Path_Table *path_table, MNM_OD_Factory *m_od_factory, bool w_buffer= false);
  Path_Table *build_shortest_pathset(PNEGraph &graph, MNM_OD_Factory *od_factory, MNM_Link_Factory *link_factory);
  // int save_path_table_w_buffer(Path_Table *path_table, MNM_OD_Factory *od_factory);
  int allocate_path_table_buffer(Path_Table *path_table, TInt num);
  int normalize_path_table_p(Path_Table *path_table);
  int copy_p_to_buffer(Path_Table *path_table, TInt col);
  int copy_buffer_to_p(Path_Table *path_table, TInt col);
  int get_ID_path_mapping(std::unordered_map<TInt, MNM_Path*>& map, Path_Table *path_table);
  MNM_Pathset* get_pathset(Path_Table *path_table, TInt origin_node_ID, TInt dest_node_ID);
}


#endif