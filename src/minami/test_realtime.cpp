#include "Snap.h"

#include "path.h"
#include "io.h"
#include "dlink.h"
#include "shortest_path.h"
#include "realtime_dta.h"

int main()
{
  std::string m_file_folder = "../../data/input_files_2link";
  // std::string m_file_folder = "../../data/input_files_philly";
  // MNM_ConfReader *m_config;
  // MNM_Node_Factory *m_node_factory;
  // MNM_Link_Factory *m_link_factory;
  // MNM_OD_Factory *m_od_factory;
  // PNEGraph m_graph;

  // m_node_factory = new MNM_Node_Factory();
  // m_link_factory = new MNM_Link_Factory();
  // m_od_factory = new MNM_OD_Factory();
  // m_config = new MNM_ConfReader(m_file_folder + "/config.conf", "DTA");

  // MNM_IO::build_node_factory(m_file_folder, m_config, m_node_factory);
  // MNM_IO::build_link_factory(m_file_folder, m_config, m_link_factory);
  // MNM_IO::build_od_factory(m_file_folder, m_config, m_od_factory, m_node_factory);
  // m_graph = MNM_IO::build_graph(m_file_folder, m_config);

  // std::map<TInt, TFlt> cost_map = std::map<TInt, TFlt>();
  // for (auto _it = m_link_factory -> m_link_map.begin(); _it != m_link_factory -> m_link_map.end(); ++_it){
  //   cost_map.insert(std::pair<TInt, TFlt>(_it -> first, TFlt(1)));
  // }

  // std::map<TInt, TInt> output_map = std::map<TInt, TInt>();
  // MNM_Shortest_Path::all_to_one_Dijkstra(6, m_graph, cost_map, output_map);
  // MNM_Path *p = MNM::extract_path(1, 6, output_map, m_graph);

  // printf("node:%d, link:%d\n", p -> m_node_vec.size(),p -> m_link_vec.size());

  // Path_Table *path_table = MNM::build_pathset(m_graph, m_od_factory, m_link_factory);

  // MNM_Path *p;
  // for (size_t i = 0; i< path_table -> find(1) -> second -> find(5) -> second -> m_path_vec.size(); ++i){
  //   p = path_table -> find(1) -> second -> find(5) -> second -> m_path_vec[i];
  //   printf("node:%d, link:%d\n", p -> m_node_vec.size(),p -> m_link_vec.size());    
  // }

  // MNM::save_path_table(path_table, m_od_factory);



  // path_table = MNM_IO::load_path_table("path_table", m_graph);

  
  // for (size_t i = 0; i< path_table -> find(1) -> second -> find(5) -> second -> m_path_vec.size(); ++i){
  //   p = path_table -> find(1) -> second -> find(5) -> second -> m_path_vec[i];
  //   printf("node:%d, link:%d\n", p -> m_node_vec.size(),p -> m_link_vec.size());    
  // // }
  // MNM_Dta_Screenshot *s;
  // s = MNM::make_screenshot(m_file_folder, m_config, m_link_factory);

  MNM_Realtime_Dta *d = new MNM_Realtime_Dta(m_file_folder);
  
  return 0;
}