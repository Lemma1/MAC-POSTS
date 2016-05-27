#include "vehicle.h"

MNM_Veh::MNM_Veh(TInt ID) {
  m_veh_ID = ID;
  m_current_link = TInt(-1);
  m_future_links = std::deque<TInt>();
}

int MNM_Veh::set_current_link(TInt ID) {
  m_current_link = ID;
  return 0;
}

TInt MNM_Veh::get_current_link() {
  return m_current_link;
}


TInt MNM_Veh::get_next_link()
{
  return m_future_links.front();
}

bool MNM_Veh::has_next_link()
{
  return m_future_links.size() >= 1;
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