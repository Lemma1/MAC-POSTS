#ifndef MARGINAL_COST_H
#define MARGINAL_COST_H

#include "Snap.h"
#include "factory.h"
#include "vehicle.h"

struct Vehicle_Record {
  MNM_Dlink* m_link;
  TFlt m_start_time; 
};

class MNM_Link_Tt
{
public:
  MNM_Link_Tt(MNM_Link_Factory *link_facotory, TFlt unit_time);
  ~MNM_Link_Tt();
  int update_tt(TInt timestamp);

  int intialize();
  void print_info();
  inline int register_vehicle(MNM_Veh* veh, TInt current_time);
  inline int change_link(MNM_Veh* veh, MNM_Dlink *link, TInt current_time);
  inline int update_indiviual_tt(MNM_Dlink *link, TFlt tt);
  TFlt m_update_speed;
  TFlt m_unit_time;
  MNM_Link_Factory *m_link_factory;
  std::map<MNM_Veh*, Vehicle_Record*> m_tracker;
  std::map<MNM_Dlink*, TFlt> m_tt_map;
};





namespace MNM{
  TFlt calculate_link_mc(MNM_Dlink *link, TFlt link_tt);
}
#endif