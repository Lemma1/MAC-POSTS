#include "Snap.h"

#include "path.h"
#include "io.h"
#include "dlink.h"
#include "shortest_path.h"
#include "realtime_dta.h"
#include "vms.h"

int main()
{
  // std::string m_file_folder = "../../data/input_files_2link";
  // std::string m_file_folder = "../../data/input_files_philly";
  std::string m_file_folder = "../../data/input_files_new_philly_dta";

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

  // printf("0\n");
  // Path_Table *path_table = MNM_IO::load_path_table("../../data/input_files_philly/path_table", m_graph, 189065);

  // Path_Table *path_table = MNM_IO::load_path_table("../../data/input_files_2link/path_table", m_graph, 2);

  
  // for (size_t i = 0; i< path_table -> find(1) -> second -> find(5) -> second -> m_path_vec.size(); ++i){
  //   p = path_table -> find(1) -> second -> find(5) -> second -> m_path_vec[i];
  //   printf("node:%d, link:%d\n", p -> m_node_vec.size(),p -> m_link_vec.size());    
  // // }
  // MNM_Dta_Screenshot *s;
  // s = MNM::make_screenshot(m_file_folder, m_config, m_link_factory);

  // MNM_Realtime_Dta *d = new MNM_Realtime_Dta(m_file_folder);

  // for(auto _it : *(d -> m_path_table)){
  //   for (auto _it_it : *(_it.second)){
  //     _it_it.second -> normalize_p();
  //   }
  // }
  // for (int i = 0; i < 1; ++i)
  // {
  //   printf("%d\n", i);
  //    d -> run_from_screenshot(d -> m_before_shot, 20, 0, d -> m_path_table);
  //    // delete d -> m_after_shot;
  //    // d -> m_after_shot = MNM::make_screenshot(d -> m_before_shot);
  //    // d -> run_from_screenshot(d -> m_after_shot, 180, -1, d -> m_path_table);
  //    // printf("delete before\n");
  //    // delete d -> m_before_shot;
  //    // printf("make before\n");
  //    // d -> m_before_shot = MNM::make_screenshot(d -> m_after_shot);

  // }

  // d -> run_from_screenshot(d -> m_before_shot, 10, 0, d -> m_path_table);

  // delete d -> m_after_shot;
  // d -> m_after_shot = MNM::make_screenshot(d -> m_before_shot);

  // // printf("___________________________________________Finish!\n");
  // d -> run_from_screenshot(d -> m_after_shot, 1, 0, d -> m_path_table);


  // MNM_Vms_Factory *f = new MNM_Vms_Factory();
  // f -> make_link_vms(TInt(0), 1, m_graph);
  // delete f;
  // MNM_Link_Vms* v = new MNM_Link_Vms(TInt(0), 1, m_graph);
  // v -> hook_path(path_table);
  // printf("Finished!\n");


  // MNM_Vms_Factory *f = new MNM_Vms_Factory();
  // // printf("1\n");
  // MNM_IO::build_vms_facotory(m_file_folder, m_graph, 1 , f);
  // // printf("2\n");
  // f -> hook_path(path_table);
// 
  // std::map<TInt, TFlt> link_spd_map = std::map<TInt, TFlt>();
  // MNM_IO::read_int_float("../../data/input_files_2link/MNM_input_spd", &link_spd_map);
  // for (auto _it : link_spd_map){
  //   printf("Link %d has spd %.4f\n", _it.first, _it.second);
  // }
  
  MNM_Realtime_Dta* r = new MNM_Realtime_Dta(m_file_folder);
  printf("1\n");
  r -> init_running();
  printf("2\n");
  // r -> estimate_previous(0);
  // r -> optimize_next(0);
  r -> one_iteration(0);

  return 0;
}