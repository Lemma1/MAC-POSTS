#ifndef REALTIME_DTA_H
#define REALTIME_DTA_H

#include "Snap.h"
#include "path.h"
#include "io.h"

#include <string>
#include <typeinfo>

class MNM_Dta_Screenshot;

class MNM_Realtime_Dta
{
public:
  MNM_Realtime_Dta(std::string file_folder);
  ~MNM_Realtime_Dta();
  int run_from_screenshot(MNM_Dta_Screenshot* screenshot, TInt start_inter, TInt max_inter);
  std::string m_file_folder;
  PNEGraph m_graph;
  MNM_ConfReader* m_config;
  MNM_Dta_Screenshot *m_before_shot;
  MNM_Dta_Screenshot *m_after_shot;
  Path_Table *m_path_table;


};

class MNM_Dta_Screenshot
{
public:
  MNM_Dta_Screenshot(std::string file_folder, MNM_ConfReader* config, PNEGraph graph);
  ~MNM_Dta_Screenshot();
  int build_static_network();
  int hook_up_node_and_link();
  PNEGraph m_graph;
  std::string m_file_folder;
  MNM_ConfReader* m_config;
  MNM_Link_Factory *m_link_factory;
  MNM_OD_Factory *m_od_factory;
  MNM_Node_Factory *m_node_factory;
  MNM_Veh_Factory *m_veh_factory;
};

namespace MNM
{
  MNM_Dta_Screenshot *make_screenshot(std::string file_folder, MNM_ConfReader* config, MNM_Link_Factory *link_factory,
                                      MNM_Node_Factory *node_factory, PNEGraph graph);
}


void static inline copy_veh(MNM_Veh* _veh, MNM_Veh *_new_veh){
  _new_veh -> m_current_link = _veh -> m_current_link;
  _new_veh -> m_start_time = _veh -> m_start_time;
  _new_veh -> m_next_link = _veh -> m_next_link;
  _new_veh -> m_dest = _veh -> m_dest;
};
#endif