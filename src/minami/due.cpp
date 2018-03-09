#include "due.h"

#include "realtime_dta.h"

MNM_Due::MNM_Due(std::string file_folder)
{
  m_file_folder = file_folder;
  // m_node_factory = new MNM_Node_Factory();
  // m_link_factory = new MNM_Link_Factory();
  m_od_factory = new MNM_OD_Factory();
  m_dta_config = new MNM_ConfReader(m_file_folder + "/config.conf", "DTA");
  m_due_config = new MNM_ConfReader(m_file_folder + "/config.conf", "DUE");
  m_total_assign_inter = m_dta_config -> get_int("max_interval");
}

// int MNM_Due::init_path_table()
// {
//   std::string _file_name = m_due_conf -> get_string("path_file_name");
//   std::string _whole_path = m_file_folder + "/" + _file_name;
//   TInt _num_path = m_due_conf -> get_int("num_path");
//   m_path_table = MNM_IO::load_path_table(_whole_path, m_graph, _num_path);
//   return 0;
// }




MNM_Due::~MNM_Due()
{
  if (m_node_factory != nullptr) delete m_node_factory;
  if (m_link_factory != nullptr) delete m_link_factory;
  if (m_od_factory != nullptr) delete m_od_factory;
  if (m_dta_config != nullptr) delete m_dta_config;
  if (m_due_config != nullptr) delete m_due_config;

}

int MNM_Due::init_from_files()
{
  m_graph = MNM_IO::build_graph(m_file_folder, m_dta_config);
  MNM_IO::build_od_factory(m_file_folder, m_dta_config, m_od_factory);
  return 0;
}

int MNM_Due::run_once()
{
  MNM_Dta_Screenshot *_shot = MNM::make_empty_screenshot(m_file_folder, m_dta_config, 
                                                  m_od_factory, m_graph);
  MNM::run_from_screenshot(_shot, m_dta_config, m_total_assign_inter, TInt(0), m_path_table,
                            m_od_factory, m_file_folder);
  return 0;
}
