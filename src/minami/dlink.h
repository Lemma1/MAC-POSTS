#ifndef DLINK_H
#define DLINK_H

#include "vehicle.h"
#include "ults.h"

// #include "g3log/g3log.hpp"
// #include "g3log/logworker.hpp"

#include <vector>
#include <deque>
#include <unordered_map>

class MNM_Veh;
class MNM_Dnode;

class MNM_Dlink
{
public:
  MNM_Dlink( TInt ID,
             TInt number_of_lane,
             TFlt length,
             TFlt ffs );
  virtual ~MNM_Dlink();
  int virtual evolve(TInt timestamp) {return 0;};
  TFlt virtual get_link_supply() {return TFlt(0);};
  int virtual clear_incoming_array() {return 0;};
  void virtual print_info() {};
  int hook_up_node(MNM_Dnode *from, MNM_Dnode *to);
  TFlt virtual get_link_flow() {return TFlt(0);};
  TFlt virtual get_link_tt() {return TFlt(0);};
// protected:
  TInt m_link_ID;
  MNM_Dnode *m_from_node;
  MNM_Dnode *m_to_node;
  TInt m_number_of_lane;
  TFlt m_length;
  TFlt m_ffs;
  std::deque<MNM_Veh *> m_finished_array;
  std::deque<MNM_Veh *> m_incoming_array;

protected:
  int move_veh_queue(std::deque<MNM_Veh*> *from_queue, std::deque<MNM_Veh*> *to_queue, TInt number_tomove);
};


class MNM_Dlink_Ctm : public MNM_Dlink
{
public:
  MNM_Dlink_Ctm(TInt ID,
                TFlt lane_hold_cap, 
                TFlt lane_flow_cap, 
                TInt number_of_lane,
                TFlt length,
                TFlt ffs,
                TFlt unit_time,
                TFlt flow_scalar);
  ~MNM_Dlink_Ctm();
  int virtual evolve(TInt timestamp) override;
  TFlt virtual get_link_supply() override;
  int virtual clear_incoming_array() override;
  void virtual print_info() override;
  TFlt virtual get_link_flow() override;
  TFlt virtual get_link_tt() override;

// private:
  class Ctm_Cell;
  int init_cell_array(TFlt unit_time, TFlt std_cell_length, TFlt lane_hold_cap_last_cell);
  int update_out_veh();
  int move_last_cell();
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

/**************************************************************************
                          Poing Queue
**************************************************************************/
class MNM_Dlink_Pq : public MNM_Dlink
{
public:
  MNM_Dlink_Pq(TInt ID,
               TFlt lane_hold_cap, 
               TFlt lane_flow_cap, 
               TInt number_of_lane,
               TFlt length,
               TFlt ffs,
               TFlt unit_time,
               TFlt flow_scalar);
  ~MNM_Dlink_Pq();
  int virtual evolve(TInt timestamp) override;
  TFlt virtual get_link_supply() override;
  int virtual clear_incoming_array() override;
  void virtual print_info() override;
  TFlt virtual get_link_flow() override;
  TFlt virtual get_link_tt() override;
// private:
  std::unordered_map<MNM_Veh*, TInt> m_veh_queue;
  TInt m_volume; //vehicle number, without the flow scalar
  TFlt m_lane_hold_cap;
  TFlt m_lane_flow_cap;
  TFlt m_flow_scalar;
  TFlt m_hold_cap;
  TInt m_max_stamp;
};



/**************************************************************************
                          Link Queue
**************************************************************************/
class MNM_Dlink_Lq : public MNM_Dlink
{
public:
  MNM_Dlink_Lq(TInt ID,
               TFlt lane_hold_cap, 
               TFlt lane_flow_cap, 
               TInt number_of_lane,
               TFlt length,
               TFlt ffs,
               TFlt unit_time,
               TFlt flow_scalar);
  ~MNM_Dlink_Lq();
  int virtual evolve(TInt timestamp) override;
  TFlt virtual get_link_supply() override;
  int virtual clear_incoming_array() override;
  void virtual print_info() override;
  TFlt virtual get_link_flow() override;
  TFlt virtual get_link_tt() override;
// private:
  std::deque<MNM_Veh*> m_veh_queue;
  TInt m_volume; //vehicle number, without the flow scalar
  TFlt m_lane_hold_cap;
  TFlt m_lane_flow_cap;
  TFlt m_flow_scalar;
  TFlt m_hold_cap;
  TFlt m_C;  //maximum free flow capacity
  TFlt m_k_c; //maximum free flow density
  TFlt m_unit_time;

  TFlt get_flow_from_density(TFlt density);
  TFlt get_demand();
};




#endif