#include "limits.h"
#include "multiclass.h"

#include <algorithm>


/******************************************************************************************************************
*******************************************************************************************************************
												Link Models
*******************************************************************************************************************
******************************************************************************************************************/

/*************************************************************************					
						Multiclass CTM Functions
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
												   TFlt veh_convert_factor, // 1 * truck = c * private cars 
												   							// when compute node demand
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

	if ((ffs_car < 0) || (ffs_truck < 0)){
		printf("free-flow speed can't be less than zero, current link ID is %d\n", m_link_ID());
		exit(-1);
	}

	if (veh_convert_factor < 1){
		printf("veh_convert_factor can't be less than 1, current link ID is %d\n", m_link_ID());
		exit(-1);
	}

	if (flow_scalar < 1){
		printf("flow_scalar can't be less than 1, current link ID is %d\n", m_link_ID());
		exit(-1);
	}

	if (unit_time <= 0){
		printf("unit_time should be positive, current link ID is %d\n", m_link_ID());
		exit(-1);
	}

	m_lane_flow_cap_car = lane_flow_cap_car;
	m_lane_flow_cap_truck = lane_flow_cap_truck;	
	m_lane_hold_cap_car = lane_hold_cap_car;
	m_lane_hold_cap_truck = lane_hold_cap_truck;
	m_ffs_car = ffs_car;
	m_ffs_truck = ffs_truck;
	m_veh_convert_factor = veh_convert_factor;
	m_flow_scalar = flow_scalar;

	m_N_in_car = NULL;
  	m_N_out_car = NULL;
  	m_N_in_truck = NULL;
  	m_N_out_truck = NULL;

	m_cell_array = std::vector<Ctm_Cell_Multiclass*>();

	// Note m_ffs_car > m_ffs_truck, use ffs_car to define the standard cell length
	TFlt _std_cell_length = m_ffs_car * unit_time;
	m_num_cells = TInt(floor(m_length / _std_cell_length));
	if (m_num_cells == 0){
		m_num_cells = 1;
	}
	TFlt _last_cell_length = m_length - TFlt(m_num_cells - 1) * _std_cell_length;

	m_lane_critical_density_car = m_lane_flow_cap_car / m_ffs_car;
	m_lane_critical_density_truck = m_lane_flow_cap_truck / m_ffs_truck;

	if (m_lane_hold_cap_car <= m_lane_critical_density_car){
		printf("Wrong private car parameters, current link ID is %d\n", m_link_ID());
		exit(-1);
	}
	m_wave_speed_car = m_lane_flow_cap_car / (m_lane_hold_cap_car - m_lane_critical_density_car);

	if (m_lane_hold_cap_truck <= m_lane_critical_density_truck){
		printf("Wrong truck parameters, current link ID is %d\n", m_link_ID());
		exit(-1);
	}
	m_wave_speed_truck = m_lane_flow_cap_truck / (m_lane_hold_cap_truck - m_lane_critical_density_truck);

	// see the reference paper for definition
	// m_lane_rho_1_N > m_lane_critical_density_car and m_lane_critical_density_truck
	m_lane_rho_1_N = m_lane_hold_cap_car * (m_wave_speed_car / (m_ffs_truck + m_wave_speed_car));

	init_cell_array(unit_time, _std_cell_length, _last_cell_length);
}

MNM_Dlink_Ctm_Multiclass::~MNM_Dlink_Ctm_Multiclass()
{
	for (Ctm_Cell_Multiclass* _cell : m_cell_array){
		delete _cell;
	}
	m_cell_array.clear();

	if (m_N_out_car != NULL) delete m_N_out_car;
  	if (m_N_in_car != NULL) delete m_N_in_car;
  	if (m_N_out_truck != NULL) delete m_N_out_truck;
  	if (m_N_in_truck != NULL) delete m_N_in_truck;
}

// use this one instead of the one in Dlink class
int MNM_Dlink_Ctm_Multiclass::install_cumulative_curve_multiclass()
{
	m_N_in_car = new MNM_Cumulative_Curve();
  	m_N_out_car = new MNM_Cumulative_Curve();
  	m_N_in_truck = new MNM_Cumulative_Curve();
  	m_N_out_truck = new MNM_Cumulative_Curve();
  	m_N_in_car -> add_record(std::pair<TFlt, TFlt>(TFlt(0), TFlt(0)));
  	m_N_out_car -> add_record(std::pair<TFlt, TFlt>(TFlt(0), TFlt(0)));
  	m_N_in_truck -> add_record(std::pair<TFlt, TFlt>(TFlt(0), TFlt(0)));
  	m_N_out_truck -> add_record(std::pair<TFlt, TFlt>(TFlt(0), TFlt(0)));
  	return 0;
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
			m_cell_array[i] -> m_out_veh_car = MNM_Ults::round(_temp_out_flux_car * m_flow_scalar);

			// truck, veh_type = TInt(1)
			_demand_truck = m_cell_array[i] -> get_perceived_demand(TInt(1));
			_supply_truck = m_cell_array[i + 1] -> get_perceived_supply(TInt(1));
			_temp_out_flux_truck = m_cell_array[i] -> m_space_fraction_truck * MNM_Ults::min(_demand_truck, _supply_truck);
			m_cell_array[i] -> m_out_veh_truck = MNM_Ults::round(_temp_out_flux_truck * m_flow_scalar);
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
	// m_class: 0 - private car, 1 - truck
	for (_veh_it = m_finished_array.begin(); _veh_it != m_finished_array.end(); _veh_it++){
		MNM_Veh_Multiclass *_veh = dynamic_cast<MNM_Veh_Multiclass *>(*_veh_it);
		if (_veh -> m_class == 0) _count_car += 1;
		if (_veh -> m_class == 1) _count_truck += 1;
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
	TFlt _r;
	while ((_num_veh_tomove_car > 0) || (_num_veh_tomove_truck > 0)){
		_r = MNM_Ults::rand_flt();
		// probability = _pstar to move a car
		if (_r < _pstar){
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
	TFlt _real_volume_both = ( TFlt(m_cell_array[0] -> m_volume_truck) * m_veh_convert_factor + 
							   TFlt(m_cell_array[0] -> m_volume_car) ) / m_flow_scalar;

	// m_cell_length can't be 0 according to implementation above
	TFlt _density = _real_volume_both / (m_cell_array[0] -> m_cell_length);
	double _tmp = std::min(double(m_cell_array[0] -> m_flow_cap_car), m_wave_speed_car * (m_cell_array[0] -> m_hold_cap_car - _density));

	return std::max(0.0, _tmp) * (m_cell_array[0] -> m_unit_time);
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

	MNM_Veh_Multiclass* _veh;
	for (int i = 0; i < m_incoming_array.size(); ++i) {
		_veh = dynamic_cast<MNM_Veh_Multiclass *>(m_incoming_array.front());
		m_incoming_array.pop_front();
		if (_veh -> m_class == 0) {
			m_cell_array[0] -> m_veh_queue_car.push_back(_veh);
		}
		else {
			m_cell_array[0] -> m_veh_queue_truck.push_back(_veh);
		}
	}
	
	m_cell_array[0] -> m_volume_car = m_cell_array[0] -> m_veh_queue_car.size();
	m_cell_array[0] -> m_volume_truck = m_cell_array[0] -> m_veh_queue_truck.size();
	return 0;
}


TFlt MNM_Dlink_Ctm_Multiclass::get_link_flow()
{
	// For adaptive routing, need modidication for multiclass case

	// TInt _total_volume_car = 0;
	// TInt _total_volume_truck = 0;
	// for (int i = 0; i < m_num_cells; ++i){
	// 	_total_volume_car += m_cell_array[i] -> m_volume_car;
	// 	_total_volume_truck += m_cell_array[i] -> m_volume_truck;
	// }
	// return TFlt(_total_volume_car + _total_volume_truck) / m_flow_scalar;

	return 0;
}

TFlt MNM_Dlink_Ctm_Multiclass::get_link_tt()
{
	// For adaptive routing, need modidication for multiclass case

	// TFlt _cost, _spd;
	// // get the density in veh/mile
	// TFlt _rho = get_link_flow()/m_number_of_lane/m_length;
	// // get the jam density
	// TFlt _rhoj = m_lane_hold_cap;
	// // get the critical density
	// TFlt _rhok = m_lane_flow_cap/m_ffs;

	// if (_rho >= _rhoj){
	// 	_cost = MNM_Ults::max_link_cost();
	// }
	// else {
	// 	if (_rho <= _rhok){
	// 		_spd = m_ffs;
	// 	}
	// 	else {
	// 		_spd = MNM_Ults::max(0.001 * m_ffs, 
	// 				m_lane_flow_cap * (_rhoj - _rho) / (_rhoj - _rhok) / _rho);
	// 	}
	// 	_cost = m_length / _spd;
	// }
	// return _cost;

	return 0;
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
	m_unit_time = unit_time;
	m_flow_scalar = flow_scalar;

	m_hold_cap_car = hold_cap_car; // Veh/m
	m_hold_cap_truck = hold_cap_truck; // Veh/m
	m_critical_density_car = critical_density_car; // Veh/m
	m_critical_density_truck = critical_density_truck; // Veh/m
	m_rho_1_N = rho_1_N; // Veh/m
	m_flow_cap_car = flow_cap_car; // Veh/s
	m_flow_cap_truck = flow_cap_truck; // Veh/s
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

	TFlt _space_fraction_car, _space_fraction_truck;
	
	// Free-flow traffic (free-flow for both car and truck classes)
	if (_density_car/m_critical_density_car + _density_truck/m_critical_density_truck <= 1) {
		_space_fraction_car = _density_car/m_critical_density_car;
		_space_fraction_truck = _density_truck/m_critical_density_truck;
		m_perceived_density_car = _density_car + m_critical_density_car * _space_fraction_truck;
		m_perceived_density_truck = _density_truck + m_critical_density_truck * _space_fraction_car;
	}
	// Semi-congested traffic (truck free-flow but car not)
	else if ((_density_truck / m_critical_density_truck < 1) && 
			 (_density_car / (1 - _density_truck/m_critical_density_truck) <= m_rho_1_N)) {
		_space_fraction_truck = _density_truck/m_critical_density_truck;
		_space_fraction_car = 1 - _space_fraction_truck;
		m_perceived_density_car = _density_car / _space_fraction_car;
		m_perceived_density_truck = m_critical_density_truck;
	}
	// Fully congested traffic (both car and truck not free-flow)
	// this case should satisfy: 1. m_perceived_density_car > m_rho_1_N
	// 							 2. m_perceived_density_truck > m_critical_density_truck
	else {
		// _density_truck (m_volume_truck) could still be 0, but _density_car can't
		if (m_volume_truck == 0) {
			m_perceived_density_car = _density_car;
			// this case same speed (u) for both private cars and trucks
			TFlt _u = (m_hold_cap_car - _density_car) * m_wave_speed_car / _density_car;
			m_perceived_density_truck = (m_hold_cap_truck * m_wave_speed_truck) / (_u + m_wave_speed_truck);
		}
		else {
			TFlt _tmp_car = m_hold_cap_car * m_wave_speed_car * _density_truck;
			TFlt _tmp_truck = m_hold_cap_truck * m_wave_speed_truck * _density_car;
			_space_fraction_car = ( _density_car * _density_car * (m_wave_speed_car - m_wave_speed_truck) 
									 + _tmp_truck ) / ( _tmp_truck + _tmp_car );
			_space_fraction_truck = ( _density_car * _density_car * (m_wave_speed_truck - m_wave_speed_car)
									   + _tmp_car ) / ( _tmp_truck + _tmp_car );
			m_perceived_density_car = _density_car / _space_fraction_car;
			m_perceived_density_truck = _density_truck / _space_fraction_truck;
		}
	}
	return 0;
}

TFlt MNM_Dlink_Ctm_Multiclass::Ctm_Cell_Multiclass::get_perceived_demand(TInt veh_type)
{	
	// car
	if (veh_type == TInt(0)) {
		return std::min(m_flow_cap_car, TFlt(m_ffs_car * m_perceived_density_car)) * m_unit_time;
	}
	// truck
	else {
		return std::min(m_flow_cap_truck, TFlt(m_ffs_truck * m_perceived_density_truck)) * m_unit_time;
	}
}

TFlt MNM_Dlink_Ctm_Multiclass::Ctm_Cell_Multiclass::get_perceived_supply(TInt veh_type)
{
	TFlt _tmp;
	// car
	if (veh_type == TInt(0)) {
		_tmp = std::min(m_flow_cap_car, TFlt(m_wave_speed_car * (m_hold_cap_car - m_perceived_density_car)));
	}
	// truck
	else {
		_tmp = std::min(m_flow_cap_truck, TFlt(m_wave_speed_truck * (m_hold_cap_truck - m_perceived_density_truck)));
	}
	return std::max(TFlt(0.0), _tmp) * m_unit_time;
}




/******************************************************************************************************************
*******************************************************************************************************************
												Node Models
*******************************************************************************************************************
******************************************************************************************************************/

/**************************************************************************
                   In-out node
**************************************************************************/
MNM_Dnode_Inout_Multiclass::MNM_Dnode_Inout_Multiclass(TInt ID, TFlt flow_scalar)
	: MNM_Dnode::MNM_Dnode(ID, flow_scalar)
{
	m_demand = NULL;
	m_supply = NULL;
	m_veh_flow = NULL;
	m_veh_moved_car = NULL;
	m_veh_moved_truck = NULL;
}

MNM_Dnode_Inout_Multiclass::~MNM_Dnode_Inout_Multiclass()
{
  	if (m_demand != NULL) free(m_demand);
  	if (m_supply != NULL) free(m_supply);
  	if (m_veh_flow != NULL) free(m_veh_flow);
  	if (m_veh_moved_car != NULL) free(m_veh_moved_car);
  	if (m_veh_moved_truck != NULL) free(m_veh_moved_truck);
}

int MNM_Dnode_Inout_Multiclass::prepare_loading()
{
	TInt _num_in = m_in_link_array.size();
	TInt _num_out = m_out_link_array.size();
	m_demand = (TFlt*) malloc(sizeof(TFlt) * _num_in * _num_out); // real-world vehicles
	memset(m_demand, 0x0, sizeof(TFlt) * _num_in * _num_out);
	m_supply = (TFlt*) malloc(sizeof(TFlt) * _num_out); // real-world vehicles
	memset(m_supply, 0x0, sizeof(TFlt) * _num_out);
	m_veh_flow = (TFlt*) malloc(sizeof(TFlt) * _num_in * _num_out); // real-world vehicles
	memset(m_veh_flow, 0x0, sizeof(TFlt) * _num_in * _num_out);
	m_veh_moved_car = (TFlt*) malloc(sizeof(TFlt) * _num_in * _num_out); // simulation vehicles = real-world vehicles * flow scalar
	memset(m_veh_moved_car, 0x0, sizeof(TFlt) * _num_in * _num_out);
	m_veh_moved_truck = (TFlt*) malloc(sizeof(TFlt) * _num_in * _num_out); // simulation vehicles = real-world vehicles * flow scalar
	memset(m_veh_moved_truck, 0x0, sizeof(TFlt) * _num_in * _num_out);
	return 0;
}

int MNM_Dnode_Inout_Multiclass::prepare_supplyANDdemand()
{
	size_t _offset = m_out_link_array.size();
	TFlt _equiv_count;
	std::deque<MNM_Veh*>::iterator _veh_it;
	MNM_Dlink_Ctm_Multiclass *_in_link, *_out_link;

	/* calculate demand */
	for (size_t i = 0; i < m_in_link_array.size(); ++i){
		_in_link = dynamic_cast<MNM_Dlink_Ctm_Multiclass *>(m_in_link_array[i]);
		for (_veh_it = _in_link -> m_finished_array.begin(); _veh_it != _in_link -> m_finished_array.end(); _veh_it++){
			if (std::find(m_out_link_array.begin(), m_out_link_array.end(), (*_veh_it) -> get_next_link()) == m_out_link_array.end()){
				printf("Vehicle in the wrong node, no exit!\n");
        		printf("Vehicle is on link %d, node %d, next link ID is: %d\n", _in_link -> m_link_ID(), m_node_ID(), 
        			   (*_veh_it) -> get_next_link() -> m_link_ID());
        		exit(-1);
			}
		}
		for (size_t j = 0; j < m_out_link_array.size(); ++j){
			_out_link = dynamic_cast<MNM_Dlink_Ctm_Multiclass *>(m_out_link_array[j]);
			_equiv_count = 0;
			for (_veh_it = _in_link -> m_finished_array.begin(); _veh_it != _in_link -> m_finished_array.end(); _veh_it++){
        		MNM_Veh_Multiclass *_veh = dynamic_cast<MNM_Veh_Multiclass *>(*_veh_it);
        		if (_veh -> get_next_link() == _out_link) {
        			if (_veh -> m_class == 0) {
        				// private car
        				_equiv_count += 1;
        			}
        			else { 
        				// truck
        				_equiv_count += _in_link -> m_veh_convert_factor;
        			}
        		}
      		}
      		m_demand[_offset * i + j] = _equiv_count / m_flow_scalar;
		}
	}

	/* calculated supply */
  	for (size_t j = 0; j < m_out_link_array.size(); ++j){
    	_out_link = dynamic_cast<MNM_Dlink_Ctm_Multiclass *>(m_out_link_array[j]);
	    m_supply[j] = _out_link -> get_link_supply();
	    // printf(" get link s fin\n");
	    // printf("Link %d, supply is %.4f\n", _out_link -> m_link_ID, m_supply[j]);
  	}

  	return 0;
}

// int MNM_Dnode_Inout_Multiclass::flow_to_vehicle()
// {
// 	size_t _offset = m_out_link_array.size();
// 	MNM_Dlink *_out_link;
//  TFlt _to_move;
// 	size_t _rand_idx;
// 	for (size_t j = 0; j < m_out_link_array.size(); ++j){
// 		_to_move = 0;
// 		_out_link = m_out_link_array[j];
// 		for (size_t i = 0; i < m_in_link_array.size(); ++i){
// 			m_veh_tomove[i * _offset + j] = m_veh_flow[i * _offset + j] * m_flow_scalar;
//			_to_move += m_veh_tomove[i * _offset + j];
// 		}
// 	}
// 	return 0;
// }

int MNM_Dnode_Inout_Multiclass::record_cumulative_curve(TInt timestamp)
{
  	TInt _temp_sum_car, _temp_sum_truck;
  	MNM_Dlink_Ctm_Multiclass *_in_link, *_out_link;
  	size_t _offset = m_out_link_array.size();

  	for (size_t j = 0; j < m_out_link_array.size(); ++j){
    	_temp_sum_car = 0;
    	_temp_sum_truck = 0;
    	_out_link = dynamic_cast<MNM_Dlink_Ctm_Multiclass *>(m_out_link_array[j]);
    	for (size_t i = 0; i < m_in_link_array.size(); ++i) {
    		_in_link = dynamic_cast<MNM_Dlink_Ctm_Multiclass *>(m_in_link_array[i]);
       		_temp_sum_car += m_veh_moved_car[i * _offset + j];
      		_temp_sum_truck += m_veh_moved_truck[i * _offset + j];
    	}
    	if (_out_link -> m_N_out_car != NULL) {
      		_out_link -> m_N_in_car -> add_increment(std::pair<TFlt, TFlt>(TFlt(timestamp+1), TFlt(_temp_sum_car)/m_flow_scalar));
    	}
    	if (_out_link -> m_N_out_truck != NULL) {
      		_out_link -> m_N_in_truck -> add_increment(std::pair<TFlt, TFlt>(TFlt(timestamp+1), TFlt(_temp_sum_truck)/m_flow_scalar));
    	}
  	}

  	for (size_t i = 0; i < m_in_link_array.size(); ++i){
    	_temp_sum_car = 0;
    	_temp_sum_truck = 0;
    	_in_link = dynamic_cast<MNM_Dlink_Ctm_Multiclass *>(m_in_link_array[i]);
    	for (size_t j = 0; j < m_out_link_array.size(); ++j) {
      		_out_link = dynamic_cast<MNM_Dlink_Ctm_Multiclass *>(m_out_link_array[j]);
      		_temp_sum_car += m_veh_moved_car[i * _offset + j];
      		_temp_sum_truck += m_veh_moved_truck[i * _offset + j];
    	}
    	if (_in_link -> m_N_in_car != NULL) {
      		_in_link -> m_N_out_car -> add_increment(std::pair<TFlt, TFlt>(TFlt(timestamp+1), TFlt(_temp_sum_car)/m_flow_scalar));
    	}
    	if (_in_link -> m_N_in_truck != NULL) {
      		_in_link -> m_N_out_truck -> add_increment(std::pair<TFlt, TFlt>(TFlt(timestamp+1), TFlt(_temp_sum_truck)/m_flow_scalar));
    	}
  	}

  	return 0;
}

int MNM_Dnode_Inout_Multiclass::move_vehicle()
{
	MNM_Dlink_Ctm_Multiclass *_in_link, *_out_link;
	size_t _offset = m_out_link_array.size();
	TFlt _to_move;
	TFlt _equiv_num;
	TFlt _r;

	for (size_t j = 0; j < m_out_link_array.size(); ++j){
		_out_link = dynamic_cast<MNM_Dlink_Ctm_Multiclass *>(m_out_link_array[j]);
		for (size_t i = 0; i < m_in_link_array.size(); ++i){
			_in_link = dynamic_cast<MNM_Dlink_Ctm_Multiclass *>(m_in_link_array[i]);
			_to_move = m_veh_flow[i * _offset + j] * m_flow_scalar;
			auto _veh_it = _in_link -> m_finished_array.begin();
			while (_veh_it != _in_link -> m_finished_array.end()){
				if (_to_move > 0){
					MNM_Veh_Multiclass *_veh = dynamic_cast<MNM_Veh_Multiclass *>(*_veh_it);
					if (_veh -> get_next_link() == _out_link){
						if (_veh -> m_class == 0) {
	        				// private car
	        				_equiv_num = 1;
	        			}
	        			else { 
	        				// truck
	        				_equiv_num = _in_link -> m_veh_convert_factor;
	        			}
	        			if (_to_move < _equiv_num) {
	        				// randomly decide to move or not in this case 
	        				// base on the probability = _to_move/_equiv_num < 1
	        				_r = MNM_Ults::rand_flt();
	        				if (_r <= _to_move/_equiv_num){
	        					_out_link -> m_incoming_array.push_back(_veh);
								_veh -> set_current_link(_out_link);
								if (_veh -> m_class == 0){
									m_veh_moved_car[i * _offset + j] += 1;
								}
								else {
									m_veh_moved_truck[i * _offset + j] += 1;
								}
								_veh_it = _in_link -> m_finished_array.erase(_veh_it);
	        				}
	        			}
	        			else {
	        				_out_link -> m_incoming_array.push_back(_veh);
							_veh -> set_current_link(_out_link);
							if (_veh -> m_class == 0){
								m_veh_moved_car[i * _offset + j] += 1;
							}
							else {
								m_veh_moved_truck[i * _offset + j] += 1;
							}
							_veh_it = _in_link -> m_finished_array.erase(_veh_it);
	        			}
						_to_move -= _equiv_num;
					}
					else {
						_veh_it++;
					}
				}
				else {
					break;
				}
			}
			if (_to_move > 0){
		        printf("Something wrong during the vehicle moving, remaining to move %.4f\n", (float)_to_move);
		        // printf("The finished veh queue is now size %d\n", (int)_in_link->m_finished_array.size());
		        // printf("But it is heading to %d\n", (int)_in_link->m_finished_array.front() -> get_next_link() -> m_link_ID);
		        exit(-1);
		    }
		}
		random_shuffle(_out_link -> m_incoming_array.begin(), _out_link -> m_incoming_array.end());
	}

	return 0;
}

void MNM_Dnode_Inout_Multiclass::print_info()
{
	;
}

int MNM_Dnode_Inout_Multiclass::add_out_link(MNM_Dlink* out_link)
{
  	m_out_link_array.push_back(out_link);
  	return 0;
}

int MNM_Dnode_Inout_Multiclass::add_in_link(MNM_Dlink *in_link)
{
  	m_in_link_array.push_back(in_link);
  	return 0;
}

int MNM_Dnode_Inout_Multiclass::evolve(TInt timestamp)
{
	// printf("Inout node evolve\n");
	// printf("1\n");
	prepare_supplyANDdemand();
	// printf("2\n"); 
	compute_flow();
	// printf("3\n");
	// flow_to_vehicle();
	// printf("4\n");
	move_vehicle();
	// printf("5\n");
	record_cumulative_curve(timestamp);
	return 0;
}

/**************************************************************************
                              FWJ node
**************************************************************************/
MNM_Dnode_FWJ_Multiclass::MNM_Dnode_FWJ_Multiclass(TInt ID, TFlt flow_scalar)
  : MNM_Dnode_Inout_Multiclass::MNM_Dnode_Inout_Multiclass(ID, flow_scalar)
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
		for (size_t i = 0; i < m_in_link_array.size(); ++i){
	  		_sum_in_flow += m_demand[i * _offset + j];
		}
		for (size_t i = 0; i < m_in_link_array.size(); ++i){
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
  : MNM_Dnode_Inout_Multiclass::MNM_Dnode_Inout_Multiclass(ID, flow_scalar)
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
  MNM_Dnode_Inout_Multiclass::prepare_loading();
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
	return 0;
}




/******************************************************************************************************************
*******************************************************************************************************************
												Multiclass Vehicle
*******************************************************************************************************************
******************************************************************************************************************/
MNM_Veh_Multiclass::MNM_Veh_Multiclass(TInt ID, TInt vehicle_class, TInt start_time)
	: MNM_Veh::MNM_Veh(ID, start_time)
{
	m_class = vehicle_class;
}

MNM_Veh_Multiclass::~MNM_Veh_Multiclass()
{
	m_current_link = NULL;
  	m_next_link = NULL;
}



/******************************************************************************************************************
*******************************************************************************************************************
												Multiclass Factory
*******************************************************************************************************************
******************************************************************************************************************/

/**************************************************************************
                          Vehicle Factory
**************************************************************************/
MNM_Veh_Factory_Multiclass::MNM_Veh_Factory_Multiclass()
{
  m_veh_map = std::unordered_map<TInt, MNM_Veh_Multiclass*>();
  m_num_veh = TInt(0);
}

MNM_Veh_Factory_Multiclass::~MNM_Veh_Factory_Multiclass()
{
  MNM_Veh_Multiclass *_veh;
  for (auto _veh_it : m_veh_map){
    _veh = _veh_it.second;
    delete _veh;
  }
  m_veh_map.clear();
}

MNM_Veh_Multiclass* MNM_Veh_Factory_Multiclass::make_veh(TInt timestamp, 
														 Vehicle_type veh_type,
														 TInt vehicle_cls)
{
  // printf("A vehicle is produce at time %d, ID is %d\n", (int)timestamp, (int)m_num_veh + 1);
  MNM_Veh_Multiclass *_veh = new MNM_Veh_Multiclass(m_num_veh + 1, vehicle_cls, timestamp);
  _veh -> m_type = veh_type;
  m_veh_map.insert(std::pair<TInt, MNM_Veh_Multiclass*>(m_num_veh + 1, _veh));
  m_num_veh += 1;
  return _veh;
}

/**************************************************************************
                          Node factory
**************************************************************************/

MNM_Node_Factory_Multiclass::MNM_Node_Factory_Multiclass()
{
  m_node_map = std::unordered_map<TInt, MNM_Dnode*>();
}

MNM_Node_Factory_Multiclass::~MNM_Node_Factory_Multiclass()
{
  for (auto _map_it = m_node_map.begin(); _map_it!= m_node_map.end(); _map_it++){
    delete _map_it -> second;
  }
  m_node_map.clear();
}

MNM_Dnode *MNM_Node_Factory_Multiclass::make_node(TInt ID, 
												  DNode_type_multiclass node_type, 
												  TFlt flow_scalar)
{
  MNM_Dnode *_node;
  switch (node_type){
    case MNM_TYPE_FWJ_MULTICLASS:
      _node = new MNM_Dnode_FWJ_Multiclass(ID, flow_scalar);
      break;
    case MNM_TYPE_ORIGIN_MULTICLASS:
      _node = new MNM_DMOND_Multiclass(ID, flow_scalar);
      break;
    case MNM_TYPE_DEST_MULTICLASS:
      _node = new MNM_DMDND_Multiclass(ID, flow_scalar);
      break;
    default:
      printf("Wrong node type.\n");
      exit(-1);
  }
  m_node_map.insert({ID, _node});
  return _node;
}

MNM_Dnode *MNM_Node_Factory_Multiclass::get_node(TInt ID)
{
  auto _node = m_node_map[ID];
  if (_node == m_node_map.end()) {
    printf("No such node ID %d\n", (int) ID);
    throw std::runtime_error("Error, MNM_Node_Factory::get_node, node not exists");
  }
  return _node;
}

/**************************************************************************
                          Link factory
**************************************************************************/
MNM_Link_Factory_Multiclass::MNM_Link_Factory_Multiclass()
{
  m_link_map = std::unordered_map<TInt, MNM_Dlink*>();
}

MNM_Link_Factory_Multiclass::~MNM_Link_Factory_Multiclass()
{
  for (auto _map_it = m_link_map.begin(); _map_it!= m_link_map.end(); _map_it++){
    delete _map_it -> second;
  }  
  m_link_map.clear();
}

MNM_Dlink *MNM_Link_Factory_Multiclass::make_link(  TInt ID,
							                        DLink_type_multiclass link_type,
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
													TFlt flow_scalar)
{
  MNM_Dlink *_link;
  switch (link_type){
    case MNM_TYPE_CTM:
      _link = new MNM_Dlink_Ctm(ID, lane_hold_cap, lane_flow_cap, number_of_lane, length, ffs, unit_time, flow_scalar);
      break;
    case MNM_TYPE_PQ:
      _link = new MNM_Dlink_Pq(ID, lane_hold_cap, lane_flow_cap, number_of_lane, length, ffs, unit_time, flow_scalar);
      break;
    case MNM_TYPE_LQ:
      _link = new MNM_Dlink_Lq(ID, lane_hold_cap, lane_flow_cap, number_of_lane, length, ffs, unit_time, flow_scalar);
      break;
    case MNM_TYPE_LTM:
      _link = new MNM_Dlink_Ltm(ID, lane_hold_cap, lane_flow_cap, number_of_lane, length, ffs, unit_time, flow_scalar);
      break;
    default:
      printf("Wrong link type.\n");
      exit(-1);
  }
  m_link_map.insert(std::pair<TInt, MNM_Dlink*>(ID, _link));
  return _link;
}

MNM_Dlink *MNM_Link_Factory::get_link(TInt ID)
{
  auto _link_it = m_link_map.find(ID);
  if (_link_it == m_link_map.end()){
    throw std::runtime_error("Error, MNM_Link_Factory::get_link, link not exists");
  }
  return _link_it -> second;
}


int MNM_Link_Factory::delete_link(TInt ID)
{
  auto _map_it = m_link_map.find(ID);
  if (_map_it == m_link_map.end()){
    printf("Can't delete link!\n");
    exit(-1);
  }
  m_link_map.erase(_map_it);
  return 0;
}





/******************************************************************************************************************
*******************************************************************************************************************
												Multiclass DTA
*******************************************************************************************************************
******************************************************************************************************************/

MNM_Dta_Multiclass::MNM_Dta_Multiclass(std::string file_folder)
	: MNM_Dta::MNM::Dta(file_folder)
{
	;
}

MNM_Dta_Multiclass::~MNM_Dta_Multiclass()
{
  delete m_veh_factory;
  delete m_node_factory;
  delete m_link_factory;
  delete m_od_factory;
  delete m_config;
  delete m_routing;
  delete m_statistics;
  delete m_workzone;
  m_graph -> Clr();  
}