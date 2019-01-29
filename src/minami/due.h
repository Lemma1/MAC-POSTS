#ifndef DUE_H
#define DUE_H

#include "path.h"
#include "dta.h"
#include "factory.h"
#include "realtime_dta.h"

class MNM_Due
{
public:
  MNM_Due(std::string file_folder);
  ~MNM_Due();

  int virtual initialize(){return 0;};
  MNM_Dta* run_dta(bool verbose);

  int virtual init_path_flow(){return 0;};
  int virtual update_path_table(MNM_Dta *dta, int iter){return 0;};
  TFlt compute_merit_function();
  TFlt get_disutility(TFlt depart_time, TFlt tt);
  TFlt get_tt(TFlt depart_time, MNM_Path* path);
  int build_cost_map(MNM_Dta *dta);
  int update_demand_from_path_table(MNM_Dta* dta);
  TFlt compute_total_demand(MNM_Origin *orig, MNM_Destination* dest, TInt total_assign_inter);
  std::string m_file_folder;
  TFlt m_unit_time;
  TInt m_total_loading_inter;
  Path_Table* m_path_table;
  // MNM_OD_Factory *m_od_factory;
  TInt m_total_assign_inter;
  MNM_ConfReader *m_dta_config;
  MNM_ConfReader *m_due_config;

  TFlt m_early_rate;
  TFlt m_late_rate;
  TFlt m_target_time;
  TFlt m_step_size;

  
  std::unordered_map<TInt, TFlt*> m_cost_map;

};

class MNM_Due_Msa : public MNM_Due
{
public:
  MNM_Due_Msa(std::string file_folder);
  ~MNM_Due_Msa();
  int virtual initialize() override;
  int virtual init_path_flow() override;
  int virtual update_path_table(MNM_Dta *dta, int iter) override;
  std::pair<MNM_Path*, TInt> get_best_route(TInt o_node_ID,
                                          MNM_TDSP_Tree* tdsp_tree);
  MNM_Dta* m_base_dta;
};

#endif