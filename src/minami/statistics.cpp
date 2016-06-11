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

  m_record_interval_volume = std::map<MNM_Dlink*, TFlt>();
  m_load_interval_volume = std::map<MNM_Dlink*, TFlt>();

  if (m_load_interval_volume_file.is_open()) m_load_interval_volume_file.close();
  if (m_record_interval_volume_file.is_open()) m_record_interval_volume_file.close();

  init_record_value();
}


MNM_Statistics::~MNM_Statistics()
{
  free(m_self_config);
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
      printf("MNM_Statistics::init_record_value::Something wrong!\n");
      exit(-1);
  }
  return 0;
}

int MNM_Statistics::init_record()
{
  MNM_Dlink *__link;
  std::string __file_name;
  if (m_record_volume){
    for (auto __link_it = m_link_factory -> m_link_map.begin(); __link_it != m_link_factory -> m_link_map.end(); __link_it++){
      __link = __link_it -> second;
      m_load_interval_volume.insert(std::pair<MNM_Dlink*, TFlt>(__link, TFlt(0)));
      m_record_interval_volume.insert(std::pair<MNM_Dlink*, TFlt>(__link, TFlt(0)));
    }

    
    if (m_self_config -> get_int("load_automatic_rec") == 1 || m_self_config -> get_int("record_automatic_rec") == 1){
      std::string __str;
      for (auto __link_it = m_link_factory -> m_link_map.begin(); __link_it != m_link_factory -> m_link_map.end(); __link_it++){
        __str += std::to_string(__link_it -> first) + " ";
      }
      __str.pop_back();
      __str += "\n";

      if (m_self_config -> get_int("load_automatic_rec") == 1){
        __file_name = m_file_folder + "/" + m_self_config -> get_string("rec_folder") + "/MNM_output_load_interval_volume";
        m_load_interval_volume_file.open(__file_name, std::ofstream::out);   
        if (!m_load_interval_volume_file.is_open()){
          printf("Error happens when open m_load_interval_volume_file\n");
          exit(-1);
        }
        m_load_interval_volume_file << __str;
      }

      if (m_self_config -> get_int("record_automatic_rec") == 1){
        __file_name = m_file_folder + "/" + m_self_config -> get_string("rec_folder") + "/MNM_output_record_interval_volume";
        m_record_interval_volume_file.open(__file_name, std::ofstream::out);     
        if (!m_record_interval_volume_file.is_open()){
          printf("Error happens when open m_record_interval_volume_file\n");
          exit(-1);
        }
        m_record_interval_volume_file << __str;
      }
    }
  }
  return 0;
}



int MNM_Statistics::record_loading_interval_condition(TInt timestamp)
{
  std::string __str;
  MNM_Dlink *__link;
  TFlt __flow;
  for (auto __link_it = m_link_factory -> m_link_map.begin(); __link_it != m_link_factory -> m_link_map.end(); __link_it++){
    __link = __link_it -> second;
    __flow = m_load_interval_volume.find(__link) -> second;
    __str += std::to_string(__flow) + " ";
  }
  __str.pop_back();
  __str += "\n"; 
  m_load_interval_volume_file << __str;
  return 0;
}

int MNM_Statistics::record_record_interval_condition(TInt timestamp)
{
  std::string __str;
  MNM_Dlink *__link;
  TFlt __flow;

  for (auto __link_it = m_link_factory -> m_link_map.begin(); __link_it != m_link_factory -> m_link_map.end(); __link_it++){
    __link = __link_it -> second;
    __flow = m_record_interval_volume.find(__link) -> second;
    __str += std::to_string(__flow) + " ";
  }
  __str.pop_back();
  __str += "\n"; 
  m_record_interval_volume_file << __str;   

  return 0;
}

int MNM_Statistics::post_record()
{
  if (m_record_volume){
    if (m_load_interval_volume_file.is_open()){
      printf("Closed\n");
      m_load_interval_volume_file.close();
    }
    if (m_record_interval_volume_file.is_open()) m_record_interval_volume_file.close();
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
  m_to_be_volume = std::map<MNM_Dlink*, TFlt>();
}

MNM_Statistics_Lrn::~MNM_Statistics_Lrn()
{

}

int MNM_Statistics_Lrn::update_record(TInt timestamp)
{
  MNM_Dlink *__link;
  TFlt __flow;
  if (m_record_volume){
    if ((timestamp + 1) % m_n == 0){
      for (auto __link_it = m_link_factory -> m_link_map.begin(); __link_it != m_link_factory -> m_link_map.end(); __link_it++){
        __link = __link_it -> second;
        __flow = __link -> get_link_flow();
        m_load_interval_volume.find(__link) -> second = __flow;
        m_record_interval_volume.find(__link) -> second = m_to_be_volume.find(__link) -> second + __flow/TFlt(m_n);
        m_to_be_volume.find(__link) -> second = TFlt(0);
      }
    }
    else{
      for (auto __link_it = m_link_factory -> m_link_map.begin(); __link_it != m_link_factory -> m_link_map.end(); __link_it++){
        __link = __link_it -> second;
        __flow = __link -> get_link_flow();
        m_load_interval_volume.find(__link) -> second = __flow;
        m_to_be_volume.find(__link) -> second += __flow/TFlt(m_n);
      }      
    }

  } 

  if (m_load_interval_volume_file.is_open()){
    record_loading_interval_condition(timestamp);
  }
  if (m_record_interval_volume_file.is_open() && (timestamp + 1) % m_n == 0){
    record_record_interval_condition(timestamp);
  }

  return 0;
}

int MNM_Statistics_Lrn::init_record()
{
  MNM_Statistics::init_record();
  MNM_Dlink *__link;
  if (m_record_volume){
    for (auto __link_it = m_link_factory -> m_link_map.begin(); __link_it != m_link_factory -> m_link_map.end(); __link_it++){
      __link = __link_it -> second;
      m_to_be_volume.insert(std::pair<MNM_Dlink*, TFlt>(__link, TFlt(0)));
    }
  }
  return 0;
}


int MNM_Statistics_Lrn::post_record()
{
  MNM_Statistics::post_record();
  return 0;
}

