#ifndef DUE_H
#define DUE_H

#include "path.h"
#include "factory.h"
#include "realtime_dta.h"

class MNM_Due
{
public:
  MNM_Due();
  ~MNM_Due();

  int init_path_set();
  int init_total_demand();
  int virtual update_p(){return 0;};
  std::string m_file_folder;
  Path_Table* m_path_set;
  MNM_Link_Factory *m_link_factory;
  MNM_Node_Factory *m_node_factory;
  MNM_ConfReader *m_dta_conf;
  MNM_ConfReader *m_due_conf;
  MNM_OD_Factory *m_od_factory;
  std::unordered_map<MNM_Origin*, std::unordered_map<MNM_Destination*, TFlt>*> *m_demand;
};

#endif