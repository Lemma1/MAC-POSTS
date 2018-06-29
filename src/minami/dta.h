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
#include "shortest_path.h"
#include "pre_routing.h"
#include "emission.h"

#include <string>

class MNM_Dta
{
public:
  MNM_Dta(std::string file_folder);
  virtual ~MNM_Dta();
  int virtual initialize();
  int virtual build_from_files();
  bool is_ok();
  int hook_up_node_and_link();
  int loading(bool verbose);
  int load_once(bool verbose, TInt load_int, TInt assign_int);
  int test();
// private:
  bool finished_loading(int cur_int);
  int set_statistics();
  int set_routing();
  int build_workzone();
  int check_origin_destination_connectivity();
  int virtual pre_loading();
  
  int record_queue_vehicles();
  int record_enroute_vehicles();

  TInt m_start_assign_interval;
  TInt m_total_assign_inter;
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
  MNM_Workzone *m_workzone;
  TInt m_current_loading_interval;
  MNM_Cumulative_Emission *m_emission;

  std::unordered_map<TInt, std::deque<TInt>*> m_queue_veh_map; //queuing vehicle number for each link
  std::deque<TInt> m_queue_veh_num; //total queuing vehicle number
  std::deque<TInt> m_enroute_veh_num; //total enroute vehicle number
};               


namespace MNM
{
  int print_vehicle_statistics(MNM_Veh_Factory *veh_factory);
  int print_vehicle_info(MNM_Veh_Factory *veh_factory);
  bool has_running_vehicle(MNM_Veh_Factory *veh_factory);
}

#endif