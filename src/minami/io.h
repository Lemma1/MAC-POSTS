#ifndef IO_H
#define IO_H

#include "Snap.h"
#include "ults.h"
#include "factory.h"
#include "enum.h"
#include "path.h"
#include "vms.h"
#include "workzone.h"

#include <string>
#include <vector>

class MNM_Node_Factory;

class MNM_IO
{
public:
  static int  build_node_factory(std::string file_folder, MNM_ConfReader *conf_reader, MNM_Node_Factory *node_factory);
  static int  build_link_factory(std::string file_folder, MNM_ConfReader *conf_reader, MNM_Link_Factory *link_factory, std::string file_name = "MNM_input_link");
  static int  build_od_factory(std::string file_folder, MNM_ConfReader *conf_reader, MNM_OD_Factory *od_factory, MNM_Node_Factory *node_factory);
  static int  build_od_factory(std::string file_folder, MNM_ConfReader *conf_reader, MNM_OD_Factory *od_factory);
  static int  hook_up_od_node(std::string file_folder, MNM_ConfReader *conf_reader, MNM_OD_Factory *od_factory, MNM_Node_Factory *node_factory);  
  static PNEGraph build_graph(std::string file_folder, MNM_ConfReader *conf_reader);
  static int build_demand(std::string file_folder, MNM_ConfReader *conf_reader, MNM_OD_Factory *od_factory);
  static Path_Table *load_path_table(std::string file_name, PNEGraph graph, TInt num_path, bool w_buffer = false, bool w_ID = false);
  static int build_vms_facotory(std::string file_folder, PNEGraph graph, TInt num_vms, MNM_Vms_Factory *vms_factory);
  static int read_int_float(std::string file_name, std::unordered_map<TInt, TFlt>* reader);
  static int read_int(std::string file_name, std::vector<TInt>* reader);
  static int read_float(std::string file_name, std::vector<TFlt*>* reader);
  static int build_workzone_list(std::string file_folder, MNM_Workzone* workzone);
  static int dump_cumulative_curve(std::string file_folder, MNM_Link_Factory *link_factory);

private:
  static std::vector<std::string> split(const std::string &text, char sep);
  static  std::string inline &ltrim(std::string &s) {s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;}

  // trim from end
  static  std::string inline &rtrim(std::string &s) {s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;}

  // trim from both ends
  static  std::string inline &trim(std::string &s) {return ltrim(rtrim(s));}
};

#endif