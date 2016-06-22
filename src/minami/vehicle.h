#ifndef VEHICLE_H
#define VEHICLE_H

#include "od.h"
#include "Snap.h"
#include "dlink.h"
#include "enum.h"

#include <deque>

class MNM_Destination;
class MNM_Origin;
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
  MNM_Origin *get_origin();
  int set_origin(MNM_Origin * origin);
// private:
  Vehicle_type m_type;
  MNM_Dlink *m_current_link;
  TInt m_start_time;
  TInt m_finish_time;
  MNM_Dlink *m_next_link;
  MNM_Destination *m_dest;
  MNM_Origin *m_origin;
};


// class MNM_Veh_Det :: public MNM_Veh
// {
// public:
//   MNM_Veh_Det(TInt ID, TInt start_time);
//   ~MNM_Veh_Det();
// };


// class MNM_Veh_Adp :: public MNM_Veh
// {
// public:
//   MNM_Veh_Adp(TInt ID, TInt start_time);
//   ~MNM_Veh_Adp();
// };
#endif