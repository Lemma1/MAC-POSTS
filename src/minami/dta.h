#ifndef DTA_H
#define DTA_H

#include "Snap.h"
#include "dlink.h"
#include "od.h"
#include "vehicle.h"
#include "dnode.h"
#include "factory.h"
#include "ults.h"
#include "io.h"

#include <string>

class MNM_Dta
{
public:
  MNM_Dta(std::string file_folder);
  ~MNM_Dta();
  int initialize();
  int build_from_files();
  bool is_ok();
  int hook_up_linkANDnode();

  TFlt m_unit_time;
  TFlt m_flow_scalar;
  std::string m_file_folder;
  MNM_ConfReader *m_config;
  MNM_Veh_Factory *m_veh_factory;
  MNM_Node_Factory *m_node_factory;
  MNM_Link_Factory *m_link_factory;
  MNM_OD_Factory *m_od_factory;
  PNGraph m_graph;
};               

#endif