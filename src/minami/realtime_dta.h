#ifndef REALTIME_DTA_H
#define REALTIME_DTA_H

#include "Snap.h"
#include "od.h"
#include "path.h"
#include "io.h"
#include "routing.h"
#include "statistics.h"
#include "marginal_cost.h"


#include <string>
#include <typeinfo>

class MNM_Dta_Screenshot;

class MNM_Realtime_Dta
{
public:
  MNM_Realtime_Dta(std::string file_folder);
  ~MNM_Realtime_Dta();
  int initialize();
  int build_path_table();
  MNM_Statistics *get_statistics(MNM_Node_Factory *node_factory, MNM_Link_Factory *link_factory);
  int get_estimation_gradient(MNM_Dta_Screenshot* screenshot,
                              TInt max_inter, TInt assign_inter, Path_Table *path_table, 
                              std::unordered_map<TInt, TFlt> *link_spd_map);
  int get_optimization_gradient(MNM_Dta_Screenshot* screenshot,
                                  TInt max_inter, TInt assign_inter, Path_Table *path_table);
  int one_iteration(TInt assign_inter);
  int estimate_previous(TInt assign_inter);
  int optimize_next(TInt next_assign_inter);
  int predict_next(TInt next_assign_inter);
  int update_compliance_ratio(TInt assign_inter);
  int update_vms(TInt next_assign_inter);
  int init_running();
  int reset();
  TInt m_estimation_iters;
  TInt m_optimization_iters;
  TInt m_sample_points;
  TInt m_prediction_length;
  TInt m_estimation_length;
  TInt m_total_assign_inter;

  TFlt m_estimation_step_size;
  TFlt m_optimization_step_size;

  std::unordered_map<TInt, TFlt> m_average_link_tt;
  std::unordered_map<TInt, TFlt> m_link_tt_difference; //only used in estimation
  std::string m_file_folder;
  PNEGraph m_graph;
  MNM_OD_Factory *m_od_factory;
  MNM_ConfReader* m_dta_config;
  MNM_ConfReader* m_realtime_dta_config;
  MNM_Dta_Screenshot *m_before_shot;
  MNM_Dta_Screenshot *m_after_shot;
  MNM_Vms_Factory *m_vms_factory;
  Path_Table *m_path_table;
};

class MNM_Dta_Screenshot
{
public:
  MNM_Dta_Screenshot(std::string file_folder, MNM_ConfReader* config, PNEGraph graph, MNM_OD_Factory *od_factory);
  ~MNM_Dta_Screenshot();
  int build_static_network();
  int hook_up_node_and_link();
  PNEGraph m_graph;
  std::string m_file_folder;
  MNM_ConfReader* m_config;
  MNM_OD_Factory *m_od_factory;
  MNM_Link_Factory *m_link_factory;
  MNM_Node_Factory *m_node_factory;
  MNM_Veh_Factory *m_veh_factory;
  MNM_Routing_Fixed *m_routing;
};

namespace MNM
{
  int run_from_screenshot(MNM_Dta_Screenshot* screenshot, MNM_ConfReader data_config,
                          TInt max_inter, TInt assign_inter, Path_Table *path_table,
                          MNM_OD_Factory *od_factory, std::string file_folder);
  MNM_Dta_Screenshot *make_screenshot(std::string file_folder, MNM_ConfReader* config, MNM_OD_Factory *od_factory,
                                  MNM_Link_Factory *link_factory, MNM_Node_Factory *node_factory, PNEGraph graph, 
                                  MNM_Routing_Fixed *old_routing);
  MNM_Dta_Screenshot *make_screenshot(MNM_Dta_Screenshot* screenshot);
  MNM_Dta_Screenshot *make_empty_screenshot(std::string file_folder, MNM_ConfReader* config, 
                                                    MNM_OD_Factory *od_factory, PNEGraph graph);

  int update_path_p(Path_Table *path_table, TInt col, TFlt step_size);
}


void static inline copy_veh(MNM_Veh* _veh, MNM_Veh *_new_veh){
  _new_veh -> m_current_link = _veh -> m_current_link;
  _new_veh -> m_start_time = _veh -> m_start_time;
  _new_veh -> m_next_link = _veh -> m_next_link;
  _new_veh -> set_destination(_veh -> get_destination());
  _new_veh -> set_origin(_veh -> get_origin());
};
#endif