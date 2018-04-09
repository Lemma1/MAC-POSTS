#include "io.h"
#include "multiclass.h"

#include <vector>

int main()
{
	printf("BEGIN multiclass test!\n");

	std::string folder = "/home/alanpi/Desktop/MAC-POSTS/data/input_files_7link_multiclass";
	MNM_Dta_Multiclass *test_dta = new MNM_Dta_Multiclass(folder);
	printf("================================ DTA set! =================================\n");
	
	test_dta -> build_from_files();
	printf("========================= Finished initialization! ========================\n");

	test_dta -> hook_up_node_and_link();
	printf("====================== Finished node and link hook-up! ====================\n");

	test_dta -> is_ok();
	printf("============================ DTA is OK to run! ============================\n");

	TInt _current_inter = 0;
	TInt _assign_inter = test_dta -> m_start_assign_interval;
	test_dta -> pre_loading();
	printf("========================== Finished pre_loading! ==========================\n");

	printf("\n\n\n====================================== Start loading! =======================================\n");
	while (!test_dta -> finished_loading(_current_inter)){
		printf("Current interval: %d\n", _current_inter());
		test_dta -> load_once(true, _current_inter, _assign_inter);
		if (_current_inter % test_dta -> m_assign_freq == 0 || _current_inter == 0){
			_assign_inter += 1;
		}
		_current_inter += 1;
		// if (_current_inter > 22){ break; };
	}

	return 0;
}