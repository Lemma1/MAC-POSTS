#include "dta.h"
#include "emission.h"


int main(){

  TInt freq = 12;


  MNM_Dta *test_dta = new MNM_Dta("../../data/input_files_7link");
  // MNM_Dta *test_dta = new MNM_Dta("../../data/input_files_1link");
  // MNM_Dta *test_dta = new MNM_Dta("../../data/input_files_PGH");
  test_dta -> build_from_files();

  MNM_Cumulative_Emission e = MNM_Cumulative_Emission(test_dta -> m_unit_time, freq);
  e.register_link(test_dta -> m_link_factory -> get_link(2));
  e.register_link(test_dta -> m_link_factory -> get_link(3));
  e.register_link(test_dta -> m_link_factory -> get_link(4));
  e.register_link(test_dta -> m_link_factory -> get_link(5));
  e.register_link(test_dta -> m_link_factory -> get_link(6));
  printf("Hooking......\n");
  test_dta -> hook_up_node_and_link();

  TInt _cur_int = 0;
  TInt _assign_inter = test_dta -> m_start_assign_interval; 
  test_dta -> pre_loading();
  printf("Start loading!\n");
  while (!test_dta -> finished_loading(_cur_int)){
    test_dta -> load_once(false, _cur_int, _assign_inter);
    if (_cur_int % test_dta -> m_assign_freq == 0 || _cur_int==0){
      _assign_inter += 1;
    }
    _cur_int += 1;
    e.update();
  }
  return 0;
}