#include "due.h"

// #include <assert.h>
#include "dta_gradient_utls.h"

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

int MNM_Due::run_dta(bool verbose)
{ 
  MNM_Dta *_dta = new MNM_Dta(m_file_folder);
  _dta -> build_from_files();
  _dta -> m_od_factory = m_od_factory;
  _dta -> m_routing -> init_routing(m_path_table);
  _dta -> hook_up_node_and_link();
  // printf("Checking......\n");
  // test_dta -> is_ok();

  for (auto _link_it = _dta -> m_link_factory -> m_link_map.begin(); _link_it != _dta -> m_link_factory -> m_link_map.end(); _link_it++){
    _link_it -> second -> install_cumulative_curve();
  }
  _dta -> loading(verbose);
  return 0;
}


MNM_Due_Msa::MNM_Due_Msa(std::string file_folder)
  : MNM_Due::MNM_Due ( file_folder )
{
  m_cost_map = std::unordered_map<TInt, TFlt*>();
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
  m_path_table =  MNM::build_shoartest_pathset(_dta -> m_graph, 
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
          _path -> m_buffer[_col] = TFlt(1.0) / _len;
        }
      }
    }
  }   
  return 0;
}


int MNM_Due_Msa::build_cost_map(MNM_Dta *dta)
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


std::pair<MNM_Path*, TFlt> MNM_Due_Msa::get_best_route(TInt o_node_ID, 
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
  tdsp_tree -> get_tdsp(o_node_ID, _cur_best_time, _path);
  std::pair<MNM_Path*, TFlt> _best = std::make_pair(_path, _cur_best_cost);
  return _best;
}


TFlt MNM_Due_Msa::get_disutility(TFlt depart_time, TFlt tt)
{
  TFlt _arrival_time = depart_time + tt;
  if (_arrival_time >= m_target_time){
    return tt + m_late_rate * (_arrival_time - m_target_time);
  }
  else{
    return tt + m_early_rate * (m_target_time - _arrival_time);
  }
  throw std::runtime_error("Error in MNM_Due_Msa::get_disutility");
}

int MNM_Due_Msa::update_path_table(MNM_Dta *dta)
{
  MNM_Origin *_orig;
  MNM_Destination *_dest;
  TInt _orig_node_ID, _dest_node_ID;
  std::pair<MNM_Path*, TFlt> _path_result;
  for (auto _it : dta -> m_od_factory -> m_destination_map){
    _dest = _it.second;
    _dest_node_ID = _dest -> m_dest_node -> m_node_ID;
    MNM_TDSP_Tree *_tdsp_tree = new MNM_TDSP_Tree(_dest_node_ID, dta -> m_graph, m_total_loading_inter);
    build_cost_map(dta);
    _tdsp_tree -> update_tree(m_cost_map);
    for (auto _map_it : dta -> m_od_factory -> m_origin_map){
      _orig = _map_it.second;
      _orig_node_ID = _orig -> m_origin_node -> m_node_ID;
      _path_result = get_best_route(_orig_node_ID, _tdsp_tree);
    }
  }
  return 0;
}