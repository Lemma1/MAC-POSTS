#include "factory.h"
#include "vehicle.h"

#include "union_find.h"

#include <unordered_map>

class MNM_Gridlock_Checker
{
public:
  MNM_Gridlock_Checker(PNEGraph graph, MNM_Link_Factory *link_factory);
  ~MNM_Gridlock_Checker();

  bool is_gridlocked();
  int initialize();
  MNM_Veh* get_last_veh(MNM_Dlink *link);
  bool static has_cycle(PNEGraph graph);

  MNM_Link_Factory *m_link_factory;
  std::unordered_map<TInt, MNM_Veh*> m_link_veh_map;
  PNEGraph m_full_graph;
  PNEGraph m_gridlock_graph;
};