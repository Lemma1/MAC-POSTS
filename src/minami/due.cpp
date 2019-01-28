#include "due.h"
#include "dta_gradient_utls.h"
#include "ults.h"

// #include <assert.h>
#include "limits.h"

MNM_Due::MNM_Due(std::string file_folder)
{
  m_file_folder = file_folder;
  m_dta_config = new MNM_ConfReader(m_file_folder + "/config.conf", "DTA");
  IAssert(m_dta_config -> get_string("routing_type") == "Due"); 
  IAssert(m_dta_config -> get_int("total_interval") > 0);
  IAssert(m_dta_config -> get_int("total_interval")  >=  
            m_dta_config -> get_int("assign_frq") * m_dta_config -> get_int("max_interval"));
  m_unit_time = m_dta_config -> get_float("unit_time");
  m_total_loading_inter = m_dta_config -> get_int("total_interval");
  m_due_config = new MNM_ConfReader(m_file_folder + "/config.conf", "DUE");
  m_total_assign_inter = m_dta_config -> get_int("max_interval");
  m_path_table = nullptr;
  m_od_factory = nullptr;
  m_early_rate = m_due_config -> get_float("early_rate");
  m_late_rate = m_due_config -> get_float("late_rate");
  m_target_time = m_due_config -> get_float("target_time");
  m_step_size = 1.0;
  m_cost_map = std::unordered_map<TInt, TFlt*>();
}

// int MNM_Due::init_path_table()
// {
//   std::string _file_name = m_due_conf -> get_string("path_file_name");
//   std::string _whole_path = m_file_folder + "/" + _file_name;
//   TInt _num_path = m_due_conf -> get_int("num_path");
//   m_path_table = MNM_IO::load_path_table(_whole_path, m_graph, _num_path);
//   return 0;
// }




MNM_Due::~MNM_Due()
{
  if (m_dta_config != nullptr) delete m_dta_config;
  if (m_due_config != nullptr) delete m_due_config;
  if (m_od_factory != nullptr) delete m_od_factory;
}

// int MNM_Due::initialize()
// {
//   // m_graph = MNM_IO::build_graph(m_file_folder, m_dta_config);
//   // MNM_IO::build_od_factory(m_file_folder, m_dta_config, m_od_factory);
//   return 0;
// }

MNM_Dta* MNM_Due::run_dta(bool verbose)
{ 
  MNM_Dta *_dta = new MNM_Dta(m_file_folder);
  _dta -> build_from_files();
  _dta -> m_od_factory = m_od_factory;
  _dta -> m_routing -> init_routing(m_path_table);
  _dta -> hook_up_node_and_link();
  // printf("Checking......\n");
  // _dta -> is_ok();

  for (auto _link_it = _dta -> m_link_factory -> m_link_map.begin(); _link_it != _dta -> m_link_factory -> m_link_map.end(); _link_it++){
    _link_it -> second -> install_cumulative_curve();
  }
  _dta -> pre_loading();
  _dta -> loading(verbose);
  return _dta;
}



TFlt MNM_Due::compute_merit_function()
{
  TFlt _tt, _depart_time, _dis_utl, _lowest_dis_utl;
  TFlt _total_gap = 0.0;
  for(auto _it : *m_path_table){
    for (auto _it_it : *(_it.second)){
      for (int _col = 0; _col < m_total_assign_inter; _col++){
        _lowest_dis_utl = DBL_MAX;
        for (MNM_Path* _path : _it_it.second -> m_path_vec){
          _depart_time = TFlt(_col);
          _tt = get_tt(_depart_time, _path);
          _dis_utl = get_disutility(_depart_time, _tt);
          if (_dis_utl < _lowest_dis_utl) _lowest_dis_utl = _dis_utl; 
        }
        for (MNM_Path* _path : _it_it.second -> m_path_vec){
          _depart_time = TFlt(_col);
          _tt = get_tt(_depart_time, _path);
          _dis_utl = get_disutility(_depart_time, _tt);
          _total_gap += _dis_utl - _lowest_dis_utl;
        }        
      }
    }
  }
  return _total_gap;
}



TFlt MNM_Due::get_disutility(TFlt depart_time, TFlt tt)
{
  TFlt _arrival_time = depart_time + tt;
  if (_arrival_time >= m_target_time){
    return tt + m_late_rate * (_arrival_time - m_target_time);
  }
  else{
    return tt + m_early_rate * (m_target_time - _arrival_time);
  }
  throw std::runtime_error("Error in MNM_Due::get_disutility");
}


TFlt MNM_Due::get_tt(TFlt depart_time, MNM_Path* path)
{
  TFlt _cur_time = depart_time;
  TInt _query_time;
  for (TInt _link_ID : path -> m_link_vec){
    _query_time = MNM_Ults::min(TInt(_cur_time), TInt(m_total_loading_inter-1));
    _cur_time += m_cost_map[_link_ID][_query_time];
  }
  return _cur_time - depart_time;
}


int MNM_Due::build_cost_map(MNM_Dta *dta)
{
  MNM_Dlink *_link;
  for(auto _link_it : dta -> m_link_factory -> m_link_map){
    _link = _link_it.second;
    for (int i=0; i<m_total_loading_inter; i++){
      m_cost_map[_link_it.first][i] = MNM_DTA_GRADIENT::get_travel_time(_link, TFlt(i * m_unit_time)) / m_unit_time;
    } 
  }
  return 0;
}
//*************************************************************************
//                                MNM_Due_Msa
//*************************************************************************

MNM_Due_Msa::MNM_Due_Msa(std::string file_folder)
  : MNM_Due::MNM_Due ( file_folder )
{
  
}

MNM_Due_Msa::~MNM_Due_Msa()
{
  for (auto _cost_it: m_cost_map){
    delete _cost_it.second;
  }
  m_cost_map.clear();
}


int MNM_Due_Msa::initialize()
{
  MNM_Dta *_dta = new MNM_Dta(m_file_folder);
  _dta -> build_from_files();
  m_path_table =  MNM::build_shortest_pathset(_dta -> m_graph, 
                      _dta -> m_od_factory, _dta -> m_link_factory);
  MNM::allocate_path_table_buffer(m_path_table, m_total_assign_inter);
  for(auto _link_it : _dta -> m_link_factory -> m_link_map){
    m_cost_map[_link_it.first] = new TFlt[m_total_loading_inter];
  }
  switch(m_due_config -> get_int("init_demand_split")){
    case 0:
      m_od_factory = _dta -> m_od_factory;
      break;
    case 1:
      throw std::runtime_error("Not inplement yet");
      break;
    default:
      throw std::runtime_error("Unknown init method");
  }
  return 0;
}

int MNM_Due_Msa::init_route_choice()
{
  TFlt _len;
  for(auto _it : *m_path_table){
    for (auto _it_it : *(_it.second)){
      _len = TFlt(_it_it.second -> m_path_vec.size());
      for (MNM_Path* _path : _it_it.second -> m_path_vec){
        for (int _col = 0; _col < m_total_assign_inter; _col++){
          
        }
      }
    }
  }   
  return 0;
}





std::pair<MNM_Path*, TInt> MNM_Due_Msa::get_best_route(TInt o_node_ID, 
                                                        MNM_TDSP_Tree* tdsp_tree)
{
  TFlt _cur_best_cost= TFlt(std::numeric_limits<double>::max());
  TInt _cur_best_time = -1;
  TFlt _tmp_tt, _tmp_cost;
  for (int i = 0; i < tdsp_tree -> m_max_interval; ++i){
    _tmp_tt = tdsp_tree -> get_distance_to_destination(o_node_ID, TFlt(i));
    _tmp_cost = get_disutility(TFlt(i) ,_tmp_tt);
    if (_tmp_cost < _cur_best_cost){
      _cur_best_cost = _tmp_cost;
      _cur_best_time = i;
    }
  }
  IAssert(_cur_best_time >= 0);
  MNM_Path *_path = new MNM_Path();
  tdsp_tree -> get_tdsp(o_node_ID, _cur_best_time, m_cost_map, _path);
  std::pair<MNM_Path*, TInt> _best = std::make_pair(_path, _cur_best_time);
  return _best;
}



int MNM_Due_Msa::update_path_table(MNM_Dta *dta, int iter)
{
  MNM_Origin *_orig;
  MNM_Destination *_dest;
  TInt _orig_node_ID, _dest_node_ID;
  std::pair<MNM_Path*, TInt> _path_result;
  MNM_Path* _path;
  MNM_Pathset* _path_set;
  for (auto _it : dta -> m_od_factory -> m_destination_map){
    _dest = _it.second;
    _dest_node_ID = _dest -> m_dest_node -> m_node_ID;
    MNM_TDSP_Tree *_tdsp_tree = new MNM_TDSP_Tree(_dest_node_ID, dta -> m_graph, m_total_loading_inter);
    _tdsp_tree -> initialize();
    build_cost_map(dta);
    // printf("111\n");
    _tdsp_tree -> update_tree(m_cost_map);
    for (auto _map_it : dta -> m_od_factory -> m_origin_map){
      _orig = _map_it.second;
      _orig_node_ID = _orig -> m_origin_node -> m_node_ID;
      _path_result = get_best_route(_orig_node_ID, _tdsp_tree);
      _path = _path_result.first;
      _path_set = MNM::get_pathset(m_path_table, _orig_node_ID, _dest_node_ID);
      TFlt _len = TFlt(_path_set -> m_path_vec.size());
      if (_path_set -> is_in(_path)){
        printf("Update current pathset\n");
        for (auto tmp_path : _path_set -> m_path_vec){
          if (tmp_path == _path){
            tmp_path -> m_buffer[int(_path_result.second)] += m_step_size / TFlt(iter + 1);
          }
          else{
            tmp_path -> m_buffer[int(_path_result.second)] -= m_step_size / TFlt(iter + 1)/ (_len - 1);
          }
        }
        delete _path;
      }
      else{
        printf("Adding new path\n");
        _path -> allocate_buffer(m_total_assign_inter);
        _path -> m_buffer[int(_path_result.second)] += m_step_size / TFlt(iter + 1);
        for (auto tmp_path : _path_set -> m_path_vec){
          tmp_path -> m_buffer[int(_path_result.second)] -= m_step_size / TFlt(iter + 1)/ _len;
        }
        _path_set -> m_path_vec.push_back(_path);
      }
    }
  }
  return 0;
}