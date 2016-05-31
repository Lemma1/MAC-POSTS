#include "dnode.h"


MNM_Dnode::MNM_Dnode(TInt ID, TFlt flow_scalar)
{
  m_node_ID = ID;
  m_flow_scalar = flow_scalar;
}

MNM_Dnode::~MNM_Dnode()
{

}
/**************************************************************************
                              FWJ
**************************************************************************/
MNM_Dnode_FWJ::MNM_Dnode_FWJ(TInt ID, TFlt flow_scalar)
  : MNM_Dnode::MNM_Dnode(ID, flow_scalar)
{

}

int MNM_Dnode_FWJ::evolve(TInt timestamp)
{
  return 0;
}

void MNM_Dnode_FWJ::print_info()
{
  ;
}

/**************************************************************************
                          Origin node
**************************************************************************/

MNM_DMOND::MNM_DMOND(TInt ID, TFlt flow_scalar)
  : MNM_Dnode::MNM_Dnode(ID, flow_scalar)
{
  m_origin = NULL;
  m_out_link_array = std::vector<MNM_Dlink*>();
  m_out_volume = std::map<MNM_Dlink*, TInt>();
}

int MNM_DMOND::evolve(TInt timestamp)
{
  /* compute our flow */
  MNM_Dlink *__link;
  std::deque<MNM_Veh*>::iterator __que_it = m_in_veh_queue.begin();
  while (__que_it != m_in_veh_queue.end()) {
    __link = (*__que_it) -> get_next_link();
    m_out_volume.find(__link) -> second += 1;
    __que_it++;
  }
  for (unsigned i=0; i<m_out_link_array.size(); ++i){
    __link = m_out_link_array[i];
    if ((__link -> get_link_supply() * m_flow_scalar) < TFlt(m_out_volume.find(__link) -> second)){
      m_out_volume.find(__link) -> second = TInt(MNM_Ults::round(__link -> get_link_supply() * m_flow_scalar));
    }
  }
  /* move vehicle */
  __que_it = m_in_veh_queue.begin();
  TInt __count;
  while (__que_it != m_in_veh_queue.end()) {
    __link = (*__que_it) -> get_next_link();
    __count = m_out_volume.find(__link) -> second;
    if (__count > 0){
      __link -> m_incoming_array.push_back(*__que_it);
      (*__que_it) -> set_current_link(__link);
      __que_it = m_in_veh_queue.erase(__que_it); //c++ 11
      m_out_volume.find(__link) -> second -= 1;
    }
    else{
      __que_it++;
    }
  }
  return 0;
}


int MNM_DMOND::add_out_link(MNM_Dlink* out_link)
{
  m_out_link_array.push_back(out_link);
  m_out_volume.insert(std::pair<MNM_Dlink*, TInt>(out_link, TInt(0)));
  return 0;
}

void MNM_DMOND::print_info()
{
  ;
}

int MNM_DMOND::hook_up_origin(MNM_Origin *origin)
{
  m_origin = origin;
  return 0;
}

/**************************************************************************
                          Destination node
**************************************************************************/

MNM_DMDND::MNM_DMDND(TInt ID, TFlt flow_scalar)
  : MNM_Dnode::MNM_Dnode(ID, flow_scalar)
{
  m_dest = NULL;
  m_in_link_array = std::vector<MNM_Dlink*>();
}

int MNM_DMDND::add_in_link(MNM_Dlink *link)
{
  m_in_link_array.push_back(link);
  return 0;
}

int MNM_DMDND::evolve(TInt timestamp)
{
  MNM_Dlink *__link;
  MNM_Veh *__veh;
  unsigned __size;
  for (unsigned i=0; i<m_in_link_array.size(); ++i){
    __link = m_in_link_array[i];
    __size = __link->m_finished_array.size();
    for (unsigned j=0; j<__size; ++j){
      __veh = __link->m_finished_array.front();
      m_out_veh_queue.push_back(__veh);
      __veh -> set_current_link(NULL);
      __link -> m_finished_array.pop_front();
    }
  }
  return 0;
}

void MNM_DMDND::print_info()
{
  ;
}

int MNM_DMDND::hook_up_destination(MNM_Destination *dest)
{
  m_dest = dest;
  return 0;
}