#include "vms.h"

MNM_Link_Vms::MNM_Link_Vms(TInt ID, TInt link_ID, PNEGraph graph)
{
  m_ID = ID;
  m_my_link_ID = link_ID;
  m_compliance_ratio = TFlt(1);
  m_out_link_vec = std::vector<TInt>();
  m_link_path_map = std::map<TInt, std::vector<MNM_Path*>*>();
  hook_link(graph);
}


MNM_Link_Vms::~MNM_Link_Vms()
{
  for (auto _it : m_link_path_map){
    delete _it.second;
  }
  m_link_path_map.clear();
  m_out_link_vec.clear();
}

int MNM_Link_Vms::hook_link(PNEGraph graph)
{
  auto _node_it = graph -> GetNI(graph -> GetEI(m_my_link_ID).GetSrcNId());
  for (int e = 0; e < _node_it.GetOutDeg(); ++e) {
    m_out_link_vec.push_back(_node_it.GetOutEId(e));
    std::vector<MNM_Path*> *_v = new std::vector<MNM_Path*>();
    m_link_path_map.insert(std::pair<TInt, std::vector<MNM_Path*>*>(_node_it.GetOutEId(e), _v));
  }
  return 0;
}


int MNM_Link_Vms::hook_path(Path_Table *path_table)
{
  for(auto _it : *path_table){
    for (auto _it_it : *(_it.second)){
      for (MNM_Path* _path : _it_it.second -> m_path_vec){
        for (TInt _link_ID : m_out_link_vec){
          // printf("Current link is %d\n", _link_ID);
          if (std::find(_path -> m_link_vec.begin(), _path -> m_link_vec.end(), _link_ID) != _path -> m_link_vec.end()){
            m_link_path_map.find(_link_ID) -> second -> push_back(_path);
          }
        }
      }
    }
  }
  return 0;
}


/**************************************************************************
                          VMS factory
**************************************************************************/

MNM_Vms_Factory::MNM_Vms_Factory()
{
  m_link_vms_map = std::map<TInt, MNM_Link_Vms*>();
}

MNM_Vms_Factory::~MNM_Vms_Factory()
{
  for (auto _it : m_link_vms_map) {
    delete _it.second;
  }
  m_link_vms_map.clear();
}

MNM_Link_Vms *MNM_Vms_Factory::make_link_vms(TInt ID, TInt link_ID, PNEGraph graph)
{
  MNM_Link_Vms *_vms = new MNM_Link_Vms(ID, link_ID, graph);
  m_link_vms_map.insert(std::pair<TInt, MNM_Link_Vms*>(ID, _vms));
  return _vms;
}

MNM_Link_Vms *MNM_Vms_Factory::get_link_vms(TInt ID)
{
  return m_link_vms_map.find(ID) -> second;
}

int MNM_Vms_Factory::hook_path(Path_Table *path_table)
{
  for (auto _it : m_link_vms_map) {
    _it.second -> hook_path(path_table);
  }
  return 0;
}