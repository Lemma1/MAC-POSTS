#include "Snap.h"

#include "path.h"
#include "io.h"
#include "dlink.h"
#include "shortest_path.h"
#include "multiclass.h"

int main()
{
  std::string m_file_folder = "../../data/input_files_6by6grid_multiclass";
  // std::string m_file_folder = "../../data/input_files_7link_fix";
  // std::string m_file_folder = "../../data/input_files_philly";
  // std::string m_file_folder = "../../data/input_files_SR41_fix";
  // std::string m_file_folder = "../../data/input_files_MckeesRocks_SPC";
  // std::string m_file_folder = "/home/lemma/Documents/truck/Final data for DPFE/tmp_truck";


  MNM_ConfReader *m_config;
  MNM_Node_Factory *m_node_factory;
  MNM_Link_Factory *m_link_factory;
  MNM_OD_Factory *m_od_factory;
  PNEGraph m_graph;
  printf("1\n");
  m_node_factory = new MNM_Node_Factory_Multiclass();
  m_link_factory = new MNM_Link_Factory_Multiclass();
  m_od_factory = new MNM_OD_Factory_Multiclass();
  m_config = new MNM_ConfReader(m_file_folder + "/config.conf", "DTA");

  MNM_IO_Multiclass::build_node_factory_multiclass(m_file_folder, m_config, m_node_factory);
  MNM_IO_Multiclass::build_link_factory_multiclass(m_file_folder, m_config, m_link_factory);
  MNM_IO_Multiclass::build_od_factory(m_file_folder, m_config, m_od_factory, m_node_factory);
  m_graph = MNM_IO_Multiclass::build_graph(m_file_folder, m_config);
  printf("3\n");
  // std::map<TInt, TFlt> cost_map = std::map<TInt, TFlt>();
  // for (auto _it = m_link_factory -> m_link_map.begin(); _it != m_link_factory -> m_link_map.end(); ++_it){
  //   cost_map.insert(std::pair<TInt, TFlt>(_it -> first, TFlt(1)));
  // }

  // std::map<TInt, TInt> output_map = std::map<TInt, TInt>();
  // MNM_Shortest_Path::all_to_one_Dijkstra(6, m_graph, cost_map, output_map);
  // MNM_Path *p = MNM::extract_path(1, 6, output_map, m_graph);

  // printf("node:%d, link:%d\n", p -> m_node_vec.size(),p -> m_link_vec.size());

  Path_Table *path_table = MNM::build_pathset(m_graph, m_od_factory, m_link_factory);
  printf("Finish running\n");
  // MNM_Path *p;
  // for (size_t i = 0; i< path_table -> find(1) -> second -> find(5) -> second -> m_path_vec.size(); ++i){
  //   p = path_table -> find(1) -> second -> find(5) -> second -> m_path_vec[i];
  //   printf("node:%d, link:%d\n", p -> m_node_vec.size(),p -> m_link_vec.size());    
  // }

  MNM::save_path_table(path_table, m_od_factory, true);

  printf("Finish saving\n");
  // Path_Table *path_table;
  // path_table = MNM_IO::load_path_table("path_table", m_graph, 189065);

  
  // for (size_t i = 0; i< path_table -> find(1) -> second -> find(5) -> second -> m_path_vec.size(); ++i){
  //   p = path_table -> find(1) -> second -> find(5) -> second -> m_path_vec[i];
  //   printf("node:%d, link:%d\n", p -> m_node_vec.size(),p -> m_link_vec.size());    
  // }

  return 0;
}