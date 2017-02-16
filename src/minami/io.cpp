#include "io.h"

#include <fstream>
#include <iostream>


int MNM_IO::build_node_factory(std::string file_folder, MNM_ConfReader *conf_reader, MNM_Node_Factory *node_factory)
{
  /* find file */
  std::string _node_file_name = file_folder + "/MNM_input_node";
  std::ifstream _node_file;
  _node_file.open(_node_file_name, std::ios::in);


  /* read confid */
  TInt _num_of_node = conf_reader -> get_int("num_of_node");
  TFlt _flow_scalar = conf_reader -> get_float("flow_scalar");

  /* read file */
  std::string _line;
  std::vector<std::string> _words;
  TInt _node_ID;
  std::string _type;

  if (_node_file.is_open())
  {
    // printf("Start build node factory.\n");
    std::getline(_node_file,_line); //skip the first line
    for (int i=0; i < _num_of_node; ++i){
      std::getline(_node_file,_line);
      _words = split(_line, ' ');
      if (_words.size() == 2) {
        // std::cout << "Processing: " << _line << "\n";
        _node_ID = TInt(std::stoi(_words[0]));
        _type = trim(_words[1]);
        if (_type == "FWJ"){
          node_factory -> make_node(_node_ID, MNM_TYPE_FWJ, _flow_scalar);
          continue;
        }
        if (_type == "GRJ"){
          node_factory -> make_node(_node_ID, MNM_TYPE_GRJ, _flow_scalar);
          continue;
        }
        if (_type =="DMOND"){
          node_factory -> make_node(_node_ID, MNM_TYPE_ORIGIN, _flow_scalar);
          continue;
        }
        if (_type =="DMDND"){
          node_factory -> make_node(_node_ID, MNM_TYPE_DEST, _flow_scalar);
          continue;
        }
        printf("Wrong node type, %s\n", _type.c_str());
        exit(-1);
      }
      else {
        printf("MNM_IO::build_node_factory::Wrong length of line.\n");
        exit(-1);
      }
    }
    _node_file.close();
  }
  return 0;
}

int MNM_IO::build_link_factory(std::string file_folder, MNM_ConfReader *conf_reader, MNM_Link_Factory *link_factory, std::string file_name)
{
  /* find file */
  std::string _link_file_name = file_folder + "/" + file_name;
  std::ifstream _link_file;
  _link_file.open(_link_file_name, std::ios::in);

  /* read config */
  TInt _num_of_link = conf_reader -> get_int("num_of_link");
  TFlt _flow_scalar = conf_reader -> get_float("flow_scalar");
  TFlt _unit_time = conf_reader -> get_float("unit_time");

  /* read file */
  std::string _line;
  std::vector<std::string> _words;
  TInt _link_ID;
  TFlt _lane_hold_cap; 
  TFlt _lane_flow_cap;
  TInt _number_of_lane;
  TFlt _length;
  TFlt _ffs;
  std::string _type;

  if (_link_file.is_open())
  {
    // printf("Start build link factory.\n");
    std::getline(_link_file,_line); //skip the first line
    for (int i=0; i < _num_of_link; ++i){
      std::getline(_link_file,_line);
      _words = split(_line, ' ');
      if (_words.size() == 7) {
        // std::cout << "Processing: " << _line << "\n";
        _link_ID = TInt(std::stoi(_words[0]));
        _type = trim(_words[1]);
        _length = TFlt(std::stod(_words[2]));
        _ffs = TFlt(std::stod(_words[3]));
        _lane_flow_cap = TFlt(std::stod(_words[4]));
        _lane_hold_cap = TFlt(std::stod(_words[5]));
        _number_of_lane = TInt(std::stoi(_words[6]));

        /* unit conversion */
        _length = _length * TFlt(1600);
        _ffs = _ffs * TFlt(1600) / TFlt(3600);
        _lane_flow_cap = _lane_flow_cap / TFlt(3600);
        _lane_hold_cap = _lane_hold_cap / TFlt(1600);

        /* build */
        if (_type == "PQ"){
          link_factory -> make_link(_link_ID, MNM_TYPE_PQ, _lane_hold_cap, _lane_flow_cap, _number_of_lane, 
                                    _length, _ffs, _unit_time, _flow_scalar);
          continue;
        }
        if (_type =="CTM"){
          link_factory -> make_link(_link_ID, MNM_TYPE_CTM, _lane_hold_cap, _lane_flow_cap, _number_of_lane, 
                                    _length, _ffs, _unit_time, _flow_scalar);
          continue;
        }
        if (_type =="LQ"){
          link_factory -> make_link(_link_ID, MNM_TYPE_LQ, _lane_hold_cap, _lane_flow_cap, _number_of_lane, 
                                    _length, _ffs, _unit_time, _flow_scalar);
          continue;
        }
        if (_type =="LTM"){
          link_factory -> make_link(_link_ID, MNM_TYPE_LTM, _lane_hold_cap, _lane_flow_cap, _number_of_lane, 
                                    _length, _ffs, _unit_time, _flow_scalar);
          continue;
        }
        printf("Wrong link type, %s\n", _type.c_str());
        exit(-1);
      }
      else{
        printf("MNM_IO::build_link_factory::Wrong length of line.\n");
        exit(-1);        
      }
    }
    _link_file.close();
  }
  return 0;
}

int  MNM_IO::build_od_factory(std::string file_folder, MNM_ConfReader *conf_reader, 
                                MNM_OD_Factory *od_factory, MNM_Node_Factory *node_factory)
{
  /* find file */
  std::string _od_file_name = file_folder + "/MNM_input_od";
  std::ifstream _od_file;
  _od_file.open(_od_file_name, std::ios::in);

  /* read config */
  TInt _num_of_O = conf_reader -> get_int("num_of_O");
  TInt _num_of_D = conf_reader -> get_int("num_of_D");
  TFlt _flow_scalar = conf_reader -> get_float("flow_scalar");
  TInt _max_interval = conf_reader -> get_int("max_interval");
  TInt _frequency = conf_reader -> get_int("assign_frq");

  /* build */
  TInt _dest_ID, _origin_ID, _node_ID;
  std::string _line;
  std::vector<std::string> _words;
  MNM_Origin *_origin;
  MNM_Destination *_dest;
  if (_od_file.is_open())
  {
    // printf("Start build Origin-Destination factory.\n");
    std::getline(_od_file,_line); //skip the first line
    // printf("Processing Origin node.\n");
    for (int i=0; i < _num_of_O; ++i){
      std::getline(_od_file,_line);
      _words = split(_line, ' ');
      if (_words.size() == 2) {
        // std::cout << "Processing: " << _line << "\n";
        _origin_ID = TInt(std::stoi(_words[0]));
        _node_ID = TInt(std::stoi(_words[1]));
        _origin = od_factory -> make_origin(_origin_ID, _max_interval, _flow_scalar, _frequency);

        /* hook up */
        _origin ->  m_origin_node =  (MNM_DMOND*) node_factory -> get_node(_node_ID);
        ((MNM_DMOND*)  node_factory -> get_node(_node_ID)) -> hook_up_origin(_origin);
      }
    }
    std::getline(_od_file,_line); // skip another line
    // printf("Processing Destination node.\n");
    for (int i=0; i < _num_of_D; ++i){
      std::getline(_od_file,_line);
      _words = split(_line, ' ');
      if (_words.size() == 2) {
        // std::cout << "Processing: " << _line << "\n";
        _dest_ID = TInt(std::stoi(_words[0]));
        _node_ID = TInt(std::stoi(_words[1]));
        _dest = od_factory -> make_destination(_dest_ID);

        /* hook up */
        _dest ->  m_dest_node =  (MNM_DMDND*) node_factory -> get_node(_node_ID);
        ((MNM_DMDND*)  node_factory -> get_node(_node_ID)) -> hook_up_destination(_dest);
      }
    }      
  }
  _od_file.close();
  return 0;
}



int MNM_IO::hook_up_od_node(std::string file_folder, MNM_ConfReader *conf_reader, MNM_OD_Factory *od_factory, MNM_Node_Factory *node_factory)
{
  /* find file */
  std::string _od_file_name = file_folder + "/MNM_input_od";
  std::ifstream _od_file;
  _od_file.open(_od_file_name, std::ios::in);

  /* read config */
  TInt _num_of_O = conf_reader -> get_int("num_of_O");
  TInt _num_of_D = conf_reader -> get_int("num_of_D");

  /* build */
  TInt _dest_ID, _origin_ID, _node_ID;
  std::string _line;
  std::vector<std::string> _words;
  MNM_Origin *_origin;
  MNM_Destination *_dest;
  if (_od_file.is_open())
  {
    // printf("Start build Origin-Destination factory.\n");
    std::getline(_od_file,_line); //skip the first line
    // printf("Processing Origin node.\n");
    for (int i=0; i < _num_of_O; ++i){
      std::getline(_od_file,_line);
      _words = split(_line, ' ');
      if (_words.size() == 2) {
        // std::cout << "Processing: " << _line << "\n";
        _origin_ID = TInt(std::stoi(_words[0]));
        _node_ID = TInt(std::stoi(_words[1]));
        _origin = od_factory ->get_origin(_origin_ID);

        /* hook up */
        _origin ->  m_origin_node =  (MNM_DMOND*) node_factory -> get_node(_node_ID);
        ((MNM_DMOND*)  node_factory -> get_node(_node_ID)) -> hook_up_origin(_origin);
      }
    }
    std::getline(_od_file,_line); // skip another line
    // printf("Processing Destination node.\n");
    for (int i=0; i < _num_of_D; ++i){
      std::getline(_od_file,_line);
      _words = split(_line, ' ');
      if (_words.size() == 2) {
        // std::cout << "Processing: " << _line << "\n";
        _dest_ID = TInt(std::stoi(_words[0]));
        _node_ID = TInt(std::stoi(_words[1]));
        _dest = od_factory -> get_destination(_dest_ID);

        /* hook up */
        _dest ->  m_dest_node =  (MNM_DMDND*) node_factory -> get_node(_node_ID);
        ((MNM_DMDND*)  node_factory -> get_node(_node_ID)) -> hook_up_destination(_dest);
      }
    }      
  }
  _od_file.close();
  return 0;  
}


int  MNM_IO::build_od_factory(std::string file_folder, MNM_ConfReader *conf_reader, 
                                MNM_OD_Factory *od_factory)
{
  /* find file */
  std::string _od_file_name = file_folder + "/MNM_input_od";
  std::ifstream _od_file;
  _od_file.open(_od_file_name, std::ios::in);

  /* read config */
  TInt _num_of_O = conf_reader -> get_int("num_of_O");
  TInt _num_of_D = conf_reader -> get_int("num_of_D");
  TFlt _flow_scalar = conf_reader -> get_float("flow_scalar");
  TInt _max_interval = conf_reader -> get_int("max_interval");
  TInt _frequency = conf_reader -> get_int("assign_frq");

  /* build */
  TInt _dest_ID, _origin_ID, _node_ID;
  std::string _line;
  std::vector<std::string> _words;
  if (_od_file.is_open())
  {
    // printf("Start build Origin-Destination factory.\n");
    std::getline(_od_file,_line); //skip the first line
    // printf("Processing Origin node.\n");
    for (int i=0; i < _num_of_O; ++i){
      std::getline(_od_file,_line);
      _words = split(_line, ' ');
      if (_words.size() == 2) {
        // std::cout << "Processing: " << _line << "\n";
        _origin_ID = TInt(std::stoi(_words[0]));
        _node_ID = TInt(std::stoi(_words[1]));
        od_factory -> make_origin(_origin_ID, _max_interval, _flow_scalar, _frequency);

      }
    }
    std::getline(_od_file,_line); // skip another line
    // printf("Processing Destination node.\n");
    for (int i=0; i < _num_of_D; ++i){
      std::getline(_od_file,_line);
      _words = split(_line, ' ');
      if (_words.size() == 2) {
        // std::cout << "Processing: " << _line << "\n";
        _dest_ID = TInt(std::stoi(_words[0]));
        _node_ID = TInt(std::stoi(_words[1]));
        od_factory -> make_destination(_dest_ID);
      }
    }      
  }
  _od_file.close();
  return 0;
}

PNEGraph MNM_IO::build_graph(std::string file_folder, MNM_ConfReader *conf_reader)
{
  /* find file */
  std::string _network_name = conf_reader -> get_string("network_name");
  std::string _graph_file_name = file_folder + "/" + _network_name;
  std::ifstream _graph_file;
  _graph_file.open(_graph_file_name, std::ios::in);

  TInt _num_of_link = conf_reader -> get_int("num_of_link");

  // printf("Start build graph.\n");
  PNEGraph _graph = PNEGraph::TObj::New();
  
  int _link_ID, _from_ID, _to_ID;
  std::string _line;
  std::vector<std::string> _words;
  std::getline(_graph_file,_line); // skip one line
  for (int i=0; i<_num_of_link; ++i){
    std::getline(_graph_file,_line);
    _words = split(_line, ' ');
    if (_words.size() == 3){
      // std::cout << "Processing: " << _line << "\n";
      _link_ID = TInt(std::stoi(_words[0]));
      _from_ID = TInt(std::stoi(_words[1]));
      _to_ID = TInt(std::stoi(_words[2]));
    if (! _graph->IsNode(_from_ID)) { _graph->AddNode(_from_ID); }
    if (! _graph->IsNode(_to_ID)) { _graph->AddNode(_to_ID); }
    _graph->AddEdge(_from_ID, _to_ID, _link_ID);
    }
  }
  _graph->Defrag();
  return _graph;
}



int MNM_IO::build_demand(std::string file_folder, MNM_ConfReader *conf_reader, MNM_OD_Factory *od_factory)
{
  /* find file */
  std::string _demand_file_name = file_folder + "/MNM_input_demand";
  std::ifstream _demand_file;
  _demand_file.open(_demand_file_name, std::ios::in);

  /* read config */
  TInt _max_interval = conf_reader -> get_int("max_interval"); 
  TInt _num_OD = conf_reader -> get_int("OD_pair");

  /* build */
  TInt _O_ID, _D_ID;
  MNM_Origin *_origin;
  MNM_Destination *_dest;
  std::string _line;
  std::vector<std::string> _words;
  if (_demand_file.is_open())
  {
    // printf("Start build demand profile.\n");
    TFlt *_demand_vector = (TFlt*) malloc(sizeof(TFlt) * _max_interval);
    memset(_demand_vector, 0x0, sizeof(TFlt) * _max_interval);
    std::getline(_demand_file,_line); //skip the first line
    for (int i=0; i < _num_OD; ++i){
      std::getline(_demand_file,_line);
      // std::cout << "Processing: " << _line << "\n";
      _words = split(_line, ' ');
      if (TInt(_words.size()) == (_max_interval + 2)) {
        _O_ID = TInt(std::stoi(_words[0]));
        _D_ID = TInt(std::stoi(_words[1]));
        for(int j=0; j<_max_interval; ++j){
          _demand_vector[j] = TFlt(std::stod(_words[j+2]));
        }
        _origin = od_factory -> get_origin(_O_ID);
        _dest = od_factory ->get_destination(_D_ID);
        _origin -> add_dest_demand(_dest, _demand_vector);
      }
      else{
        printf("Something wrong in build_demand!\n");
        free(_demand_vector);
        exit(-1);
      }
    }
    free(_demand_vector);
    _demand_file.close();
  }  
  return 0;
}



Path_Table *MNM_IO::load_path_table(std::string file_name, PNEGraph graph, TInt num_path)
{
  printf("Loading Path Table!\n");
  TInt Num_Path = num_path;

  std::ifstream _path_table_file;
  _path_table_file.open(file_name, std::ios::in);
  Path_Table *_path_table = new Path_Table();

  /* read file */
  std::string _line;
  std::vector<std::string> _words;
  TInt _origin_node_ID, _dest_node_ID, _node_ID;
  std::unordered_map<TInt, MNM_Pathset*> *_new_map;
  MNM_Pathset *_pathset;
  MNM_Path *_path;
  TInt _from_ID, _to_ID, _link_ID;
  if (_path_table_file.is_open()){
    for (int i=0; i<Num_Path; ++i){
      std::getline(_path_table_file,_line);
      _words = split(_line, ' ');
      if (_words.size() >= 2){
        _origin_node_ID = TInt(std::stoi(_words[0]));
        _dest_node_ID = TInt(std::stoi(_words.back()));
        if (_path_table -> find(_origin_node_ID) == _path_table -> end()){
          _new_map = new std::unordered_map<TInt, MNM_Pathset*>();
          _path_table -> insert(std::pair<TInt, std::unordered_map<TInt, MNM_Pathset*>*>(_origin_node_ID, _new_map));
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
  else{
    printf("Can't open path table file!\n");
    exit(-1);
  }
   printf("Finish Loading Path Table!\n");
  return _path_table;
}


int MNM_IO::build_vms_facotory(std::string file_folder, PNEGraph graph, TInt num_vms, MNM_Vms_Factory *vms_factory)
{
  /* find file */
  std::string _vms_file_name = file_folder + "/MNM_input_vms";
  std::ifstream _vms_file;
  _vms_file.open(_vms_file_name, std::ios::in); 

  std::string _line;
  std::vector<std::string> _words;
  TInt _vms_ID, _link_ID;
  if (_vms_file.is_open())
  {
    // printf("Start build demand profile.\n");
    std::getline(_vms_file,_line); //skip the first line
    for (int i=0; i < num_vms; ++i){
      std::getline(_vms_file,_line);
      // std::cout << "Processing: " << _line << "\n";
      _words = split(_line, ' ');
      if (TInt(_words.size()) == 2) {
        _vms_ID = TInt(std::stoi(trim(_words[0])));
        _link_ID = TInt(std::stoi(trim(_words[1])));
        vms_factory -> make_link_vms(_vms_ID, _link_ID, graph);
      }
      else{
        printf("Something wrong in build_vms!\n");
        exit(-1);
      }
    }
    _vms_file.close();
  }  
  return 0;
}


int MNM_IO::read_int_float(std::string file_name, std::unordered_map<TInt, TFlt>* reader)
{
  /* find file */
  std::ifstream _file;
  _file.open(file_name, std::ios::in);   

  std::string _line;
  std::vector<std::string> _words;
  TInt _int;
  TFlt _float;
  if (_file.is_open())
  {
    printf("Start build read_int_float.\n");
    std::getline(_file,_line);
    std::cout << "Processing: " << _line << "\n";
    TInt num_record = TInt(std::stoi(trim(_line)));
    printf("Total is %d\n", num_record());
    for (int i=0; i < num_record; ++i){
      std::getline(_file,_line);
      // std::cout << "Processing: " << _line << "\n";
      _words = split(_line, ' ');
      if (TInt(_words.size()) == 2) {
        _int = TInt(std::stoi(trim(_words[0])));
        _float = TFlt(std::stof(trim(_words[1])));
        reader -> insert(std::pair<TInt, TFlt>(_int, _float));
      }
      else{
        printf("Something wrong in read_int_float!\n");
        exit(-1);
      }
    }
    _file.close();
  }  
  return 0;
}

int MNM_IO::read_int(std::string file_name, std::vector<TInt>* reader)
{
  /* find file */
  std::ifstream _file;
  _file.open(file_name, std::ios::in);   

  std::string _line;
  TInt _int;
  if (_file.is_open())
  {
    printf("Start read int file.\n");
    std::getline(_file,_line);
    std::cout << "Processing: " << _line << "\n";
    TInt num_record = TInt(std::stoi(trim(_line)));
    printf("Total is %d\n", num_record());
    for (int i=0; i < num_record; ++i){
      std::getline(_file,_line);
      // std::cout << "Processing: " << _line << "\n";
      _int = TInt(std::stoi(trim(_line)));
      reader -> push_back(_int);
    }
    _file.close();
  }  
  return 0;
}

int MNM_IO::build_workzone_list(std::string file_folder, MNM_Workzone* workzone)
{
  /* find file */
  std::string _workzone_file_name = file_folder + "/MNM_input_workzone";
  std::ifstream _workzone_file;
  _workzone_file.open(_workzone_file_name, std::ios::in);  

  std::string _line;
  std::vector<std::string> _words;
  TInt _link_ID;

  if (_workzone_file.is_open())
  {
    printf("Start build workzone profile.\n");
    std::getline(_workzone_file,_line);
    TInt num_workzone = TInt(std::stoi(trim(_line)));
    for (int i=0; i < num_workzone; ++i){
      std::getline(_workzone_file,_line);
      // std::cout << "Processing: " << _line << "\n";
      _words = split(_line, ' ');
      if (TInt(_words.size()) == 1) {
        _link_ID = TInt(std::stoi(trim(_words[0])));
        Link_Workzone _w = {_link_ID};
        workzone -> m_workzone_list.push_back(_w);
      }
      else{
        printf("Something wrong in build_workzone!\n");
        exit(-1);
      }
    }
    _workzone_file.close();
  }  

  return 0;

}

int MNM_IO::dump_cumulative_curve(std::string file_folder, MNM_Link_Factory *link_factory)
{
  /* find file */
  std::string _cc_file_name = file_folder + "/cc_record";
  std::ofstream _cc_file;
  _cc_file.open(_cc_file_name, std::ios::out);  

  MNM_Dlink *_link;
  for (auto _link_it = link_factory -> m_link_map.begin(); _link_it != link_factory -> m_link_map.end(); _link_it++){
    _link = _link_it -> second;
    std::string _temp_s = std::to_string(_link -> m_link_ID) + ",";
    if (_link -> m_N_in != NULL) {
      std::string _temp_s_in = _temp_s + "in," + _link -> m_N_in -> to_string() + "\n";
      _cc_file << _temp_s_in;
    }
    if (_link -> m_N_out != NULL) {
      std::string _temp_s_out = _temp_s + "out," + _link -> m_N_out -> to_string() + "\n";
      _cc_file << _temp_s_out;
    }
  }
  _cc_file.close();
  return 0;
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


