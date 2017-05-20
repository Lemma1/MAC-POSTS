// // #include "base.h"

// #include "Snap.h"
// #include "dlink.h"
// #include "vehicle.h"
// #include "dnode.h"
// #include "od.h"
// #include "factory.h"

// #include "g3log/g3log.hpp"
// #include "g3log/logworker.hpp"

// #include <deque>

// int main()
// {

//   auto worker = g3::LogWorker::createLogWorker();
//   g3::initializeLogging(worker.get());

//   LOG(DEBUG) << "Test SLOG INFO";
//   MNM_Destination *d = new MNM_Destination(1);
//   MNM_Origin *o = new MNM_Origin(1, 1, 5);
//   d -> m_Dest_ID = 11;
//   // MNM_Dlink *a = new MNM_Dlink(1,2.0,3.0);
//   MNM_Dlink *l = new MNM_Dlink_Ctm(0.125, 0.611, 2, 500, 13.33, 5, 1);
//   l -> print_info();
//   // LOG(DEBUG) << b -> get_link_supply();

//   MNM_DMOND *o_node = new MNM_DMOND(1, 5);
//   o_node -> hook_up_origin(o);
//   o_node -> evolve(4);
//   MNM_DMDND *d_node = new MNM_DMDND(1, 5);
//   d_node -> hook_up_destination(d);
//   d_node -> evolve(4);

//   MNM_Veh *v;
//   for (int i = 0; i < 4; ++i) {
//     v = new MNM_Veh(i, 0);
//     v -> set_current_link(l);
//     v -> set_destination(d);
//     l -> m_incoming_array.push_back(v);
//   }

  
//   LOG(DEBUG) << v -> get_current_link();

//   l -> clear_incoming_array();

//   for (int i = 0; i < 2; ++i) {
//     l -> evolve(i);
//     // printf("%d\n", int(l -> m_incoming_array.size()));
//     l -> print_info();
//     for (int j = 0; j < 4; ++j) {
//       v = new MNM_Veh(j, i);
//       v -> set_current_link(l);
//       v -> set_destination(d);
//       l -> m_incoming_array.push_back(v);
//     }
//     l -> clear_incoming_array();
//   }

//   return 0;
// }

#include "Snap.h"
#include "dlink.h"
#include "vehicle.h"
#include "dnode.h"
#include "od.h"
#include "factory.h"

#include "g3log/g3log.hpp"
#include "g3log/logworker.hpp"

#include <deque>
#include <iostream>


int main()
{

  // auto worker = g3::LogWorker::createLogWorker();
  // g3::initializeLogging(worker.get());

  // LOG(DEBUG) << "Test SLOG INFO";

  std::cout << "START !!!!" << std::endl;

  MNM_Destination *d = new MNM_Destination(1);
  std::cout << "START 1" << std::endl;
  MNM_Origin *o = new MNM_Origin(1, 1, 5,1);
  std::cout << "START 2" << std::endl;
  d -> m_Dest_ID = 11;
  std::cout << "START 3" << std::endl;
  MNM_Dlink *l = new MNM_Dlink_Ctm(0, 0.125, 0.611, 2, 500, 13.33, 5, 1);
  std::cout << "START 4" << std::endl;
  l -> print_info();
  std::cout << "START 5" << std::endl;
  MNM_DMOND *o_node = new MNM_DMOND(1, 5);
  std::cout << "START 6" << std::endl;
  o_node -> hook_up_origin(o);
  std::cout << "START 7" << std::endl;
  o_node -> evolve(4);
  std::cout << "START 8" << std::endl;
  MNM_DMDND *d_node = new MNM_DMDND(1, 5);
  std::cout << "START 9" << std::endl;
  d_node -> hook_up_destination(d);
  std::cout << "START 10" << std::endl;
  d_node -> evolve(4);
  std::cout << "START 11" << std::endl;
  MNM_Veh *v;
  std::cout << "START 12" << std::endl;
  std::cout << "START 13" << std::endl;
  for (int i = 0; i < 4; ++i) {
    std::cout << "Loop build link" << std::endl;
    v = new MNM_Veh(i, 0);
    v -> set_current_link(l);
    v -> set_destination(d);
    l -> m_incoming_array.push_back(v);
  }
  std::cout << "END Loop build link" << std::endl;

  
  // LOG(DEBUG) << v -> get_current_link();

  l -> clear_incoming_array();
  std::cout << "Start Evolve" << std::endl;
  for (int i = 0; i < 2; ++i) {
    std::cout << "Evolve loop" << std::endl;
    l -> evolve(i);
    // printf("%d\n", int(l -> m_incoming_array.size()));
    l -> print_info();
    for (int j = 0; j < 4; ++j) {
      v = new MNM_Veh(j, i);
      v -> set_current_link(l);
      v -> set_destination(d);
      l -> m_incoming_array.push_back(v);
    }
    l -> clear_incoming_array();
  }
  std::cout << "End Evolve" << std::endl;

  return 0;
}
