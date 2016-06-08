#include "vehicle.h"

MNM_Veh::MNM_Veh(TInt ID, TInt start_time) {
  m_veh_ID = ID;
  m_current_link = NULL;
  m_next_link = NULL;
  m_start_time = start_time;
}

int MNM_Veh::set_current_link(MNM_Dlink *link) {
  m_current_link = link;
  return 0;
}

MNM_Dlink *MNM_Veh::get_current_link() {
  return m_current_link;
}


MNM_Dlink *MNM_Veh::get_next_link()
{
  return m_next_link;
}

bool MNM_Veh::has_next_link()
{
  return (m_next_link != NULL);
}


MNM_Destination *MNM_Veh::get_destionation()
{
  return m_dest;
}

int MNM_Veh::set_destination(MNM_Destination * dest)
{
  m_dest = dest;
  return 0;
}


int MNM_Veh::set_next_link(MNM_Dlink *link)
{
  m_next_link = link;
  return 0;
}