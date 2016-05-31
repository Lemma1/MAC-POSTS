#ifndef FACTORY_H
#define FACTORY_H

#include "Snap.h"
#include "vehicle.h"
#include "dnode.h"
#include "dlink.h"

#include <map>

class MNM_Veh;
class MNM_Dnode;
class MNM_Destination;
class MNM_Origin;

class MNM_Veh_Factory
{
public:
  MNM_Veh_Factory();
  ~MNM_Veh_Factory();
  MNM_Veh * make_veh(TInt timestamp);
private:
  TInt m_num_veh;
  std::map<TInt, MNM_Veh*> m_veh_map;
};

enum DNode_type {MNM_TYPE_ORIGIN, MNM_TYPE_DEST, MNM_TYPE_FWJ};

class MNM_Node_Factory
{
public:
  MNM_Node_Factory();
  ~MNM_Node_Factory();
  MNM_Dnode *make_node(TInt ID, TFlt flow_scalar, DNode_type node_type);
private:
  std::map<TInt, MNM_Dnode*> m_node_map;
};

class MNM_Link_Factory
{
public:
  MNM_Link_Factory();
  ~MNM_Link_Factory();
};
#endif
