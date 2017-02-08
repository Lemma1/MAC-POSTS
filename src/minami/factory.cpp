#include "factory.h"

/**************************************************************************
                          Vehicle node
**************************************************************************/
MNM_Veh_Factory::MNM_Veh_Factory()
{
  m_veh_map = std::unordered_map<TInt, MNM_Veh*>();
  m_num_veh = TInt(0);
}

MNM_Veh_Factory::~MNM_Veh_Factory()
{
  MNM_Veh *_veh;
  for (auto _veh_it : m_veh_map){
    _veh = _veh_it.second;
    delete _veh;
  }
  m_veh_map.clear();
}

MNM_Veh *MNM_Veh_Factory::make_veh(TInt timestamp, Vehicle_type veh_type)
{
  // printf("A vehicle is produce at time %d, ID is %d\n", (int)timestamp, (int)m_num_veh + 1);
  MNM_Veh *_veh = new MNM_Veh(m_num_veh + 1, timestamp);
  _veh -> m_type = veh_type;
  m_veh_map.insert(std::pair<TInt, MNM_Veh*>(m_num_veh + 1, _veh));
  m_num_veh += 1;
  return _veh;
}


/**************************************************************************
                          Node factory
**************************************************************************/

MNM_Node_Factory::MNM_Node_Factory()
{
  m_node_map = std::unordered_map<TInt, MNM_Dnode*>();
}

MNM_Node_Factory::~MNM_Node_Factory()
{
  for (auto _map_it = m_node_map.begin(); _map_it!= m_node_map.end(); _map_it++){
    delete _map_it -> second;
  }
  m_node_map.clear();
}

MNM_Dnode *MNM_Node_Factory::make_node(TInt ID, DNode_type node_type, TFlt flow_scalar)
{
  MNM_Dnode *_node;
  switch (node_type){
    case MNM_TYPE_FWJ:
      _node = new MNM_Dnode_FWJ(ID, flow_scalar);
      break;
    case MNM_TYPE_GRJ:
      _node = new MNM_Dnode_GRJ(ID, flow_scalar);
      break;
    case MNM_TYPE_ORIGIN:
      _node = new MNM_DMOND(ID, flow_scalar);
      break;
    case MNM_TYPE_DEST:
      _node = new MNM_DMDND(ID, flow_scalar);
      break;
    default:
      printf("Wrong node type.\n");
      exit(-1);
  }
  m_node_map.insert(std::pair<TInt, MNM_Dnode*>(ID, _node));
  return _node;
}

MNM_Dnode *MNM_Node_Factory::get_node(TInt ID)
{
  auto _node_it = m_node_map.find(ID);
  if (_node_it == m_node_map.end()) {
    printf("No such node ID %d\n", (int) ID);
    return NULL;
  }
  return _node_it -> second;
}

/**************************************************************************
                          Link factory
**************************************************************************/
MNM_Link_Factory::MNM_Link_Factory()
{
  m_link_map = std::unordered_map<TInt, MNM_Dlink*>();
}

MNM_Link_Factory::~MNM_Link_Factory()
{
  for (auto _map_it = m_link_map.begin(); _map_it!= m_link_map.end(); _map_it++){
    delete _map_it -> second;
  }  
  m_link_map.clear();
}

MNM_Dlink *MNM_Link_Factory::make_link( TInt ID,
                      DLink_type link_type,
                      TFlt lane_hold_cap, 
                      TFlt lane_flow_cap, 
                      TInt number_of_lane,
                      TFlt length,
                      TFlt ffs,
                      TFlt unit_time,
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
  return m_link_map.find(ID) -> second;
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
/**************************************************************************
                          OD factory
**************************************************************************/
MNM_OD_Factory::MNM_OD_Factory()         
{
  m_origin_map = std::unordered_map<TInt, MNM_Origin*>();
  m_destination_map = std::unordered_map<TInt, MNM_Destination*>();
}


MNM_OD_Factory::~MNM_OD_Factory()         
{
  for (auto _map_it = m_origin_map.begin(); _map_it!= m_origin_map.end(); _map_it++){
    delete _map_it -> second;
  }  
  m_origin_map.clear();
    for (auto _map_it = m_destination_map.begin(); _map_it!= m_destination_map.end(); _map_it++){
    delete _map_it -> second;
  }  
  m_destination_map.clear();
}

MNM_Destination *MNM_OD_Factory::make_destination(TInt ID)
{
  MNM_Destination *_dest;
  _dest = new MNM_Destination(ID);
  m_destination_map.insert(std::pair<TInt, MNM_Destination*>(ID, _dest));
  return _dest;
}


MNM_Origin *MNM_OD_Factory::make_origin(TInt ID, TInt max_interval, TFlt flow_scalar, TInt frequency)
{
  MNM_Origin *_origin;
  _origin = new MNM_Origin(ID, max_interval, flow_scalar, frequency);
  m_origin_map.insert(std::pair<TInt, MNM_Origin*>(ID, _origin));
  return _origin;
}

MNM_Destination *MNM_OD_Factory::get_destination(TInt ID)
{
  return m_destination_map.find(ID) -> second;
}

MNM_Origin *MNM_OD_Factory::get_origin(TInt ID)
{
  return m_origin_map.find(ID) -> second;
}