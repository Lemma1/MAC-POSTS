#ifndef ROUTING_H
#define ROUTING_H

class MNM_Routing
{
public:
  MNM_Routing(PNGraph graph, MNM_Condition* condition);
  ~MNM_Routing();
  int virtual init_routing(){return 0;};
  int virtual update_routing(){return 0;};
};


/* only used for test */
class MNM_Routing_Random : public MNM_Routing
{
public:
  MNM_Routing_Random();
  ~MNM_Routing_Random();
};
#endif