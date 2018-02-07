#ifndef MULTICLASS_H
#define MULTICLASS_H

#include "dlink.h"
#include "dnode.h"
#include "vehicle.h"


/******************************************************************************************************************
*******************************************************************************************************************
												Link Models
*******************************************************************************************************************
******************************************************************************************************************/


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
							 TFlt veh_convert_factor,
							 TFlt flow_scalar);
	~MNM_Dlink_Ctm_Multiclass();
	int virtual evolve(TInt timestamp) override;
	TFlt virtual get_link_supply() override;
	int virtual clear_incoming_array() override;
	void virtual print_info() override;
	TFlt virtual get_link_flow() override;
	TFlt virtual get_link_tt() override;

	// use this one instead of the one in Dlink class
	int install_cumulative_curve_multiclass();

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
	TFlt m_veh_convert_factor;
	TFlt m_flow_scalar;
	TFlt m_wave_speed_car;
	TFlt m_wave_speed_truck;
	std::vector<Ctm_Cell_Multiclass*> m_cell_array;

	// Two seperate N-curves for private cars and trucks
	MNM_Cumulative_Curve *m_N_in_car;
  	MNM_Cumulative_Curve *m_N_out_car;
  	MNM_Cumulative_Curve *m_N_in_truck;
  	MNM_Cumulative_Curve *m_N_out_truck;
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


/******************************************************************************************************************
*******************************************************************************************************************
												Node Models
*******************************************************************************************************************
******************************************************************************************************************/

class MNM_Dnode_Inout_Multiclass : public MNM_Dnode
{
public:
	MNM_Dnode_Inout_Multiclass(TInt ID, TFlt flow_scalar);
	~MNM_Dnode_Inout_Multiclass();
	int virtual evolve(TInt timestamp) override;
	void virtual print_info();
	int virtual prepare_loading() override;
	int virtual add_out_link(MNM_Dlink* out_link) override;
	int virtual add_in_link(MNM_Dlink* in_link) override;
protected:
	int prepare_supplyANDdemand();
	int virtual compute_flow(){return 0;};
	// int flow_to_vehicle();
	int move_vehicle();
	int record_cumulative_curve(TInt timestamp);
	TFlt *m_demand; //2d
	TFlt *m_supply; //1d
	TFlt *m_veh_flow; //2d
	TFlt *m_veh_moved_car; //2d
	TFlt *m_veh_moved_truck; //2d
};

/**************************************************************************
                              FWJ node
**************************************************************************/
class MNM_Dnode_FWJ_Multiclass : public MNM_Dnode_Inout_Multiclass
{
public:
	MNM_Dnode_FWJ_Multiclass(TInt ID, TFlt flow_scalar);
	~MNM_Dnode_FWJ_Multiclass();
	void virtual print_info() override;
	int virtual compute_flow() override;
};

/**************************************************************************
                   General Road Junction node
**************************************************************************/
class MNM_Dnode_GRJ_Multiclass : public MNM_Dnode_Inout_Multiclass
{
public:
  MNM_Dnode_GRJ_Multiclass(TInt ID, TFlt flow_scalar);
  ~MNM_Dnode_GRJ_Multiclass();
  void virtual print_info() override;
  int virtual compute_flow() override;
  int virtual prepare_loading() override;
private:
  std::vector<std::vector<MNM_Dlink*>> m_pow;
  TFlt get_theta();
  int prepare_outflux();
  TFlt *m_d_a; //1d array
  TFlt *m_C_a; //1d array
  template<typename T> std::vector<std::vector<T> > powerSet(const std::vector<T>& set);
  std::vector<int> getOnLocations(int a);
};



/******************************************************************************************************************
*******************************************************************************************************************
												Vehicle Class
*******************************************************************************************************************
******************************************************************************************************************/

class MNM_Veh_Multiclass : public MNM_Veh
{
public:
  MNM_Veh_Multiclass(TInt ID, TInt vehicle_class, TInt start_time);
  ~MNM_Veh_Multiclass();

  TInt m_class;
};


#endif
