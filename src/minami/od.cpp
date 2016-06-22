#include "od.h"

MNM_Origin::MNM_Origin(TInt ID, TInt max_interval, TFlt flow_scalar, TInt frequency)
{
  m_Origin_ID = ID;
  m_max_assign_interval = max_interval;
  m_flow_scalar = flow_scalar;
  m_current_assign_interval = 0;
  if (frequency <= 0){
    printf("MNM_Origin::MNM_Origin::Wrong frequency.\n");
    exit(-1);
  }
  m_frequency = frequency;
  m_demand = std::map<MNM_Destination*, TFlt*>();
}

MNM_Origin::~MNM_Origin()
{
  for (auto __demand_it = m_demand.begin(); __demand_it != m_demand.end(); __demand_it++) {
    free(__demand_it -> second);
  }
  m_demand.clear();
}

int MNM_Origin::add_dest_demand(MNM_Destination *dest, TFlt* demand)
{
  TFlt* __demand = (TFlt*) malloc(sizeof(TFlt) * m_max_assign_interval);
  for (int i = 0; i < m_max_assign_interval; ++i) {
    __demand[i] =  TFlt(demand[i]);
  }
  m_demand.insert(std::pair<MNM_Destination*, TFlt*>(dest, __demand));
  return 0;
}

int MNM_Origin::release(MNM_Veh_Factory* veh_factory, TInt current_interval)
{
  if (m_current_assign_interval < m_max_assign_interval && current_interval % m_frequency == 0){
    TInt __veh_to_release;
    MNM_Veh *__veh;
    std::map<MNM_Destination*, TFlt*>::iterator __demand_it;
    for (__demand_it = m_demand.begin(); __demand_it != m_demand.end(); __demand_it++) {
      __veh_to_release = TInt(MNM_Ults::round((__demand_it -> second)[m_current_assign_interval] * m_flow_scalar));
      for (int i=0; i<__veh_to_release; ++i) {
        __veh = veh_factory -> make_veh(current_interval);
        __veh -> set_destination(__demand_it -> first);
        m_origin_node -> m_in_veh_queue.push_back(__veh);
      }
    }
    m_current_assign_interval ++;
  }
  return 0;
}

MNM_Destination::MNM_Destination(TInt ID)
{
  m_Dest_ID = ID;
}


MNM_Destination::~MNM_Destination()
{
}

int MNM_Destination::receive(TInt current_interval)
{

  MNM_Veh *__veh;
  size_t __num_to_receive = m_dest_node -> m_out_veh_queue.size();
  for (size_t i=0; i < __num_to_receive; ++i){
    __veh = m_dest_node -> m_out_veh_queue.front();
    if (__veh -> get_destionation() != this){
      printf("The veh is heading to %d, but we are %d\n", (int)__veh -> get_destionation() -> m_dest_node -> m_node_ID, (int)m_dest_node -> m_node_ID);
      printf("MNM_Destination::receive: Something wrong!\n");
      exit(-1);
    }
    __veh -> finish(current_interval);
    // printf("_______Receive Vehicle ID: %d\n", (int) __veh -> m_veh_ID);
    m_dest_node -> m_out_veh_queue.pop_front();
  }
  
  return 0;
}


