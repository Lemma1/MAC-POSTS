#include "dnode.h"

#include <algorithm>

MNM_Dnode::MNM_Dnode(TInt ID, TFlt flow_scalar)
{
  m_node_ID = ID;
  m_flow_scalar = flow_scalar;
  m_out_link_array = std::vector<MNM_Dlink*>();
  m_in_link_array = std::vector<MNM_Dlink*>();
}

MNM_Dnode::~MNM_Dnode()
{
  m_out_link_array.clear();
  m_in_link_array.clear();
}

/**************************************************************************
                          Origin node
**************************************************************************/

MNM_DMOND::MNM_DMOND(TInt ID, TFlt flow_scalar)
  : MNM_Dnode::MNM_Dnode(ID, flow_scalar)
{
  m_origin = NULL;
  m_out_volume = std::unordered_map<MNM_Dlink*, TInt>();
  m_in_veh_queue = std::deque<MNM_Veh *>();
}

MNM_DMOND::~MNM_DMOND()
{
  m_in_veh_queue.clear();
  m_out_volume.clear();
}


int MNM_DMOND::evolve(TInt timestamp)
{
  // printf("MNM_DMOND node evolve\n");
  MNM_Dlink *_link, *_to_link;

  for (unsigned i=0; i<m_out_link_array.size(); ++i){
    _link = m_out_link_array[i];
    m_out_volume.find(_link) -> second = 0;
  }  

  /* compute our flow */
  std::deque<MNM_Veh*>::iterator _que_it = m_in_veh_queue.begin();
  while (_que_it != m_in_veh_queue.end()) {
    _link = (*_que_it) -> get_next_link();
    m_out_volume.find(_link) -> second += 1;
    _que_it++;
  }

  for (unsigned i=0; i<m_out_link_array.size(); ++i){
    _link = m_out_link_array[i];
    if ((_link -> get_link_supply() * m_flow_scalar) < TFlt(m_out_volume.find(_link) -> second)){
      m_out_volume.find(_link) -> second = TInt(MNM_Ults::round(_link -> get_link_supply() * m_flow_scalar));
    }
  }
  /* move vehicle */
  MNM_Veh *_veh;
  for (unsigned i=0; i<m_out_link_array.size(); ++i){
    _link = m_out_link_array[i];
    if (_link -> m_N_in != NULL) {
      _link -> m_N_in -> add_increment(std::pair<TFlt, TFlt>(TFlt(timestamp + 1), TFlt(m_out_volume.find(_link) -> second)/m_flow_scalar));
    }
    if (m_in_veh_queue.size() != 0){
        // printf("In node %d, %d veh to move to link %d, total veh %d \n", m_node_ID, m_out_volume.find(_link) -> second, _link -> m_link_ID, m_in_veh_queue.size());  
    }
    _que_it = m_in_veh_queue.begin();
    while (_que_it != m_in_veh_queue.end()) {
      if (m_out_volume.find(_link) -> second > 0){
        _veh = *_que_it;
        _to_link = _veh -> get_next_link();
        if (_to_link == _link){
           _to_link -> m_incoming_array.push_back(_veh);
          (_veh) -> set_current_link(_to_link);
          _que_it = m_in_veh_queue.erase(_que_it); //c++ 11
          m_out_volume.find(_to_link) -> second -= 1;
        }
        else{
          _que_it++;
        }
      }
      else{
        break; //break whill loop
      }

    }
  }

  // for (unsigned i=0; i<m_out_link_array.size(); ++i){
  //   _link = m_out_link_array[i];
  //   if (m_out_volume.find(_link) -> second != 0){
  //     printf("Something wrong in moving vehicles on DMOND\n");
  //     printf("Remaining out volume in link %d is %d\n", (int)_link -> m_link_ID, (int)m_out_volume.find(_link) -> second);
  //     exit(-1);
  //   }
  // }  
  // printf("Finish MNM_DMOND evolve\n");
  return 0;
}


int MNM_DMOND::add_out_link(MNM_Dlink* out_link)
{
  m_out_link_array.push_back(out_link);
  m_out_volume.insert(std::pair<MNM_Dlink*, TInt>(out_link, TInt(0)));
  return 0;
}

void MNM_DMOND::print_info()
{
  ;
}

int MNM_DMOND::hook_up_origin(MNM_Origin *origin)
{
  m_origin = origin;
  return 0;
}

/**************************************************************************
                          Destination node
**************************************************************************/

MNM_DMDND::MNM_DMDND(TInt ID, TFlt flow_scalar)
  : MNM_Dnode::MNM_Dnode(ID, flow_scalar)
{
  m_dest = NULL;
}

MNM_DMDND::~MNM_DMDND()
{
  m_out_veh_queue.clear();
}

int MNM_DMDND::add_in_link(MNM_Dlink *link)
{
  m_in_link_array.push_back(link);
  return 0;
}

int MNM_DMDND::evolve(TInt timestamp)
{
  // printf("MNM_DMDND node evomlve\n");
  MNM_Dlink *_link;
  MNM_Veh *_veh;
  size_t _size;
  // printf("in link size:%d\n", m_in_link_array.size());
  for (size_t i = 0; i<m_in_link_array.size(); ++i){
    _link = m_in_link_array[i];
    _size = _link->m_finished_array.size();
    if (_link -> m_N_out != NULL) {
      _link -> m_N_out -> add_increment(std::pair<TFlt, TFlt>(TFlt(timestamp + 1), TFlt(_size)/m_flow_scalar));
    }
    for (size_t j=0; j<_size; ++j){
      _veh = _link->m_finished_array.front();
      if (_veh -> get_next_link() != NULL){
        printf("Something wrong in DMDND evolve\n");
        exit(-1);
      }
      m_out_veh_queue.push_back(_veh);
      _veh -> set_current_link(NULL);
      _link -> m_finished_array.pop_front();
    }
  }
  return 0;
}

void MNM_DMDND::print_info()
{
  ;
}

int MNM_DMDND::hook_up_destination(MNM_Destination *dest)
{
  m_dest = dest;
  return 0;
}


/**************************************************************************
                              In-out node
**************************************************************************/
MNM_Dnode_Inout::MNM_Dnode_Inout(TInt ID, TFlt flow_scalar)
  : MNM_Dnode::MNM_Dnode(ID, flow_scalar)
{
  m_demand = NULL;
  m_supply = NULL;
  m_veh_flow = NULL;
  m_veh_tomove = NULL;
}

MNM_Dnode_Inout::~MNM_Dnode_Inout()
{
  if (m_demand != NULL) free(m_demand);
  if (m_supply != NULL) free(m_supply);
  if (m_veh_flow != NULL) free(m_veh_flow);
  if (m_veh_tomove != NULL) free(m_veh_tomove);
}

int MNM_Dnode_Inout::prepare_loading()
{
  TInt _num_in = m_in_link_array.size();
  TInt _num_out = m_out_link_array.size();
  // printf("num_in: %d, num_out: %d\n", _num_in, _num_out);
  m_demand = (TFlt*) malloc(sizeof(TFlt) * _num_in * _num_out);
  memset(m_demand, 0x0, sizeof(TFlt) * _num_in * _num_out);
  m_supply = (TFlt*) malloc(sizeof(TFlt) * _num_out);
  memset(m_supply, 0x0, sizeof(TFlt) * _num_out);
  m_veh_flow = (TFlt*) malloc(sizeof(TFlt) * _num_in * _num_out);
  memset(m_veh_flow, 0x0, sizeof(TFlt) * _num_in * _num_out);
  m_veh_tomove = (TInt*) malloc(sizeof(TInt) * _num_in * _num_out);
  memset(m_veh_tomove, 0x0, sizeof(TInt) * _num_in * _num_out);
  return 0;
}

int MNM_Dnode_Inout::prepare_supplyANDdemand()
{
  // printf("MNM_Dnode_Inout::prepare_supplyANDdemand\n");
   /* calculate demand */
  size_t _offset = m_out_link_array.size();
  TInt _count;
  std::deque <MNM_Veh*>::iterator _veh_it;
  MNM_Dlink *_in_link, *_out_link;


  for (size_t i=0; i < m_in_link_array.size(); ++i){
    _in_link = m_in_link_array[i];
    // for (_veh_it = _in_link -> m_finished_array.begin(); _veh_it != _in_link -> m_finished_array.end(); _veh_it++){
    //   if (std::find(m_out_link_array.begin(), m_out_link_array.end(), (*_veh_it) -> get_next_link()) == m_out_link_array.end()) {
    //     printf("Vehicle in the wrong node, no exit!\n");
    //     exit(-1);
    //   }
    // }
    for (size_t j=0; j< m_out_link_array.size(); ++j){
      _out_link = m_out_link_array[j];
      // printf("Current out link is %d\n", _out_link -> m_link_ID);
      _count = 0;
      
      for (_veh_it = _in_link -> m_finished_array.begin(); _veh_it != _in_link -> m_finished_array.end(); _veh_it++){
        if ((*_veh_it) -> get_next_link() == _out_link) _count += 1;
      }
      m_demand[_offset*i + j] = TFlt(_count) / m_flow_scalar;
    }
  }
  // printf("Finished\n");
  /* calculated supply */
  for (size_t i=0; i< m_out_link_array.size(); ++i){
    _out_link = m_out_link_array[i];
    // printf("Get link s\n");
    // printf("The out link is %d\n", _out_link -> m_link_ID);
    m_supply[i] = _out_link -> get_link_supply();
    // printf(" get link s fin\n");
    // printf("Link %d, supply is %.4f\n", _out_link -> m_link_ID, m_supply[i]);
  } 

  return 0;
}

int MNM_Dnode_Inout::round_flow_to_vehicle()
{
  // printf("MNM_Dnode_Inout::round_flow_to_vehicle\n");
  // the rounding mechanism may cause the lack of vehicle in m_finished_array
  // but demand is always a integer and only supply can be float, so don't need to worry about it
  size_t _offset = m_out_link_array.size();
  MNM_Dlink *_out_link;
  TInt _to_move;
  size_t _rand_idx;
  for (size_t j=0; j< m_out_link_array.size(); ++j){
    _to_move = 0;
    _out_link = m_out_link_array[j];
    for (size_t i=0; i< m_in_link_array.size(); ++i){
      m_veh_tomove[i * _offset + j] = MNM_Ults::round(m_veh_flow[i * _offset + j] * m_flow_scalar);
      _to_move += m_veh_tomove[i * _offset + j];
      // printf("Rounding %d, %d the value %f to %d\n", i, j, m_veh_flow[i * _offset + j] * m_flow_scalar, m_veh_tomove[i * _offset + j]);
    }
    // printf("Going to loop %d vs supply %f\n", _to_move, _out_link -> get_link_supply());
    while (TFlt(_to_move) > (_out_link -> get_link_supply() * m_flow_scalar)){
      _rand_idx = rand() % m_in_link_array.size();
      if (m_veh_tomove[_rand_idx * _offset + j] >= 1){
        m_veh_tomove[_rand_idx * _offset + j] -= 1;
        _to_move -= 1;
      }
    }
    // printf("Rounding %d, %d the value %f to %d\n", i, j, m_veh_flow[i * _offset + j] * m_flow_scalar, m_veh_tomove[i * _offset + j]);
  }
  return 0;
}


int MNM_Dnode_Inout::record_cumulative_curve(TInt timestamp)
{
  TInt _num_to_move;
  TInt _temp_sum;
  MNM_Dlink *_in_link, *_out_link;
  size_t _offset = m_out_link_array.size();

  for (size_t j=0; j<m_out_link_array.size(); ++j){
    _temp_sum = 0;
    _out_link = m_out_link_array[j];
    for (size_t i=0; i<m_in_link_array.size(); ++i) {
      _in_link = m_in_link_array[i];
      _num_to_move = m_veh_tomove[i * _offset + j];
      _temp_sum += _num_to_move;
    }
    if (_out_link -> m_N_out != NULL) {
      _out_link -> m_N_in -> add_increment(std::pair<TFlt, TFlt>(TFlt(timestamp+1), TFlt(_temp_sum)/m_flow_scalar));
    }
  }

  for (size_t i=0; i<m_in_link_array.size(); ++i){
    _temp_sum = 0;
    _in_link = m_in_link_array[i];
    for (size_t j=0; j<m_out_link_array.size(); ++j) {
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


int MNM_Dnode_Inout::move_vehicle()
{
  // printf("MNM_Dnode_Inout::move_vehicle\n");
  MNM_Dlink *_in_link, *_out_link;
  MNM_Veh *_veh;
  size_t _offset = m_out_link_array.size();
  TInt _num_to_move;

  for (size_t j=0; j<m_out_link_array.size(); ++j){
    _out_link = m_out_link_array[j];
    for (size_t i=0; i<m_in_link_array.size(); ++i) {
      _in_link = m_in_link_array[i];
      _num_to_move = m_veh_tomove[i * _offset + j];
      // printf("In node %d, from link %d to link %d, %d to move\n", m_node_ID, _in_link ->m_link_ID, _out_link->m_link_ID, _num_to_move);
      // for (size_t k=0; k<_size; ++k){
      //   if (_num_to_move > 0){
      //     _veh = _in_link->m_finished_array[k];
      //     if (_veh -> get_next_link() == _out_link){
      //       _out_link ->m_incoming_array.push_back(_veh);
      //       _veh -> set_current_link(_out_link);
      //       // _in_link -> m_finished_array.pop_front();

      //       _num_to_move -= 1;            
      //     }
      //   }
      //   else{
      //     break; // break the inner most structure
      //   }
      // }
      auto _veh_it = _in_link->m_finished_array.begin();
      while (_veh_it != _in_link->m_finished_array.end()) {
        if (_num_to_move > 0) {
          _veh = *_veh_it;
          if (_veh -> get_next_link() == _out_link){
            _out_link ->m_incoming_array.push_back(_veh);
            _veh -> set_current_link(_out_link);
            _veh_it = _in_link->m_finished_array.erase(_veh_it); //c++ 11
            _num_to_move -= 1; 
          }
          else {
            _veh_it++;
          }
        }
        else{
          break; // break the inner most structure
        }
      }
      if (_num_to_move != 0){
        printf("Something wrong during the vehicle moving, remaining to move %d\n", (int)_num_to_move);
        printf("The finished veh queue is now %d size\n", (int)_in_link->m_finished_array.size());
        printf("But it is heading to %d\n", (int)_in_link->m_finished_array.front() -> get_next_link() -> m_link_ID);
        exit(-1);
      }
    }
    // make the queue randomly perturbed, may not be true in signal controlled intersection
    // TODO 
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
  move_vehicle();
  // printf("5\n");
  return 0;
}
/**************************************************************************
                              FWJ node
**************************************************************************/

MNM_Dnode_FWJ::MNM_Dnode_FWJ(TInt ID, TFlt flow_scalar)
  : MNM_Dnode_Inout::MNM_Dnode_Inout(ID, flow_scalar)
{
}

MNM_Dnode_FWJ::~MNM_Dnode_FWJ()
{

}

void MNM_Dnode_FWJ::print_info()
{
  ;
}

int MNM_Dnode_FWJ::compute_flow()
{
  // printf("MNM_Dnode_FWJ::compute_flow\n");
  size_t _offset = m_out_link_array.size();
  TFlt _sum_in_flow, _portion;
  for (size_t j=0; j< m_out_link_array.size(); ++j){
    _sum_in_flow = TFlt(0);
    for (size_t i=0; i< m_in_link_array.size(); ++i){
      _sum_in_flow += m_demand[i * _offset + j];
    }
    for (size_t i=0; i< m_in_link_array.size(); ++i){
      _portion = MNM_Ults::divide(m_demand[i * _offset + j], _sum_in_flow);
      // printf("Portion is %.4f, sum in flow is %.4f, demand is %.4f\n", _portion, _sum_in_flow, m_demand[i * _offset + j]);
      m_veh_flow[i * _offset + j] = MNM_Ults::min(m_demand[i * _offset + j], _portion * m_supply[j]);
      // printf("to link %d the flow is %.4f\n", m_out_link_array[j] -> m_link_ID, m_veh_flow[i * _offset + j]);
    }
  }
  return 0;
}
