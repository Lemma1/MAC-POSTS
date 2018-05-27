#include "io.h"
#include "dta.h"

#include <vector>

int main()
{
	// MNM_Dlink *_link;

	printf("BEGIN sigle class DTA test!\n");

	std::string folder = "/home/alanpi/Desktop/MAC-POSTS/data/input_files_MckeesRocks_SPC";
	// std::string folder = "/home/alanpi/Desktop/MAC-POSTS/data/input_files_SR41";
	MNM_Dta *test_dta = new MNM_Dta(folder);
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
	bool _verbose = false;
	bool output_veh_locs = false; // if true output veh location every vis_frequency
	TInt vis_frequency = 60; // 5 minutes
	std::ofstream _vis_file;
	std::string _str;
	if (output_veh_locs){
		_vis_file.open(folder + "/veh_loc/veh_loc_raw.txt", std::ofstream::out); 
		if (! _vis_file.is_open()){
        	printf("Error happens when open _vis_file\n");
        	exit(-1);
        }
	}

	while (!test_dta -> finished_loading(_current_inter)){
		printf("Current interval: %d\n", _current_inter());
		test_dta -> load_once(_verbose, _current_inter, _assign_inter);
		if (_current_inter % test_dta -> m_assign_freq == 0 || _current_inter == 0){
			_assign_inter += 1;
		}
		if (output_veh_locs && (_current_inter % vis_frequency == 0)){
			for (auto _map_it : test_dta -> m_veh_factory -> m_veh_map){
				if (_map_it.second -> m_finish_time < 0) {
					MNM_Veh* _veh = _map_it.second;
					_str = "Car ";
					_str += std::to_string(_current_inter) + " ";
					_str += std::to_string(_veh -> get_current_link() -> m_link_ID);
					_str += "\n";
					_vis_file << _str;
				}
			}
		}
		_current_inter += 1;
	}

	if (output_veh_locs){
		if (_vis_file.is_open()) _vis_file.close();
	}

	return 0;
}