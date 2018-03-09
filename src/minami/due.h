#ifndef DUE_H
#define DUE_H

#include "path.h"
#include "factory.h"
#include "realtime_dta.h"

class MNM_Due
{
public:
  MNM_Due(std::string file_folder);
  ~MNM_Due();

  int init_from_files();
  int run_once();

  int virtual init_path_table(){return 0;};
  int virtual update_path_table(){return 0;};
  std::string m_file_folder;
  Path_Table* m_path_table;
  PNEGraph m_graph;

  TInt m_total_assign_inter;

  MNM_Link_Factory *m_link_factory;
  MNM_Node_Factory *m_node_factory;
  MNM_ConfReader *m_dta_config;
  MNM_ConfReader *m_due_config;
  MNM_OD_Factory *m_od_factory;
};

#endif