#ifndef MULTICLASS_H
#define MULTICLASS_H

#include "dlink.h"


/**************************************************************************
							Multiclass CTM
**************************************************************************/
class MNM_Dlink_Ctm_Multiclass : public MNM_Dlink
{
public:
	MNM_Dlink_Ctm_Multiclass(TInt ID,
							 TFlt lane_hold_cap,
							 TFlt lane_flow_cap,
							 TInt number_of_lane,
							 TFlt length,
							 TFlt ffs,
							 TFlt unit_time,
							 TFlt flow_scalar);
	~MNM_Dlink_Ctm_Multiclass();
	int virtual evolve(TInt timestamp) override;
	TFlt virtual get_link_supply() override;
	int virtual clear_incoming_array() override;
	void virtual print_info() override;
	TFlt virtual get_link_flow() override;
	TFlt virtual get_link_tt() override;

	class Ctm_Cell_Multiclass;
	int init_cell_array(TFlt unit_time, TFlt std_cell_length, TFlt lane_hold_cap_last_cell);
	int update_out_veh();
	int move_last_cell();
	TInt m_num_cells;
	TFlt m_lane_hold_cap;
	TFlt m_lane_flow_cap;
	TFlt m_flow_scalar;
	TFlt m_wave_ratio;
	TFlt m_last_wave_ratio;
	std::vector<Ctm_Cell_Multiclass*> m_cell_array;
};

class MNM_Dlink_Ctm_Multiclass::Ctm_Cell_Multiclass
{
public:
	Ctm_Cell_Multiclass(TFlt hold_cap, TFlt flow_cap, TFlt flow_scalar, TFlt wave_ratio);
	~Ctm_Cell_Multiclass();
	TFlt get_demand();
	TFlt get_supply();

	TFlt m_volume;
	TFlt m_flow_scalar;
	TFlt m_hold_cap;
	TFlt m_flow_cap;
	TFlt m_wave_ratio;
	TFlt m_out_veh;
	std::deque<MNM_Veh*> m_veh_queue;
};

#endif
