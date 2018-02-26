#include "dta.h"
#include "omp.h"

MNM_Dta::MNM_Dta(std::string file_folder)
{
  m_file_folder = file_folder;
  initialize();
}

MNM_Dta::~MNM_Dta()
{
  delete m_veh_factory;
  delete m_node_factory;
  delete m_link_factory;
  delete m_od_factory;
  delete m_config;
  delete m_routing;
  delete m_statistics;
  delete m_workzone;
  m_graph -> Clr();  
}

int MNM_Dta::initialize()
{
  m_veh_factory = new MNM_Veh_Factory();
  m_node_factory = new MNM_Node_Factory();
  m_link_factory = new MNM_Link_Factory();
  m_od_factory = new MNM_OD_Factory();
  m_config = new MNM_ConfReader(m_file_folder + "/config.conf", "DTA");
  m_unit_time = m_config -> get_int("unit_time");
  m_flow_scalar = m_config -> get_int("flow_scalar");
  m_assign_freq = m_config -> get_int("assign_frq");
  m_start_assign_interval = m_config -> get_int("start_assign_interval");
  m_total_assign_inter = m_config ->  get_int("max_interval");

  return 0;
}

int MNM_Dta::set_statistics()
{
  MNM_ConfReader *_record_config = new MNM_ConfReader(m_file_folder + "/config.conf", "STAT");
  if (_record_config -> get_string("rec_mode") == "LRn"){
    m_statistics = new MNM_Statistics_Lrn(m_file_folder, m_config, _record_config,
                                   m_od_factory, m_node_factory, m_link_factory);
  }
  // printf("set_statistics finished\n");
  return 0;
}

int MNM_Dta::set_routing()
{
  if (m_config -> get_string("routing_type") == "Hybrid"){
    m_routing = new MNM_Routing_Hybrid(m_file_folder, m_graph, m_statistics,
                                   m_od_factory, m_node_factory, m_link_factory);
  }else if(m_config ->get_string("routing_type")=="Predetermined"){
    
    Path_Table *_path_table = MNM::build_pathset(m_graph, m_od_factory, m_link_factory);
    MNM_Pre_Routing *_pre_routing = new MNM_Pre_Routing(_path_table,m_od_factory);
    m_routing = new MNM_Routing_Predetermined(m_graph,m_od_factory,m_node_factory,
                    m_link_factory, _path_table,_pre_routing, 
                    m_total_assign_inter);
  }


  // m_routing = new MNM_Routing_Random(m_graph, m_statistics, m_od_factory, m_node_factory, m_link_factory);

  return 0;  
}

int MNM_Dta::build_workzone()
{
  m_workzone = new MNM_Workzone(m_node_factory, m_link_factory, m_graph);
  MNM_IO::build_workzone_list(m_file_folder, m_workzone);  
  return 0;
}

int MNM_Dta::build_from_files()
{
  MNM_IO::build_node_factory(m_file_folder, m_config, m_node_factory);
  // std::cout << test_dta -> m_node_factory -> m_node_map.size() << "\n";
  MNM_IO::build_link_factory(m_file_folder, m_config, m_link_factory);
  // std::cout << test_dta -> m_link_factory -> m_link_map.size() << "\n";
  MNM_IO::build_od_factory(m_file_folder, m_config, m_od_factory, m_node_factory);
  // std::cout << test_dta -> m_od_factory -> m_origin_map.size() << "\n";
  // std::cout << test_dta -> m_od_factory -> m_destination_map.size() << "\n";
  m_graph = MNM_IO::build_graph(m_file_folder, m_config);
  MNM_IO::build_demand(m_file_folder, m_config, m_od_factory);
  build_workzone();
  set_statistics();
  set_routing();
  return 0;  
}

int MNM_Dta::hook_up_node_and_link()
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


/*
 *  perform a series of checks to ensure we can run the DTA model
 */
bool MNM_Dta::is_ok()
{
  bool _flag = true;
  bool _temp_flag = true;
  //Checks the graph data structure for internal consistency.
  //For each node in the graph check that its neighbors are also nodes in the graph.
  printf("Checking......Graph consistent!\n");
  _temp_flag = m_graph -> IsOk(); 
  _flag = _flag && _temp_flag;
  if (_temp_flag)  printf("Passed!\n");

  //check node
  printf("Checking......Node consistent!\n");
  _temp_flag = (m_graph -> GetNodes() == m_config -> get_int("num_of_node"))
                && (m_graph -> GetNodes() == TInt(m_node_factory -> m_node_map.size()));
  _flag = _flag && _temp_flag;
  if (_temp_flag)  printf("Passed!\n");

  //check link
  printf("Checking......Link consistent!\n");
  _temp_flag = (m_graph -> GetEdges() == m_config -> get_int("num_of_link"))
                && (m_graph -> GetEdges() == TInt(m_link_factory -> m_link_map.size()));
  _flag = _flag && _temp_flag;
  if (_temp_flag)  printf("Passed!\n");  

  //check OD node
  printf("Checking......OD consistent!\n");
  TInt _node_ID;
  _temp_flag = (TInt(m_od_factory -> m_origin_map.size()) == m_config -> get_int("num_of_O"))
                && (TInt(m_od_factory -> m_destination_map.size()) == m_config -> get_int("num_of_D"));
  std::unordered_map<TInt, MNM_Origin*>::iterator _origin_map_it;
  for (_origin_map_it = m_od_factory->m_origin_map.begin();
       _origin_map_it != m_od_factory->m_origin_map.end(); _origin_map_it++){
    _node_ID = _origin_map_it -> second -> m_origin_node -> m_node_ID;
    _temp_flag = _temp_flag && ((m_graph -> GetNI(_node_ID)).GetId() == _node_ID)
                  && (m_graph -> GetNI(_node_ID).GetOutDeg() >= 1)
                  && (m_graph -> GetNI(_node_ID).GetInDeg() == 0);
  }
  std::unordered_map<TInt, MNM_Destination*>::iterator _dest_map_it;
  for (_dest_map_it = m_od_factory->m_destination_map.begin();
       _dest_map_it != m_od_factory->m_destination_map.end(); _dest_map_it++){
    _node_ID = _dest_map_it -> second -> m_dest_node -> m_node_ID;
    _temp_flag = _temp_flag && ((m_graph -> GetNI(_node_ID)).GetId() == _node_ID)
                  && (m_graph -> GetNI(_node_ID).GetOutDeg() == 0)
                  && (m_graph -> GetNI(_node_ID).GetInDeg() >= 1);
  }  
  _flag = _flag && _temp_flag;
  if (_temp_flag)  printf("Passed!\n");  

  printf("Checking......OD connectivity!\n");
  _temp_flag = check_origin_destination_connectivity();
  _flag = _flag && _temp_flag;
  if (_temp_flag)  printf("Passed!\n");  

  return _flag;
}

int MNM_Dta::pre_loading()
{
  MNM_Dnode *_node;
  printf("MNM: Prepare loading!\n");
  m_routing -> init_routing();
  // printf("Finish prepare routing\n");
  m_statistics -> init_record();
  for (auto _node_it = m_node_factory -> m_node_map.begin(); _node_it != m_node_factory -> m_node_map.end(); _node_it++){
    _node = _node_it -> second;
    // printf("Node ID: %d\n", _node -> m_node_ID);
    _node -> prepare_loading();
  }
  m_workzone -> init_workzone();

  // MNM_Dlink *_link;
  // for (auto _link_it = m_link_factory -> m_link_map.begin(); _link_it != m_link_factory -> m_link_map.end(); _link_it++){
  //   _link = _link_it -> second;
  //   _link -> install_cumulative_curve();
  // }
  return 0;
}

int MNM_Dta::load_once(bool verbose, TInt load_int, TInt assign_int)
{
  MNM_Origin *_origin;
  MNM_Dnode *_node;
  MNM_Dlink *_link;
  MNM_Destination *_dest;
  if(verbose) printf("-------------------------------    Interval %d   ------------------------------ \n", (int)load_int);
  // step 1: Origin release vehicle
  if(verbose) printf("Realsing!\n");
  // for (auto _origin_it = m_od_factory -> m_origin_map.begin(); _origin_it != m_od_factory -> m_origin_map.end(); _origin_it++){
  //   _origin = _origin_it -> second;
  //   _origin -> release(m_veh_factory, _cur_int);
  // }      
  if (load_int % m_assign_freq == 0 || load_int==0){
    for (auto _origin_it = m_od_factory -> m_origin_map.begin(); _origin_it != m_od_factory -> m_origin_map.end(); _origin_it++){
      _origin = _origin_it -> second;
      if (assign_int >= m_total_assign_inter) {
        _origin -> release_one_interval(load_int, m_veh_factory, -1, TFlt(0));
      }
      else{
        _origin -> release_one_interval(load_int, m_veh_factory, assign_int, TFlt(0));
      }
      // _assign_inter = _assign_inter % m_total_assign_inter;
      // _origin -> release_one_interval(_cur_int, m_veh_factory, _assign_inter, TFlt(0));
    } 
  }

  if(verbose) printf("Routing!\n");
  // step 2: route the vehicle
  m_routing -> update_routing(load_int);

  if(verbose) printf("Moving through node!\n");
  // step 3: move vehicles through node
  for (auto _node_it = m_node_factory -> m_node_map.begin(); _node_it != m_node_factory -> m_node_map.end(); _node_it++){
    _node = _node_it -> second;
    // printf("node ID is %d\n", _node -> m_node_ID());
    _node -> evolve(load_int);
  }

  if(verbose) printf("Moving through link\n");
  // step 4: move vehicles through link
  for (auto _link_it = m_link_factory -> m_link_map.begin(); _link_it != m_link_factory -> m_link_map.end(); _link_it++){
    _link = _link_it -> second;
    // if (_link -> get_link_flow() > 0){
    //   printf("Current Link %d:, traffic flow %.4f, incomming %d, finished %d\n", 
    //       _link -> m_link_ID(), _link -> get_link_flow()(), (int)_link -> m_incoming_array.size(),  (int)_link -> m_finished_array.size());
    //   _link -> print_info();
    // }
    
    _link -> clear_incoming_array();
    _link -> evolve(load_int);
    // _link -> print_info();
  }

  if(verbose) printf("Receiving!\n");
  // step 5: Destination receive vehicle  
  for (auto _dest_it = m_od_factory -> m_destination_map.begin(); _dest_it != m_od_factory -> m_destination_map.end(); _dest_it++){
    _dest = _dest_it -> second;
    _dest -> receive(load_int);
  }

    printf("Update record!\n");
    // step 5: update record
    m_statistics -> update_record(load_int);

  if(verbose) MNM::print_vehicle_statistics(m_veh_factory);
  // test();  
  return 0;
}


int MNM_Dta::loading(bool verbose)
{
  TInt _cur_int = 0;
  MNM_Origin *_origin;
  MNM_Dnode *_node;
  MNM_Dlink *_link;
  MNM_Destination *_dest;
  TInt _assign_inter = m_start_assign_interval;

  pre_loading();
  while (!finished_loading(_cur_int)){
    printf("-------------------------------    Interval %d   ------------------------------ \n", (int)_cur_int);
    // step 1: Origin release vehicle
    if(verbose) printf("Realsing!\n");
    // for (auto _origin_it = m_od_factory -> m_origin_map.begin(); _origin_it != m_od_factory -> m_origin_map.end(); _origin_it++){
    //   _origin = _origin_it -> second;
    //   _origin -> release(m_veh_factory, _cur_int);
    // }      
    if (_cur_int % m_assign_freq == 0 || _cur_int==0){
      for (auto _origin_it = m_od_factory -> m_origin_map.begin(); _origin_it != m_od_factory -> m_origin_map.end(); _origin_it++){
        _origin = _origin_it -> second;
        if (_assign_inter >= m_total_assign_inter) {
          _origin -> release_one_interval(_cur_int, m_veh_factory, -1, TFlt(0));
        }
        else{
          _origin -> release_one_interval(_cur_int, m_veh_factory, _assign_inter, TFlt(0));
        }
        // _assign_inter = _assign_inter % m_total_assign_inter;
        // _origin -> release_one_interval(_cur_int, m_veh_factory, _assign_inter, TFlt(0));
      } 
      _assign_inter += 1;
    }

    if(verbose) printf("Routing!\n");
    // step 2: route the vehicle
    m_routing -> update_routing(_cur_int);

    if(verbose) printf("Moving through node!\n");
    // step 3: move vehicles through node
    for (auto _node_it = m_node_factory -> m_node_map.begin(); _node_it != m_node_factory -> m_node_map.end(); _node_it++){
      _node = _node_it -> second;
      _node -> evolve(_cur_int);
    }

    if(verbose) printf("Moving through link\n");
    // step 4: move vehicles through link
    for (auto _link_it = m_link_factory -> m_link_map.begin(); _link_it != m_link_factory -> m_link_map.end(); _link_it++){
      _link = _link_it -> second;
      // if (_link -> get_link_flow() > 0){
      //   printf("Current Link %d:, traffic flow %.4f, incomming %d, finished %d\n", 
      //       _link -> m_link_ID(), _link -> get_link_flow()(), (int)_link -> m_incoming_array.size(),  (int)_link -> m_finished_array.size());
      //   _link -> print_info();
      // }
      
      _link -> clear_incoming_array();
      _link -> evolve(_cur_int);
      // _link -> print_info();
    }

    if(verbose) printf("Receiving!\n");
    // step 5: Destination receive vehicle  
    for (auto _dest_it = m_od_factory -> m_destination_map.begin(); _dest_it != m_od_factory -> m_destination_map.end(); _dest_it++){
      _dest = _dest_it -> second;
      _dest -> receive(_cur_int);
    }

    if(verbose) printf("Update record!\n");
    // step 5: update record
    m_statistics -> update_record(_cur_int);

    if(verbose) MNM::print_vehicle_statistics(m_veh_factory);
    // test();
    _cur_int ++;
  }

  // MNM_IO::dump_cumulative_curve(m_file_folder, m_link_factory);
  m_statistics -> post_record();
  return 0;
}


int MNM_Dta::check_origin_destination_connectivity()
{
  MNM_Destination *_dest;
  TInt _dest_node_ID;
  std::unordered_map<TInt, TInt> _shortest_path_tree = std::unordered_map<TInt, TInt>();
  std::unordered_map<TInt, TFlt> _cost_map;
  for (auto _map_it : m_link_factory -> m_link_map){
    _cost_map.insert(std::pair<TInt, TFlt>(_map_it.first, TFlt(1)));
  }
  
  for (auto _it = m_od_factory -> m_destination_map.begin(); _it != m_od_factory -> m_destination_map.end(); _it++){
    _dest = _it -> second;
    _dest_node_ID = _dest -> m_dest_node -> m_node_ID;
    MNM_Shortest_Path::all_to_one_FIFO(_dest_node_ID, m_graph, _cost_map, _shortest_path_tree);
    for (auto _map_it : m_od_factory -> m_origin_map){
      if (_shortest_path_tree.find(_map_it.second -> m_origin_node -> m_node_ID)-> second == -1){
        return false;
      }
    }
// }
  }
  return true;
}

bool MNM_Dta::finished_loading(int cur_int)
{
  TInt _total_int = m_config ->get_int("total_interval");
  if (_total_int > 0){
    return cur_int >= _total_int;
  }
  else{
    return !(MNM::has_running_vehicle(m_veh_factory) && cur_int != 0);
  }
}

// int MNM_Dta::test()
// {
//   auto output_map = std::unordered_map<TInt, TInt>();
//   MNM_Shortest_Path::all_to_one_FIFO(6, 
//                         m_graph, m_statistics -> m_load_interval_tt,
//                         output_map);
//   for (auto _it = output_map.begin(); _it!=output_map.end(); _it++){
//     printf("For node %d, it should head to %d\n", _it -> first, _it -> second);
//   }
//   return 0;
// }



namespace MNM
{
int print_vehicle_statistics(MNM_Veh_Factory *veh_factory)
{
  TInt _total_veh = TInt(veh_factory -> m_veh_map.size());
  TInt _finished_veh = 0;
  TInt _enroute_veh;
  for (auto _map_it : veh_factory -> m_veh_map){
    if (_map_it.second -> m_finish_time > 0) _finished_veh += 1;
  }
  _enroute_veh = _total_veh - _finished_veh;
  printf("Released vehicle %d, Enroute vehicle %d, Finished vehicle %d\n", _total_veh(), _enroute_veh(), _finished_veh());
  return 0;
}

int print_vehicle_info(MNM_Veh_Factory *veh_factory)
{
  MNM_Veh *_veh;
  for (auto _map_it : veh_factory -> m_veh_map){
    _veh = _map_it.second;
    printf("Vehicle ID %d, from origin node %d to destination node %d, is current on link %d and heads to link %d\n",
            _veh -> m_veh_ID(), _veh -> get_origin() -> m_origin_node -> m_node_ID(), _veh -> get_destination() -> m_dest_node -> m_node_ID(),
            _veh -> get_current_link() -> m_link_ID(), _veh -> get_next_link() -> m_link_ID());
  }
  return 0;
}

bool has_running_vehicle(MNM_Veh_Factory *veh_factory)
{
  TInt _total_veh = TInt(veh_factory -> m_veh_map.size());
  TInt _finished_veh = 0;
  TInt _enroute_veh;
  for (auto _map_it : veh_factory -> m_veh_map){
    if (_map_it.second -> m_finish_time > 0) _finished_veh += 1;
  }
  _enroute_veh = _total_veh - _finished_veh;  
  return _enroute_veh != 0;
}

}