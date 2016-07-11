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
  m_out_link_array.clear();
  m_in_link_array.clear();
}


/**************************************************************************
                          Origin node
**************************************************************************/

MNM_DMOND::MNM_DMOND(TInt ID, TFlt flow_scalar)
  : MNM_Dnode::MNM_Dnode(ID, flow_scalar)
{
  m_origin = NULL;
  m_out_volume = std::unordered_map<MNM_Dlink*, TInt>();
  m_in_veh_queue = std::deque<MNM_Veh *>();
}

MNM_DMOND::~MNM_DMOND()
{
  m_in_veh_queue.clear();
  m_out_volume.clear();
}


int MNM_DMOND::evolve(TInt timestamp)
{
  MNM_Dlink *__link, *__to_link;

  for (unsigned i=0; i<m_out_link_array.size(); ++i){
    __link = m_out_link_array[i];
    m_out_volume.find(__link) -> second = 0;
  }  

  /* compute our flow */
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
  MNM_Veh *__veh;
  for (unsigned i=0; i<m_out_link_array.size(); ++i){
    __link = m_out_link_array[i];
    if (m_in_veh_queue.size() != 0){
        // printf("In node %d, %d veh to move to link %d, total veh %d \n", m_node_ID, m_out_volume.find(__link) -> second, __link -> m_link_ID, m_in_veh_queue.size());  
    }
    __que_it = m_in_veh_queue.begin();
    while (__que_it != m_in_veh_queue.end()) {
      if (m_out_volume.find(__link) -> second > 0){
        __veh = *__que_it;
        __to_link = __veh -> get_next_link();
        if (__to_link == __link){
           __to_link -> m_incoming_array.push_back(__veh);
          (__veh) -> set_current_link(__to_link);
          __que_it = m_in_veh_queue.erase(__que_it); //c++ 11
          m_out_volume.find(__to_link) -> second -= 1;
        }
        else{
          __que_it++;
        }
      }
      else{
        break; //break whill loop
      }

    }
  }

  // for (unsigned i=0; i<m_out_link_array.size(); ++i){
  //   __link = m_out_link_array[i];
  //   if (m_out_volume.find(__link) -> second != 0){
  //     printf("Something wrong in moving vehicles on DMOND\n");
  //     printf("Remaining out volume in link %d is %d\n", (int)__link -> m_link_ID, (int)m_out_volume.find(__link) -> second);
  //     exit(-1);
  //   }
  // }  

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

MNM_DMDND::~MNM_DMDND()
{
  m_out_veh_queue.clear();
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
  size_t __size;
  // printf("in link size:%d\n", m_in_link_array.size());
  for (size_t i = 0; i<m_in_link_array.size(); ++i){
    __link = m_in_link_array[i];
    __size = __link->m_finished_array.size();
    for (size_t j=0; j<__size; ++j){
      __veh = __link->m_finished_array.front();
      if (__veh -> get_next_link() != NULL){
        printf("Something wrong in DMDND evolve\n");
        exit(-1);
      }
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
  // printf("num_in: %d, num_out: %d\n", __num_in, __num_out);
  m_demand = (TFlt*) malloc(sizeof(TFlt) * __num_in * __num_out);
  memset(m_demand, 0x0, sizeof(TFlt) * __num_in * __num_out);
  m_supply = (TFlt*) malloc(sizeof(TFlt) * __num_out);
  memset(m_supply, 0x0, sizeof(TFlt) * __num_out);
  m_veh_flow = (TFlt*) malloc(sizeof(TFlt) * __num_in * __num_out);
  memset(m_veh_flow, 0x0, sizeof(TFlt) * __num_in * __num_out);
  m_veh_tomove = (TInt*) malloc(sizeof(TInt) * __num_in * __num_out);
  memset(m_veh_tomove, 0x0, sizeof(TInt) * __num_in * __num_out);
  return 0;
}

int MNM_Dnode_Inout::prepare_supplyANDdemand()
{
  // printf("MNM_Dnode_Inout::prepare_supplyANDdemand\n");
   /* calculate demand */
  size_t __offset = m_out_link_array.size();
  TInt __count;
  std::deque <MNM_Veh*>::iterator __veh_it;
  MNM_Dlink *__in_link, *__out_link;
  for (size_t i=0; i < m_in_link_array.size(); ++i){
    __in_link = m_in_link_array[i];
    for (size_t j=0; j< m_out_link_array.size(); ++j){
      __out_link = m_out_link_array[j];
      // printf("Current out link is %d\n", __out_link -> m_link_ID);
      __count = 0;
      
      for (__veh_it = __in_link -> m_finished_array.begin(); __veh_it != __in_link -> m_finished_array.end(); __veh_it++){
        if ((*__veh_it) -> get_next_link() == __out_link) __count += 1;
      }
      m_demand[__offset*i + j] = TFlt(__count) / m_flow_scalar;
    }
  }
  // printf("Finished\n");
  /* calculated supply */
  for (size_t i=0; i< m_out_link_array.size(); ++i){
    __out_link = m_out_link_array[i];
    // printf("Get link s\n");
    // printf("The out link is %d\n", __out_link -> m_link_ID);
    m_supply[i] = __out_link -> get_link_supply();
    // printf(" get link s fin\n");
    // printf("Link %d, supply is %.4f\n", __out_link -> m_link_ID, m_supply[i]);
  } 

  return 0;
}

int MNM_Dnode_Inout::round_flow_to_vehicle()
{
  // printf("MNM_Dnode_Inout::round_flow_to_vehicle\n");
  // the rounding mechanism may cause the lack of vehicle in m_finished_array
  // but demand is always a integer and only supply can be float, so don't need to worry about it
  size_t __offset = m_out_link_array.size();
  MNM_Dlink *__out_link;
  TInt __to_move;
  size_t __rand_idx;
  for (size_t j=0; j< m_out_link_array.size(); ++j){
    __to_move = 0;
    __out_link = m_out_link_array[j];
    for (size_t i=0; i< m_in_link_array.size(); ++i){
      m_veh_tomove[i * __offset + j] = MNM_Ults::round(m_veh_flow[i * __offset + j] * m_flow_scalar);
      __to_move += m_veh_tomove[i * __offset + j];
      // printf("Rounding %d, %d the value %f to %d\n", i, j, m_veh_flow[i * __offset + j] * m_flow_scalar, m_veh_tomove[i * __offset + j]);
    }
    // printf("Going to loop %d vs supply %f\n", __to_move, __out_link -> get_link_supply());
    while (TFlt(__to_move) > (__out_link -> get_link_supply() * m_flow_scalar)){
      __rand_idx = rand() % m_in_link_array.size();
      if (m_veh_tomove[__rand_idx * __offset + j] >= 1){
        m_veh_tomove[__rand_idx * __offset + j] -= 1;
        __to_move -= 1;
      }
    }
    // printf("Rounding %d, %d the value %f to %d\n", i, j, m_veh_flow[i * __offset + j] * m_flow_scalar, m_veh_tomove[i * __offset + j]);
  }
  return 0;
}

int MNM_Dnode_Inout::move_vehicle()
{
  // printf("MNM_Dnode_Inout::move_vehicle\n");
  MNM_Dlink *__in_link, *__out_link;
  MNM_Veh *__veh;
  size_t __offset = m_out_link_array.size();
  TInt __num_to_move;
  for (size_t j=0; j<m_out_link_array.size(); ++j){
    __out_link = m_out_link_array[j];
    for (size_t i=0; i<m_in_link_array.size(); ++i) {
      __in_link = m_in_link_array[i];
      __num_to_move = m_veh_tomove[i * __offset + j];
      // printf("In node %d, from link %d to link %d, %d to move\n", m_node_ID, __in_link ->m_link_ID, __out_link->m_link_ID, __num_to_move);
      // for (size_t k=0; k<__size; ++k){
      //   if (__num_to_move > 0){
      //     __veh = __in_link->m_finished_array[k];
      //     if (__veh -> get_next_link() == __out_link){
      //       __out_link ->m_incoming_array.push_back(__veh);
      //       __veh -> set_current_link(__out_link);
      //       // __in_link -> m_finished_array.pop_front();

      //       __num_to_move -= 1;            
      //     }
      //   }
      //   else{
      //     break; // break the inner most structure
      //   }
      // }
      auto __veh_it = __in_link->m_finished_array.begin();
      while (__veh_it != __in_link->m_finished_array.end()) {
        if (__num_to_move > 0) {
          __veh = *__veh_it;
          if (__veh -> get_next_link() == __out_link){
            __out_link ->m_incoming_array.push_back(__veh);
            __veh -> set_current_link(__out_link);
            __veh_it = __in_link->m_finished_array.erase(__veh_it); //c++ 11
            __num_to_move -= 1; 
          }
          else {
            __veh_it++;
          }
        }
        else{
          break; // break the inner most structure
        }
      }
      if (__num_to_move != 0){
        printf("Something wrong during the vehicle moving, remaining to move %d\n", (int)__num_to_move);
        printf("The finished veh queue is now %d size\n", (int)__in_link->m_finished_array.size());
        printf("But it is heading to %d\n", (int)__in_link->m_finished_array.front() -> get_next_link() -> m_link_ID);
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
  // printf("1\n");
  prepare_supplyANDdemand();
  // printf("2\n"); 
  compute_flow();
  // printf("3\n");
  round_flow_to_vehicle();
  // printf("4\n");
  move_vehicle();
  // printf("5\n");
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
  // printf("MNM_Dnode_FWJ::compute_flow\n");
  size_t __offset = m_out_link_array.size();
  TFlt __sum_in_flow, __portion;
  for (size_t j=0; j< m_out_link_array.size(); ++j){
    __sum_in_flow = TFlt(0);
    for (size_t i=0; i< m_in_link_array.size(); ++i){
      __sum_in_flow += m_demand[i * __offset + j];
    }
    for (size_t i=0; i< m_in_link_array.size(); ++i){
      __portion = MNM_Ults::divide(m_demand[i * __offset + j], __sum_in_flow);
      // printf("Portion is %.4f, sum in flow is %.4f, demand is %.4f\n", __portion, __sum_in_flow, m_demand[i * __offset + j]);
      m_veh_flow[i * __offset + j] = MNM_Ults::min(m_demand[i * __offset + j], __portion * m_supply[j]);
      // printf("to link %d the flow is %.4f\n", m_out_link_array[j] -> m_link_ID, m_veh_flow[i * __offset + j]);
    }
  }
  return 0;
}
