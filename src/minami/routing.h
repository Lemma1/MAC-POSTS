#ifndef ROUTING_H
#define ROUTING_H

#include "Snap.h"
#include "statistics.h"
#include "vehicle.h"
#include "ults.h"
#include "shortest_path.h"
#include "path.h"
#include "pre_routing.h"

#include <unordered_map>

typedef std::unordered_map<MNM_Destination*, std::unordered_map<TInt, TInt>*> Routing_Table; 

class MNM_Routing
{
public:
  MNM_Routing(PNEGraph &graph,
              MNM_OD_Factory *od_factory, MNM_Node_Factory *node_factory, MNM_Link_Factory *link_factory);
  virtual ~MNM_Routing();
  int virtual init_routing(Path_Table *path_table=NULL){return 0;};
  int virtual update_routing(TInt timestamp){return 0;};
  PNEGraph m_graph;
  MNM_OD_Factory *m_od_factory;
  MNM_Link_Factory *m_link_factory;
  MNM_Node_Factory *m_node_factory;
};


/* only used for test */
class MNM_Routing_Random : public MNM_Routing
{
public:
  MNM_Routing_Random(PNEGraph &graph,
                      MNM_OD_Factory *od_factory, MNM_Node_Factory *node_factory, MNM_Link_Factory *link_factory);
  ~MNM_Routing_Random();
  int virtual init_routing(Path_Table *path_table=NULL);
  int virtual update_routing(TInt timestamp);
};



class MNM_Routing_Hybrid : public MNM_Routing
{
public:
  MNM_Routing_Hybrid(std::string file_folder, PNEGraph &graph, MNM_Statistics* statistics, 
                      MNM_OD_Factory *od_factory, MNM_Node_Factory *node_factory, MNM_Link_Factory *link_factory);
  ~MNM_Routing_Hybrid();
  int virtual init_routing(Path_Table *path_table=NULL);
  int virtual update_routing(TInt timestamp);  
private:
  MNM_Statistics* m_statistics;
  Routing_Table *m_table;
  TInt m_routing_freq;
  MNM_ConfReader *m_self_config;
};



class MNM_Routing_Fixed : public MNM_Routing
{
public:
  MNM_Routing_Fixed(PNEGraph &graph,
              MNM_OD_Factory *od_factory, MNM_Node_Factory *node_factory, MNM_Link_Factory *link_factory);
  ~MNM_Routing_Fixed();
  int virtual init_routing(Path_Table *path_table=NULL);
  int virtual update_routing(TInt timestamp);
// private:
  int set_path_table(Path_Table *path_table);
  int register_veh(MNM_Veh* veh);
  int add_veh_path(MNM_Veh* veh, std::deque<TInt> *link_que);
  Path_Table *m_path_table;
  std::unordered_map<MNM_Veh*, std::deque<TInt>*> m_tracker;
};

// ==================== For SO-DTA, by pinchao =========================//



class MNM_Routing_Predetermined : public MNM_Routing
{
public:
  MNM_Routing_Predetermined(PNEGraph &graph,
              MNM_OD_Factory *od_factory, MNM_Node_Factory *node_factory, MNM_Link_Factory *link_factory
              ,Path_Table *p_table, MNM_Pre_Routing *pre_routing);
  ~MNM_Routing_Predetermined();
  int virtual init_routing(Path_Table *path_table=NULL);
  int virtual update_routing(TInt timestamp);
  // private:
  // int set_path_table(Path_Table *path_table);
  // int register_veh(MNM_Veh* veh);
  // int add_veh_path(MNM_Veh* veh, std::deque<TInt> *link_que);
  Path_Table *m_path_table;
  MNM_Pre_Routing *m_pre_routing;
  std::unordered_map<MNM_Veh*, std::deque<TInt>*> m_tracker;
  // std::unordered_map<MNM_Veh*, std::deque<TInt>*> m_tracker;

};

#endif