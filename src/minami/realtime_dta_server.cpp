#include "Snap.h"

#include "path.h"
#include "io.h"
#include "dlink.h"
#include "shortest_path.h"
#include "realtime_dta.h"
#include "vms.h"

#include <unistd.h>

#include <fstream>
#include <iostream>
#include <string>

#define SLEEP_TIME 100000
#define RESET_THRESHOLD 500000

bool is_file_exist(const char *fileName)
{
    std::ifstream infile(fileName);
    return infile.good();
}

int main()
{

  std::string file_folder = "../../data/input_files_2link";
  std::string instruction_file_name = file_folder + "/instruction";

  MNM_Realtime_Dta* rd = new MNM_Realtime_Dta(file_folder);
  rd -> init_running();

  while(true){
    if (!is_file_exist(instruction_file_name.c_str())){
      usleep( SLEEP_TIME );
      continue;
    }
    std::ifstream instruction_file;
    instruction_file.open(instruction_file_name, std::ios::in);

    std::string line;
    TInt assign_inter;
    if (instruction_file.is_open()){
      getline(instruction_file, line);
      assign_inter = TInt(std::stoi(line));
      instruction_file.close();
    }
    
    
    
    rd -> one_iteration(assign_inter);

    if (rd -> m_before_shot -> m_veh_factory -> m_veh_map.size() > RESET_THRESHOLD 
          || rd -> m_after_shot -> m_veh_factory -> m_veh_map.size() > RESET_THRESHOLD){
      rd -> reset();
    }

    std::remove(instruction_file_name.c_str());
  }
  return 0;
}



