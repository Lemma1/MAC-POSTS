#include "dta.h"

int main()
{
  MNM_Dta *test_dta = new MNM_Dta(".");
  test_dta -> build_from_files();
  test_dta -> hook_up_node_and_link();
  test_dta -> loading(true);

  return 0;
}