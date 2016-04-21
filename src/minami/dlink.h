#ifndef DLINK_H
#define DLINK_H

#include "Snap.h"

#include "g3log/g3log.hpp"
#include "g3log/logworker.hpp"

#include <vector>
#include <deque>
#include <map>

class MNM_Veh;

class MNM_Dlink
{
public:
  MNM_Dlink( TInt number_of_lane,
             TFlt length,
             TFlt ffs );
  ~MNM_Dlink();
  void virtual evolve(TInt timestamp){};

  TInt m_number_of_lane;
  TFlt m_length;
  TFlt m_ffs;

  std::map<MNM_Dlink*, std::deque<MNM_Veh*>> m_finished_array;
  std::deque<MNM_Veh *> m_incoming_array;
};


class MNM_Dlink_Ctm : MNM_Dlink
{
public:
  MNM_Dlink_Ctm(TFlt lane_hold_cap, 
                TFlt lane_flow_cap, 
                TInt number_of_lane,
                TFlt length,
                TFlt ffs,
                TFlt unit_time,
                TFlt flow_scalar);
  ~MNM_Dlink_Ctm();

private:
  class Ctm_Cell;
  int init_cell_array(TFlt unit_time, TFlt std_cell_length, TFlt lane_hold_cap_last_cell);
  int update_in_out_number();
  TInt m_num_cells;
  TFlt m_lane_hold_cap;
  TFlt m_lane_flow_cap;
  TFlt m_flow_scalar;
  TFlt m_wave_ratio;
  TFlt m_last_wave_ratio;
  std::vector<Ctm_Cell*> m_cell_array;
};


class MNM_Dlink_Ctm::Ctm_Cell
{
public:
  Ctm_Cell(TFlt hold_cap, TFlt flow_cap, TFlt flow_scalar, TFlt wave_ratio);
  ~Ctm_Cell();
  TFlt get_demand();
  TFlt get_supply();
  TInt m_volume;
  TFlt m_flow_scalar;
  TFlt m_hold_cap;
  TFlt m_flow_cap;
  TFlt m_wave_ratio;
  TInt m_out_veh;
  std::deque<MNM_Veh*>  m_veh_queue; 
};


class MNM_Veh
{
public:
  MNM_Veh();
  ~MNM_Veh();
private:
  TInt m_ID;
  MNM_Dlink* m_current_link;
  std::deque<MNM_Dlink*> m_future_links;
};
#endif