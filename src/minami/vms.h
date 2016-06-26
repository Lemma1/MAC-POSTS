#ifndef VMS_H
#define VMS_H

#include "Snap.h"
#include "factory.h"
#include "path.h"

#include <vector>
#include <map>

class MNM_Link_Vms
{
public:
  MNM_Link_Vms(TInt ID, TInt link_ID, PNEGraph graph);
  ~MNM_Link_Vms();
  TInt m_ID;
  TInt m_my_link_ID;
  TInt m_detour_link_ID;
  std::vector<TInt> m_out_link_vec;
  std::map<TInt, std::vector<MNM_Path*>*> m_link_path_map;

  TInt generate_detour_link(Path_Table *path_table, TInt next_assign_inter, MNM_Node_Factory *node_factory);
  TFlt m_compliance_ratio;
  int hook_link(PNEGraph graph);
  int hook_path(Path_Table *path_table);
};


class MNM_Vms_Factory
{
public:
  MNM_Vms_Factory();
  ~MNM_Vms_Factory();
  std::map<TInt, MNM_Link_Vms*> m_link_vms_map;
  MNM_Link_Vms *make_link_vms(TInt ID, TInt link_ID, PNEGraph graph);
  MNM_Link_Vms *get_link_vms(TInt ID);
  int hook_path(Path_Table *path_table);
};

namespace MNM {
  int generate_vms_instructions(std::string file_name, MNM_Vms_Factory* vms_factory, MNM_Link_Factory *link_factory);
}
#endif