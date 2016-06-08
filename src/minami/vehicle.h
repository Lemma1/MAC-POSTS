#ifndef VEHICLE_H
#define VEHICLE_H

#include "od.h"
#include "Snap.h"
#include "dlink.h"

#include <deque>

class MNM_Destination;
class MNM_Dlink;

class MNM_Veh
{
public:
  MNM_Veh(TInt ID, TInt start_time);
  ~MNM_Veh();
  TInt m_veh_ID;
  int set_current_link(MNM_Dlink *link);
  MNM_Dlink *get_current_link();
  MNM_Dlink *get_next_link();
  int set_next_link(MNM_Dlink *link);
  bool has_next_link();
  MNM_Destination *get_destionation();
  int set_destination(MNM_Destination * dest);
  int finish(TInt finish_time);
private:
  MNM_Dlink *m_current_link;
  TInt m_start_time;
  TInt m_finish_time;
  MNM_Dlink *m_next_link;
  MNM_Destination *m_dest;
};

#endif