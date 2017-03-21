#include "realtime_dta.h"

MNM_Realtime_Dta::MNM_Realtime_Dta(std::string file_folder)
{
  m_file_folder = file_folder;
  m_path_table = NULL;
  m_before_shot = NULL;
  m_after_shot = NULL;
  m_od_factory = new MNM_OD_Factory();
  m_vms_factory = new MNM_Vms_Factory();
  initialize();
}


MNM_Realtime_Dta::~MNM_Realtime_Dta()
{
  delete m_dta_config;
  delete m_realtime_dta_config;
}

int MNM_Realtime_Dta::initialize()
{
  m_dta_config = new MNM_ConfReader(m_file_folder + "/config.conf", "DTA");
  m_realtime_dta_config = new MNM_ConfReader(m_file_folder + "/config.conf", "REALTIME_DTA");
  m_graph = MNM_IO::build_graph(m_file_folder, m_dta_config);
  MNM_IO::build_od_factory(m_file_folder, m_dta_config, m_od_factory);
  MNM_IO::build_demand(m_file_folder, m_dta_config, m_od_factory);
  // m_path_table = MNM_IO::load_path_table(m_file_name, m_graph, TInt num_path);
  // MNM_IO::hook_up_od_node(m_file_folder, m_dta_conf_reader, m_od_factory, m_before_shot -> node_factory);  

  m_average_link_tt = std::unordered_map<TInt, TFlt>();
  m_link_tt_difference = std::unordered_map<TInt, TFlt>();


  build_path_table();
  MNM::allocate_path_table_buffer(m_path_table, 4);

  MNM_IO::build_vms_facotory(m_file_folder, m_graph, m_realtime_dta_config -> get_int("num_vms"), m_vms_factory);
  m_vms_factory -> hook_path(m_path_table);

  m_estimation_iters = m_realtime_dta_config -> get_int("estimation_iters");
  m_optimization_iters = m_realtime_dta_config -> get_int("optimization_iters");
  m_prediction_length = m_realtime_dta_config -> get_int("prediction_length");
  m_estimation_length = m_realtime_dta_config -> get_int("estimation_length");
  m_sample_points = m_realtime_dta_config -> get_int("sample_point");
  m_total_assign_inter = m_dta_config -> get_int("max_interval");

  m_estimation_step_size = m_realtime_dta_config -> get_float("estimation_step_size");
  m_optimization_step_size = m_realtime_dta_config -> get_float("optimization_step_size");
  return 0;
}

int MNM_Realtime_Dta::init_running()
{
  if (m_before_shot != NULL) delete m_before_shot;
  if (m_after_shot != NULL) delete m_after_shot;
  m_before_shot = MNM::make_empty_screenshot(m_file_folder, m_dta_config, m_od_factory, m_graph);
  m_after_shot = MNM::make_empty_screenshot(m_file_folder, m_dta_config, m_od_factory, m_graph);

  m_average_link_tt.clear();
  m_link_tt_difference.clear();
  for (auto _link_it = m_graph->BegEI(); _link_it < m_graph->EndEI(); _link_it++){
    m_average_link_tt.insert(std::pair<TInt, TFlt>(_link_it.GetId(), 0));
    m_link_tt_difference.insert(std::pair<TInt, TFlt>(_link_it.GetId(), 0));
  }

  for(auto _it : *m_path_table){
    for (auto _it_it : *(_it.second)){
      for (MNM_Path* _path : _it_it.second -> m_path_vec){
        memset(_path -> buffer, 0x0, sizeof(TFlt) * 4);
        _path -> m_p = 0;
      }
    }
  } 

  MNM::normalize_path_table_p(m_path_table);
  return 0;
}

int MNM_Realtime_Dta::reset()
{
  if (m_before_shot != NULL) delete m_before_shot;
  if (m_after_shot != NULL) delete m_after_shot;
  m_before_shot = MNM::make_empty_screenshot(m_file_folder, m_dta_config, m_od_factory, m_graph);
  m_after_shot = MNM::make_empty_screenshot(m_file_folder, m_dta_config, m_od_factory, m_graph);
  return 0;
}

int MNM_Realtime_Dta::build_path_table()
{
  std::string _file_name = m_realtime_dta_config -> get_string("path_file_name");
  std::string _whole_path = m_file_folder + "/" + _file_name;
  TInt _num_path = m_realtime_dta_config -> get_int("num_path");
  m_path_table = MNM_IO::load_path_table(_whole_path, m_graph, _num_path);
  return 0;
}


int MNM_Realtime_Dta::get_estimation_gradient(MNM_Dta_Screenshot* screenshot,
                                            TInt max_inter, TInt assign_inter, Path_Table *path_table, 
                                            std::unordered_map<TInt, TFlt> *link_spd_map)
{
  TInt _grad_position = 1;

  TInt _cur_inter = 0;
  TInt _total_inter = max_inter;
  TInt _real_inter;
  MNM_Origin *_origin;
  MNM_Dnode *_node;
  MNM_Dlink *_link;
  MNM_Destination *_dest;

  MNM_Node_Factory *_node_factory = screenshot -> m_node_factory;
  MNM_Link_Factory *_link_factory = screenshot -> m_link_factory;
  MNM_Veh_Factory *_veh_factory = screenshot -> m_veh_factory;
  MNM_Routing *_routing = screenshot -> m_routing;
  TFlt num_link = TFlt(_link_factory -> m_link_map.size());

  MNM_Link_Tt *_link_tt = new MNM_Link_Tt(_link_factory, m_dta_config -> get_float("unit_time"));
  for (auto _map_it : m_average_link_tt){
    _map_it.second = 0;
  }
  for (auto _map_it : m_link_tt_difference){
    _map_it.second = 0;
  }
  printf("MNM: Prepare loading!\n");
  _routing -> init_routing(path_table);
  MNM_IO::hook_up_od_node(m_file_folder, m_dta_config, m_od_factory, _node_factory);  
  printf("Finish prepare routing\n");
  // m_statistics -> init_record();
  for (auto _node_it = _node_factory -> m_node_map.begin(); _node_it != _node_factory -> m_node_map.end(); _node_it++){
    _node = _node_it -> second;
    // printf("Node ID: %d\n", _node -> m_node_ID);
    _node -> prepare_loading();
  }
  while (_cur_inter < _total_inter){
    _real_inter = _cur_inter;
    // printf("-------------------------------    Interval %d   ------------------------------ \n", (int)_real_inter);
    // step 1: Origin release vehicle

    // printf("Realsing!\n");
    if (_cur_inter == 0){
      for (auto _origin_it = m_od_factory -> m_origin_map.begin(); _origin_it != m_od_factory -> m_origin_map.end(); _origin_it++){
        _origin = _origin_it -> second;
        _origin -> release_one_interval(_real_inter, _veh_factory, assign_inter, TFlt(0));
      }       
    }

    // printf("Routing!\n");
    // step 2: route the vehicle
    _routing -> update_routing(_real_inter);

    // printf("Moving through node!\n");
    // step 3: move vehicles through node
    for (auto _node_it = _node_factory -> m_node_map.begin(); _node_it != _node_factory -> m_node_map.end(); _node_it++){
      _node = _node_it -> second;
      // printf("Current node %d\n", _node_it -> first);
      _node -> evolve(_real_inter);
    }

    _link_tt -> update_tt(_real_inter);
    // printf("Moving through link\n");
    // step 4: move vehicles through link
    for (auto _link_it = _link_factory -> m_link_map.begin(); _link_it != _link_factory -> m_link_map.end(); _link_it++){
      _link = _link_it -> second;
      // printf("Current Link %d:, incomming %d, finished %d\n", _link -> m_link_ID, _link -> m_incoming_array.size(),  _link -> m_finished_array.size());
      _link -> clear_incoming_array();
      _link -> evolve(_real_inter);
      // _link -> print_info();
    }

    // printf("Receiving!\n");
    // step 5: Destination receive vehicle  
    for (auto _dest_it = m_od_factory -> m_destination_map.begin(); _dest_it != m_od_factory -> m_destination_map.end(); _dest_it++){
      _dest = _dest_it -> second;
      _dest -> receive(_real_inter);
    }

    // step 6: sample link tt
    if ((_cur_inter+1) % (max_inter / m_sample_points) == 0){
      for (auto _map_it : _link_tt -> m_tt_map){
        m_average_link_tt.find(_map_it.first -> m_link_ID) -> second += _map_it.second/TFlt(m_sample_points);
      }
    }
    // _link_tt -> print_info();
    _cur_inter ++;
  }

  for (auto _map_it : *link_spd_map){
    TInt _link_ID = _map_it.first;
    TFlt _measure_tt = _link_factory -> get_link(_link_ID) -> m_length / _map_it.second;
    m_link_tt_difference.find(_link_ID) -> second = m_average_link_tt.find(_link_ID)->second - _measure_tt;
    // printf("For link ID %d, the tt difference is %.4f\n", (int)_link_ID, (float)m_link_tt_difference.find(_link_ID)->second);
  }

  for(auto _it : *path_table){
    for (auto _it_it : *(_it.second)){
      for (MNM_Path* _path : _it_it.second -> m_path_vec){
        _path -> buffer[_grad_position] = 0;
      }
    }
  } 

  TFlt _tmp_tt;
  TFlt _demand;
  TFlt _tmp_grad;
  for(auto _it : *path_table){
    for (auto _it_it : *(_it.second)){
      _demand = MNM::get_demand_bynode(_it.first, _it_it.first, assign_inter, _node_factory);
      // printf("Demand is %.4f\n", _demand);
      for (MNM_Path* _path : _it_it.second -> m_path_vec){
        _tmp_grad = 0;
        for (TInt _link_ID : _path -> m_link_vec){
          _tmp_tt = m_average_link_tt.find(_link_ID) -> second;
          // printf("Average tt is %.4f, marginal cost is %.4f\n", (float)_tmp_tt, (float)MNM::calculate_link_mc(_link_factory->get_link(_link_ID), _tmp_tt));
          _tmp_grad += MNM_Ults::divide(m_link_tt_difference.find(_link_ID)-> second
                       * MNM::calculate_link_mc(_link_factory->get_link(_link_ID), _tmp_tt)
                       , _demand * num_link);
        }
        _path -> buffer[_grad_position] = _tmp_grad;
        // printf("grad is %.4f\n", _tmp_grad);
      }
    }
  } 
  // // m_statistics -> post_record();
  delete _link_tt;  
  return 0;
}

int MNM_Realtime_Dta::get_optimization_gradient(MNM_Dta_Screenshot* screenshot,
                                            TInt max_inter, TInt assign_inter, Path_Table *path_table)
{
  TInt _grad_position = 3;

  TInt _cur_inter = 0;
  TInt _total_inter = max_inter;
  TInt _real_inter;
  MNM_Origin *_origin;
  MNM_Dnode *_node;
  MNM_Dlink *_link;
  MNM_Destination *_dest;
  

  MNM_Node_Factory *_node_factory = screenshot -> m_node_factory;
  MNM_Link_Factory *_link_factory = screenshot -> m_link_factory;
  MNM_Veh_Factory *_veh_factory = screenshot -> m_veh_factory;
  MNM_Routing *_routing = screenshot -> m_routing;

  TFlt num_link = TFlt(_link_factory -> m_link_map.size());

  MNM_Link_Tt *_link_tt = new MNM_Link_Tt(_link_factory, m_dta_config -> get_float("unit_time"));
  for (auto _map_it : m_average_link_tt){
    _map_it.second = 0;
  }

  // printf("MNM: Prepare loading!\n");
  _routing -> init_routing(path_table);
  MNM_IO::hook_up_od_node(m_file_folder, m_dta_config, m_od_factory, _node_factory);  
  // printf("Finish prepare routing\n");
  // m_statistics -> init_record();
  for (auto _node_it = _node_factory -> m_node_map.begin(); _node_it != _node_factory -> m_node_map.end(); _node_it++){
    _node = _node_it -> second;
    // printf("Node ID: %d\n", _node -> m_node_ID);
    _node -> prepare_loading();
  }
  while (_cur_inter < _total_inter){
    _real_inter = _cur_inter;
    // printf("-------------------------------    Interval %d   ------------------------------ \n", (int)_real_inter);
    // step 1: Origin release vehicle

    // printf("Realsing!\n");
    if (_cur_inter == 0){
      for (auto _origin_it = m_od_factory -> m_origin_map.begin(); _origin_it != m_od_factory -> m_origin_map.end(); _origin_it++){
        _origin = _origin_it -> second;
        _origin -> release_one_interval(_real_inter, _veh_factory, assign_inter, TFlt(0));
      }       
    }

    // printf("Routing!\n");
    // step 2: route the vehicle
    _routing -> update_routing(_real_inter);

    // printf("Moving through node!\n");
    // step 3: move vehicles through node
    for (auto _node_it = _node_factory -> m_node_map.begin(); _node_it != _node_factory -> m_node_map.end(); _node_it++){
      _node = _node_it -> second;
      _node -> evolve(_real_inter);
    }

    _link_tt -> update_tt(_real_inter);
    // printf("Moving through link\n");
    // step 4: move vehicles through link
    for (auto _link_it = _link_factory -> m_link_map.begin(); _link_it != _link_factory -> m_link_map.end(); _link_it++){
      _link = _link_it -> second;
      // printf("Current Link %d:, incomming %d, finished %d\n", _link -> m_link_ID, _link -> m_incoming_array.size(),  _link -> m_finished_array.size());
      _link -> clear_incoming_array();
      _link -> evolve(_real_inter);
      // _link -> print_info();
    }

    // printf("Receiving!\n");
    // step 5: Destination receive vehicle  
    for (auto _dest_it = m_od_factory -> m_destination_map.begin(); _dest_it != m_od_factory -> m_destination_map.end(); _dest_it++){
      _dest = _dest_it -> second;
      _dest -> receive(_real_inter);
    }

    // step 6: sample link tt
    if ((_cur_inter+1) % (max_inter / m_sample_points) == 0){
      for (auto _map_it : _link_tt -> m_tt_map){
        m_average_link_tt.find(_map_it.first -> m_link_ID) -> second += _map_it.second/TFlt(m_sample_points);
      }
    }
    // _link_tt -> print_info();
    _cur_inter ++;
  }

  for(auto _it : *path_table){
    for (auto _it_it : *(_it.second)){
      for (MNM_Path* _path : _it_it.second -> m_path_vec){
        _path -> buffer[_grad_position] = 0;
      }
    }
  } 

  TFlt _tmp_tt;
  TFlt _demand;
  for(auto _it : *path_table){
    for (auto _it_it : *(_it.second)){
      _demand = MNM::get_demand_bynode(_it.first, _it_it.first, assign_inter, _node_factory);
      // printf("Demand is %.4f\n", _demand);
      for (MNM_Path* _path : _it_it.second -> m_path_vec){
        TFlt _tmp_grad = 0;
        for (TInt _link_ID : _path -> m_link_vec){
          _link = _link_factory->get_link(_link_ID);
          _tmp_tt = m_average_link_tt.find(_link_ID) -> second;
          // printf("Average tt is %.4f, marginal cost is %.4f\n", (float)_tmp_tt, (float)MNM::calculate_link_mc(_link_factory->get_link(_link_ID), _tmp_tt));
          _tmp_grad += MNM_Ults::divide(_tmp_tt, _demand);
          _tmp_grad += MNM_Ults::divide(MNM::calculate_link_mc(_link, _tmp_tt)
                       * _link -> get_link_flow()
                       , _demand * num_link);
        }
        _path -> buffer[_grad_position] = _tmp_grad;
        // printf("grad is %.4f\n", _tmp_grad);
      }
    }
  } 
  // // m_statistics -> post_record();
  delete _link_tt;  
  return 0;
}

int MNM_Realtime_Dta::one_iteration(TInt assign_inter)
{
  assign_inter = assign_inter % m_total_assign_inter;
  printf("estimate_previous\n");
  estimate_previous(assign_inter);
  printf("update_compliance_ratio\n");
  update_compliance_ratio(assign_inter);
  printf("copy_p_to_buffer\n");
  MNM::copy_p_to_buffer(m_path_table, 0);
  TInt _next_assign_inter = (assign_inter + 1) % m_total_assign_inter;
  printf("optimize_next\n");
  optimize_next(_next_assign_inter);
  printf("copy_p_to_buffer\n");
  MNM::copy_p_to_buffer(m_path_table, 2);
  printf("update_vms\n");
  update_vms(_next_assign_inter);
  printf("predict_next\n");
  predict_next(_next_assign_inter);
  printf("generate_vms_instructions\n");
  std::string _vms_info = m_file_folder + "/record/vms_info";
  MNM::generate_vms_instructions(_vms_info, m_vms_factory, m_after_shot -> m_link_factory);
  return 0;
}

int MNM_Realtime_Dta::update_compliance_ratio(TInt assign_inter)
{
  TFlt Learning_Rate = 0.3;
  // m_p real p in last interval, buffer[0] estimated best in last interval
  // ratio =  (estimated_flow  - real_flow) / estimated_flow   on non_detour links
  TInt _link_ID;
  TFlt _est_flow, _real_flow;
  TFlt _tmp_ratio;
  for (auto _map_it : m_vms_factory -> m_link_vms_map){
    MNM_Link_Vms *_vms  = _map_it.second;
    _est_flow = 0;
    _real_flow = 0;
    for (auto _link_path_it : _vms -> m_link_path_map){
      _link_ID = _link_path_it.first;
      if (_link_ID != _vms -> m_detour_link_ID && _vms -> m_detour_link_ID > 0){
        for (MNM_Path* _path : *_link_path_it.second){
          _est_flow = _path -> buffer[0] 
              * MNM::get_demand_bynode(_path -> m_node_vec.front(), _path -> m_node_vec.back(), assign_inter, m_before_shot -> m_node_factory);
          _real_flow = _path -> m_p
              * MNM::get_demand_bynode(_path -> m_node_vec.front(), _path -> m_node_vec.back(), assign_inter, m_before_shot -> m_node_factory);
        }
      }
    }
    _tmp_ratio = MNM_Ults::max(MNM_Ults::min(MNM_Ults::divide(_est_flow - _real_flow, _est_flow), 0),1);
    _vms -> m_compliance_ratio = _vms -> m_compliance_ratio * (1 - Learning_Rate) +
                                    _tmp_ratio * Learning_Rate;
  }
  return 0;
}

int MNM_Realtime_Dta::update_vms(TInt next_assign_inter){
  for (auto _map_it : m_vms_factory -> m_link_vms_map){
    MNM_Link_Vms *_vms  = _map_it.second;
    _vms -> generate_detour_link(m_path_table, next_assign_inter, m_before_shot -> m_node_factory);
  }
  return 0;
}

int MNM_Realtime_Dta::estimate_previous(TInt assign_inter)
{
  std::string _spd_name = m_file_folder + "/MNM_input_spd";
  std::unordered_map<TInt, TFlt> _link_spd_map = std::unordered_map<TInt, TFlt>();
  MNM_IO::read_int_float(_spd_name, &_link_spd_map);
  MNM_Dta_Screenshot *_tmp_shot;

  for (int i=0; i < m_estimation_iters; ++i){
    printf("estimate_previousn::make_screenshot\n");
    _tmp_shot = MNM::make_screenshot(m_before_shot);
    printf("estimate_previousn::get_estimation_gradient\n");
    get_estimation_gradient(_tmp_shot, m_estimation_length, assign_inter, m_path_table, &_link_spd_map);
    // for(auto _it : *m_path_table){
    //   for (auto _it_it : *(_it.second)){
    //     for (MNM_Path* _path : _it_it.second -> m_path_vec){
    //       printf("Now the probability is %.4f\n", (float)_path -> m_p);
    //     }
    //   }
    // } 
    printf("estimate_previousn::update_path_p\n");
    MNM::update_path_p(m_path_table, 1, m_estimation_step_size/TFlt(i + 1));
    printf("estimate_previousn::delete shot\n");
    if (i == m_estimation_iters - 1){
      delete m_before_shot;
      m_before_shot = _tmp_shot;
    }
    else{
      delete _tmp_shot;
    }
  }
  _link_spd_map.clear();
  return 0;
}

int MNM_Realtime_Dta::optimize_next(TInt next_assign_inter)
{
  MNM_Dta_Screenshot *_tmp_shot;
  for (int i=0; i < m_optimization_iters; ++i){
    printf("optimize_next::make_screenshot\n");
    _tmp_shot = MNM::make_screenshot(m_before_shot);
    printf("optimize_next::get_optimization_gradient\n");
    get_optimization_gradient(_tmp_shot, m_estimation_length, next_assign_inter, m_path_table);
    printf("optimize_next::update_path_p\n");
    MNM::update_path_p(m_path_table, 3, m_optimization_step_size/TFlt(i + 1));
    printf("optimize_next::delete\n");
    // for(auto _it : *m_path_table){
    //   for (auto _it_it : *(_it.second)){
    //     for (MNM_Path* _path : _it_it.second -> m_path_vec){
    //       printf("Now the probability is %.4f\n", (float)_path -> m_p);
    //     }
    //   }
    // } 
    if (i == m_optimization_iters - 1){
      delete m_after_shot;
      m_after_shot = _tmp_shot;
    }
    else{
      delete _tmp_shot;
    }
  }

  return 0;  
}

int MNM_Realtime_Dta::predict_next(TInt next_assign_inter)
{
  TInt _cur_inter = 0;
  MNM_Origin *_origin;
  MNM_Dnode *_node;
  MNM_Dlink *_link;
  MNM_Destination *_dest;

  TInt _assign_inter = next_assign_inter;
  TInt _assign_freq = m_dta_config -> get_int("assign_frq");

  MNM_Node_Factory *_node_factory = m_before_shot -> m_node_factory;
  MNM_Link_Factory *_link_factory = m_before_shot -> m_link_factory;
  MNM_Veh_Factory *_veh_factory = m_before_shot -> m_veh_factory;
  MNM_Routing *_routing = m_before_shot -> m_routing;

  MNM_Statistics* _statistics= get_statistics(_node_factory, _link_factory);

  // printf("MNM: Prepare loading!\n");
  _routing -> init_routing(m_path_table);
  MNM_IO::hook_up_od_node(m_file_folder, m_dta_config, m_od_factory, _node_factory);  
  // printf("Finish prepare routing\n");
  _statistics -> init_record();
  for (auto _node_it = _node_factory -> m_node_map.begin(); _node_it != _node_factory -> m_node_map.end(); _node_it++){
    _node = _node_it -> second;
    // printf("Node ID: %d\n", _node -> m_node_ID);
    _node -> prepare_loading();
  }
  

  // estimate p
  MNM_Link_Vms *_vms;
  for (auto _map_it : m_vms_factory -> m_link_vms_map){
    _vms = _map_it.second;
    for (auto _it : _vms -> m_link_path_map){
      if (_it.first != _vms -> m_detour_link_ID){
        for (MNM_Path* _path : *_it.second){
          _path -> m_p *= _vms -> m_compliance_ratio;
        }
      }
    }
  }
  MNM::normalize_path_table_p(m_path_table);


  while (_cur_inter < m_prediction_length){
    if ((_cur_inter+1) % 10 == 0){
      printf("-------------------------------    Interval %d   ------------------------------ \n", (int)_cur_inter);
    }
      
    // step 1: Origin release vehicle

    // printf("Realsing!\n");
    if ((_cur_inter + 1) % _assign_freq == 0){
      for (auto _origin_it = m_od_factory -> m_origin_map.begin(); _origin_it != m_od_factory -> m_origin_map.end(); _origin_it++){
        _origin = _origin_it -> second;
        _assign_inter = _assign_inter % m_total_assign_inter;
        _origin -> release_one_interval(_cur_inter, _veh_factory, _assign_inter, TFlt(0));
      }  
      _assign_inter += 1;
    }
     

    // printf("Routing!\n");
    // step 2: route the vehicle
    _routing -> update_routing(_cur_inter);

    // printf("Moving through node!\n");
    // step 3: move vehicles through node
    for (auto _node_it = _node_factory -> m_node_map.begin(); _node_it != _node_factory -> m_node_map.end(); _node_it++){
      _node = _node_it -> second;
      _node -> evolve(_cur_inter);
    }

    // printf("Moving through link\n");
    // step 4: move vehicles through link
    for (auto _link_it = _link_factory -> m_link_map.begin(); _link_it != _link_factory -> m_link_map.end(); _link_it++){
      _link = _link_it -> second;
      // printf("Current Link %d:, incomming %d, finished %d\n", _link -> m_link_ID, _link -> m_incoming_array.size(),  _link -> m_finished_array.size());
      _link -> clear_incoming_array();
      _link -> evolve(_cur_inter);
      // _link -> print_info();
    }

    // printf("Receiving!\n");
    // step 5: Destination receive vehicle  
    for (auto _dest_it = m_od_factory -> m_destination_map.begin(); _dest_it != m_od_factory -> m_destination_map.end(); _dest_it++){
      _dest = _dest_it -> second;
      _dest -> receive(_cur_inter);
    }

    _statistics -> update_record(_cur_inter);
    _cur_inter ++;
  }
  _statistics -> post_record();
  delete _statistics;
  return 0;
}

MNM_Statistics *MNM_Realtime_Dta::get_statistics(MNM_Node_Factory *node_factory, MNM_Link_Factory *link_factory)
{
  MNM_ConfReader *_record_config = new MNM_ConfReader(m_file_folder + "/config.conf", "STAT");
  if (_record_config -> get_string("rec_mode") == "LRn"){
    MNM_Statistics *_statistics = new MNM_Statistics_Lrn(m_file_folder, m_dta_config, _record_config,
                                   m_od_factory, node_factory, link_factory);
    return _statistics;
  }
  else{
    printf("Only support LRN now!\n");
    exit(-1);
  }
}
/**************************************************************************
                          Screen shot
**************************************************************************/
MNM_Dta_Screenshot::MNM_Dta_Screenshot(std::string file_folder, MNM_ConfReader* config, PNEGraph graph, MNM_OD_Factory *od_factory)
{
  m_graph = graph;
  m_file_folder = file_folder;
  m_config = config;
  m_od_factory = od_factory;
  m_veh_factory = new MNM_Veh_Factory();
  m_node_factory = new MNM_Node_Factory();
  m_link_factory = new MNM_Link_Factory();
  m_routing = new MNM_Routing_Fixed(graph, od_factory, m_node_factory, m_link_factory);
}

MNM_Dta_Screenshot::~MNM_Dta_Screenshot()
{
  delete m_veh_factory;
  delete m_node_factory;
  delete m_link_factory;
  delete m_routing;
}

int MNM_Dta_Screenshot::build_static_network()
{
  MNM_IO::build_node_factory(m_file_folder, m_config, m_node_factory);
  MNM_IO::build_link_factory(m_file_folder, m_config, m_link_factory);
  hook_up_node_and_link();
  return 0;
}


int MNM_Dta_Screenshot::hook_up_node_and_link()
{
  TInt _node_ID;
  MNM_Dnode *_node;
  MNM_Dlink *_link;
  // hook up node to link
  for (auto _node_it = m_graph->BegNI(); _node_it < m_graph->EndNI(); _node_it++) {
    // printf("node id %d with out-degree %d and in-degree %d\n",
      // _node_it.GetId(), _node_it.GetOutDeg(), _node_it.GetInDeg());
    _node_ID = _node_it.GetId();
    _node = m_node_factory -> get_node(_node_ID);
    for (int e = 0; e < _node_it.GetOutDeg(); ++e) {
      // printf("Out: edge (%d %d)\n", _node_it.GetId(), _node_it.GetOutNId(e));
      _link = m_link_factory -> get_link(_node_it.GetOutEId(e));
      _node -> add_out_link(_link);
    }
    for (int e = 0; e < _node_it.GetInDeg(); ++e) {
      // printf("In: edge (%d %d)\n", _node_it.GetInNId(e), _node_it.GetId());
      _link = m_link_factory -> get_link(_node_it.GetInEId(e));
      _node -> add_in_link(_link);
    }   
  }
  // printf("Hook up link to node\n");
  // hook up link to node
  for (auto _link_it = m_graph->BegEI(); _link_it < m_graph->EndEI(); _link_it++){
    _link = m_link_factory -> get_link(_link_it.GetId());
    _link -> hook_up_node(m_node_factory -> get_node(_link_it.GetSrcNId()), m_node_factory -> get_node(_link_it.GetDstNId()));
  }
  return 0;
}

namespace MNM
{
int run_from_screenshot(MNM_Dta_Screenshot* screenshot, MNM_ConfReader *dta_config,
                        TInt max_inter, TInt assign_inter, Path_Table *path_table,
                        MNM_OD_Factory *od_factory, std::string file_folder)
{
  TInt _cur_inter = 0;
  TInt _total_inter = max_inter;
  TInt _real_inter;
  MNM_Origin *_origin;
  MNM_Dnode *_node;
  MNM_Dlink *_link;
  MNM_Destination *_dest;

  MNM_Node_Factory *_node_factory = screenshot -> m_node_factory;
  MNM_Link_Factory *_link_factory = screenshot -> m_link_factory;
  MNM_Veh_Factory *_veh_factory = screenshot -> m_veh_factory;
  MNM_Routing *_routing = screenshot -> m_routing;

  // printf("MNM: Prepare loading!\n");
  _routing -> init_routing(path_table);
  MNM_IO::hook_up_od_node(file_folder, dta_config, od_factory, _node_factory);  
  // printf("Finish prepare routing\n");
  // m_statistics -> init_record();
  for (auto _node_it = _node_factory -> m_node_map.begin(); _node_it != _node_factory -> m_node_map.end(); _node_it++){
    _node = _node_it -> second;
    // printf("Node ID: %d\n", _node -> m_node_ID);
    _node -> prepare_loading();
  }
  while (_cur_inter < _total_inter){
    _real_inter = _cur_inter;
    // printf("-------------------------------    Interval %d   ------------------------------ \n", (int)_real_inter);
    // step 1: Origin release vehicle

    // printf("Realsing!\n");
    if (_cur_inter == 0){
      for (auto _origin_it = od_factory -> m_origin_map.begin(); _origin_it != od_factory -> m_origin_map.end(); _origin_it++){
        _origin = _origin_it -> second;
        _origin -> release_one_interval(_real_inter, _veh_factory, assign_inter, TFlt(0));
      }       
    }

    // printf("Routing!\n");
    // step 2: route the vehicle
    _routing -> update_routing(_real_inter);

    // printf("Moving through node!\n");
    // step 3: move vehicles through node
    for (auto _node_it = _node_factory -> m_node_map.begin(); _node_it != _node_factory -> m_node_map.end(); _node_it++){
      _node = _node_it -> second;
      _node -> evolve(_real_inter);
    }

    // _link_tt -> update_tt(_real_inter);
    // printf("Moving through link\n");
    // step 4: move vehicles through link
    for (auto _link_it = _link_factory -> m_link_map.begin(); _link_it != _link_factory -> m_link_map.end(); _link_it++){
      _link = _link_it -> second;
      // printf("Current Link %d:, incomming %d, finished %d\n", _link -> m_link_ID, _link -> m_incoming_array.size(),  _link -> m_finished_array.size());
      _link -> clear_incoming_array();
      _link -> evolve(_real_inter);
      // _link -> print_info();
    }

    // printf("Receiving!\n");
    // step 5: Destination receive vehicle  
    for (auto _dest_it = od_factory -> m_destination_map.begin(); _dest_it != od_factory -> m_destination_map.end(); _dest_it++){
      _dest = _dest_it -> second;
      _dest -> receive(_real_inter);
    }

  //   // printf("Update record!\n");
  //   // // step 5: update record
  //   // m_statistics -> update_record(_cur_int);

    // _link_tt -> print_info();
    _cur_inter ++;
  }

  // // m_statistics -> post_record();

  return 0;
}

MNM_Dta_Screenshot *make_screenshot(std::string file_folder, MNM_ConfReader* config, MNM_OD_Factory *od_factory,
                                            MNM_Link_Factory *link_factory, MNM_Node_Factory *node_factory, PNEGraph graph,
                                            MNM_Routing_Fixed *old_routing)
{
  MNM_Dta_Screenshot *_shot = new MNM_Dta_Screenshot(file_folder, config, graph, od_factory);
  _shot -> build_static_network();
  MNM_Dlink *_dlink, *_new_dlink;
  TInt _link_ID;
  MNM_Veh_Factory *_new_veh_factory;
  _new_veh_factory = _shot -> m_veh_factory;
  MNM_Veh *_new_veh;
  for (auto _link_it = link_factory -> m_link_map.begin(); _link_it != link_factory -> m_link_map.end(); _link_it++){
    _link_ID = _link_it -> first;
    _dlink = _link_it -> second;
    _new_dlink = _shot -> m_link_factory -> get_link(_link_ID);

    for (MNM_Veh* _veh : _dlink -> m_finished_array){
      _new_veh = _new_veh_factory -> make_veh(_veh -> m_start_time, _veh -> m_type);
      copy_veh(_veh, _new_veh);
      _new_dlink -> m_finished_array.push_back(_new_veh);
      _shot -> m_routing -> add_veh_path(_new_veh, old_routing -> m_tracker.find(_veh) -> second);
    }

    for (MNM_Veh* _veh : _dlink -> m_incoming_array){
      _new_veh = _new_veh_factory -> make_veh(_veh -> m_start_time, _veh -> m_type);
      copy_veh(_veh, _new_veh);
      _new_dlink -> m_incoming_array.push_back(_new_veh);
      _shot -> m_routing -> add_veh_path(_new_veh, old_routing -> m_tracker.find(_veh) -> second);
    }

    if (MNM_Dlink_Ctm *_ctm = dynamic_cast<MNM_Dlink_Ctm *>(_dlink)){
      MNM_Dlink_Ctm::Ctm_Cell *_new_cell, *_cell;
      MNM_Dlink_Ctm *_new_ctm = dynamic_cast<MNM_Dlink_Ctm *>(_new_dlink);
      for (size_t i=0; i < _ctm -> m_cell_array.size(); ++i){
        _cell = _ctm -> m_cell_array[i];
        _new_cell = _new_ctm -> m_cell_array[i];
        for (MNM_Veh* _veh : _cell -> m_veh_queue){
          _new_veh = _new_veh_factory -> make_veh(_veh -> m_start_time, _veh -> m_type);
          copy_veh(_veh, _new_veh);
          _new_cell -> m_veh_queue.push_back(_new_veh);
          _shot -> m_routing -> add_veh_path(_new_veh, old_routing -> m_tracker.find(_veh) -> second);
        }
      }
    }
    else if (MNM_Dlink_Pq *_pq = dynamic_cast<MNM_Dlink_Pq *> (_dlink)) {
      MNM_Dlink_Pq *_new_pq = dynamic_cast<MNM_Dlink_Pq *>(_new_dlink);
      for (auto _veh_it = _pq -> m_veh_queue.begin(); _veh_it != _pq -> m_veh_queue.end(); _veh_it++){
        MNM_Veh *_veh = _veh_it -> first;
        _new_veh = _new_veh_factory -> make_veh(_veh -> m_start_time, _veh -> m_type);
        copy_veh(_veh, _new_veh);
        _new_pq -> m_veh_queue.insert(std::pair<MNM_Veh*, TInt>(_new_veh, _veh_it->second));
        _shot -> m_routing -> add_veh_path(_new_veh, old_routing -> m_tracker.find(_veh) -> second);
      }
    }
  }

  //TODO node vehicle migration
  return _shot;
}

MNM_Dta_Screenshot *make_screenshot(MNM_Dta_Screenshot* screenshot)
{
  return MNM::make_screenshot(screenshot -> m_file_folder, screenshot -> m_config, screenshot -> m_od_factory,
                              screenshot -> m_link_factory, screenshot -> m_node_factory, screenshot -> m_graph, screenshot -> m_routing);
}


MNM_Dta_Screenshot *make_empty_screenshot(std::string file_folder, MNM_ConfReader* config, 
                                                  MNM_OD_Factory *od_factory, PNEGraph graph)
{
  MNM_Dta_Screenshot *_shot = new MNM_Dta_Screenshot(file_folder, config, graph, od_factory);
  _shot -> build_static_network();
  return _shot;  
}


int update_path_p(Path_Table *path_table, TInt col, TFlt step_size)
{
  TFlt Possible_Large = 10000;
  for(auto _it : *path_table){
    for (auto _it_it : *(_it.second)){
      for (MNM_Path* _path : _it_it.second -> m_path_vec){
        // printf("Before m_p is %lf\n", _path -> m_p);
        if (_path -> buffer[col] > 0){
          _path -> m_p /= (1 + step_size);
        }
        else {
          _path -> m_p *= (1 + step_size);
        }
        // _path -> m_p -= step_size * _path -> buffer[col];
        // printf("Now m_p is %lf\n", _path -> m_p);
        _path -> m_p = MNM_Ults::max(_path -> m_p, -Possible_Large);
        _path -> m_p = MNM_Ults::min(_path -> m_p, Possible_Large);
      }
      _it_it.second -> normalize_p();
    }
  } 
  return 0; 
}

}// end namespace MNM_Dta

