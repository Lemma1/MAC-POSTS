#include "dlink.h"
#include "limits.h"
#include "multiclass.h"

#include <algorithm>


/******************************************************************************************************************
*******************************************************************************************************************
												Link Models
*******************************************************************************************************************
******************************************************************************************************************/


/*						Multiclass CTM Functions
**************************************************************************/

MNM_Dlink_Ctm_Multiclass::MNM_Dlink_Ctm_Multiclass(TInt ID,
												   TInt number_of_lane,
												   TFlt length,
												   TFlt lane_hold_cap_car,
												   TFlt lane_hold_cap_truck,
												   TFlt lane_flow_cap_car,
												   TFlt lane_flow_cap_truck,
												   TFlt ffs_car,
												   TFlt ffs_truck,
												   TFlt unit_time,
												   TFlt flow_scalar)
	: MNM_Dlink::MNM_Dlink(ID, number_of_lane, length) // Note: m_ffs is not used in child class
{
	// Jam density for private cars and trucks cannot be negative
	if ((lane_hold_cap_car < 0) || (lane_hold_cap_truck < 0)){
		printf("lane_hold_cap can't be negative, current link ID is %d\n", m_link_ID());
		exit(-1);
	}
	// Jam density for private cars cannot be too large
	if (lane_hold_cap_car > TFlt(300) / TFlt(1600)){
		// "lane_hold_cap is too large, set to 300 veh/mile
		lane_hold_cap_car = TFlt(300) / TFlt(1600);
	}
	// Jam density for trucks cannot be too large !!!NEED CHECK FOR THRESHOLD!!!
	if (lane_hold_cap_truck > TFlt(300) / TFlt(1600)){
		// "lane_hold_cap is too large, set to 300 veh/mile
		lane_hold_cap_truck = TFlt(300) / TFlt(1600);
	}

	// Maximum flux for private cars and trucks cannot be negative
	if (lane_flow_cap < 0){
		printf("lane_flow_cap can't be less than zero, current link ID is %d\n", m_link_ID());
		exit(-1);
	}
	// Maximum flux for private cars cannot be too large
	if (lane_flow_cap_car > TFlt(3500) / TFlt(3600)){
		// lane_flow_cap is too large, set to 3500 veh/hour
		lane_flow_cap_car = TFlt(3500) / TFlt(3600);
	}
	// Maximum flux for trucks cannot be too large !!!NEED CHECK FOR THRESHOLD!!!
	if (lane_flow_cap_car > TFlt(3500) / TFlt(3600)){
		// lane_flow_cap is too large, set to 3500 veh/hour
		lane_flow_cap_car = TFlt(3500) / TFlt(3600);
	}

	m_lane_flow_cap_car = lane_flow_cap_car;
	m_lane_flow_cap_truck = lane_flow_cap_truck;	
	m_lane_hold_cap_car = lane_hold_cap_car;
	m_lane_hold_cap_truck = lane_hold_cap_truck;
	m_ffs_car = ffs_car;
	m_ffs_truck = ffs_truck;
	m_flow_scalar = flow_scalar;

	m_cell_array = std::vector<Ctm_Cell_Multiclass*>();

	// Note m_ffs_car > m_ffs_truck, so use ffs_car to define the cell length
	TFlt _std_cell_length = m_ffs_car * unit_time;
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
		printf("Wave ratio can't be less than 0, current link ID is %d\n", m_link_ID());
		exit(-1);
	}

	m_last_wave_ratio = (m_lane_flow_cap / (_lane_hold_cap_last_cell - m_lane_flow_cap / m_ffs))/m_ffs;
	if (m_last_wave_ratio < 0){
		printf("Last cell wave ratio can't be less than 0, current link ID is %d\n", m_link_ID());
		exit(-1);
	}
	init_cell_array(unit_time, _std_cell_length, _lane_hold_cap_last_cell);
}

MNM_Dlink_Ctm_Multiclass::~MNM_Dlink_Ctm_Multiclass()
{
	for (Ctm_Cell_Multiclass* _cell : m_cell_array){
		delete _cell;
	}
	m_cell_array.clear();
}

int MNM_Dlink_Ctm_Multiclass::init_cell_array(TFlt unit_time, 
											  TFlt std_cell_length, 
											  TFlt lane_hold_cap_last_cell)
{
	// previous cells
	Ctm_Cell_Multiclass *cell = NULL;
	for (int i = 0; i < m_num_cells - 1; ++i){
		cell = new Ctm_Cell_Multiclass(TFlt(m_number_of_lane) * std_cell_length * m_lane_hold_cap,
							TFlt(m_number_of_lane) * m_lane_flow_cap * unit_time,
							m_flow_scalar,
							m_wave_ratio);
		if (cell == NULL) {
			printf("Fail to init the cell.\n");
			exit(-1);
		}
		m_cell_array.push_back(cell);
	}

	// last cell
	if (m_length > 0.0) {
		cell = new Ctm_Cell_Multiclass(TFlt(m_number_of_lane) * std_cell_length * lane_hold_cap_last_cell,
									   TFlt(m_number_of_lane) * m_lane_flow_cap * unit_time,
									   m_flow_scalar,
									   m_last_wave_ratio);
		if (cell == NULL) {
			printf("Fail to init the last cell.\n");
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
		printf("%d, ", int(m_cell_array[i] -> m_volume));
	}
	printf("%d\n", int(m_cell_array[m_num_cells - 1] -> m_volume));
}

int MNM_Dlink_Ctm_Multiclass::update_out_veh()
{
	TFlt _temp_out_flux, _supply, _demand;
	// no update is needed if only one cell
	if (m_num_cells > 1){
		for (int i = 0; i < m_num_cells - 1; ++i){
			_demand = m_cell_array[i] -> get_demand();
			_supply = m_cell_array[i + 1] -> get_supply();
			_temp_out_flux = MNM_Ults::min(_demand, _supply) * m_flow_scalar;
			m_cell_array[i] -> m_out_veh = MNM_Ults::round(_temp_out_flux);
		}
	}
	m_cell_array[m_num_cells - 1] -> m_out_veh = m_cell_array[m_num_cells - 1] -> m_veh_queue.size();
	return 0;
}

int MNM_Dlink_Ctm_Multiclass::evolve(TInt timestamp)
{
	update_out_veh();
	TInt _num_veh_tomove;
	/* previous cells */
	if (m_num_cells > 1){
		for (int i = 0; i < m_num_cells - 1; ++i)
		{
			_num_veh_tomove = m_cell_array[i] -> m_out_veh;
			move_veh_queue( &(m_cell_array[i] -> m_veh_queue),
							&(m_cell_array[i+1] -> m_veh_queue),
							_num_veh_tomove);
		}
	}
	/* last cell */
	move_last_cell();

	/* update volume */
	if (m_num_cells > 1){
		for (int i = 0; i < m_num_cells - 1; ++i)
		{
			m_cell_array[i] -> m_volume = m_cell_array[i] -> m_veh_queue.size();
		}
	}
	m_cell_array[m_num_cells - 1] -> m_volume = 
		m_cell_array[m_num_cells - 1] -> m_veh_queue.size() + m_finished_array.size();

	return 0;
}


int MNM_Dlink_Ctm_Multiclass::move_last_cell() 
{
	TInt _num_veh_tomove;
	_num_veh_tomove = m_cell_array[m_num_cells - 1] -> m_out_veh;
	MNM_Veh* _veh;
	// std::map<TInt, std::deque<MNM_Veh*>*>::iterator _que_it;
	for (int i = 0; i < _num_veh_tomove; ++i)
	{
		_veh = m_cell_array[m_num_cells - 1] -> m_veh_queue.front();
		m_cell_array[m_num_cells - 1] -> m_veh_queue.pop_front();
		if (_veh -> has_next_link()){
			m_finished_array.push_back(_veh);
		}
		else {
			printf("Dlink_CTM_Multiclass::Some thing wrong!\n");
			exit(-1);
		}
	}
	return 0;
}

TFlt MNM_Dlink_Ctm_Multiclass::get_link_supply()
{
	return m_cell_array[0] -> get_supply();
}

int MNM_Dlink_Ctm_Multiclass::clear_incoming_array()
{
	// printf("link ID: %d, incoming: %d, supply: %d\n", 
	//        (int)m_link_ID, (int)m_incoming_array.size(), 
	//        (int)(get_link_supply() * m_flow_scalar));
	if (get_link_supply() * m_flow_scalar < m_incoming_array.size()){
		// LOG(WARNING) << "Wrong incoming array size";
		exit(-1);
	}
	move_veh_queue(&m_incoming_array, &(m_cell_array[0] -> m_veh_queue), m_incoming_array.size());
	
	m_cell_array[0] -> m_volume = m_cell_array[0] -> m_veh_queue.size();
	return 0;	
}


TFlt MNM_Dlink_Ctm_Multiclass::get_link_flow()
{
	TInt _total_volume = 0;
	for (int i = 0; i < m_num_cells; ++i){
		_total_volume += m_cell_array[i] -> m_volume;
	}
	return TFlt(_total_volume) / m_flow_scalar;
}

TFlt MNM_Dlink_Ctm_Multiclass::get_link_tt()
{
	TFlt _cost, _spd;
	// get the density in veh/mile
	TFlt _rho = get_link_flow()/m_number_of_lane/m_length;
	// get the jam density
	TFlt _rhoj = m_lane_hold_cap;
	// get the critical density
	TFlt _rhok = m_lane_flow_cap/m_ffs;

	if (_rho >= _rhoj){
		_cost = MNM_Ults::max_link_cost();
	}
	else {
		if (_rho <= _rhok){
			_spd = m_ffs;
		}
		else {
			_spd = MNM_Ults::max(0.001 * m_ffs, 
					m_lane_flow_cap * (_rhoj - _rho) / (_rhoj - _rhok) / _rho);
		}
		_cost = m_length / _spd;
	}
	return _cost;
}


/*							Multiclass CTM Cells
**************************************************************************/

MNM_Dlink_Ctm_Multiclass::Ctm_Cell_Multiclass::Ctm_Cell_Multiclass(TFlt hold_cap, 
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
	TFlt _real_volume = TFlt(m_volume) / m_flow_scalar;
	if (_real_volume >= m_hold_cap)
	{
		return TFlt(0.0);
	}
	if (m_wave_ratio <= 1.0)
		return m_flow_cap > _real_volume ? m_flow_cap: TFlt((m_hold_cap - _real_volume) * m_wave_ratio);
	else
		return std::min(m_flow_cap, TFlt(m_hold_cap - _real_volume));
}



/******************************************************************************************************************
*******************************************************************************************************************
												Node Models
*******************************************************************************************************************
******************************************************************************************************************/




