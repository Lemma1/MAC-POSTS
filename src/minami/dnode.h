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
  TInt m_node_ID;
  int virtual evolve(TInt timestamp){return 0;};
  void virtual print_info(){};
  int virtual prepare_loading(){return 0;};
  int virtual add_out_link(MNM_Dlink* out_link){printf("Error!\n"); return 0;};
  int virtual add_in_link(MNM_Dlink* int_link){printf("Error!\n"); return 0;};
  std::vector<MNM_Dlink*> m_out_link_array;
  std::vector<MNM_Dlink*> m_in_link_array;
protected:
  TFlt m_flow_scalar;
};

class MNM_DMOND : public MNM_Dnode
{
public:
  MNM_DMOND(TInt ID, TFlt flow_scalar);
  ~MNM_DMOND();
  int virtual evolve(TInt timestamp);
  void virtual print_info();
  int virtual add_out_link(MNM_Dlink* out_link);
  int hook_up_origin(MNM_Origin *origin);
  std::deque<MNM_Veh *> m_in_veh_queue;
private:
  MNM_Origin *m_origin;
  std::map<MNM_Dlink*, TInt> m_out_volume;
};

class MNM_DMDND : public MNM_Dnode
{
public:
  MNM_DMDND(TInt ID, TFlt flow_scalar);
  ~MNM_DMDND();
  int virtual evolve(TInt timestamp);
  void virtual print_info();
  int virtual add_in_link(MNM_Dlink *link);
  int hook_up_destination(MNM_Destination *dest);
  std::deque<MNM_Veh *> m_out_veh_queue;
private:
  MNM_Destination *m_dest;
};

/**************************************************************************
                              In-out node
**************************************************************************/

class MNM_Dnode_Inout : public MNM_Dnode
{
public:
  MNM_Dnode_Inout(TInt ID, TFlt flow_scalar);
  ~MNM_Dnode_Inout();
  int virtual evolve(TInt timestamp);
  void virtual print_info();
  int virtual prepare_loading();
  int virtual add_out_link(MNM_Dlink* out_link);
  int virtual add_in_link(MNM_Dlink* int_link);
protected:
  int prepare_supplyANDdemand();
  int virtual compute_flow(){return 0;};
  int round_flow_to_vehicle();
  int move_vehicle();
  TFlt *m_demand; //2d array
  TFlt *m_supply; //1d array
  TFlt *m_veh_flow; //2d array
  TInt *m_veh_tomove; //2d array
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
