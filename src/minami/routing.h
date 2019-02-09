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
  int virtual init_routing(Path_Table *path_table=NULL) override;
  int virtual update_routing(TInt timestamp) override;
};



class MNM_Routing_Adaptive : public MNM_Routing
{
public:
  MNM_Routing_Adaptive(std::string file_folder, PNEGraph &graph, MNM_Statistics* statistics, 
                      MNM_OD_Factory *od_factory, MNM_Node_Factory *node_factory, MNM_Link_Factory *link_factory);
  ~MNM_Routing_Adaptive();
  int virtual init_routing(Path_Table *path_table=NULL) override;
  int virtual update_routing(TInt timestamp) override;  
// private:
  MNM_Statistics* m_statistics;
  Routing_Table *m_table;
  TInt m_routing_freq;
  MNM_ConfReader *m_self_config;
};



class MNM_Routing_Fixed : public MNM_Routing
{
public:
  MNM_Routing_Fixed(PNEGraph &graph,
              MNM_OD_Factory *od_factory, MNM_Node_Factory *node_factory, 
              MNM_Link_Factory *link_factory, 
              TInt route_frq = TInt(-1), TInt buffer_len = TInt(-1));
  ~MNM_Routing_Fixed();
  int virtual init_routing(Path_Table *path_table=NULL) override;
  int virtual update_routing(TInt timestamp) override;
// private:
  int set_path_table(Path_Table *path_table);
  int register_veh(MNM_Veh* veh);
  int add_veh_path(MNM_Veh* veh, std::deque<TInt> *link_que);
  int change_choice_portion(TInt interval);
  Path_Table *m_path_table;
  std::unordered_map<MNM_Veh*, std::deque<TInt>*> m_tracker;
  bool m_buffer_as_p;
  TInt m_routing_freq;
  TInt m_buffer_length;
  // TInt m_cur_routing_interval;
};


// just a wrapper of Adaptive and Fixed routing
class MNM_Routing_Hybrid : public MNM_Routing
{
public:
  MNM_Routing_Hybrid(std::string file_folder, PNEGraph &graph, MNM_Statistics* statistics, MNM_OD_Factory *od_factory, 
                    MNM_Node_Factory *node_factory, MNM_Link_Factory *link_factory,
                     TInt route_frq_fixed = TInt(-1), TInt buffer_len = TInt(-1));
  ~MNM_Routing_Hybrid();
  int virtual init_routing(Path_Table *path_table=NULL) override;
  int virtual update_routing(TInt timestamp) override;

  MNM_Routing_Adaptive* m_routing_adaptive;
  MNM_Routing_Fixed* m_routing_fixed;
};



// ============================ For seperate routing by vehicle class ============================
// Fixed routing for cars and trucks.
class MNM_Routing_Biclass_Fixed : public MNM_Routing_Fixed
{
public:
  MNM_Routing_Biclass_Fixed(PNEGraph &graph,
              MNM_OD_Factory *od_factory, MNM_Node_Factory *node_factory, 
              MNM_Link_Factory *link_factory, TInt route_frq = TInt(-1), 
              TInt buffer_length= TInt(-1), TInt veh_class = TInt(-1));
  ~MNM_Routing_Biclass_Fixed();
  int virtual update_routing(TInt timestamp) override;
  int change_choice_portion(TInt interval);
  TInt m_buffer_length;
  TInt m_veh_class;
};

// Hybrid routing for cars and trucks.
// For adaptive vehicles: cars and trucks route together
// For static (fixed-route) vehicles: cars & trucks have different path_tables and buffers
class MNM_Routing_Biclass_Hybrid : public MNM_Routing
{
public:
  MNM_Routing_Biclass_Hybrid(std::string file_folder, PNEGraph &graph, MNM_Statistics* statistics, MNM_OD_Factory *od_factory, 
    MNM_Node_Factory *node_factory, MNM_Link_Factory *link_factory, TInt route_frq_fixed = TInt(-1), TInt buffer_length = TInt(-1));
  ~MNM_Routing_Biclass_Hybrid();
  int virtual init_routing(Path_Table *path_table=NULL) override;
  int virtual update_routing(TInt timestamp) override;

  MNM_Routing_Adaptive* m_routing_adaptive;
  MNM_Routing_Biclass_Fixed* m_routing_fixed_car;
  MNM_Routing_Biclass_Fixed* m_routing_fixed_truck;
};
// ============================ For seperate routing by vehicle class ============================




// ==================== For SO-DTA, by pinchao =========================//
class MNM_Routing_Predetermined : public MNM_Routing
{
public:
  MNM_Routing_Predetermined(PNEGraph &graph,
              MNM_OD_Factory *od_factory, MNM_Node_Factory *node_factory, MNM_Link_Factory *link_factory
              ,Path_Table *p_table, MNM_Pre_Routing *pre_routing, TInt max_int);
  ~MNM_Routing_Predetermined();
  int virtual init_routing(Path_Table *path_table=NULL);
  int virtual update_routing(TInt timestamp);
  // MNM_Pre_Routing* virtual get_routing_table(){return m_pre_routing;};
  // private:
  // int set_path_table(Path_Table *path_table);
  // int register_veh(MNM_Veh* veh);
  // int add_veh_path(MNM_Veh* veh, std::deque<TInt> *link_que);
  TInt m_total_assign_inter;
  Path_Table *m_path_table;
  MNM_Pre_Routing *m_pre_routing;
  std::unordered_map<MNM_Veh*, std::deque<TInt>*> m_tracker;

};

#endif