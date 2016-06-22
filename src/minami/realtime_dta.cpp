#include "realtime_dta.h"

MNM_Realtime_Dta::MNM_Realtime_Dta(std::string file_folder)
{
  m_file_folder = file_folder;
  m_path_table = NULL;
  m_od_factory = new MNM_OD_Factory();
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
  // m_path_table = MNM_IO::load_path_table(m_file_name, m_graph, TInt num_path)
  m_before_shot = MNM::make_empty_screenshot(m_file_folder, m_dta_config, m_od_factory, m_graph);
  m_after_shot = MNM::make_empty_screenshot(m_file_folder, m_dta_config, m_od_factory, m_graph);
  build_path_table();
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


int MNM_Realtime_Dta::run_from_screenshot(MNM_Dta_Screenshot* screenshot, TInt start_inter, 
                                            TInt max_inter, TInt assign_inter, Path_Table *path_table)
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

  printf("MNM: Prepare loading!\n");
  _routing -> init_routing(path_table);
  MNM_IO::hook_up_od_node(m_file_folder, m_dta_config, m_od_factory, _node_factory);  
  printf("Finish prepare routing\n");
  // m_statistics -> init_record();
  for (auto _node_it = _node_factory -> m_node_map.begin(); _node_it != _node_factory -> m_node_map.end(); _node_it++){
    _node = _node_it -> second;
    // printf("Node ID: %d\n", __node -> m_node_ID);
    _node -> prepare_loading();
  }
  while (_cur_inter < _total_inter){
    _real_inter = _cur_inter + start_inter;
  //   printf("-------------------------------    Interval %d   ------------------------------ \n", (int)__cur_int);
    // step 1: Origin release vehicle

    printf("Realsing!\n");
    if (_cur_inter == 0){
      for (auto _origin_it = m_od_factory -> m_origin_map.begin(); _origin_it != m_od_factory -> m_origin_map.end(); _origin_it++){
        _origin = _origin_it -> second;
        _origin -> release_one_interval(_real_inter, _veh_factory, assign_inter, TFlt(0));
      }       
    }

    printf("Routing!\n");
    // step 2: route the vehicle
    _routing -> update_routing(_real_inter);

    printf("Moving through node!\n");
    // step 3: move vehicles through node
    for (auto _node_it = _node_factory -> m_node_map.begin(); _node_it != _node_factory -> m_node_map.end(); _node_it++){
      _node = _node_it -> second;
      _node -> evolve(_real_inter);
    }

    printf("Moving through link\n");
    // step 4: move vehicles through link
    for (auto _link_it = _link_factory -> m_link_map.begin(); _link_it != _link_factory -> m_link_map.end(); _link_it++){
      _link = _link_it -> second;
      // printf("Current Link %d:, incomming %d, finished %d\n", __link -> m_link_ID, __link -> m_incoming_array.size(),  __link -> m_finished_array.size());
      _link -> clear_incoming_array();
      _link -> evolve(_real_inter);
      // __link -> print_info();
    }

    printf("Receiving!\n");
    // step 5: Destination receive vehicle  
    for (auto _dest_it = m_od_factory -> m_destination_map.begin(); _dest_it != m_od_factory -> m_destination_map.end(); _dest_it++){
      _dest = _dest_it -> second;
      _dest -> receive(_real_inter);
    }

  //   // printf("Update record!\n");
  //   // // step 5: update record
  //   // m_statistics -> update_record(__cur_int);

    _cur_inter ++;
  }

  // // m_statistics -> post_record();
  return 0;
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
  TInt __node_ID;
  MNM_Dnode *__node;
  MNM_Dlink *__link;
  // hook up node to link
  for (auto __node_it = m_graph->BegNI(); __node_it < m_graph->EndNI(); __node_it++) {
    // printf("node id %d with out-degree %d and in-degree %d\n",
      // __node_it.GetId(), __node_it.GetOutDeg(), __node_it.GetInDeg());
    __node_ID = __node_it.GetId();
    __node = m_node_factory -> get_node(__node_ID);
    for (int e = 0; e < __node_it.GetOutDeg(); ++e) {
      // printf("Out: edge (%d %d)\n", __node_it.GetId(), __node_it.GetOutNId(e));
      __link = m_link_factory -> get_link(__node_it.GetOutEId(e));
      __node -> add_out_link(__link);
    }
    for (int e = 0; e < __node_it.GetInDeg(); ++e) {
      // printf("In: edge (%d %d)\n", __node_it.GetInNId(e), __node_it.GetId());
      __link = m_link_factory -> get_link(__node_it.GetInEId(e));
      __node -> add_in_link(__link);
    }   
  }
  // printf("Hook up link to node\n");
  // hook up link to node
  for (auto __link_it = m_graph->BegEI(); __link_it < m_graph->EndEI(); __link_it++){
    __link = m_link_factory -> get_link(__link_it.GetId());
    __link -> hook_up_node(m_node_factory -> get_node(__link_it.GetSrcNId()), m_node_factory -> get_node(__link_it.GetDstNId()));
  }
  return 0;
}

namespace MNM
{

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

MNM_Dta_Screenshot *make_screenshot(MNM_Dta_Screenshot* screenshot, MNM_Routing_Fixed *old_routing)
{
  return MNM::make_screenshot(screenshot -> m_file_folder, screenshot -> m_config, screenshot -> m_od_factory,
                              screenshot -> m_link_factory, screenshot -> m_node_factory, screenshot -> m_graph, old_routing);
}


MNM_Dta_Screenshot *make_empty_screenshot(std::string file_folder, MNM_ConfReader* config, 
                                                  MNM_OD_Factory *od_factory, PNEGraph graph)
{
  MNM_Dta_Screenshot *_shot = new MNM_Dta_Screenshot(file_folder, config, graph, od_factory);
  _shot -> build_static_network();
  return _shot;  
}

}// end namespace MNM_Dta

