#ifndef MULTICLASS_H
#define MULTICLASS_H

#include "dlink.h"

/**************************************************************************
							Multiclass CTM
			(currently only for car & truck two classes)
	(see: Z. (Sean) Qian et al./Trans. Res. Part B 99 (2017) 183-204)
**************************************************************************/
class MNM_Dlink_Ctm_Multiclass : public MNM_Dlink
{
public:
	MNM_Dlink_Ctm_Multiclass(TInt ID,
							 TInt number_of_lane,
							 TFlt length,
							 TFlt lane_hold_cap_car,
							 TFlt lane_hold_cap_truck,
							 TFlt lane_flow_cap_car,
							 TFlt lane_flow_cap_truck,
							 TFlt ffs_car,
							 TFlt ffs_truck,
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
	int init_cell_array(TFlt unit_time, TFlt std_cell_length, TFlt last_cell_length);
	int update_out_veh();
	int move_last_cell();
	TInt m_num_cells;
	TFlt m_ffs_car;
	TFlt m_ffs_truck;
	TFlt m_lane_hold_cap_car;
	TFlt m_lane_hold_cap_truck;
	TFlt m_lane_critical_density_car;
	TFlt m_lane_critical_density_truck;
	TFlt m_lane_rho_1_N;
	TFlt m_lane_flow_cap_car;
	TFlt m_lane_flow_cap_truck;
	TFlt m_flow_scalar;
	TFlt m_wave_speed_car;
	TFlt m_wave_speed_truck;
	std::vector<Ctm_Cell_Multiclass*> m_cell_array;
};

class MNM_Dlink_Ctm_Multiclass::Ctm_Cell_Multiclass
{
public:
	Ctm_Cell_Multiclass(TFlt cell_length,
						TFlt unit_time,
						TFlt hold_cap_car,
						TFlt hold_cap_truck, 
						TFlt critical_density_car,
						TFlt critical_density_truck,
						TFlt m_rho_1_N,
						TFlt flow_cap_car,
						TFlt flow_cap_truck,
						TFlt ffs_car,
						TFlt ffs_truck,
						TFlt wave_speed_car,
						TFlt wave_speed_truck,
						TFlt flow_scalar);
	~Ctm_Cell_Multiclass();
	TFlt get_perceived_demand(TInt veh_type);
	TFlt get_perceived_supply(TInt veh_type);
	int update_perceived_density();

	TFlt m_cell_length;
	TFlt m_unit_time;
	TFlt m_flow_scalar;
	
	TFlt m_hold_cap_car;
	TFlt m_hold_cap_truck;
	TFlt m_critical_density_car;
	TFlt m_critical_density_truck;
	TFlt m_rho_1_N;
	TFlt m_flow_cap_car;
	TFlt m_flow_cap_truck;
	TFlt m_ffs_car;
	TFlt m_ffs_truck;
	TFlt m_wave_speed_car;
	TFlt m_wave_speed_truck;

	TInt m_volume_car;
	TInt m_volume_truck;
	TFlt m_space_fraction_car;
	TFlt m_space_fraction_truck;
	TFlt m_perceived_density_car;
	TFlt m_perceived_density_truck;
	TInt m_out_veh_car;
	TInt m_out_veh_truck;
	std::deque<MNM_Veh*> m_veh_queue_car;
	std::deque<MNM_Veh*> m_veh_queue_truck;
};

#endif
