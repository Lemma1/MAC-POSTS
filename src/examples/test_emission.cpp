#include "dta.h"
#include "emission.h"
#include "io.h"

#include <vector>

int main(){

  TInt freq = 12;

  std::string folder = "../../data/input_files_PGH_emission";
  MNM_Dta *test_dta = new MNM_Dta(folder);
  // MNM_Dta *test_dta = new MNM_Dta("../../data/input_files_1link");
  // MNM_Dta *test_dta = new MNM_Dta("../../data/input_files_PGH");
  test_dta -> build_from_files();

  printf("register_link......\n");
  MNM_Cumulative_Emission e = MNM_Cumulative_Emission(test_dta -> m_unit_time, freq);
  std::vector<TInt> emission_list = std::vector<TInt>();
  MNM_IO::read_int(folder + "/" + "emission_list.txt", &emission_list);
  printf("The observed link number id %d\n", (int)emission_list.size());
  for (TInt link_ID : emission_list){
    if (test_dta -> m_link_factory -> m_link_map.find(link_ID) != test_dta -> m_link_factory -> m_link_map.end())
    e.register_link(test_dta -> m_link_factory -> get_link(link_ID));
  }
  printf("The register link number id %d\n", (int)e.m_link_vector.size());
  printf("Hooking......\n");
  test_dta -> hook_up_node_and_link();

  TInt _cur_int = 0;
  TInt _assign_inter = test_dta -> m_start_assign_interval; 
  test_dta -> pre_loading();
  printf("Start loading!\n");
  while (!test_dta -> finished_loading(_cur_int)){
    test_dta -> load_once(true, _cur_int, _assign_inter);
    if (_cur_int % test_dta -> m_assign_freq == 0 || _cur_int==0){
      _assign_inter += 1;
    }
    _cur_int += 1;
    e.update();
  }
  return 0;
}