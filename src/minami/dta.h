#ifndef DTA_H
#define DTA_H

#include "Snap.h"
#include "dlink.h"
#include "od.h"
#include "dnode.h"
#include "factory.h"
#include "ults.h"
#include "io.h"
#include "statistics.h"
#include "routing.h"


#include <string>

class MNM_Dta
{
public:
  MNM_Dta(std::string file_folder);
  ~MNM_Dta();
  int initialize();
  int build_from_files();
  bool is_ok();
  int hook_up_node_and_link();
  int loading(bool verbose);
private:
  int set_statistics();
  TFlt m_unit_time;
  TFlt m_flow_scalar;
  TInt m_assign_freq;
  std::string m_file_folder;
  MNM_ConfReader *m_config;
  MNM_Veh_Factory *m_veh_factory;
  MNM_Node_Factory *m_node_factory;
  MNM_Link_Factory *m_link_factory;
  MNM_OD_Factory *m_od_factory;
  PNEGraph m_graph;
  MNM_Statistics *m_statistics;
  MNM_Routing *m_routing;
};               

#endif