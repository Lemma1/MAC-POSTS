#include "od.h"

MNM_Origin::MNM_Origin(TInt ID, TInt max_interval, TFlt flow_scalar)
{
  m_Origin_ID = ID;
  m_max_interval = max_interval;
  m_flow_scalar = flow_scalar;
  m_current_interval = 0;
  m_demand = std::map<MNM_Destination*, TFlt*>();
}

MNM_Origin::~MNM_Origin()
{

}

int MNM_Origin::add_dest_demand(MNM_Destination *dest, TFlt* demand)
{
  TFlt* __demand = (TFlt*) malloc(sizeof(TFlt) * m_max_interval);
  for (int i = 0; i < m_max_interval; ++i) {
    __demand[i] =  TFlt(demand[i]);
  }
  m_demand.insert(std::pair<MNM_Destination*, TFlt*>(dest, __demand));
  return 0;
}

int MNM_Origin::release(MNM_Veh_Factory* veh_factory)
{
  TInt __veh_to_release;
  MNM_Veh *__veh;
  std::map<MNM_Destination*, TFlt*>::iterator __demand_it;
  for (__demand_it = m_demand.begin(); __demand_it != m_demand.end(); __demand_it++) {
    __veh_to_release = TInt(MNM_Ults::round((__demand_it -> second)[m_current_interval] * m_flow_scalar));
    for (int i=0; i<__veh_to_release; ++i) {
      __veh = veh_factory -> make_veh(m_current_interval);
      m_origin_node -> m_in_veh_queue.push_back(__veh);
    }
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

int MNM_Destination::receive_veh(MNM_Veh* veh)
{
  free(veh);
  return 0;
}


