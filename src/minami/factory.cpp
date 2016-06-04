#include "factory.h"

/**************************************************************************
                          Vehicle node
**************************************************************************/
MNM_Veh_Factory::MNM_Veh_Factory()
{
  m_veh_map = std::map<TInt, MNM_Veh*>();
  m_num_veh = TInt(0);
}

MNM_Veh_Factory::~MNM_Veh_Factory()
{

}

MNM_Veh *MNM_Veh_Factory::make_veh(TInt timestamp)
{
  MNM_Veh *__veh = new MNM_Veh(m_num_veh + 1, timestamp);
  m_veh_map.insert(std::pair<TInt, MNM_Veh*>(m_num_veh + 1, __veh));
  m_num_veh += 1;
  return __veh;
}


/**************************************************************************
                          Node factory
**************************************************************************/

MNM_Node_Factory::MNM_Node_Factory()
{
  m_node_map = std::map<TInt, MNM_Dnode*>();
}

MNM_Dnode *MNM_Node_Factory::make_node(TInt ID, DNode_type node_type, TFlt flow_scalar)
{
  MNM_Dnode *__node;
  switch (node_type){
    case MNM_TYPE_FWJ:
      __node = new MNM_Dnode_FWJ(ID, flow_scalar);
      break;
    case MNM_TYPE_ORIGIN:
      __node = new MNM_DMOND(ID, flow_scalar);
      break;
    case MNM_TYPE_DEST:
      __node = new MNM_DMDND(ID, flow_scalar);
      break;
    default:
      printf("Wrong node type.\n");
      exit(-1);
  }
  m_node_map.insert(std::pair<TInt, MNM_Dnode*>(ID, __node));
  return __node;
}

MNM_Dnode *MNM_Node_Factory::get_node(TInt ID)
{
  return m_node_map.find(ID) -> second;
}
MNM_Destination *get_destination(TInt ID);
/**************************************************************************
                          Link factory
**************************************************************************/
MNM_Link_Factory::MNM_Link_Factory()
{
  m_link_map = std::map<TInt, MNM_Dlink*>();
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
  MNM_Dlink *__link;
  switch (link_type){
    case MNM_TYPE_CTM:
      __link = new MNM_Dlink_Ctm(ID, lane_hold_cap, lane_flow_cap, number_of_lane, length, ffs, unit_time, flow_scalar);
      break;
    case MNM_TYPE_PQ:
      __link = new MNM_Dlink_Pq(ID, lane_hold_cap, lane_flow_cap, number_of_lane, length, ffs, unit_time, flow_scalar);
      break;
    default:
      printf("Wrong link type.\n");
      exit(-1);
  }
  m_link_map.insert(std::pair<TInt, MNM_Dlink*>(ID, __link));
  return __link;
}

/**************************************************************************
                          OD factory
**************************************************************************/
MNM_OD_Factory::MNM_OD_Factory()         
{
  m_origin_map = std::map<TInt, MNM_Origin*>();
  m_destination_map = std::map<TInt, MNM_Destination*>();
}


MNM_Destination *MNM_OD_Factory::make_destination(TInt ID)
{
  MNM_Destination *__dest;
  __dest = new MNM_Destination(ID);
  m_destination_map.insert(std::pair<TInt, MNM_Destination*>(ID, __dest));
  return __dest;
}


MNM_Origin *MNM_OD_Factory::make_origin(TInt ID, TInt max_interval, TFlt flow_scalar)
{
  MNM_Origin *__origin;
  __origin = new MNM_Origin(ID, max_interval, flow_scalar);
  m_origin_map.insert(std::pair<TInt, MNM_Origin*>(ID, __origin));
  return __origin;
}

MNM_Destination *MNM_OD_Factory::get_destination(TInt ID)
{
  return m_destination_map.find(ID) -> second;
}

MNM_Origin *MNM_OD_Factory::get_origin(TInt ID)
{
  return m_origin_map.find(ID) -> second;
}