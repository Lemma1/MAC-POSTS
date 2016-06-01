#include "dta.h"

MNM_Dta::MNM_Dta(std::string file_folder)
{
  m_file_folder = file_folder;
  initialize();
}

int MNM_Dta::initialize()
{
  m_veh_factory = new MNM_Veh_Factory();
  m_node_factory = new MNM_Node_Factory();
  m_link_factory = new MNM_Link_Factory();
  m_od_factory = new MNM_OD_Factory();
  m_config = new MNM_ConfReader(m_file_folder + "/config.conf", "DTA");
  m_unit_time = m_config -> get_int("unit_time");
  m_flow_scalar = m_config -> get_int("flow_scalar");
  return 0;
}




int main()
{
  MNM_Dta *test_dta = new MNM_Dta("input_files");
  MNM_IO::build_node_factory(test_dta -> m_file_folder, test_dta->m_config, test_dta->m_node_factory);
  std::cout << test_dta -> m_node_factory -> m_node_map.size() << "\n";
  return 0;
}