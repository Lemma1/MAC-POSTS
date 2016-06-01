#include "io.h"

#include <fstream>
#include <iostream>


int MNM_IO::build_node_factory(std::string file_folder, MNM_ConfReader *conf_reader, MNM_Node_Factory *node_factory)
{
  /* find file */
  std::string __node_file_name = file_folder + "/MNM_input_node";
  std::ifstream __node_file;
  __node_file.open(__node_file_name, std::ios::in);

  /* read confid */
  TInt __num_of_node = conf_reader -> get_int("num_of_node");
  TFlt __flow_scalar = conf_reader -> get_float("flow_scalar");

  /* read file */
  std::string __line;
  std::vector<std::string> __words;
  TInt __node_ID;
  std::string __type;

  if (__node_file.is_open())
  {
    printf("Start build node factory.\n");
    std::getline(__node_file,__line); //skip the first line
    for (int i=0; i < __num_of_node; ++i){
      std::getline(__node_file,__line);
      __words = split(__line, ' ');
      if (__words.size() == 2) {
        std::cout << "Processing: " << __line << "\n";
        __node_ID = TInt(std::stoi(__words[0]));
        __type = __words[1];
        if (__type == "FWJ"){
          node_factory -> make_node(__node_ID, MNM_TYPE_FWJ, __flow_scalar);
        }
        if (__type =="DMOND"){
          node_factory -> make_node(__node_ID, MNM_TYPE_ORIGIN, __flow_scalar);
        }
        if (__type =="DMDND"){
          node_factory -> make_node(__node_ID, MNM_TYPE_ORIGIN, __flow_scalar);
        }
      }
    }
    __node_file.close();
  }
  return 0;
}

int MNM_IO::build_link_factory(std::string file_folder, MNM_ConfReader *conf_reader, MNM_Link_Factory *link_factory)
{
  /* find file */
  std::string __link_file_name = file_folder + "/MNM_input_link";
  std::ifstream __link_file;
  __link_file.open(__link_file_name, std::ios::in);

  /* read config */
  TInt __num_of_link = conf_reader -> get_int("num_of_link");
  TFlt __flow_scalar = conf_reader -> get_float("flow_scalar");
  TFlt __unit_time = conf_reader -> get_float("unit_time");

  /* read file */
  std::string __line;
  std::vector<std::string> __words;
  TInt __link_ID;
  TFlt __lane_hold_cap; 
  TFlt __lane_flow_cap;
  TInt __number_of_lane;
  TFlt __length;
  TFlt __ffs;
  std::string __type;

  if (__link_file.is_open())
  {
    printf("Start build link factory.\n");
    std::getline(__link_file,__line); //skip the first line
    for (int i=0; i < __num_of_link; ++i){
      std::getline(__link_file,__line);
      __words = split(__line, ' ');
      if (__words.size() == 7) {
        std::cout << "Processing: " << __line << "\n";
        __link_ID = TInt(std::stoi(__words[0]));
        __type = __words[1];
        __length = TFlt(std::stod(__words[2]));
        __ffs = TFlt(std::stod(__words[3]));
        __lane_flow_cap = TFlt(std::stod(__words[4]));
        __lane_hold_cap = TFlt(std::stod(__words[5]));
        __number_of_lane = TInt(std::stoi(__words[6]));

        /* unit conversion */
        __length = __length * TFlt(1600);
        __ffs = __ffs * TFlt(1600) / TFlt(3600);
        __lane_flow_cap = __lane_flow_cap / TFlt(3600);
        __lane_hold_cap = __lane_hold_cap / TFlt(1600);

        /* build */
        if (__type == "PQ"){
          link_factory -> make_link(__link_ID, MNM_TYPE_PQ, __lane_hold_cap, __lane_flow_cap, __number_of_lane, 
                                    __length, __ffs, __unit_time, __flow_scalar);
        }
        if (__type =="CTM"){
          link_factory -> make_link(__link_ID, MNM_TYPE_CTM, __lane_hold_cap, __lane_flow_cap, __number_of_lane, 
                                    __length, __ffs, __unit_time, __flow_scalar);
        }
      }
    }
    __link_file.close();
  }
  return 0;
}

int  MNM_IO::build_od_factory(std::string file_folder, MNM_ConfReader *conf_reader, MNM_OD_Factory *od_factory)
{
  /* find file */
  std::string __od_file_name = file_folder + "/MNM_input_od";
  std::ifstream __od_file;
  __od_file.open(__od_file_name, std::ios::in);

  /* read config */
  TInt __num_of_O = conf_reader -> get_int("num_of_O");
  TInt __num_of_D = conf_reader -> get_int("num_of_D");
  TFlt __flow_scalar = conf_reader -> get_float("flow_scalar");
  TInt __max_interval = conf_reader -> get_int("max_interval");

  /* build */
  TInt __dest_ID, __origin_ID, __node_ID;
  std::string __line;
  std::vector<std::string> __words;
  if (__od_file.is_open())
  {
    printf("Start build Origin-Destination factory.\n");
    std::getline(__od_file,__line); //skip the first line
    printf("Processing Origin node.\n");
    for (int i=0; i < __num_of_O; ++i){
      std::getline(__od_file,__line);
      __words = split(__line, ' ');
      if (__words.size() == 2) {
        std::cout << "Processing: " << __line << "\n";
        __origin_ID = TInt(std::stoi(__words[0]));
        __node_ID = TInt(std::stoi(__words[1]));
        od_factory -> make_origin(__origin_ID, __max_interval, __flow_scalar);
      }
    }
    std::getline(__od_file,__line); // skip another line
    printf("Processing Destination node.\n");
    for (int i=0; i < __num_of_D; ++i){
      std::getline(__od_file,__line);
      __words = split(__line, ' ');
      if (__words.size() == 2) {
        std::cout << "Processing: " << __line << "\n";
        __dest_ID = TInt(std::stoi(__words[0]));
        __node_ID = TInt(std::stoi(__words[1]));
        od_factory -> make_destination(__dest_ID);
      }
    }      
  }
  __od_file.close();
  return 0;
}

PNGraph MNM_IO::build_graph(std::string file_folder, MNM_ConfReader *conf_reader)
{
  /* find file */
  std::string __network_name = conf_reader -> get_string("network_name");
  std::string __graph_file_name = file_folder + "/" + __network_name;
  PNGraph __graph = TSnap::LoadEdgeList <PNGraph> (__graph_file_name.c_str());
  return __graph;
}

std::vector<std::string> MNM_IO::split(const std::string &text, char sep) 
{
  std::vector<std::string> tokens;
  std::size_t start = 0, end = 0;
  while ((end = text.find(sep, start)) != std::string::npos) {
    tokens.push_back(text.substr(start, end - start));
    start = end + 1;
  }
  tokens.push_back(text.substr(start));
  return tokens;
}