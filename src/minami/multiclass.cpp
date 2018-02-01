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
			(currently only for car & truck two classes)
	(see: Z. (Sean) Qian et al./Trans. Res. Part B 99 (2017) 183-204)			
**************************************************************************/

MNM_Dlink_Ctm_Multiclass::MNM_Dlink_Ctm_Multiclass(TInt ID,
												   TInt number_of_lane,
												   TFlt length, // (m)
												   TFlt lane_hold_cap_car, // Jam density (veh/m)
												   TFlt lane_hold_cap_truck,
												   TFlt lane_flow_cap_car, // Max flux (veh/s)
												   TFlt lane_flow_cap_truck,
												   TFlt ffs_car, // Free-flow speed (m/s)
												   TFlt ffs_truck, 
												   TFlt unit_time, // (s)
												   TFlt flow_scalar) // flow_scalar can be 2.0, 5.0, 10.0, etc.
	: MNM_Dlink::MNM_Dlink(ID, number_of_lane, length, 0.0) // Note: m_ffs is not used in child class, so let it be 0.0
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
	if ((lane_flow_cap_car < 0) || (lane_flow_cap_truck < 0)){
		printf("lane_flow_cap can't be less than zero, current link ID is %d\n", m_link_ID());
		exit(-1);
	}
	// Maximum flux for private cars cannot be too large
	if (lane_flow_cap_car > TFlt(3500) / TFlt(3600)){
		// lane_flow_cap is too large, set to 3500 veh/hour
		lane_flow_cap_car = TFlt(3500) / TFlt(3600);
	}
	// Maximum flux for trucks cannot be too large !!!NEED CHECK FOR THRESHOLD!!!
	if (lane_flow_cap_truck > TFlt(3500) / TFlt(3600)){
		// lane_flow_cap is too large, set to 3500 veh/hour
		lane_flow_cap_truck = TFlt(3500) / TFlt(3600);
	}

	m_lane_flow_cap_car = lane_flow_cap_car;
	m_lane_flow_cap_truck = lane_flow_cap_truck;	
	m_lane_hold_cap_car = lane_hold_cap_car;
	m_lane_hold_cap_truck = lane_hold_cap_truck;
	m_ffs_car = ffs_car;
	m_ffs_truck = ffs_truck;
	m_flow_scalar = flow_scalar;

	m_cell_array = std::vector<Ctm_Cell_Multiclass*>();

	// Note m_ffs_car > m_ffs_truck, so use ffs_car to define the standard cell length
	TFlt _std_cell_length = m_ffs_car * unit_time;
	m_num_cells = TInt(floor(m_length / _std_cell_length));
	if (m_num_cells == 0){
		m_num_cells = 1;
	}
	TFlt _last_cell_length = m_length - TFlt(m_num_cells - 1) * _std_cell_length;

	m_lane_critical_density_car = m_lane_flow_cap_car / m_ffs_car;
	m_lane_critical_density_truck = m_lane_flow_cap_truck / m_ffs_truck;
	m_wave_speed_car = m_lane_flow_cap_car / (m_lane_hold_cap_car - m_lane_critical_density_car);
	m_wave_speed_truck = m_lane_flow_cap_truck / (m_lane_hold_cap_truck - m_lane_critical_density_truck);

	// see the reference paper for definition
	m_lane_rho_1_N = m_lane_hold_cap_car * (m_wave_speed_car / (m_ffs_truck + m_wave_speed_car));

	init_cell_array(unit_time, _std_cell_length, _last_cell_length);
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
											  TFlt last_cell_length)
{
	// All previous cells
	Ctm_Cell_Multiclass *cell = NULL;
	for (int i = 0; i < m_num_cells - 1; ++i){
		cell = new Ctm_Cell_Multiclass(std_cell_length,
									   unit_time,
									   // Covert lane parameters to cell (link) parameters by multiplying # of lanes
									   TFlt(m_number_of_lane) * m_lane_hold_cap_car,
									   TFlt(m_number_of_lane) * m_lane_hold_cap_truck,
									   TFlt(m_number_of_lane) * m_lane_critical_density_car,
									   TFlt(m_number_of_lane) * m_lane_critical_density_truck,
									   TFlt(m_number_of_lane) * m_lane_rho_1_N,
									   TFlt(m_number_of_lane) * m_lane_flow_cap_car,
									   TFlt(m_number_of_lane) * m_lane_flow_cap_truck,
									   m_ffs_car,
									   m_ffs_truck,
									   m_wave_speed_car,
									   m_wave_speed_truck,
									   m_flow_scalar);
		if (cell == NULL) {
			printf("Fail to initialize some standard cell.\n");
			exit(-1);
		}
		m_cell_array.push_back(cell);
	}

	// The last cell
	// last cell must exist as long as link length > 0, see definition above
	if (m_length > 0.0) {
		cell = new Ctm_Cell_Multiclass(last_cell_length, // Note last cell length is longer but < 2X
									   unit_time,
									   TFlt(m_number_of_lane) * m_lane_hold_cap_car,
									   TFlt(m_number_of_lane) * m_lane_hold_cap_truck,
									   TFlt(m_number_of_lane) * m_lane_critical_density_car,
									   TFlt(m_number_of_lane) * m_lane_critical_density_truck,
									   TFlt(m_number_of_lane) * m_lane_rho_1_N,
									   TFlt(m_number_of_lane) * m_lane_flow_cap_car,
									   TFlt(m_number_of_lane) * m_lane_flow_cap_truck,
									   m_ffs_car,
									   m_ffs_truck,
									   m_wave_speed_car,
									   m_wave_speed_truck,
									   m_flow_scalar);
		if (cell == NULL) {
			printf("Fail to initialize the last cell.\n");
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
	printf("Total number of cell: \t%d\n Flow scalar: \t%.4f\n", int(m_num_cells), double(m_flow_scalar));

	printf("Car volume for each cell is:\n");
	for (int i = 0; i < m_num_cells - 1; ++i){
		printf("%d, ", int(m_cell_array[i] -> m_volume_car));
	}
	printf("%d\n", int(m_cell_array[m_num_cells - 1] -> m_volume_car));

	printf("Truck volume for each cell is:\n");
	for (int i = 0; i < m_num_cells - 1; ++i){
		printf("%d, ", int(m_cell_array[i] -> m_volume_truck));
	}
	printf("%d\n", int(m_cell_array[m_num_cells - 1] -> m_volume_truck));
}

int MNM_Dlink_Ctm_Multiclass::update_out_veh()
{
	TFlt _temp_out_flux_car, _supply_car, _demand_car;
	TFlt _temp_out_flux_truck, _supply_truck, _demand_truck;
	// no update is needed if only one cell
	if (m_num_cells > 1){
		for (int i = 0; i < m_num_cells - 1; ++i){
			// car, veh_type = TInt(0)
			_demand_car = m_cell_array[i] -> get_perceived_demand(TInt(0));
			_supply_car = m_cell_array[i + 1] -> get_perceived_supply(TInt(0));
			_temp_out_flux_car = m_cell_array[i] -> m_space_fraction_car * MNM_Ults::min(_demand_car, _supply_car);
			m_cell_array[i] -> m_out_veh_car = MNM_Ults::round(_temp_out_flux_car * m_unit_time * m_flow_scalar);

			// truck, veh_type = TInt(1)
			_demand_truck = m_cell_array[i] -> get_perceived_demand(TInt(1));
			_supply_truck = m_cell_array[i + 1] -> get_perceived_supply(TInt(1));
			_temp_out_flux_truck = m_cell_array[i] -> m_space_fraction_truck * MNM_Ults::min(_demand_truck, _supply_truck);
			m_cell_array[i] -> m_out_veh_truck = MNM_Ults::round(_temp_out_flux_truck * m_unit_time * m_flow_scalar);
		}
	}
	m_cell_array[m_num_cells - 1] -> m_out_veh_car = m_cell_array[m_num_cells - 1] -> m_veh_queue_car.size();
	m_cell_array[m_num_cells - 1] -> m_out_veh_truck = m_cell_array[m_num_cells - 1] -> m_veh_queue_truck.size();
	return 0;
}

int MNM_Dlink_Ctm_Multiclass::evolve(TInt timestamp)
{
	update_out_veh();
	
	TInt _num_veh_tomove_car, _num_veh_tomove_truck;
	/* previous cells */
	if (m_num_cells > 1){
		for (int i = 0; i < m_num_cells - 1; ++i)
		{
			// Car
			_num_veh_tomove_car = m_cell_array[i] -> m_out_veh_car;
			move_veh_queue( &(m_cell_array[i] -> m_veh_queue_car),
							&(m_cell_array[i+1] -> m_veh_queue_car),
							_num_veh_tomove_car);
			// Truck
			_num_veh_tomove_truck = m_cell_array[i] -> m_out_veh_truck;
			move_veh_queue( &(m_cell_array[i] -> m_veh_queue_truck),
							&(m_cell_array[i+1] -> m_veh_queue_truck),
							_num_veh_tomove_truck);
		}
	}
	/* last cell */
	move_last_cell();

	/* update volume */
	if (m_num_cells > 1){
		for (int i = 0; i < m_num_cells - 1; ++i)
		{
			m_cell_array[i] -> m_volume_car = m_cell_array[i] -> m_veh_queue_car.size();
			m_cell_array[i] -> m_volume_truck = m_cell_array[i] -> m_veh_queue_truck.size();
			// Update perceived density of the i-th cell
			m_cell_array[i] -> update_perceived_density();
		}
	}

	std::deque<MNM_Veh*>::iterator _veh_it;
	TInt _count_car = 0;
	TInt _count_truck = 0;
	// enum Vehicle_class {MNM_TYPE_CAR, MNM_TYPE_TRUCK};
	for (_veh_it = m_finished_array.begin(); _veh_it != m_finished_array.end(); _veh_it++){
		if ((*_veh_it) -> m_class == 0) _count_car += 1;
		if ((*_veh_it) -> m_class == 1) _count_truck += 1;
	}
	m_cell_array[m_num_cells - 1] -> m_volume_car = 
		m_cell_array[m_num_cells - 1] -> m_veh_queue_car.size() + _count_car;
	m_cell_array[m_num_cells - 1] -> m_volume_truck = 
		m_cell_array[m_num_cells - 1] -> m_veh_queue_truck.size() + _count_truck;	
	m_cell_array[m_num_cells - 1] -> update_perceived_density();

	return 0;
}


int MNM_Dlink_Ctm_Multiclass::move_last_cell() 
{
	TInt _num_veh_tomove_car = m_cell_array[m_num_cells - 1] -> m_out_veh_car;
	TInt _num_veh_tomove_truck = m_cell_array[m_num_cells - 1] -> m_out_veh_truck;
	TFlt _pstar = TFlt(_num_veh_tomove_car)/TFlt(_num_veh_tomove_car + _num_veh_tomove_truck);
	MNM_Veh* _veh;
	TFlt r;
	while (_num_veh_tomove_car > 0) || (_num_veh_tomove_truck > 0){
		r = TFlt((double) rand() / (RAND_MAX))
		// probability = _pstar to move a car
		if (r < _pstar){
			// still has car to move
			if (_num_veh_tomove_car > 0){
				_veh = m_cell_array[m_num_cells - 1] -> m_veh_queue_car.front();
				m_cell_array[m_num_cells - 1] -> m_veh_queue_car.pop_front();
				if (_veh -> has_next_link()){
					m_finished_array.push_back(_veh);
				}
				else {
					printf("Dlink_CTM_Multiclass::Some thing wrong!\n");
					exit(-1);
				}
			}
			// no car to move, move a truck
			else {
				_veh = m_cell_array[m_num_cells - 1] -> m_veh_queue_truck.front();
				m_cell_array[m_num_cells - 1] -> m_veh_queue_truck.pop_front();
				if (_veh -> has_next_link()){
					m_finished_array.push_back(_veh);
				}
				else {
					printf("Dlink_CTM_Multiclass::Some thing wrong!\n");
					exit(-1);
				}
			}
		}
		// probability = 1 - _pstar to move a truck
		else {
			// still has truck to move
			if (_num_veh_tomove_truck > 0){
				_veh = m_cell_array[m_num_cells - 1] -> m_veh_queue_truck.front();
				m_cell_array[m_num_cells - 1] -> m_veh_queue_truck.pop_front();
				if (_veh -> has_next_link()){
					m_finished_array.push_back(_veh);
				}
				else {
					printf("Dlink_CTM_Multiclass::Some thing wrong!\n");
					exit(-1);
				}
			}
			// no truck to move, move a car
			else {
				_veh = m_cell_array[m_num_cells - 1] -> m_veh_queue_car.front();
				m_cell_array[m_num_cells - 1] -> m_veh_queue_car.pop_front();
				if (_veh -> has_next_link()){
					m_finished_array.push_back(_veh);
				}
				else {
					printf("Dlink_CTM_Multiclass::Some thing wrong!\n");
					exit(-1);
				}
			}
		}
	}
	return 0;
}

TFlt MNM_Dlink_Ctm_Multiclass::get_link_supply()
{
	TFlt _real_volume_both = (TFlt(m_cell_array[0] -> m_volume_car) + 
							  TFlt(m_cell_array[0] -> m_volume_truck)) / m_flow_scalar;
	TFlt _density = _real_volume_both / m_cell_array[0] -> m_cell_length;
	TFlt _tmp = std::min(m_flow_cap_car, m_wave_speed_car * (m_hold_cap_car - _density));

	return std::max(0.0, _tmp);
}

int MNM_Dlink_Ctm_Multiclass::clear_incoming_array()
{
	// printf("link ID: %d, incoming: %d, supply: %d\n", 
	//        (int)m_link_ID, (int)m_incoming_array.size(), 
	//        (int)(get_link_supply() * m_flow_scalar));
	if (get_link_supply() * m_flow_scalar < m_incoming_array.size()){
		printf("Wrong incoming array size\n");
		exit(-1);
	}

	MNM_Veh* _veh;
	for (int i = 0; i < m_incoming_array.size(); ++i) {
		_veh = m_incoming_array -> front();
		m_incoming_array -> pop_front();
		if (_veh -> m_class == 0) {
			m_cell_array[0] -> m_veh_queue_car -> push_back(_veh);
		}
		else {
			m_cell_array[0] -> m_veh_queue_truck -> push_back(_veh);
		}
	}
	
	m_cell_array[0] -> m_volume_car = m_cell_array[0] -> m_veh_queue_car.size();
	m_cell_array[0] -> m_volume_truck = m_cell_array[0] -> m_veh_queue_truck.size();
	return 0;
}


TFlt MNM_Dlink_Ctm_Multiclass::get_link_flow()
{
	TInt _total_volume = 0;
	for (int i = 0; i < m_num_cells; ++i){
		_total_volume += m_cell_array[i] -> m_volume_car + m_cell_array[i] -> m_volume_truck;
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

MNM_Dlink_Ctm_Multiclass::Ctm_Cell_Multiclass::Ctm_Cell_Multiclass(TFlt cell_length,
														TFlt unit_time,
														TFlt hold_cap_car,
														TFlt hold_cap_truck,
														TFlt critical_density_car,
														TFlt critical_density_truck, 
														TFlt rho_1_N,
														TFlt flow_cap_car,
														TFlt flow_cap_truck,
														TFlt ffs_car,
														TFlt ffs_truck,
														TFlt wave_speed_car,
														TFlt wave_speed_truck,
														TFlt flow_scalar)
{
	m_cell_length = cell_length;
	m_unit_time = unit_time
	m_flow_scalar = flow_scalar;

	m_hold_cap_car = hold_cap_car;
	m_hold_cap_truck = hold_cap_truck;
	m_critical_density_car = critical_density_car;
	m_critical_density_truck = critical_density_truck;
	m_rho_1_N = rho_1_N;
	m_flow_cap_car = flow_cap_car;
	m_flow_cap_truck = flow_cap_truck;
	m_ffs_car = ffs_car;
	m_ffs_truck = ffs_truck;
	m_wave_speed_car = wave_speed_car;
	m_wave_speed_truck = wave_speed_truck;

	m_volume_car = TInt(0);
	m_volume_truck = TInt(0);
	m_out_veh_car = TInt(0);
	m_out_veh_truck = TInt(0);
	m_veh_queue_car = std::deque<MNM_Veh*>();
	m_veh_queue_truck = std::deque<MNM_Veh*>();
}

MNM_Dlink_Ctm_Multiclass::Ctm_Cell_Multiclass::~Ctm_Cell_Multiclass()
{
	m_veh_queue_car.clear();
	m_veh_queue_truck.clear();
}

int MNM_Dlink_Ctm_Multiclass::Ctm_Cell_Multiclass::update_perceived_density()
{
	TFlt _real_volume_car = TFlt(m_volume_car) / m_flow_scalar;
	TFlt _real_volume_truck = TFlt(m_volume_truck) / m_flow_scalar;

	TFlt _density_car = _real_volume_car / m_cell_length;
	TFlt _density_truck = _real_volume_truck / m_cell_length;
	
	// Free-flow traffic (free-flow for both car and truck classes)
	if (_density_car/m_critical_density_car + _density_truck/m_critical_density_truck <= 1) {
		m_space_fraction_car = _density_car/m_critical_density_car;
		m_space_fraction_truck = _density_truck/m_critical_density_truck;
		m_perceived_density_car = _density_car + m_critical_density_car * _space_fraction_truck;
		m_perceived_density_truck = _density_truck + m_critical_density_truck * _space_fraction_car;
	}
	// Semi-congested traffic (truck free-flow but car not)d
	else if (_density_car / (1 - _density_truck/m_critical_density_truck) <= m_rho_1_N) {
		m_space_fraction_truck = _density_truck/m_critical_density_truck;
		m_space_fraction_car = 1 - _space_fraction_truck;
		m_perceived_density_car = _density_car / _space_fraction_car;
		m_perceived_density_truck = _density_truck / _space_fraction_truck;
	}
	// Fully congested traffic (both car and truck not free-flow)
	// this case should satisfy: 1. m_perceived_density_car > m_rho_1_N
	// 							 2. m_perceived_density_truck > m_critical_density_truck
	else {
		TFlt _tmp_car = m_hold_cap_car * m_wave_speed_car / _density_car;
		TFlt _tmp_truck = m_hold_cap_truck * m_wave_speed_truck / _density_truck;
		m_space_fraction_car = (m_wave_speed_car - m_wave_speed_truck + _tmp_truck) / (_tmp_car + _tmp_truck);
		m_space_fraction_truck = (m_wave_speed_truck - m_wave_speed_car + _tmp_car) / (_tmp_car + _tmp_truck);
		m_perceived_density_car = _density_car / _space_fraction_car;
		m_perceived_density_truck = _density_truck / _space_fraction_truck;
	}
	return 0;
}

TFlt MNM_Dlink_Ctm_Multiclass::Ctm_Cell_Multiclass::get_perceived_demand(TInt veh_type)
{	
	// car
	if (veh_type == TInt(0)) {
		return std::min(m_flow_cap_car, m_ffs_car * m_perceived_density_car);
	}
	// truck
	else {
		return std::min(m_flow_cap_truck, m_ffs_truck * m_perceived_density_truck);
	}
}

TFlt MNM_Dlink_Ctm_Multiclass::Ctm_Cell_Multiclass::get_perceived_supply(TInt veh_type)
{
	// car
	if (veh_type == TInt(0)) {
		TFlt _tmp = std::min(m_flow_cap_car, m_wave_speed_car * (m_hold_cap_car - m_perceived_density_car));
	}
	// truck
	else {
		TFlt _tmp = std::min(m_flow_cap_truck, m_wave_speed_truck * (m_hold_cap_truck - m_perceived_density_truck));
	}
	return std::max(0.0, _tmp);
}



/******************************************************************************************************************
*******************************************************************************************************************
												Node Models
*******************************************************************************************************************
******************************************************************************************************************/

MNM_Dnode_Inout_Multiclass::MNM_Dnode_Inout_Multiclass(TInt ID, TFlt flow_scalar)
	: MNM_Dnode::MNM_Dnode(ID, flow_scalar)
{
	m_demand = NULL;
	m_supply = NULL;
	m_veh_queue = NULL;
	m_veh_tomove = NULL;
}

MNM_Dnode_Inout_Multiclass::~MNM_Dnode_Inout_Multiclass()
{
  	if (m_demand != NULL) free(m_demand);
  	if (m_supply != NULL) free(m_supply);
  	if (m_veh_flow != NULL) free(m_veh_flow);
  	if (m_veh_tomove != NULL) free(m_veh_tomove);
}

int MNM_Dnode_Inout_Multiclass::prepare_loading()
{
	TInt _num_in = m_in_link_array.size();
	TInt _num_out = m_out_link_array.size();
	m_demand = (TFlt*) malloc(sizeof(TFlt) * _num_in * _num_out); // real-world vehicles
	memset(m_demand, 0x0, sizeof(TFlt) * _num_in * _num_out);
	m_supply = (TFlt*) malloc(sizeof(TFlt) * _num_out); // real-world vehicles
	memset(m_supply, 0x0, sizeof(TFlt) * _num_out);
	m_veh_flow (TFlt*) malloc(sizeof(TFlt) * _num_in * _num_out); // real-world vehicles
	memset(m_veh_flow, 0x0, sizeof(TFlt) * _num_in * _num_out);
	m_veh_tomove (TFlt*) malloc(sizeof(TFlt) * _num_in * _num_out); // simulation vehicles
																	// = real-world vehicles * flow scalar
	memset(m_veh_tomove, 0x0, sizeof(TFlt) * _num_in * _num_out);
	return 0;
}

int MNM_Dnode_Inout_Multiclass::prepare_supplyANDdemand()
{
	size_t _offset = m_out_link_array.size();
	TInt _count;
	std::deque<MNM_Veh*>::iterator _veh_it;
	MNM_Dlink *_in_link, *_out_link;

	/* calculate demand */
	for (size_t i = 0; i < m_in_link_array.size(); ++i){
		_in_link = m_in_link_array[i];
		for (_veh_it = _in_link -> m_finished_array.begin(); _veh_it != _in_link -> m_finished_array.end(); _veh_it++){
			if (std::find(m_out_link_array.begin(), m_out_link_array.end(), (*_veh_it) -> get_next_link()) == m_out_link_array.end()){
				printf("Vehicle in the wrong node, no exit!\n");
        		printf("Vehicle is on link %d, node %d, next link ID is: %d\n", _in_link -> m_link_ID(), m_node_ID(), (*_veh_it) -> get_next_link() -> m_link_ID());
        		exit(-1);
			}
		}
		for (size_t j = 0; j < m_out_link_array.size(); ++j){
			_out_link = m_out_link_array[j];
			_count = 0;
			for (_veh_it = _in_link -> m_finished_array.begin(); _veh_it != _in_link -> m_finished_array.end(); _veh_it++){
        		if ((*_veh_it) -> get_next_link() == _out_link) _count += 1;
      		}
      		m_demand[_offset * i + j] = TFlt(_count) / m_flow_scalar;
		}
	}

	/* calculated supply */
  	for (size_t j = 0; j< m_out_link_array.size(); ++j){
    	_out_link = m_out_link_array[j];
	    // printf("Get link s\n");
	    // printf("The out link is %d\n", _out_link -> m_link_ID);
	    m_supply[j] = _out_link -> get_link_supply();
	    // printf(" get link s fin\n");
	    // printf("Link %d, supply is %.4f\n", _out_link -> m_link_ID, m_supply[j]);
  	}

  	return 0;
}

int MNM_Dnode_Inout_Multiclass::round_flow_to_vehicle()
{
	size_t _offset = m_out_link_array.size();
	MNM_Dlink *_out_link;
	TInt _to_move;
	size_t _rand_idx;
	for (size_t j = 0; j < m_out_link_array.size(); ++j){
		_to_move = 0;
		_out_link = m_out_link_array[j];
		for (size_t i = 0; i < m_in_link_array.size(); ++i){
			m_veh_tomove[i * _offset + j] = MNM_Ults::round(m_veh_flow[i * _offset + j] * m_flow_scalar);
			_to_move += m_veh_tomove[i * _offset + j]
		}
		while (TFlt(_to_move) > (_out_link -> get_link_supply() * m_flow_scalar)){
			_rand_idx = rand() % m_in_link_array.size();
			if (m_veh_tomove[_rand_idx * _offset + j] >= 1){
				m_veh_tomove[_rand_idx * _offset + j] -= 1;
				_to_move -= 1;
			}
		}
	}

	return 0;
}

int MNM_Dnode_Inout_Multiclass::record_cumulative_curve(TInt timestamp)
{
  	TInt _num_to_move;
  	TInt _temp_sum;
  	MNM_Dlink *_in_link, *_out_link;
  	size_t _offset = m_out_link_array.size();

  	for (size_t j  0; j < m_out_link_array.size(); ++j){
    	_temp_sum = 0;
    	_out_link = m_out_link_array[j];
    	for (size_t i = 0; i < m_in_link_array.size(); ++i) {
      		_in_link = m_in_link_array[i];
      		_num_to_move = m_veh_tomove[i * _offset + j];
      		_temp_sum += _num_to_move;
    	}
    	if (_out_link -> m_N_out != NULL) {
      		_out_link -> m_N_in -> add_increment(std::pair<TFlt, TFlt>(TFlt(timestamp+1), TFlt(_temp_sum)/m_flow_scalar));
    	}
  	}

  	for (size_t i = 0; i < m_in_link_array.size(); ++i){
    	_temp_sum = 0;
    	_in_link = m_in_link_array[i];
    	for (size_t j = 0; j < m_out_link_array.size(); ++j) {
      		_out_link = m_out_link_array[j];
      		_num_to_move = m_veh_tomove[i * _offset + j];
      		_temp_sum += _num_to_move;
    	}
    	if (_in_link -> m_N_in != NULL) {
      		_in_link -> m_N_out -> add_increment(std::pair<TFlt, TFlt>(TFlt(timestamp+1), TFlt(_temp_sum)/m_flow_scalar));
    	}
  	}

  	return 0;
}

int MNM_Dnode_Inout_Multiclass::move_vehicle()
{
	MNM_Dlink *_in_link, *_out_link;
	MNM_Veh *_veh;
	size_t _offset = m_out_link_array.size();
	TInt _num_to_move;

	for (size_t j = 0; j < m_out_link_array.size(); ++j){
		_out_link = m_out_link_array[j];
		for (size_t i = 0; i < m_in_link_array.size(); ++i){
			_in_link = m_in_link_array[i];
			_num_to_move = m_veh_tomove[i * _offset + j];
			auto _veh_it = _in_link -> m_finished_array.begin();
			while (_veh_it != _in_link -> m_finished_array.end()){
				if (_num_to_move > 0){
					_veh = *_veh_it;
					if (_veh -> get_next_link() == _out_link){
						_out_link -> m_incoming_array.push_back(_veh);
						_veh -> set_current_link(_out_link);
						_veh_it = _in_link -> m_finished_array.erase(_veh_it);
						_num_to_move -= 1;
					}
					else {
						_veh_it++;
					}
				}
				else {
					break;
				}
			}
			if (_num_to_move != 0){
		        printf("Something wrong during the vehicle moving, remaining to move %d\n", (int)_num_to_move);
		        printf("The finished veh queue is now size %d\n", (int)_in_link->m_finished_array.size());
		        printf("But it is heading to %d\n", (int)_in_link->m_finished_array.front() -> get_next_link() -> m_link_ID);
		        exit(-1);
		    }
		}
		random_shuffle(_out_link -> m_incoming_array.begin(), _out_link -> m_incoming_array.end());
	}

	return 0;
}

void MNM_Dnode_Inout::print_info()
{
	;
}

int MNM_Dnode_Inout::add_out_link(MNM_Dlink* out_link)
{
  	m_out_link_array.push_back(out_link);
  	return 0;
}

int MNM_Dnode_Inout::add_in_link(MNM_Dlink *in_link)
{
  	m_in_link_array.push_back(in_link);
  	return 0;
}

int MNM_Dnode_Inout::evolve(TInt timestamp)
{
	// printf("Inout node evolve\n");
	// printf("1\n");
	prepare_supplyANDdemand();
	// printf("2\n"); 
	compute_flow();
	// printf("3\n");
	round_flow_to_vehicle();
	// printf("4\n");
	record_cumulative_curve(timestamp);
	// // printf("4.1\n");
	move_vehicle();
	// printf("5\n");
	return 0;
}

/**************************************************************************
                              FWJ node
**************************************************************************/

MNM_Dnode_FWJ_Multiclass::MNM_Dnode_FWJ_Multiclass(TInt ID, TFlt flow_scalar)
  : MNM_Dnode_Inout::MNM_Dnode_Inout(ID, flow_scalar)
{
}

MNM_Dnode_FWJ_Multiclass::~MNM_Dnode_FWJ_Multiclass()
{

}

void MNM_Dnode_FWJ_Multiclass::print_info()
{
 	;
}

int MNM_Dnode_FWJ_Multiclass::compute_flow()
{
	size_t _offset = m_out_link_array.size();
	TFlt _sum_in_flow, _portion;
	for (size_t j = 0; j < m_out_link_array.size(); ++j){
		_sum_in_flow = TFlt(0);
		for (size_t i=0; i< m_in_link_array.size(); ++i){
	  		_sum_in_flow += m_demand[i * _offset + j];
		}
		for (size_t i=0; i< m_in_link_array.size(); ++i){
	  		_portion = MNM_Ults::divide(m_demand[i * _offset + j], _sum_in_flow);
		  	m_veh_flow[i * _offset + j] = MNM_Ults::min(m_demand[i * _offset + j], _portion * m_supply[j]);
		}
	}

	return 0;
}


/**************************************************************************
                   General Road Junction node
**************************************************************************/
MNM_Dnode_GRJ_Multiclass::MNM_Dnode_GRJ_Multiclass(TInt ID, TFlt flow_scalar)
  : MNM_Dnode_Inout::MNM_Dnode_Inout(ID, flow_scalar)
{
  m_d_a = NULL;
  m_C_a = NULL;
}

MNM_Dnode_GRJ_Multiclass::~MNM_Dnode_GRJ_Multiclass()
{
  if (m_d_a != NULL) free(m_d_a);
  if (m_C_a != NULL) free(m_C_a);
}

int MNM_Dnode_GRJ_Multiclass::prepare_loading()
{
  MNM_Dnode_Inout::prepare_loading();
  TInt _num_in = m_in_link_array.size();
  m_d_a = (TFlt*) malloc(sizeof(TFlt) * _num_in);
  memset(m_d_a, 0x0, sizeof(TFlt) * _num_in);
  m_C_a = (TFlt*) malloc(sizeof(TFlt) * _num_in);
  memset(m_C_a, 0x0, sizeof(TFlt) * _num_in);
  return 0;
}

void MNM_Dnode_GRJ_Multiclass::print_info()
{
  ;
}

int MNM_Dnode_GRJ_Multiclass::compute_flow()
{
	
}