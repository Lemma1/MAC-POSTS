#include "io.h"
#include "multiclass.h"
#include "Snap.h"

#include <vector>

int main()
{
	MNM_Dlink *_link;
	MNM_Dlink_Multiclass *_link_m;

	printf("BEGIN multiclass test!\n");

	// On ubuntu (PC)
	std::string folder = "/home/alanpi/Desktop/MAC-POSTS/data/input_files_MckeesRocks_SPC";
	// std::string folder = "/home/lemma/Documents/MAC-POSTS/src/examples/mcDODE/a6e7b31067d2ead8d3725fc0ed587d06c958f63c";
	// std::string folder = "/home/alanpi/Desktop/MAC-POSTS/data/input_files_7link_multiclass";

	// on macOS (Mac air)
	// std::string folder = "/Users/alan-air/Dropbox/MAC-POSTS/data/input_files_MckeesRocks_SPC";
	// std::string folder = "/Users/alan-air/Dropbox/MAC-POSTS/data/input_files_7link_multiclass";


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
	bool _verbose = false;
	bool output_link_cong = false; // if true output link congestion level every cong_frequency
	TInt cong_frequency = 180; // 15 minutes
	bool output_veh_locs = false; // if true output veh location every vis_frequency
	TInt vis_frequency = 60; // 5 minutes
	MNM_Veh_Multiclass* _veh;
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
					_veh = dynamic_cast<MNM_Veh_Multiclass *>(_map_it.second);
					if (_veh -> m_class == 0){
						_str = "Car ";
					}
					else {
						_str = "Truck ";
					}
					_str += std::to_string(_current_inter) + " ";
					_str += std::to_string(_veh -> get_current_link() -> m_link_ID) + " ";
					_str += std::to_string(_veh -> m_visual_position_on_link);
					_str += "\n";
					_vis_file << _str;
				}
			}
		}
		_current_inter += 1;
		if (_current_inter > 600) break;
	}

	// Output total travels and travel time, before divided by flow_scalar
	TInt _count_car = 0, _count_truck = 0;
	TFlt _tot_tt = 0.0;
	for (auto _map_it : test_dta -> m_veh_factory -> m_veh_map){
		if (_map_it.second -> m_finish_time > 0) {
			_veh = dynamic_cast<MNM_Veh_Multiclass *>(_map_it.second);
			if (_veh -> m_class == 0){
				_count_car += 1;
			}
			else {
				_count_truck += 1;
			}
			_tot_tt += (_veh -> m_finish_time - _veh -> m_start_time) * 5.0 / 3600.0;
		}
	}
	printf("\n\n\nTotal car: %d, Total truck: %d, Total tt: %.2f hours\n\n\n\n", int(_count_car), int(_count_truck), float(_tot_tt));



	if (output_veh_locs){
		if (_vis_file.is_open()) _vis_file.close();
	}


	std::ofstream _vis_file2;
	if (output_link_cong){
		_vis_file2.open(folder + "/link_cong/link_cong_raw.txt", std::ofstream::out); 
		if (! _vis_file2.is_open()){
        	printf("Error happens when open _vis_file2\n");
        	exit(-1);
        }
		TInt _iter = 0;
		while (_iter < _current_inter){
			if (_iter % cong_frequency == 0){
				printf("Current iteration: %d\n", int(_iter));
				for (auto _link_it = test_dta -> m_link_factory -> m_link_map.begin(); 
						  _link_it != test_dta -> m_link_factory -> m_link_map.end(); _link_it++){
					_link = _link_it -> second;
					_link_m = dynamic_cast<MNM_Dlink_Multiclass*>(_link);
					_str = std::to_string(_link -> m_link_ID()) + " ";
					_str += std::to_string(_iter) + " ";
					_str += std::to_string(MNM_DTA_GRADIENT::get_link_inflow_car(_link_m, _iter, _iter + 1)) + " ";
					_str += std::to_string(MNM_DTA_GRADIENT::get_link_inflow_truck(_link_m, _iter, _iter + 1)) + " ";
					_str += std::to_string(MNM_DTA_GRADIENT::get_travel_time_car(_link_m, TFlt(_iter + 1))) + " ";
					_str += std::to_string(MNM_DTA_GRADIENT::get_travel_time_truck(_link_m, TFlt(_iter + 1))) + " ";
					_str += std::to_string(_link_m -> get_link_freeflow_tt_car()) + " ";
					_str += std::to_string(_link_m -> get_link_freeflow_tt_truck()) + " ";
					_str += std::to_string(_link_m -> m_length/MNM_DTA_GRADIENT::get_travel_time_car(_link_m, TFlt(_iter + 1))*3600/1600) + " ";
					_str += std::to_string(_link_m -> m_length/MNM_DTA_GRADIENT::get_travel_time_truck(_link_m, TFlt(_iter + 1))*3600/1600) + "\n";
					_vis_file2 << _str;
				}
			}
			_iter += 1;
		}
		if (_vis_file2.is_open()) _vis_file2.close();
	}
	
	// output tt of some special links
	for (auto _link_it = test_dta -> m_link_factory -> m_link_map.begin(); _link_it != test_dta -> m_link_factory -> m_link_map.end(); _link_it++){
			_link = _link_it -> second;
			if (_link -> m_link_ID() == 7186) {
				TInt _iter = 0;
				while (_iter < _current_inter){
					// if (_iter == 984){
						_link_m = dynamic_cast<MNM_Dlink_Multiclass*>(_link);
						printf("%d,%.2f,%.2f\n", int(_iter),
							double(MNM_DTA_GRADIENT::get_travel_time_car(_link_m, TFlt(_iter + 1))), 
							double(MNM_DTA_GRADIENT::get_travel_time_truck(_link_m, TFlt(_iter + 1))));
					// }
					_iter += 1;
				}
			}
	}

	// output CC of some special links
	for (auto _link_it = test_dta -> m_link_factory -> m_link_map.begin(); 
				  _link_it != test_dta -> m_link_factory -> m_link_map.end(); _link_it++){
			_link = _link_it -> second;
		if (_link -> m_link_ID() == 7186){
			_link_m = dynamic_cast<MNM_Dlink_Multiclass*>(_link);
			printf("\n\nm_N_in_car: \n");
			std::cout <<_link_m -> m_N_in_car -> to_string() << std::endl;
			printf("\n\nm_N_out_car: \n");
			std::cout <<_link_m -> m_N_out_car -> to_string() << std::endl;
			printf("\n\nm_N_in_truck: \n");
			std::cout <<_link_m -> m_N_in_truck -> to_string() << std::endl;
			printf("\n\nm_N_out_truck: \n");
			std::cout <<_link_m -> m_N_out_truck -> to_string() << std::endl;
		}
	}

	delete test_dta;
	printf("Finished delete test_dta!\n\n\n");

	return 0;
}