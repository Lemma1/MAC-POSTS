#ifndef OD_H
#define OD_H

#include "Snap.h"
#include "vehicle.h"
#include "factory.h"
#include "dnode.h"

#include "g3log/g3log.hpp"
#include "g3log/logworker.hpp"

class MNM_Destination;
class MNM_Veh;
class MNM_Veh_Factory;

class MNM_Origin
{
public:
  MNM_Origin(TInt ID, TInt max_interval, TFlt flow_scalar);
  ~MNM_Origin();
  TInt m_Origin_ID;
  int release(MNM_Veh_Factory* veh_factory);
  int add_dest_demand(MNM_Destination *dest, TFlt* demand);
private:
  TInt m_current_interval;
  TInt m_max_interval;
  TFlt m_flow_scalar;
  MNM_DMOND *m_origin_node;
  std::map<MNM_Destination*, TFlt*> m_demand;
};


class MNM_Destination
{
public:
  MNM_Destination(TInt ID);
  ~MNM_Destination();
  TInt m_Dest_ID;
  TFlt m_flow_scalar;
  int receive_veh(MNM_Veh* veh);
};

#endif
