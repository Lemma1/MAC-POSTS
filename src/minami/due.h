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
  int run_dta(bool verbose);

  int virtual init_route_choice(){return 0;};
  int virtual update_path_table(MNM_Dta *dta){return 0;};
  std::string m_file_folder;
  TFlt m_unit_time;
  TInt m_total_loading_inter;
  Path_Table* m_path_table;
  MNM_OD_Factory *m_od_factory;
  TInt m_total_assign_inter;
  MNM_ConfReader *m_dta_config;
  MNM_ConfReader *m_due_config;

  TFlt m_early_rate;
  TFlt m_late_rate;
  TFlt m_target_time;


};

class MNM_Due_Msa : public MNM_Due
{
public:
  MNM_Due_Msa(std::string file_folder);
  ~MNM_Due_Msa();
  int virtual initialize() override;
  int virtual init_route_choice() override;
  int virtual update_path_table(MNM_Dta *dta) override;
  std::pair<MNM_Path*, TFlt> get_best_route(TInt o_node_ID,
                                          MNM_TDSP_Tree* tdsp_tree);
  int build_cost_map(MNM_Dta *dta);
  TFlt get_disutility(TFlt depart_time, TFlt tt);
  std::unordered_map<TInt, TFlt*> m_cost_map;
};

#endif