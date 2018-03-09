#include "due.h"

#include <string>

int main()
{
  std::string file_folder = "../../data/input_files_new_philly";
  MNM_Due due = MNM_Due(file_folder); 
  printf("Finished\n");
  return 0;
}