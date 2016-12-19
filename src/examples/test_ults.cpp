#include "Snap.h"

#include "ults.h"

int main()
{

  std::string s = "../../data/input_files_new_philly/1";
  MNM_Ults::copy_file( s.c_str(), "../../data/input_files_new_philly/777" );

  return 0;
}