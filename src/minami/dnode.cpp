#include "dnode.h"


MNM_Dnode::MNM_Dnode(TInt ID, TFlt flow_scalar)
{
  m_node_ID = ID;
  m_flow_scalar = flow_scalar;
  m_out_link_array = std::vector<MNM_Dlink*>();
  m_in_link_array = std::vector<MNM_Dlink*>();
}

MNM_Dnode::~MNM_Dnode()
{

}


/**************************************************************************
                          Origin node
**************************************************************************/

MNM_DMOND::MNM_DMOND(TInt ID, TFlt flow_scalar)
  : MNM_Dnode::MNM_Dnode(ID, flow_scalar)
{
  m_origin = NULL;
  m_out_volume = std::map<MNM_Dlink*, TInt>();
  m_in_veh_queue = std::deque<MNM_Veh *>();
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


/**************************************************************************
                              In-out node
**************************************************************************/
MNM_Dnode_Inout::MNM_Dnode_Inout(TInt ID, TFlt flow_scalar)
  : MNM_Dnode::MNM_Dnode(ID, flow_scalar)
{
  m_demand = NULL;
  m_supply = NULL;
  m_veh_flow = NULL;
  m_veh_tomove = NULL;
}

MNM_Dnode_Inout::~MNM_Dnode_Inout()
{
  if (m_demand != NULL) free(m_demand);
  if (m_supply != NULL) free(m_supply);
  if (m_veh_flow != NULL) free(m_veh_flow);
  if (m_veh_tomove != NULL) free(m_veh_tomove);
}

int MNM_Dnode_Inout::prepare_loading()
{
  TInt __num_in = m_in_link_array.size();
  TInt __num_out = m_out_link_array.size();
  m_demand = (TFlt*) malloc(sizeof(TFlt) * __num_in * __num_out);
  memset(m_demand, 0x0, sizeof(TFlt) * __num_in * __num_out);
  m_supply = (TFlt*) malloc(sizeof(TFlt) * __num_out);
  memset(m_supply, 0x0, sizeof(TFlt) * __num_out);
  m_veh_flow = (TFlt*) malloc(sizeof(TFlt) * __num_out);
  memset(m_veh_flow, 0x0, sizeof(TFlt) * __num_in * __num_out);
  m_veh_tomove = (TInt*) malloc(sizeof(TInt) * __num_in * __num_out);
  memset(m_veh_tomove, 0x0, sizeof(TInt) * __num_in * __num_out);
  return 0;
}

int MNM_Dnode_Inout::prepare_supplyANDdemand()
{
   /* calculate demand */
  size_t __offset = m_out_link_array.size();
  TInt __count;
  std::deque <MNM_Veh*>::iterator __veh_it;
  MNM_Dlink *__in_link, *__out_link;
  for (size_t i=0; i < m_in_link_array.size(); ++i){
    __in_link = m_in_link_array[i];
    for (size_t j=0; j< m_out_link_array.size(); ++j){
      __count = 0;
      __out_link = m_out_link_array[j];
      for (__veh_it = __in_link -> m_finished_array.begin(); __veh_it != __in_link -> m_finished_array.end(); __veh_it++){
        if ((*__veh_it) -> get_next_link() == __out_link) __count += 1;
      }
      m_demand[__offset*i + j] = TFlt(__count) / m_flow_scalar;
    }
  }

  /* calculated supply */
   for (size_t i=0; i< m_out_link_array.size(); ++i){
    __out_link = m_out_link_array[i];
    m_supply[i] = __out_link -> get_link_supply();
  } 

  return 0;
}

int MNM_Dnode_Inout::round_flow_to_vehicle()
{
  // the rounding mechanism may cause the lack of vehicle in m_finished_array
  // but demand is always a integer and only supply can be float, so don't need to worry about it
  size_t __offset = m_out_link_array.size();
  for (size_t i=0; i< m_in_link_array.size(); ++i){
    for (size_t j=0; j< m_out_link_array.size(); ++j){
      m_veh_tomove[i * __offset + j] = MNM_Ults::round(m_veh_flow[i * __offset + j] * m_flow_scalar);
    }
  }
  return 0;
}

int MNM_Dnode_Inout::move_vehicle()
{
  MNM_Dlink *__in_link, *__out_link;
  MNM_Veh *__veh;
  size_t __size;
  size_t __offset = m_out_link_array.size();
  TInt __num_to_move;
  for (size_t j=0; j<m_out_link_array.size(); ++j){
    __out_link = m_out_link_array[j];
    for (size_t i=0; i<m_in_link_array.size(); ++i) {
      __in_link = m_in_link_array[i];
      __num_to_move = m_veh_tomove[i * __offset + j];
      __size = __in_link->m_finished_array.size();
      for (size_t k=0; k<__size; ++k){
        if (__num_to_move > 0){
          __veh = __in_link->m_finished_array.front();
          __out_link ->m_incoming_array.push_back(__veh);
          __veh -> set_current_link(__out_link);
          __in_link -> m_finished_array.pop_front();
          __num_to_move -= 1;
        }
        else{
          break; // break the inner most structure
        }
      }
      if (__num_to_move != 0){
        printf("Something wrong during the vehicle moving.\n");
        exit(-1);
      }
    }
    // make the queue randomly perturbed, may not be true in signal controlled intersection
    // TODO 
    random_shuffle(__out_link -> m_incoming_array.begin(), __out_link -> m_incoming_array.end());
  }
  return 0;
}

void MNM_Dnode_Inout::print_info()
{
  ;
}

int MNM_Dnode_Inout::add_out_link(MNM_Dlink* out_link)
{
  m_out_link_array.push_back(out_link);
  return 0;
}

int MNM_Dnode_Inout::add_in_link(MNM_Dlink *link)
{
  m_in_link_array.push_back(link);
  return 0;
}


int MNM_Dnode_Inout::evolve(TInt timestamp)
{
  prepare_supplyANDdemand();
  compute_flow();
  round_flow_to_vehicle();
  move_vehicle();
  return 0;
}
/**************************************************************************
                              FWJ node
**************************************************************************/

MNM_Dnode_FWJ::MNM_Dnode_FWJ(TInt ID, TFlt flow_scalar)
  : MNM_Dnode_Inout::MNM_Dnode_Inout(ID, flow_scalar)
{
}

MNM_Dnode_FWJ::~MNM_Dnode_FWJ()
{

}

void MNM_Dnode_FWJ::print_info()
{
  ;
}

int MNM_Dnode_FWJ::compute_flow()
{
  size_t __offset = m_out_link_array.size();
  TFlt __sum_in_flow, __portion;
  for (size_t j=0; j< m_out_link_array.size(); ++j){
    __sum_in_flow = 0;
    for (size_t i=0; i< m_in_link_array.size(); ++i){
      __sum_in_flow += m_demand[i * __offset + j];
    }
    for (size_t i=0; i< m_in_link_array.size(); ++i){
      __portion = MNM_Ults::divide(m_demand[i * __offset + j], __sum_in_flow);
      m_veh_flow[i * __offset + j] = MNM_Ults::min(m_demand[i * __offset + j], __portion * m_supply[j]);
    }
  }
  return 0;
}
