#ifndef DNODE_H
#define DNODE_H

#include "Snap.h"
#include "dlink.h"
#include "od.h"
#include "vehicle.h"
#include "ults.h"

#include <vector>
#include <map>
#include <deque>

class MNM_Dlink;
class MNM_Destination;
class MNM_Origin;
class MNM_Veh;

class MNM_Dnode
{
public:
  MNM_Dnode(TInt ID, TFlt flow_scalar);
  ~MNM_Dnode();
  int virtual evolve(TInt timestamp){return 0;};
  void virtual print_info(){};
  int virtual prepare_loading(){return 0;};
protected:
  TInt m_node_ID;
  TFlt m_flow_scalar;
};

class MNM_DMOND : public MNM_Dnode
{
public:
  MNM_DMOND(TInt ID, TFlt flow_scalar);
  ~MNM_DMOND();
  int virtual evolve(TInt timestamp);
  void virtual print_info();
  int add_out_link(MNM_Dlink* out_link);
  int hook_up_origin(MNM_Origin *origin);
  std::deque<MNM_Veh *> m_in_veh_queue;
private:
  MNM_Origin *m_origin;
  std::vector<MNM_Dlink*> m_out_link_array;
  std::map<MNM_Dlink*, TInt> m_out_volume;
};

class MNM_DMDND : public MNM_Dnode
{
public:
  MNM_DMDND(TInt ID, TFlt flow_scalar);
  ~MNM_DMDND();
  int virtual evolve(TInt timestamp);
  void virtual print_info();
  int add_in_link(MNM_Dlink *link);
  int hook_up_destination(MNM_Destination *dest);
  std::deque<MNM_Veh *> m_out_veh_queue;
private:
  MNM_Destination *m_dest;
  std::vector<MNM_Dlink*> m_in_link_array;
};

/**************************************************************************
                              In-out node
**************************************************************************/

class MNM_Dnode_Inout : public MNM_Dnode
{
public:
  MNM_Dnode_Inout(TInt ID, TFlt flow_scalar);
  ~MNM_Dnode_Inout();
  int virtual evolve(TInt timestamp){return 0;};
  void virtual print_info();
  int virtual prepare_loading();
protected:
  int prepare_supplyANDdemand();
  int virtual compute_flow(){return 0;};
  int round_flow_to_vehicle();
  int move_vehicle();
  std::vector<MNM_Dlink*> m_out_link_array;
  std::vector<MNM_Dlink*> m_in_link_array;
  TFlt *m_demand;
  TFlt *m_supply;
  TFlt *m_veh_flow;
  TInt *m_veh_tomove;
};

/**************************************************************************
                              FWJ node
**************************************************************************/
class MNM_Dnode_FWJ : public MNM_Dnode_Inout
{
public:
  MNM_Dnode_FWJ(TInt ID, TFlt flow_scalar);
  ~MNM_Dnode_FWJ();
  void virtual print_info();
  int virtual compute_flow();
};

#endif
