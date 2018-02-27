#include "Snap.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <string>

#include "path.h"
#include "io.h"
#include "dlink.h"
#include "shortest_path.h"
#include "realtime_dta.h"
#include "vms.h"

int main()
{
  std::string file_folder = "../../data/input_files_new_philly";
  // std::string file_folder = "../../data/input_files_2link";
  std::string record_folder = file_folder + "/record";
  MNM_Realtime_Dta* rd = new MNM_Realtime_Dta(file_folder);
  rd -> init_running();

  printf("Finish init\n");
  TInt _interval = 4;
  TInt _start_assign_interval = 27;
  MNM_Dta_Screenshot *_tmp_shot, *_tmp_shot2, *_tmp_shot_stay;
  for (int i = 0; i<_interval; ++i){
    MNM_Ults::copy_file(file_folder + "/" + std::to_string(i), std::string(file_folder + "/MNM_input_spd"));
    TInt assign_inter = _start_assign_interval + i % rd -> m_total_assign_inter;
    printf("estimate_previous\n");
    rd -> estimate_previous(assign_inter);
    printf("update_compliance_ratio\n");
    rd -> update_compliance_ratio(assign_inter);
    printf("copy_p_to_buffer\n");
    MNM::copy_p_to_buffer(rd -> m_path_table, 0);
    TInt _next_assign_inter = (assign_inter + 1) % rd -> m_total_assign_inter;
    printf("optimize_next\n");
    rd -> optimize_next(_next_assign_inter);
    printf("copy_p_to_buffer\n");
    MNM::copy_p_to_buffer(rd -> m_path_table, 2);
    printf("update_vms\n");
    rd -> update_vms(_next_assign_inter);
    _tmp_shot = MNM::make_screenshot(rd -> m_before_shot);
    _tmp_shot2 = MNM::make_screenshot(rd -> m_before_shot);
    printf("predict_next\n");
    rd -> predict_next(_next_assign_inter);
    printf("generate_vms_instructions\n");
    std::string _vms_info = rd -> m_file_folder + "/record/vms_info";
    MNM::generate_vms_instructions(_vms_info, rd -> m_vms_factory, rd -> m_after_shot -> m_link_factory);
    MNM_Ults::copy_file(record_folder + "/MNM_output_record_interval_volume", record_folder + "/" + std::to_string(i) + "_volume_pre");
    MNM_Ults::copy_file(record_folder + "/MNM_output_record_interval_tt", record_folder + "/" + std::to_string(i) + "_speed_pre");
    MNM_Ults::copy_file(record_folder + "/vms_info", record_folder + "/" + std::to_string(i) + "_dms");
    

    MNM::copy_buffer_to_p(rd -> m_path_table, 0);
    _tmp_shot_stay = rd -> m_before_shot;
    rd -> m_before_shot = _tmp_shot;
    rd -> predict_next(_next_assign_inter);
    MNM_Ults::copy_file(record_folder + "/MNM_output_record_interval_volume", record_folder + "/" + std::to_string(i) + "_volume_est");
    MNM_Ults::copy_file(record_folder + "/MNM_output_record_interval_tt", record_folder + "/" + std::to_string(i) + "_speed_est");

    MNM::copy_buffer_to_p(rd -> m_path_table, 2);
    rd -> m_before_shot = _tmp_shot2;
    rd -> predict_next(_next_assign_inter);
    MNM_Ults::copy_file(record_folder + "/MNM_output_record_interval_volume", record_folder + "/" + std::to_string(i) + "_volume_opt");
    MNM_Ults::copy_file(record_folder + "/MNM_output_record_interval_tt", record_folder + "/" + std::to_string(i) + "_speed_opt");

    delete _tmp_shot;
    delete _tmp_shot2;
    rd -> m_before_shot = _tmp_shot_stay;
  }
}