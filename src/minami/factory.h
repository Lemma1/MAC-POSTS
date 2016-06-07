#ifndef FACTORY_H
#define FACTORY_H

#include "Snap.h"
#include "vehicle.h"
#include "dnode.h"
#include "dlink.h"
#include "enum.h"

#include <iostream>
#include <map>

class MNM_Veh;
class MNM_Dnode;
class MNM_Destination;
class MNM_Origin;
class MNM_Dlink;



class MNM_Veh_Factory
{
public:
  MNM_Veh_Factory();
  ~MNM_Veh_Factory();
  MNM_Veh * make_veh(TInt timestamp);
  TInt m_num_veh;
  std::map<TInt, MNM_Veh*> m_veh_map;
};



class MNM_Node_Factory
{
public:
  MNM_Node_Factory();
  ~MNM_Node_Factory();
  MNM_Dnode *make_node(TInt ID, DNode_type node_type, TFlt flow_scalar);
  MNM_Dnode *get_node(TInt ID);
  std::map<TInt, MNM_Dnode*> m_node_map;
};


class MNM_Link_Factory
{
public:
  MNM_Link_Factory();
  ~MNM_Link_Factory();
  MNM_Dlink *make_link( TInt ID,
                        DLink_type link_type,
                        TFlt lane_hold_cap, 
                        TFlt lane_flow_cap, 
                        TInt number_of_lane,
                        TFlt length,
                        TFlt ffs,
                        TFlt unit_time,
                        TFlt flow_scalar);
  MNM_Dlink *get_link(TInt ID);
  std::map<TInt, MNM_Dlink*> m_link_map;
};


class MNM_OD_Factory
{
public:
  MNM_OD_Factory();
  ~MNM_OD_Factory();
  MNM_Destination *make_destination(TInt ID);
  MNM_Origin *make_origin(TInt ID, TInt max_interval, TFlt flow_scalar);
  MNM_Destination *get_destination(TInt ID);
  MNM_Origin *get_origin(TInt ID);
  std::map<TInt, MNM_Origin*> m_origin_map;
  std::map<TInt, MNM_Destination*> m_destination_map;
};
#endif
