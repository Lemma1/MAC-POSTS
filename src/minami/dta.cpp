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


int MNM_Dta::build_from_files()
{
  MNM_IO::build_node_factory(m_file_folder, m_config, m_node_factory);
  // std::cout << test_dta -> m_node_factory -> m_node_map.size() << "\n";
  MNM_IO::build_link_factory(m_file_folder, m_config, m_link_factory);
  // std::cout << test_dta -> m_link_factory -> m_link_map.size() << "\n";
  MNM_IO::build_od_factory(m_file_folder, m_config, m_od_factory, m_node_factory);
  // std::cout << test_dta -> m_od_factory -> m_origin_map.size() << "\n";
  // std::cout << test_dta -> m_od_factory -> m_destination_map.size() << "\n";
  m_graph = MNM_IO::build_graph(m_file_folder, m_config);
  MNM_IO::build_demand(m_file_folder, m_config, m_od_factory);
  return 0;  
}

int MNM_Dta:: hook_up_odANDnode()
{

  return 0;
}

int main()
{
  MNM_Dta *test_dta = new MNM_Dta("input_files");
  test_dta -> build_from_files();
  // MNM_IO::build_node_factory(test_dta -> m_file_folder, test_dta->m_config, test_dta->m_node_factory);
  // std::cout << test_dta -> m_node_factory -> m_node_map.size() << "\n";
  // MNM_IO::build_link_factory(test_dta -> m_file_folder, test_dta->m_config, test_dta->m_link_factory);
  // std::cout << test_dta -> m_link_factory -> m_link_map.size() << "\n";
  // MNM_IO::build_od_factory(test_dta -> m_file_folder, test_dta->m_config, test_dta->m_od_factory, test_dta->m_node_factory);
  // std::cout << test_dta -> m_od_factory -> m_origin_map.size() << "\n";
  // std::cout << test_dta -> m_od_factory -> m_destination_map.size() << "\n";
  // test_dta -> m_graph = MNM_IO::build_graph(test_dta -> m_file_folder,test_dta -> m_config);
  // test_dta -> m_graph  -> Dump();
  return 0;
}