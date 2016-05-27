#ifndef DNODE_H
#define DNODE_H

#include "Snap.h"
#include "dlink.h"
#include "od.h"

#include <vector>
#include <map>
#include <deque>

class MNM_Dlink;
class MNM_Origin;
class MNM_Veh;

class MNM_Dnode
{
public:
  MNM_Dnode(TInt ID);
  ~MNM_Dnode();
  int virtual evolve(TInt timestamp){return 0;};
  void virtual print_info(){};

protected:
  TInt m_node_ID;
  std::vector<MNM_Dlink*> m_in_link_array;
  std::vector<MNM_Dlink*> m_out_link_array;
};

class MNM_Dnode_FWJ : public MNM_Dnode
{

};


class MNM_DMOND : public MNM_Dnode
{
public:
  MNM_DMOND(TInt ID, MNM_Origin* origin);
  ~MNM_DMOND();
  int virtual evolve(TInt timestamp){return 0;};
  void virtual print_info(){};
  std::deque<MNM_Veh *> m_in_veh_queue;
private:
  MNM_Origin *m_origin;
  std::vector<MNM_Dlink*> m_out_link_array;

};
#endif
