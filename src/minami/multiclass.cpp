#include "dlink.h"
#include "limits.h"

#include <algorithm>

MNM_Dlink_Ctm_Multiclass::MNM_Dlink_Ctm_Multiclass(TInt ID,
												   TFlt lane_hold_cap,
												   TFlt lane_flow_cap,
												   TInt number_of_lane,
												   TFlt length,
												   TFlt ffs,
												   TFlt unit_time,
												   TFlt flow_scalar)
	: MNM_Dlink::MNM_Dlink(ID, number_of_lane, length, ffs)
{
	if (lane_hold_cap < 0){
		printf("lane_hold_cap can't be less than zero, current link ID is %d\n", m_link_ID);
		exit(-1);
	}

	if (lane_hold_cap > TFlt(300) / TFlt(1600)){
		// "lane_hold_cap is too large, set to 300 veh/mile
		lane_hold_cap = TFlt(300) / TFlt(1600)
	}

	if (lane_flow_cap < 0){
		printf("lane_flow_cap can't be less than zero, current link ID is %d\n", m_link_ID);
		exit(-1);
	}

	if (lane_flow_cap > TFlt(3500) / TFlt(3600)){
		// lane_flow_cap is too large, set to 3500 veh/hour
		lane_flow_cap = TFlt(3500) / TFlt(3600);
	}

	m_lane_flow_cap = lane_flow_cap;
	m_flow_scalar = flow_scalar;
	m_cell_array = std::vector<Ctm_Cell*>();
	TFlt _std_cell_length = m_ffs * unit_time;
	m_num_cells = TInt(floor(m_length / _std_cell_length));
	if (m_num_cells == 0){
		m_num_cells = 1;
	}

	TFlt _lane_hold_cap_last_cell = MNM_Ults::max(m_lane_hold_cap,
		((m_length - TFlt(m_num_cells - 1) * _std_cell_length) / _std_cell_length) * m_lane_hold_cap);

	// lane_flow_cap/ffs is the critical density
	TFlt _wave_speed = m_lane_flow_cap / (m_lane_hold_cap - m_lane_flow_cap / m_ffs);

	// note that h >= 2c/v, where h is the holding capacity, c is capacity, v is free-flow speed.
	// i.e., wave_ratio should < 1.
	m_wave_ratio = _wave_speed / m_ffs;
	if (m_wave_ratio < 0){
		printf("Wave ratio can't be less than 0, current link ID is %d\n", m_link_ID);
		exit(-1);
	}

	m_last_wave_ratio = (m_lane_flow_cap / (_lane_hold_cap_last_cell - m_lane_flow_cap / m_ffs))/m_ffs;
	if (m_last_wave_ratio < 0){
		printf("Last cell wave ratio can't be less than 0, current link ID is %d\n", m_link_ID);
		exit(-1);
	}
	init_cell_array(unit_time, _std_cell_length, _lane_hold_cap_last_cell);
}

MNM_Dlink_Ctm_Multiclass::~MNM_Dlink_Ctm_Multiclass()
{
	for (Ctm_Cell* _cell : m_cell_array){
		delete _cell;
	}
	m_cell_array.clear();
}

int MNM_Dlink_Ctm_Multiclass::init_cell_array(TFlt unit_time, 
											  TFlt std_cell_length, 
											  TFlt lane_hold_cap_last_cell)
{
	Ctm_Cell *cell = NULL;
	for (int i = 0; i < m_num_cells - 1; ++i){
		cell = new Ctm_Cell(TFlt(m_number_of_lane) * std_cell_length * m_lane_hold_cap,
							TFlt(m_number_of_lane) * m_lane_flow_cap * unit_time,
							m_flow_scalar,
							m_wave_ratio);
		if (cell == NULL) {
			printf("Fail to init the cell.\n");
			exit(-1);
		}
		m_cell_array.push_back(cell);
	}

	// compress the cell_array to reduce space
	m_cell_array.shrink_to_fit();

	return 0;
}

void MNM_Dlink_Ctm_Multiclass::print_info()
{
	printf("Total number of cell: \t%d\t Standard wave ratio: \t%.4f\nLast cell wave ratio: \t%.4f\n",
			int(m_num_cells), double(m_wave_ratio), double(m_last_wave_ratio));
	printf("Volume for each cell is:\n");
	for (int i = 0; i < m_num_cells - 1; ++i){
		printf("%d, " int(m_cell_array[i] -> m_volume));
	}
	printf("%d\n", int(m_cell_array[m_num_cells - 1] -> m_volume));
}

MNM_Dlink_Ctm_Multiclass::Ctm_Cell_Multiclass::Ctem_Cell_Multiclass(TFlt hold_cap, 
																	TFlt flow_cap, 
																	TFlt flow_scalar, 
																	TFlt wave_ratio)
{
	m_hold_cap = hold_cap;
	m_flow_cap = flow_cap;
	m_flow_scalar = flow_scalar;
	m_wave_ratio = wave_ratio;
	m_volume = TInt(0);
	m_veh_queue = std::deque<MNM_Veh*>();
}

MNM_Dlink_Ctm_Multiclass::Ctm_Cell_Multiclass::~Ctm_Cell_Multiclass()
{
	m_veh_queue.clear();
}

TFlt MNM_Dlink_Ctm_Multiclass::Ctm_Cell_Multiclass::get_demand()
{
	TFlt _real_volume = TFlt(m_volume) / m_flow_scalar;
	return std::min(_real_volume, m_flow_cap);
}

TFlt MNM_Dlink_Ctm_Multiclass::Ctm_Cell_Multiclass::get_supply()
{
	TFlt _rel_volume = TFlt(m_volume) / m_flow_scalar;
}











