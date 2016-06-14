#ifndef STATISTICS_H
#define STATISTICS_H

#include "Snap.h"
#include "dnode.h"
#include "dlink.h"
#include "factory.h"
#include "enum.h"

#include <string>
#include <iostream>
#include <fstream>

class MNM_Statistics
{
public:
  MNM_Statistics(std::string file_folder, MNM_ConfReader *conf_reader, MNM_ConfReader *record_config, 
                  MNM_OD_Factory *od_factory, MNM_Node_Factory *node_factory, MNM_Link_Factory *link_factory);
  ~MNM_Statistics();

  /* may or may not be initialized */
  std::map<TInt, TFlt> m_load_interval_volume;
  std::map<TInt, TFlt> m_record_interval_volume;
  std::map<TInt, TFlt> m_record_interval_tt;
  std::map<TInt, TFlt> m_load_interval_tt;

  /* universal function */
  int record_loading_interval_condition(TInt timestamp);
  int record_record_interval_condition(TInt timestamp);
  int virtual update_record(TInt timestamp){return 0;};
  int virtual init_record();
  int virtual post_record();
protected:
  int init_record_value();
  bool m_record_volume;
  bool m_record_tt;
  std::string m_file_folder;
  Record_type m_record_type;
  MNM_ConfReader *m_self_config;
  MNM_ConfReader *m_global_config;
  MNM_OD_Factory *m_od_factory;
  MNM_Node_Factory *m_node_factory;
  MNM_Link_Factory *m_link_factory;

  std::ofstream m_load_interval_volume_file;
  std::ofstream m_record_interval_volume_file;
  std::ofstream m_load_interval_tt_file;
  std::ofstream m_record_interval_tt_file;

};


class MNM_Statistics_Lrn : public MNM_Statistics
{
public:
  MNM_Statistics_Lrn(std::string file_folder, MNM_ConfReader *conf_reader, MNM_ConfReader *record_config,
                  MNM_OD_Factory *od_factory, MNM_Node_Factory *node_factory, MNM_Link_Factory *link_factory);
  ~MNM_Statistics_Lrn();
  int virtual update_record(TInt timestamp);
  int virtual init_record();
  int virtual post_record();
private:
  TInt m_n;
  std::map<TInt, TFlt> m_to_be_volume;
  std::map<TInt, TFlt> m_to_be_tt;
};
#endif