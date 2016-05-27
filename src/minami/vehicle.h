#ifndef VEHICLE_H
#define VEHICLE_H

#include "od.h"
#include "Snap.h"

#include <deque>

class MNM_Destination;

class MNM_Veh
{
public:
  MNM_Veh(TInt ID);
  ~MNM_Veh();
  int set_current_link(TInt ID);
  TInt get_current_link();
  TInt get_next_link();
  bool has_next_link();
  MNM_Destination *get_destionation();
  int set_destination(MNM_Destination * dest);
private:
  TInt m_veh_ID;
  TInt m_current_link;
  MNM_Destination *m_dest;
  std::deque<TInt> m_future_links;
};

#endif