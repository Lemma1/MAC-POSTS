#include "dta.h"
#include "emission.h"
#include "io.h"
#include "dta_gradient_utls.h"
#include "dlink.h"


#include <vector>

int main(){

  // std::string folder = "../../data/input_files_PGH_emission";
  std::string folder = "../../data/input_files_2link_fix";
  MNM_Dta *test_dta = new MNM_Dta(folder);
  // MNM_Dta *test_dta = new MNM_Dta("../../data/input_files_2link_fix");
  // MNM_Dta *test_dta = new MNM_Dta("../../data/input_files_PGH");
  printf("DTA set\n");
  test_dta -> build_from_files();

  printf("Finished init\n");
  MNM_Path *p;
  // Path_Table *path_table  = ((MNM_Routing_Fixed*) (test_dta -> m_routing)) -> m_path_table;
  // printf("path set length %d\n", path_table -> find(1) -> second -> find(6) -> second -> m_path_vec.size());
  // for (size_t i = 0; i< path_table -> find(1) -> second -> find(6) -> second -> m_path_vec.size(); ++i){
  //   p = path_table -> find(1) -> second -> find(6) -> second -> m_path_vec[i];
  //   printf("node:%d, link:%d\n", p -> m_node_vec.size(), p -> m_link_vec.size());
  //   printf("path buffer %f\n", p -> m_buffer[9]);
  // }

  // for(auto _it : *path_table){
  //   printf("11\n");
  //   for (auto _it_it : *(_it.second)){
  //     printf("22\n");
  //     for (MNM_Path* _path : _it_it.second -> m_path_vec){
  //       printf("current buffer is %f\n", _path -> m_buffer[0]());
  //     }
  //   }
  // }


  // std::vector<TInt> v;
  // std::vector<MNM_Dlink*> v2;
  // double **c = new double[4][5];
  // MNM_DTA_GRADIENT::get_link_inflow(c, v2, v);

  test_dta -> hook_up_node_and_link();
  MNM_Dlink *_link;
  for (auto _link_it = test_dta -> m_link_factory -> m_link_map.begin(); _link_it != test_dta -> m_link_factory -> m_link_map.end(); _link_it++){
    _link = _link_it -> second;
    _link -> install_cumulative_curve();
  }

  TInt _cur_int = 0;
  TInt _assign_inter = test_dta -> m_start_assign_interval; 
  test_dta -> pre_loading();
  printf("Start loading!\n");
  while (!test_dta -> finished_loading(_cur_int)){
    printf("Current interval %d\n", _cur_int());
    test_dta -> load_once(true, _cur_int, _assign_inter);
    if (_cur_int % test_dta -> m_assign_freq == 0 || _cur_int==0){
      _assign_inter += 1;
    }
    _cur_int += 1;
  }


  
  TFlt _flow;
  for (int i =0; i< _cur_int() - 1; ++i){
    printf("1\n");
    for (auto _link_it = test_dta -> m_link_factory -> m_link_map.begin(); _link_it != test_dta -> m_link_factory -> m_link_map.end(); _link_it++){
      _link = _link_it -> second;
      printf("2\n");
      _flow = MNM_DTA_GRADIENT::get_link_inflow(_link, TFlt(i), TFlt(i+1));
      printf("Start time %d, end time %d, link ID %d, flow %f\n", i, i+1, _link -> m_link_ID(), _flow());
      // _link -> print_info();
    }
  }


  return 0;
}