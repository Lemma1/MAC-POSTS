#include "dlink.h"
#include "limits.h"

MNM_Dlink::MNM_Dlink( TInt ID,
                      TInt number_of_lane,
                      TFlt length,
                      TFlt ffs )
{
  m_link_ID = ID;
  m_ffs = ffs;
  m_number_of_lane = number_of_lane;
  m_length = length;

  m_finished_array = std::deque<MNM_Veh *>();
  m_incoming_array = std::deque<MNM_Veh *>();
}

MNM_Dlink::~MNM_Dlink()
{
  m_finished_array.clear();
  m_incoming_array.clear();
}

int MNM_Dlink::hook_up_node(MNM_Dnode *from, MNM_Dnode *to)
{
  m_from_node = from;
  m_to_node = to;
  return 0;
}




int MNM_Dlink::move_veh_queue(std::deque<MNM_Veh*> *from_queue,
                                  std::deque<MNM_Veh*> *to_queue, 
                                  TInt number_tomove)
{
  MNM_Veh* __veh;
  for (int i=0; i<number_tomove; ++i) {
    __veh = from_queue -> front();
    from_queue -> pop_front();
    to_queue -> push_back(__veh);
  }
  return 0;
}

MNM_Dlink_Ctm::MNM_Dlink_Ctm( TInt ID,
                              TFlt lane_hold_cap, 
                              TFlt lane_flow_cap, 
                              TInt number_of_lane,
                              TFlt length,
                              TFlt ffs,
                              TFlt unit_time,
                              TFlt flow_scalar)
  : MNM_Dlink::MNM_Dlink ( ID, number_of_lane, length, ffs )
{
  m_lane_hold_cap = lane_hold_cap;
  m_lane_flow_cap = lane_flow_cap;
  m_flow_scalar = flow_scalar;
  m_cell_array = std::vector<Ctm_Cell*>();
  TFlt __std_cell_length = m_ffs * unit_time;
  m_num_cells = TInt(floor(m_length/__std_cell_length)); 
  if(m_num_cells == 0) {
    m_num_cells = 1;
  }
  TFlt __lane_hold_cap_last_cell = ((m_length - TFlt(m_num_cells - 1) * __std_cell_length) / __std_cell_length) * m_lane_hold_cap ;
  TFlt __wave_speed =  m_lane_flow_cap / (m_lane_hold_cap - m_lane_flow_cap / m_ffs); //laneFlwCap/ffs is the critical density.
  m_wave_ratio = __wave_speed / m_ffs; // note that h >= 2c/v, where h is holding capacity, c is capcity, v is free flow speed. i.e., wvRatio should < 1.
  m_last_wave_ratio = (m_lane_flow_cap / (__lane_hold_cap_last_cell - m_lane_flow_cap / m_ffs))/m_ffs;
  init_cell_array(unit_time, __std_cell_length, __lane_hold_cap_last_cell);
}

MNM_Dlink_Ctm::~MNM_Dlink_Ctm()
{
  for (Ctm_Cell* _cell : m_cell_array){
    delete _cell;
  }
  m_cell_array.clear();
}

int MNM_Dlink_Ctm::init_cell_array( TFlt unit_time, TFlt std_cell_length, TFlt lane_hold_cap_last_cell )
{

  Ctm_Cell *aCell = NULL;
  for(int i = 0; i<m_num_cells - 1; ++i) {
    aCell = new Ctm_Cell(TFlt(m_number_of_lane) * std_cell_length * m_lane_hold_cap, 
                         TFlt(m_number_of_lane) * m_lane_flow_cap * unit_time,
                         m_flow_scalar,
                         m_wave_ratio);
    if(aCell == NULL) {
      LOG(WARNING) << "Fail to init the cell.";
      exit(-1);
    };
    m_cell_array.push_back(aCell);
    aCell = NULL;
  }
  
  //since the last cell is a non-standard cell
  if(m_length > 0.0) {
    aCell = new Ctm_Cell(TFlt(m_number_of_lane) * std_cell_length * lane_hold_cap_last_cell, 
                         TFlt(m_number_of_lane) * m_lane_flow_cap * unit_time,
                         m_flow_scalar,
                         m_last_wave_ratio);
    if(aCell == NULL) {
      LOG(WARNING) << "Fail to init the cell.";
      exit(-1);
    }
    m_cell_array.push_back(aCell);
  }

  //compress the cellArray to reduce space 
  m_cell_array.shrink_to_fit();

  return 0;
}

void MNM_Dlink_Ctm::print_info() {
  printf("Total number of cell: \t%d\t Standard wave ratio: \t%.4f\nLast cell wave ratio: \t%.4f\n", 
          int(m_num_cells), double(m_wave_ratio), double(m_last_wave_ratio));
  printf("Volume for each cell is:\n");
  for (int i = 0; i < m_num_cells; ++i)
  {
    printf("%d, ", int(m_cell_array[i] -> m_volume));
  }
  printf("\n");
}

int MNM_Dlink_Ctm::update_out_veh()
{
  TFlt __temp_out_flux, __supply, __demand;
  if(m_num_cells > 1) // if only one cell, no update is needed
  {
    for (int i = 0; i < m_num_cells - 1; ++i)
    {
      __demand = m_cell_array[i]->get_demand();
      __supply = m_cell_array[i+1]->get_supply();
      __temp_out_flux = MNM_Ults::min(__demand, __supply) * m_flow_scalar;
      m_cell_array[i] -> m_out_veh= TInt(MNM_Ults::round(__temp_out_flux)); 
    }

    m_cell_array[m_num_cells - 1] -> m_out_veh = m_cell_array[m_num_cells - 1] -> m_veh_queue.size();
  }
  return 0;
}

int MNM_Dlink_Ctm::evolve(TInt timestamp)
{
  update_out_veh();
  TInt __num_veh_tomove;
  /* previous cells */
  if(m_num_cells > 1) {
    for (int i = 0; i < m_num_cells - 1; ++i) {
      __num_veh_tomove = m_cell_array[i] -> m_out_veh;
      move_veh_queue(&(m_cell_array[i] -> m_veh_queue),
                 &(m_cell_array[i+1] -> m_veh_queue),
                 __num_veh_tomove);
    }
  }
  /* last cell */
  move_last_cell();
  /* update volume */
  if(m_num_cells > 1) {
    for (int i = 0; i < m_num_cells - 1; ++i) {
      m_cell_array[i] -> m_volume = m_cell_array[i] -> m_veh_queue.size();
    }
  }
  m_cell_array[m_num_cells - 1] -> m_volume = m_cell_array[m_num_cells - 1] -> m_veh_queue.size()
                                                + m_finished_array.size();
  return 0;
}


TFlt MNM_Dlink_Ctm::get_link_supply() {
  return m_cell_array[0] -> get_supply();
}

int MNM_Dlink_Ctm::clear_incoming_array() {
  // printf("link ID: %d, in comming: %d, supply : %d\n", (int )m_link_ID,(int)m_incoming_array.size(), (int) (get_link_supply() * m_flow_scalar) );
  if (get_link_supply() * m_flow_scalar < m_incoming_array.size()) {
    LOG(WARNING) << "Wrong incoming array size";
    exit(-1);
  }
  move_veh_queue(&m_incoming_array, &(m_cell_array[0] -> m_veh_queue), m_incoming_array.size());

  m_cell_array[0] -> m_volume = m_cell_array[0] ->m_veh_queue.size();
  return 0;
}


int MNM_Dlink_Ctm::move_last_cell() {
  TInt __num_veh_tomove;
  __num_veh_tomove = m_cell_array[m_num_cells - 1] -> m_out_veh;
  MNM_Veh* __veh;
  std::map<TInt, std::deque<MNM_Veh*>*>::iterator __que_it;
  for (int i=0; i<__num_veh_tomove; ++i) {
    __veh = m_cell_array[m_num_cells - 1] -> m_veh_queue.front();
    m_cell_array[m_num_cells - 1] -> m_veh_queue.pop_front();
    if (__veh -> has_next_link()) {
      m_finished_array.push_back(__veh);
    }
    else {
      printf("Dlink_CTM::Some thing wrong!\n");
      exit(-1);
      // __veh -> get_destionation() -> receive_veh(__veh);
    }
  }
  return 0;
}

MNM_Dlink_Ctm::Ctm_Cell::Ctm_Cell(TFlt hold_cap, TFlt flow_cap, TFlt flow_scalar, TFlt wave_ratio)
{
  m_hold_cap = hold_cap;
  m_flow_cap = flow_cap;
  m_flow_scalar = flow_scalar;
  m_wave_ratio = wave_ratio;
  m_volume = TInt(0);
  m_veh_queue = std::deque<MNM_Veh*>();
}

MNM_Dlink_Ctm::Ctm_Cell::~Ctm_Cell()
{
  m_veh_queue.clear();
}


TFlt MNM_Dlink_Ctm::Ctm_Cell::get_demand()
{
  TFlt __real_volume = TFlt(m_volume) / m_flow_scalar;
  return std::min(__real_volume, m_flow_cap);
}

TFlt MNM_Dlink_Ctm::Ctm_Cell::get_supply()
{
  TFlt __real_volume = TFlt(m_volume) / m_flow_scalar;
  if (__real_volume >= m_hold_cap) 
  {
    __real_volume = m_hold_cap;
    return TFlt(0.0);
  }
  if(m_wave_ratio <= 1.0) //this one is quite tricky, why not just __min(flwCap, hldCap - curDensity)*wvRatio? 
    return m_flow_cap > __real_volume ? m_flow_cap: TFlt((m_hold_cap - __real_volume) * m_wave_ratio);  //flowCap equals to critical density
  else 
    return std::min(m_flow_cap, TFlt(m_hold_cap - __real_volume));
}


TFlt MNM_Dlink_Ctm::get_link_flow()
{
  TInt __total_volume = 0;
  for (int i = 0; i < m_num_cells; ++i){
    __total_volume += m_cell_array[i] -> m_volume;
  }
  return TFlt(__total_volume) / m_flow_scalar;
}


TFlt MNM_Dlink_Ctm::get_link_tt()
{
  TFlt __cost, __spd;
  TFlt __rho  = get_link_flow()/m_number_of_lane/m_length;// get the density in veh/mile
  TFlt __rhoj = m_lane_hold_cap; //get the jam density
  TFlt __rhok = m_lane_flow_cap/m_ffs; //get the critical density
  //  if (abs(rho - rhok) <= 0.0001) cost = POS_INF_INT;
  if (__rho >= __rhoj) {
    __cost = DBL_MAX; //sean: i think we should use rhoj, not rhok
  } 
  else {
    if (__rho <= __rhok) {
      __spd = m_ffs;
    }
    else {
      __spd = MNM_Ults::max(0.001*m_ffs, m_lane_flow_cap *(__rhoj - __rho)/((__rhoj - __rhok)*__rho));
    }
    __cost = m_length/__spd;
  } 
  return __cost;
}

/**************************************************************************
                          Poing Queue
**************************************************************************/
MNM_Dlink_Pq::MNM_Dlink_Pq(   TInt ID,
                              TFlt lane_hold_cap, 
                              TFlt lane_flow_cap, 
                              TInt number_of_lane,
                              TFlt length,
                              TFlt ffs,
                              TFlt unit_time,
                              TFlt flow_scalar)
  : MNM_Dlink::MNM_Dlink ( ID, number_of_lane, length, ffs )
{
  m_lane_hold_cap = lane_hold_cap;
  m_lane_flow_cap = lane_flow_cap;
  m_flow_scalar = flow_scalar;
  m_hold_cap = m_lane_hold_cap * TFlt(number_of_lane) * m_length;
  m_max_stamp = TInt(floor(m_length/(m_ffs * unit_time)));
  m_veh_queue = std::map<MNM_Veh*, TInt>();
  m_volume = TInt(0);
}


MNM_Dlink_Pq::~MNM_Dlink_Pq()
{
  m_veh_queue.clear();
}

TFlt MNM_Dlink_Pq::get_link_supply()
{
  return m_hold_cap;
}

int MNM_Dlink_Pq::clear_incoming_array() {
  TInt __num_veh_tomove = TInt(m_incoming_array.size()) < TInt(get_link_supply() * m_flow_scalar)
                      ? TInt(m_incoming_array.size()) : TInt(get_link_supply() * m_flow_scalar);
  MNM_Veh *__veh;
  for (int i=0; i < __num_veh_tomove; ++i) {
    __veh = m_incoming_array.front();
    m_incoming_array.pop_front();
    m_veh_queue.insert(std::pair<MNM_Veh*, TInt>(__veh, TInt(0)));
  }
  // move_veh_queue(&m_incoming_array, , m_incoming_array.size());

  // m_cell_array[0] -> m_volume = m_cell_array[0] ->m_veh_queue.size();
  return 0;
}

void MNM_Dlink_Pq::print_info()
{
  printf("Link Dynamic model: Poing Queue\n");
  printf("Real volume in the link: %.4f\n", (float)(m_volume/m_flow_scalar));
  printf("Finished real volume in the link: %.2f\n", (float)(TFlt(m_finished_array.size())/m_flow_scalar));
}

int MNM_Dlink_Pq::evolve(TInt timestamp)
{
  std::map<MNM_Veh*, TInt>::iterator __que_it = m_veh_queue.begin();
  while (__que_it != m_veh_queue.end()) {
    if (__que_it -> second >= m_max_stamp) {
      m_finished_array.push_back(__que_it -> first);
      __que_it = m_veh_queue.erase(__que_it); //c++ 11
    }
    else {
      __que_it -> second += 1;
      __que_it++;
    }
  }

  /* volume */
  m_volume = TInt(m_finished_array.size() + m_veh_queue.size());

  return 0;
}

TFlt MNM_Dlink_Pq::get_link_flow()
{
  return TFlt(m_volume) / m_flow_scalar;
}



TFlt MNM_Dlink_Pq::get_link_tt()
{
  TFlt __cost, __spd;
  TFlt __rho  = get_link_flow()/m_number_of_lane/m_length;// get the density in veh/mile
  TFlt __rhoj = m_lane_hold_cap; //get the jam density
  TFlt __rhok = m_lane_flow_cap/m_ffs; //get the critical density
  //  if (abs(rho - rhok) <= 0.0001) cost = POS_INF_INT;
  if (__rho >= __rhoj) {
    __cost = DBL_MAX; //sean: i think we should use rhoj, not rhok
  } 
  else {
    if (__rho <= __rhok) {
      __spd = m_ffs;
    }
    else {
      __spd = MNM_Ults::max(DBL_EPSILON * m_ffs, m_lane_flow_cap *(__rhoj - __rho)/((__rhoj - __rhok)*__rho));
    }
    __cost = m_length/__spd;
  } 
  return __cost;
}



