#include "due.h"
#include "Snap.h"

#include <string>

int main()
{
  std::string file_folder = "../../data/input_files_2link_due";
  // std::string file_folder = "../../data/input_files_new_philly";
  MNM_Due* due = new MNM_Due_Msa(file_folder); 
  due -> initialize();
  due -> init_route_choice();
  for (int i=0; i<10; ++i){
    printf("---------- Iteration %d ----------\n", i);
    MNM_Dta* dta = due -> run_dta(false);
    due -> update_path_table(dta, i);
    printf("%lf\n", due -> compute_merit_function());
    // delete dta;
  }
  

  printf("Finished\n");
  return 0;
}