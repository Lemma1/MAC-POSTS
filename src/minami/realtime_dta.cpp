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

  MNM_Link_Factory *_tmp_link_factory = new MNM_Link_Factory();
  MNM_Node_Factory *_tmp_node_factory = new MNM_Node_Factory();
  m_before_shot = MNM::make_screenshot(m_file_folder, m_dta_config, _tmp_link_factory, _tmp_node_factory, m_graph);
  m_after_shot = MNM::make_screenshot(m_file_folder, m_dta_config, _tmp_link_factory, _tmp_node_factory, m_graph);
  delete _tmp_link_factory;
  delete _tmp_node_factory;
  return 0;
}

/**************************************************************************
                          Screen shot
**************************************************************************/
MNM_Dta_Screenshot::MNM_Dta_Screenshot(std::string file_folder, MNM_ConfReader* config, PNEGraph graph)
{
  m_graph = graph;
  m_file_folder = file_folder;
  m_config = config;
  m_veh_factory = new MNM_Veh_Factory();
  m_node_factory = new MNM_Node_Factory();
  m_link_factory = new MNM_Link_Factory();
}

MNM_Dta_Screenshot::~MNM_Dta_Screenshot()
{
  delete m_veh_factory;
  delete m_node_factory;
  delete m_link_factory;
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

MNM_Dta_Screenshot *make_screenshot(std::string file_folder, MNM_ConfReader* config, 
                                            MNM_Link_Factory *link_factory, MNM_Node_Factory *node_factory, PNEGraph graph)
{
  MNM_Dta_Screenshot *_shot = new MNM_Dta_Screenshot(file_folder, config, graph);
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
    }

    for (MNM_Veh* _veh : _dlink -> m_incoming_array){
      _new_veh = _new_veh_factory -> make_veh(_veh -> m_start_time, _veh -> m_type);
      copy_veh(_veh, _new_veh);
      _new_dlink -> m_incoming_array.push_back(_new_veh);
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
      }
    }
  }

  //TODO node vehicle migration
  return _shot;
}

}// end namespace MNM_Dta

