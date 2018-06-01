#include "od.h"

MNM_Origin::MNM_Origin(TInt ID, TInt max_interval, TFlt flow_scalar, TInt frequency)
{
  m_Origin_ID = ID;
  m_max_assign_interval = max_interval;
  m_flow_scalar = flow_scalar;
  m_current_assign_interval = 0;
  if (frequency <= 0){
    printf("MNM_Origin::MNM_Origin::Wrong frequency.\n");
    exit(-1);
  }
  m_frequency = frequency;
  m_demand = std::unordered_map<MNM_Destination*, TFlt*>();
}

MNM_Origin::~MNM_Origin()
{
  for (auto _demand_it = m_demand.begin(); _demand_it != m_demand.end(); _demand_it++) {
    free(_demand_it -> second);
  }
  m_demand.clear();
}

int MNM_Origin::add_dest_demand(MNM_Destination *dest, TFlt* demand)
{
  TFlt* _demand = (TFlt*) malloc(sizeof(TFlt) * m_max_assign_interval);
  for (int i = 0; i < m_max_assign_interval; ++i) {
    _demand[i] =  TFlt(demand[i]);
  }
  m_demand.insert(std::pair<MNM_Destination*, TFlt*>(dest, _demand));
  return 0;
}

int MNM_Origin::release(MNM_Veh_Factory* veh_factory, TInt current_interval)
{
  if (m_current_assign_interval < m_max_assign_interval && current_interval % m_frequency == 0){
    TInt _veh_to_release;
    MNM_Veh *_veh;
    for (auto _demand_it = m_demand.begin(); _demand_it != m_demand.end(); _demand_it++) {
      _veh_to_release = TInt(MNM_Ults::round((_demand_it -> second)[m_current_assign_interval] * m_flow_scalar));
      for (int i=0; i<_veh_to_release; ++i) {
        _veh = veh_factory -> make_veh(current_interval, MNM_TYPE_ADAPTIVE);
        _veh -> set_destination(_demand_it -> first);
        _veh -> set_origin(this);
        m_origin_node -> m_in_veh_queue.push_back(_veh);
      }
    }
    m_current_assign_interval ++;
  }
  return 0;
}

int MNM_Origin::release_one_interval(TInt current_interval, MNM_Veh_Factory* veh_factory, TInt assign_interval, TFlt adaptive_ratio)
{
  if (assign_interval < 0) return 0;
  TInt _veh_to_release;
  MNM_Veh *_veh;
  for (auto _demand_it = m_demand.begin(); _demand_it != m_demand.end(); _demand_it++) {
    _veh_to_release = TInt(MNM_Ults::round((_demand_it -> second)[assign_interval] * m_flow_scalar)) ;
    for (int i = 0; i < _veh_to_release; ++i) {
      if (adaptive_ratio == TFlt(0)){
        _veh = veh_factory -> make_veh(current_interval, MNM_TYPE_STATIC);
      }
      else if (adaptive_ratio == TFlt(1)){
        _veh = veh_factory -> make_veh(current_interval, MNM_TYPE_ADAPTIVE);
      }
      else{
        TFlt _r = MNM_Ults::rand_flt();
        if (_r <= adaptive_ratio){
          _veh = veh_factory -> make_veh(current_interval, MNM_TYPE_ADAPTIVE);
        }
        else{
          _veh = veh_factory -> make_veh(current_interval, MNM_TYPE_STATIC);
        }
      }
      _veh -> set_destination(_demand_it -> first);
      _veh -> set_origin(this);
      _veh -> m_assign_interval = assign_interval;
      m_origin_node -> m_in_veh_queue.push_back(_veh);
    }
  }
  return 0;
}

MNM_Destination::MNM_Destination(TInt ID)
{
  m_Dest_ID = ID;
}


MNM_Destination::~MNM_Destination()
{
}

int MNM_Destination::receive(TInt current_interval)
{

  MNM_Veh *_veh;
  size_t _num_to_receive = m_dest_node -> m_out_veh_queue.size();
  // printf("Dest node %d out vehicle: %d\n", m_dest_node -> m_node_ID, _num_to_receive);
  for (size_t i=0; i < _num_to_receive; ++i){
    _veh = m_dest_node -> m_out_veh_queue.front();
    if (_veh -> get_destination() != this){
      printf("The veh is heading to %d, but we are %d\n", (int)_veh -> get_destination() -> m_dest_node -> m_node_ID, (int)m_dest_node -> m_node_ID);
      printf("MNM_Destination::receive: Something wrong!\n");
      exit(-1);
    }
    _veh -> finish(current_interval);
    // printf("Receive Vehicle ID: %d, origin node is %d, destination node is %d\n", _veh -> m_veh_ID(), _veh -> get_origin() -> m_origin_node -> m_node_ID(), _veh -> get_destination() -> m_dest_node -> m_node_ID());
    m_dest_node -> m_out_veh_queue.pop_front();
  }
  
  return 0;
}


namespace MNM{

TFlt get_demand_bynode(TInt O_node, TInt D_node, TInt assign_inter, MNM_Node_Factory *node_factory)
{
  if (MNM_DMOND* _origin_node = dynamic_cast<MNM_DMOND*>(node_factory->get_node(O_node))){
    MNM_Origin* _origin = _origin_node -> m_origin;
    if (MNM_DMDND* _dest_node = dynamic_cast<MNM_DMDND*>(node_factory->get_node(D_node))){
      MNM_Destination* _dest = _dest_node -> m_dest;
      if (_origin -> m_demand.find(_dest) == _origin -> m_demand.end()){
        return TFlt(0);
      }
      return _origin -> m_demand.find(_dest) -> second[assign_inter];
    }
  }
  printf("get_demand_bynode::Can't find the origin or destination.\n");
  exit(-1);
}

}

