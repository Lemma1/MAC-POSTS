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
    // printf("Start build node factory.\n");
    std::getline(__node_file,__line); //skip the first line
    for (int i=0; i < __num_of_node; ++i){
      std::getline(__node_file,__line);
      __words = split(__line, ' ');
      if (__words.size() == 2) {
        // std::cout << "Processing: " << __line << "\n";
        __node_ID = TInt(std::stoi(__words[0]));
        __type = trim(__words[1]);
        if (__type == "FWJ"){
          node_factory -> make_node(__node_ID, MNM_TYPE_FWJ, __flow_scalar);
          continue;
        }
        if (__type =="DMOND"){
          node_factory -> make_node(__node_ID, MNM_TYPE_ORIGIN, __flow_scalar);
          continue;
        }
        if (__type =="DMDND"){
          node_factory -> make_node(__node_ID, MNM_TYPE_DEST, __flow_scalar);
          continue;
        }
        printf("Wrong node type, %s\n", __type.c_str());
        exit(-1);
      }
      else {
        printf("MNM_IO::build_node_factory::Wrong length of line.\n");
        exit(-1);
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
    // printf("Start build link factory.\n");
    std::getline(__link_file,__line); //skip the first line
    for (int i=0; i < __num_of_link; ++i){
      std::getline(__link_file,__line);
      __words = split(__line, ' ');
      if (__words.size() == 7) {
        // std::cout << "Processing: " << __line << "\n";
        __link_ID = TInt(std::stoi(__words[0]));
        __type = trim(__words[1]);
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
          continue;
        }
        if (__type =="CTM"){
          link_factory -> make_link(__link_ID, MNM_TYPE_CTM, __lane_hold_cap, __lane_flow_cap, __number_of_lane, 
                                    __length, __ffs, __unit_time, __flow_scalar);
          continue;
        }
        printf("Wrong link type, %s\n", __type.c_str());
        exit(-1);
      }
      else{
        printf("MNM_IO::build_link_factory::Wrong length of line.\n");
        exit(-1);        
      }
    }
    __link_file.close();
  }
  return 0;
}

int  MNM_IO::build_od_factory(std::string file_folder, MNM_ConfReader *conf_reader, 
                                MNM_OD_Factory *od_factory, MNM_Node_Factory *node_factory)
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
  TInt __frequency = conf_reader -> get_int("assign_frq");

  /* build */
  TInt __dest_ID, __origin_ID, __node_ID;
  std::string __line;
  std::vector<std::string> __words;
  MNM_Origin *__origin;
  MNM_Destination *__dest;
  if (__od_file.is_open())
  {
    // printf("Start build Origin-Destination factory.\n");
    std::getline(__od_file,__line); //skip the first line
    // printf("Processing Origin node.\n");
    for (int i=0; i < __num_of_O; ++i){
      std::getline(__od_file,__line);
      __words = split(__line, ' ');
      if (__words.size() == 2) {
        // std::cout << "Processing: " << __line << "\n";
        __origin_ID = TInt(std::stoi(__words[0]));
        __node_ID = TInt(std::stoi(__words[1]));
        __origin = od_factory -> make_origin(__origin_ID, __max_interval, __flow_scalar, __frequency);

        /* hook up */
        __origin ->  m_origin_node =  (MNM_DMOND*) node_factory -> get_node(__node_ID);
        ((MNM_DMOND*)  node_factory -> get_node(__node_ID)) -> hook_up_origin(__origin);
      }
    }
    std::getline(__od_file,__line); // skip another line
    // printf("Processing Destination node.\n");
    for (int i=0; i < __num_of_D; ++i){
      std::getline(__od_file,__line);
      __words = split(__line, ' ');
      if (__words.size() == 2) {
        // std::cout << "Processing: " << __line << "\n";
        __dest_ID = TInt(std::stoi(__words[0]));
        __node_ID = TInt(std::stoi(__words[1]));
        __dest = od_factory -> make_destination(__dest_ID);

        /* hook up */
        __dest ->  m_dest_node =  (MNM_DMDND*) node_factory -> get_node(__node_ID);
        ((MNM_DMDND*)  node_factory -> get_node(__node_ID)) -> hook_up_destination(__dest);
      }
    }      
  }
  __od_file.close();
  return 0;
}



int MNM_IO::hook_up_od_node(std::string file_folder, MNM_ConfReader *conf_reader, MNM_OD_Factory *od_factory, MNM_Node_Factory *node_factory)
{
  /* find file */
  std::string __od_file_name = file_folder + "/MNM_input_od";
  std::ifstream __od_file;
  __od_file.open(__od_file_name, std::ios::in);

  /* read config */
  TInt __num_of_O = conf_reader -> get_int("num_of_O");
  TInt __num_of_D = conf_reader -> get_int("num_of_D");

  /* build */
  TInt __dest_ID, __origin_ID, __node_ID;
  std::string __line;
  std::vector<std::string> __words;
  MNM_Origin *__origin;
  MNM_Destination *__dest;
  if (__od_file.is_open())
  {
    // printf("Start build Origin-Destination factory.\n");
    std::getline(__od_file,__line); //skip the first line
    // printf("Processing Origin node.\n");
    for (int i=0; i < __num_of_O; ++i){
      std::getline(__od_file,__line);
      __words = split(__line, ' ');
      if (__words.size() == 2) {
        // std::cout << "Processing: " << __line << "\n";
        __origin_ID = TInt(std::stoi(__words[0]));
        __node_ID = TInt(std::stoi(__words[1]));
        __origin = od_factory ->get_origin(__origin_ID);

        /* hook up */
        __origin ->  m_origin_node =  (MNM_DMOND*) node_factory -> get_node(__node_ID);
        ((MNM_DMOND*)  node_factory -> get_node(__node_ID)) -> hook_up_origin(__origin);
      }
    }
    std::getline(__od_file,__line); // skip another line
    // printf("Processing Destination node.\n");
    for (int i=0; i < __num_of_D; ++i){
      std::getline(__od_file,__line);
      __words = split(__line, ' ');
      if (__words.size() == 2) {
        // std::cout << "Processing: " << __line << "\n";
        __dest_ID = TInt(std::stoi(__words[0]));
        __node_ID = TInt(std::stoi(__words[1]));
        __dest = od_factory -> get_destination(__dest_ID);

        /* hook up */
        __dest ->  m_dest_node =  (MNM_DMDND*) node_factory -> get_node(__node_ID);
        ((MNM_DMDND*)  node_factory -> get_node(__node_ID)) -> hook_up_destination(__dest);
      }
    }      
  }
  __od_file.close();
  return 0;  
}


int  MNM_IO::build_od_factory(std::string file_folder, MNM_ConfReader *conf_reader, 
                                MNM_OD_Factory *od_factory)
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
  TInt __frequency = conf_reader -> get_int("assign_frq");

  /* build */
  TInt __dest_ID, __origin_ID, __node_ID;
  std::string __line;
  std::vector<std::string> __words;
  if (__od_file.is_open())
  {
    // printf("Start build Origin-Destination factory.\n");
    std::getline(__od_file,__line); //skip the first line
    // printf("Processing Origin node.\n");
    for (int i=0; i < __num_of_O; ++i){
      std::getline(__od_file,__line);
      __words = split(__line, ' ');
      if (__words.size() == 2) {
        // std::cout << "Processing: " << __line << "\n";
        __origin_ID = TInt(std::stoi(__words[0]));
        __node_ID = TInt(std::stoi(__words[1]));
        od_factory -> make_origin(__origin_ID, __max_interval, __flow_scalar, __frequency);

      }
    }
    std::getline(__od_file,__line); // skip another line
    // printf("Processing Destination node.\n");
    for (int i=0; i < __num_of_D; ++i){
      std::getline(__od_file,__line);
      __words = split(__line, ' ');
      if (__words.size() == 2) {
        // std::cout << "Processing: " << __line << "\n";
        __dest_ID = TInt(std::stoi(__words[0]));
        __node_ID = TInt(std::stoi(__words[1]));
        od_factory -> make_destination(__dest_ID);
      }
    }      
  }
  __od_file.close();
  return 0;
}

PNEGraph MNM_IO::build_graph(std::string file_folder, MNM_ConfReader *conf_reader)
{
  /* find file */
  std::string __network_name = conf_reader -> get_string("network_name");
  std::string __graph_file_name = file_folder + "/" + __network_name;
  std::ifstream __graph_file;
  __graph_file.open(__graph_file_name, std::ios::in);

  TInt __num_of_link = conf_reader -> get_int("num_of_link");

  // printf("Start build graph.\n");
  PNEGraph __graph = PNEGraph::TObj::New();
  
  int __link_ID, __from_ID, __to_ID;
  std::string __line;
  std::vector<std::string> __words;
  std::getline(__graph_file,__line); // skip one line
  for (int i=0; i<__num_of_link; ++i){
    std::getline(__graph_file,__line);
    __words = split(__line, ' ');
    if (__words.size() == 3){
      // std::cout << "Processing: " << __line << "\n";
      __link_ID = TInt(std::stoi(__words[0]));
      __from_ID = TInt(std::stoi(__words[1]));
      __to_ID = TInt(std::stoi(__words[2]));
    if (! __graph->IsNode(__from_ID)) { __graph->AddNode(__from_ID); }
    if (! __graph->IsNode(__to_ID)) { __graph->AddNode(__to_ID); }
    __graph->AddEdge(__from_ID, __to_ID, __link_ID);
    }
  }
  __graph->Defrag();
  return __graph;
}



int MNM_IO::build_demand(std::string file_folder, MNM_ConfReader *conf_reader, MNM_OD_Factory *od_factory)
{
  /* find file */
  std::string __demand_file_name = file_folder + "/MNM_input_demand";
  std::ifstream __demand_file;
  __demand_file.open(__demand_file_name, std::ios::in);

  /* read config */
  TInt __max_interval = conf_reader -> get_int("max_interval"); 
  TInt __num_OD = conf_reader -> get_int("OD_pair");

  /* build */
  TInt __O_ID, __D_ID;
  MNM_Origin *__origin;
  MNM_Destination *__dest;
  std::string __line;
  std::vector<std::string> __words;
  if (__demand_file.is_open())
  {
    // printf("Start build demand profile.\n");
    TFlt *__demand_vector = (TFlt*) malloc(sizeof(TFlt) * __max_interval);
    memset(__demand_vector, 0x0, sizeof(TFlt) * __max_interval);
    std::getline(__demand_file,__line); //skip the first line
    for (int i=0; i < __num_OD; ++i){
      std::getline(__demand_file,__line);
      // std::cout << "Processing: " << __line << "\n";
      __words = split(__line, ' ');
      if (TInt(__words.size()) == (__max_interval + 2)) {
        __O_ID = TInt(std::stoi(__words[0]));
        __D_ID = TInt(std::stoi(__words[1]));
        for(int j=0; j<__max_interval; ++j){
          __demand_vector[j] = TFlt(std::stod(__words[j+2]));
        }
        __origin = od_factory -> get_origin(__O_ID);
        __dest = od_factory ->get_destination(__D_ID);
        __origin -> add_dest_demand(__dest, __demand_vector);
      }
      else{
        printf("Something wrong in build_demand!\n");
        free(__demand_vector);
        exit(-1);
      }
    }
    free(__demand_vector);
    __demand_file.close();
  }  
  return 0;
}



Path_Table *MNM_IO::load_path_table(std::string file_name, PNEGraph graph, TInt num_path)
{
  TInt Num_Path = num_path;

  std::ifstream _path_table_file;
  _path_table_file.open(file_name, std::ios::in);
  Path_Table *_path_table = new Path_Table();

  /* read file */
  std::string _line;
  std::vector<std::string> _words;
  TInt _origin_node_ID, _dest_node_ID, _node_ID;
  std::map<TInt, MNM_Pathset*> *_new_map;
  MNM_Pathset *_pathset;
  MNM_Path *_path;
  TInt _from_ID, _to_ID, _link_ID;
  if (_path_table_file.is_open())
  {
    for (int i=0; i<Num_Path; ++i){
      std::getline(_path_table_file,_line);
      _words = split(_line, ' ');
      if (_words.size() >= 2){
        _origin_node_ID = TInt(std::stoi(_words[0]));
        _dest_node_ID = TInt(std::stoi(_words.back()));
        if (_path_table -> find(_origin_node_ID) == _path_table -> end()){
          _new_map = new std::map<TInt, MNM_Pathset*>();
          _path_table -> insert(std::pair<TInt, std::map<TInt, MNM_Pathset*>*>(_origin_node_ID, _new_map));
        }
        if (_path_table -> find(_origin_node_ID) -> second -> find(_dest_node_ID) == _path_table -> find(_origin_node_ID) -> second -> end()){
          _pathset = new MNM_Pathset();
          _path_table -> find(_origin_node_ID) -> second -> insert(std::pair<TInt, MNM_Pathset*>(_dest_node_ID, _pathset));
        }
        _path = new MNM_Path();
        for (std::string _s_node_ID : _words){
          _node_ID = TInt(std::stoi(_s_node_ID));
          _path -> m_node_vec.push_back(_node_ID);
        }
        for (size_t i = 0; i < _path -> m_node_vec.size() - 1; ++i){
          _from_ID = _path -> m_node_vec[i];
          _to_ID = _path -> m_node_vec[i+1];
          _link_ID = graph -> GetEI(_from_ID, _to_ID).GetId();
          _path -> m_link_vec.push_back(_link_ID);
        }
        _path_table -> find(_origin_node_ID) -> second -> find(_dest_node_ID) -> second -> m_path_vec.push_back(_path);
      }
    }
    _path_table_file.close();
  }
  return _path_table;
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


