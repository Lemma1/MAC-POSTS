#ifndef ROUTING_H
#define ROUTING_H

#include "Snap.h"
#include "statistics.h"
#include "vehicle.h"
#include "ults.h"

class MNM_Routing
{
public:
  MNM_Routing(PNEGraph &graph, MNM_Statistics* statistics, 
              MNM_OD_Factory *od_factory, MNM_Link_Factory *link_factory);
  ~MNM_Routing();
  int virtual init_routing(){return 0;};
  int virtual update_routing(TInt Timestamp){return 0;};
  MNM_Statistics* m_statistics;
  PNEGraph m_graph;
  MNM_OD_Factory *m_od_factory;
  MNM_Link_Factory *m_link_factory;
};


/* only used for test */
class MNM_Routing_Random : public MNM_Routing
{
public:
  MNM_Routing_Random(PNEGraph &graph, MNM_Statistics* statistics, 
                      MNM_OD_Factory *od_factory, MNM_Link_Factory *link_factory);
  ~MNM_Routing_Random();
  int virtual init_routing();
  int virtual update_routing(TInt Timestamp);
};



class MNM_Routing_Hybrid : public MNM_Routing
{
  MNM_Routing_Hybrid(PNEGraph &graph, MNM_Statistics* statistics, 
                      MNM_OD_Factory *od_factory, MNM_Link_Factory *link_factory);
  ~MNM_Routing_Hybrid();
  int virtual init_routing();
  int virtual update_routing(TInt Timestamp);  
};
#endif