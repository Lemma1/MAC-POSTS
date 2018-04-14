#include "io.h"
#include "multiclass.h"

#include <vector>

int main()
{
	MNM_Dlink *_link;
	MNM_Dlink_Multiclass *_link_m;

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
		// if (_current_inter > 100){ break; };
	}


	TInt _iter = 0;
	while (_iter < _current_inter){
		printf("---------------------------------- Current interval: %d ----------------------------------\n", int(_iter));
		for (auto _link_it = test_dta -> m_link_factory -> m_link_map.begin(); 
				  _link_it != test_dta -> m_link_factory -> m_link_map.end(); _link_it++){
			_link = _link_it -> second;
			printf("link ID is %d: ", _link -> m_link_ID());
			_link_m = dynamic_cast<MNM_Dlink_Multiclass*>(_link);
			printf("car flow(%.4f), ", double(MNM_DTA_GRADIENT::get_link_inflow_car(_link_m, _iter, _iter + 1)));
			printf("truck flow(%.4f), ", double(MNM_DTA_GRADIENT::get_link_inflow_truck(_link_m, _iter, _iter + 1)));
			printf("car tt(%.4f s), ", double(MNM_DTA_GRADIENT::get_travel_time_car(_link_m, TFlt(_iter + 1))));
			printf("truck tt(%.4f s), ", double(MNM_DTA_GRADIENT::get_travel_time_truck(_link_m, TFlt(_iter + 1))));
			printf("car tt(%.4f s), ", double(_link_m -> get_link_freeflow_tt_car()));
			printf("truck tt(%.4f s)\n", double(_link_m -> get_link_freeflow_tt_truck()));
		}
		_iter += 1;
		// if (_iter > 100){ break; };
	}

	
	// for (auto _link_it = test_dta -> m_link_factory -> m_link_map.begin(); _link_it != test_dta -> m_link_factory -> m_link_map.end(); _link_it++){
	// 		_link = _link_it -> second;
	// 		if (_link -> m_link_ID() == 3) {
	// 			TInt _iter = 0;
	// 			while (_iter < _current_inter){
	// 				_link_m = dynamic_cast<MNM_Dlink_Multiclass*>(_link);
	// 				printf("%d,%.4f,%.4f\n", int(_iter),
	// 					double(MNM_DTA_GRADIENT::get_travel_time_car(_link_m, TFlt(_iter + 1))), 
	// 					double(MNM_DTA_GRADIENT::get_travel_time_truck(_link_m, TFlt(_iter + 1))));
	// 				_iter += 1;
	// 			}
	// 		}
	// }


	// for (auto _link_it = test_dta -> m_link_factory -> m_link_map.begin(); 
	// 			  _link_it != test_dta -> m_link_factory -> m_link_map.end(); _link_it++){
	// 		_link = _link_it -> second;
	// 	if (_link -> m_link_ID() == 3){
	// 		_link_m = dynamic_cast<MNM_Dlink_Multiclass*>(_link);
	// 		printf("m_N_in_car: \n");
	// 		std::cout <<_link_m -> m_N_in_car -> to_string() << std::endl;
	// 		printf("m_N_out_car: \n");
	// 		std::cout <<_link_m -> m_N_out_car -> to_string() << std::endl;
	// 		printf("m_N_in_truck: \n");
	// 		std::cout <<_link_m -> m_N_in_truck -> to_string() << std::endl;
	// 		printf("m_N_out_truck: \n");
	// 		std::cout <<_link_m -> m_N_out_truck -> to_string() << std::endl;
	// 	}
	// }

	return 0;
}