#include "statistics.h"

MNM_Statistics::MNM_Statistics(std::string file_folder, MNM_ConfReader *conf_reader, MNM_ConfReader *record_config, 
                    MNM_OD_Factory *od_factory, MNM_Node_Factory *node_factory, MNM_Link_Factory *link_factory)
{
  m_file_folder = file_folder;
  m_global_config = conf_reader;
  m_self_config = record_config;
  m_od_factory = od_factory;
  m_node_factory = node_factory;
  m_link_factory = link_factory;

  m_record_interval_volume = std::unordered_map<TInt, TFlt>();
  m_load_interval_volume = std::unordered_map<TInt, TFlt>();
  m_record_interval_tt = std::unordered_map<TInt, TFlt>();
  m_load_interval_tt = std::unordered_map<TInt, TFlt>();

  m_link_order = std::vector<MNM_Dlink*>();

  if (m_load_interval_volume_file.is_open()) m_load_interval_volume_file.close();
  if (m_record_interval_volume_file.is_open()) m_record_interval_volume_file.close();
  if (m_load_interval_tt_file.is_open()) m_load_interval_tt_file.close();
  if (m_record_interval_tt_file.is_open()) m_record_interval_tt_file.close();

  init_record_value();
}


MNM_Statistics::~MNM_Statistics()
{
  delete m_self_config;
  m_record_interval_volume.clear();
  m_load_interval_volume.clear();
  m_record_interval_tt.clear();
  m_load_interval_tt.clear();
}

int MNM_Statistics::init_record_value()
{
  switch(m_self_config -> get_int("rec_volume")){
    case 1:
      m_record_volume = true;
      break;
    case 0:
      m_record_volume = false;
      break;
    default:
      printf("MNM_Statistics::init_record_value_volume::Something wrong!\n");
      exit(-1);
  }
  switch(m_self_config -> get_int("rec_tt")){
    case 1:
      m_record_tt = true;
      break;
    case 0:
      m_record_tt = false;
      break;
    default:
      printf("MNM_Statistics::init_record_value_tt::Something wrong!\n");
      exit(-1);
  }
  return 0;
}

int MNM_Statistics::init_record()
{
  TInt _link_ID;
  std::string _file_name;
  if (m_record_volume){
    for (auto _link_it = m_link_factory -> m_link_map.begin(); _link_it != m_link_factory -> m_link_map.end(); _link_it++){
      _link_ID = _link_it -> first;
      m_load_interval_volume.insert(std::pair<TInt, TFlt>(_link_ID, TFlt(0)));
      m_record_interval_volume.insert(std::pair<TInt, TFlt>(_link_ID, TFlt(0)));
    }

    
    if (m_self_config -> get_int("volume_load_automatic_rec") == 1 || m_self_config -> get_int("volume_record_automatic_rec") == 1){
      std::string _str;
      for (auto _link_it = m_link_factory -> m_link_map.begin(); _link_it != m_link_factory -> m_link_map.end(); _link_it++){
        _str += std::to_string(_link_it -> first) + " ";
      }
      _str.pop_back();
      _str += "\n";

      if (m_self_config -> get_int("volume_load_automatic_rec") == 1){
        _file_name = m_file_folder + "/" + m_self_config -> get_string("rec_folder") + "/MNM_output_load_interval_volume";
        m_load_interval_volume_file.open(_file_name, std::ofstream::out);   
        if (!m_load_interval_volume_file.is_open()){
          printf("Error happens when open m_load_interval_volume_file\n");
          exit(-1);
        }
        m_load_interval_volume_file << _str;
      }

      if (m_self_config -> get_int("volume_record_automatic_rec") == 1){
        _file_name = m_file_folder + "/" + m_self_config -> get_string("rec_folder") + "/MNM_output_record_interval_volume";
        m_record_interval_volume_file.open(_file_name, std::ofstream::out);     
        if (!m_record_interval_volume_file.is_open()){
          printf("Error happens when open m_record_interval_volume_file\n");
          exit(-1);
        }
        m_record_interval_volume_file << _str;
      }
    }
  }

  if (m_record_tt){
    for (auto _link_it = m_link_factory -> m_link_map.begin(); _link_it != m_link_factory -> m_link_map.end(); _link_it++){
      _link_ID = _link_it -> first;
      m_load_interval_tt.insert(std::pair<TInt, TFlt>(_link_ID, TFlt(0)));
      m_record_interval_tt.insert(std::pair<TInt, TFlt>(_link_ID, TFlt(0)));
    }

    
    if (m_self_config -> get_int("tt_load_automatic_rec") == 1 || m_self_config -> get_int("tt_record_automatic_rec") == 1){
      std::string _str;
      for (auto _link_it = m_link_factory -> m_link_map.begin(); _link_it != m_link_factory -> m_link_map.end(); _link_it++){
        _str += std::to_string(_link_it -> first) + " ";
      }
      _str.pop_back();
      _str += "\n";

      if (m_self_config -> get_int("tt_load_automatic_rec") == 1){
        _file_name = m_file_folder + "/" + m_self_config -> get_string("rec_folder") + "/MNM_output_load_interval_tt";
        m_load_interval_tt_file.open(_file_name, std::ofstream::out);   
        if (!m_load_interval_tt_file.is_open()){
          printf("Error happens when open m_load_interval_tt_file\n");
          exit(-1);
        }
        m_load_interval_tt_file << _str;
      }

      if (m_self_config -> get_int("tt_record_automatic_rec") == 1){
        _file_name = m_file_folder + "/" + m_self_config -> get_string("rec_folder") + "/MNM_output_record_interval_tt";
        m_record_interval_tt_file.open(_file_name, std::ofstream::out);     
        if (!m_record_interval_tt_file.is_open()){
          printf("Error happens when open m_record_interval_tt_file\n");
          exit(-1);
        }
        m_record_interval_tt_file << _str;
      }
    }
  }

  for (auto _link_it = m_link_factory -> m_link_map.begin(); _link_it != m_link_factory -> m_link_map.end(); _link_it++){
    m_link_order.push_back(_link_it -> second);
  }  
  return 0;
}



int MNM_Statistics::record_loading_interval_condition(TInt timestamp)
{
  std::string _str;
  MNM_Dlink *_link;
  TFlt _flow,  _tt;
  if (m_record_volume && m_load_interval_volume_file.is_open()){
    for (auto _link_it = m_link_order.begin(); _link_it != m_link_order.end(); _link_it++){
      _link = *_link_it;
      _flow = m_load_interval_volume.find(_link -> m_link_ID) -> second;
      _str += std::to_string(_flow) + " ";
    }
    _str.pop_back();
    _str += "\n"; 
    m_load_interval_volume_file << _str;    
  }

  _str.clear();
  if (m_record_tt && m_load_interval_tt_file.is_open()){
    for (auto _link_it = m_link_order.begin(); _link_it != m_link_order.end(); _link_it++){
      _link = *_link_it;
      _tt = m_load_interval_tt.find(_link -> m_link_ID) -> second;
      _str += std::to_string(_tt) + " ";
    }
    _str.pop_back();
    _str += "\n"; 
    m_load_interval_tt_file << _str;    
  }

  return 0;
}

int MNM_Statistics::record_record_interval_condition(TInt timestamp)
{
  std::string _str;
  MNM_Dlink *_link;
  TFlt _flow, _tt;
  if (m_record_volume && m_record_interval_volume_file.is_open()){
    for (auto _link_it = m_link_order.begin(); _link_it != m_link_order.end(); _link_it++){
      _link = *_link_it;
      _flow = m_record_interval_volume.find(_link -> m_link_ID) -> second;
      _str += std::to_string(_flow) + " ";
    }
    _str.pop_back();
    _str += "\n"; 
    m_record_interval_volume_file << _str;      
  }
  _str.clear();

  if (m_record_tt && m_record_interval_tt_file.is_open()){
    for (auto _link_it = m_link_order.begin(); _link_it != m_link_order.end(); _link_it++){
      _link = *_link_it;
      _tt = m_record_interval_tt.find(_link -> m_link_ID) -> second;
      _str += std::to_string(_tt) + " ";
    }
    _str.pop_back();
    _str += "\n"; 
    m_record_interval_tt_file << _str;      
  }
  return 0;
}

int MNM_Statistics::post_record()
{
  if (m_record_volume){
    if (m_load_interval_volume_file.is_open()){
      m_load_interval_volume_file.close();
    }
    if (m_record_interval_volume_file.is_open()) m_record_interval_volume_file.close();
  }
  if (m_record_tt){
    if (m_load_interval_tt_file.is_open()){
      m_load_interval_tt_file.close();
    }
    if (m_record_interval_tt_file.is_open()) m_record_interval_tt_file.close();
  }
  return 0;
}
/**************************************************************************
                              LRN
**************************************************************************/

MNM_Statistics_Lrn::MNM_Statistics_Lrn(std::string file_folder, MNM_ConfReader *conf_reader, MNM_ConfReader *record_config, 
                  MNM_OD_Factory *od_factory, MNM_Node_Factory *node_factory, MNM_Link_Factory *link_factory)
  : MNM_Statistics::MNM_Statistics(file_folder,conf_reader, record_config, od_factory, node_factory, link_factory)
{
  m_n = record_config -> get_int("rec_mode_para");
  m_to_be_volume = std::unordered_map<TInt, TFlt>();
  m_to_be_tt = std::unordered_map<TInt, TFlt>();
}

MNM_Statistics_Lrn::~MNM_Statistics_Lrn()
{

}

int MNM_Statistics_Lrn::update_record(TInt timestamp)
{
  MNM_Dlink *_link;
  TFlt _flow;
  if (m_record_volume){
    if ((timestamp) % m_n == 0 || timestamp == 0){
      for (auto _link_it = m_link_factory -> m_link_map.begin(); _link_it != m_link_factory -> m_link_map.end(); _link_it++){
        _link = _link_it -> second;
        _flow = _link -> get_link_flow();
        m_load_interval_volume.find(_link -> m_link_ID) -> second = _flow;
        m_record_interval_volume.find(_link -> m_link_ID) -> second = m_to_be_volume.find(_link -> m_link_ID) -> second + _flow/TFlt(m_n);
        m_to_be_volume.find(_link -> m_link_ID) -> second = TFlt(0);
      }
    }
    else{
      for (auto _link_it = m_link_factory -> m_link_map.begin(); _link_it != m_link_factory -> m_link_map.end(); _link_it++){
        _link = _link_it -> second;
        _flow = _link -> get_link_flow();
        m_load_interval_volume.find(_link -> m_link_ID) -> second = _flow;
        m_to_be_volume.find(_link -> m_link_ID) -> second += _flow/TFlt(m_n);
      }      
    }
  } 
  if (m_record_tt){
    if ((timestamp) % m_n == 0 || timestamp == 0){
      for (auto _link_it = m_link_factory -> m_link_map.begin(); _link_it != m_link_factory -> m_link_map.end(); _link_it++){
        _link = _link_it -> second;
        _flow = _link -> get_link_tt();
        m_load_interval_tt.find(_link -> m_link_ID) -> second = _flow;
        m_record_interval_tt.find(_link -> m_link_ID) -> second = m_to_be_tt.find(_link -> m_link_ID) -> second + _flow/TFlt(m_n);
        m_to_be_tt.find(_link -> m_link_ID) -> second = TFlt(0);
      }
    }
    else{
      for (auto _link_it = m_link_factory -> m_link_map.begin(); _link_it != m_link_factory -> m_link_map.end(); _link_it++){
        _link = _link_it -> second;
        _flow = _link -> get_link_tt();
        m_load_interval_tt.find(_link -> m_link_ID) -> second = _flow;
        m_to_be_tt.find(_link -> m_link_ID) -> second += _flow/TFlt(m_n);
      }      
    }
  } 

  record_loading_interval_condition(timestamp);

  if ((timestamp ) % m_n == 0  || timestamp == 0){
    record_record_interval_condition(timestamp);
  }
  return 0;
}

int MNM_Statistics_Lrn::init_record()
{
  MNM_Statistics::init_record();
  TInt _link_ID;
  if (m_record_volume){
    for (auto _link_it = m_link_factory -> m_link_map.begin(); _link_it != m_link_factory -> m_link_map.end(); _link_it++){
      _link_ID = _link_it -> first;
      m_to_be_volume.insert(std::pair<TInt, TFlt>(_link_ID, TFlt(0)));
    }
  }
  if (m_record_tt){
    for (auto _link_it = m_link_factory -> m_link_map.begin(); _link_it != m_link_factory -> m_link_map.end(); _link_it++){
      _link_ID = _link_it -> first;
      m_to_be_tt.insert(std::pair<TInt, TFlt>(_link_ID, TFlt(0)));
    }
  }
  return 0;
}


int MNM_Statistics_Lrn::post_record()
{
  MNM_Statistics::post_record();
  return 0;
}

