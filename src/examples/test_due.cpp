#include "due.h"
#include "Snap.h"

#include <string>

int main()
{
  std::string file_folder = "../../data/input_files_2link_due";
  // std::string file_folder = "../../data/input_files_new_philly";
  MNM_Due due = MNM_Due(file_folder); 
  printf("Finished\n");
  return 0;
}