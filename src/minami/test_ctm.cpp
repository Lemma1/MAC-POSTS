// #include "base.h"

#include "Snap.h"
#include "dlink.h"
#include "od.h"
#include "vehicle.h"
#include "dnode.h"
#include "factory.h"

#include "g3log/g3log.hpp"
#include "g3log/logworker.hpp"

#include <deque>

int main()
{

  auto worker = g3::LogWorker::createLogWorker();
  g3::initializeLogging(worker.get());

  LOG(DEBUG) << "Test SLOG INFO";
  MNM_Destination *d = new MNM_Destination(1);
  d -> m_Dest_ID = 11;
  // MNM_Dlink *a = new MNM_Dlink(1,2.0,3.0);
  MNM_Dlink *l = new MNM_Dlink_Ctm(0.125, 0.611, 2, 500, 13.33, 5, 1);
  l -> print_info();
  // LOG(DEBUG) << b -> get_link_supply();

  MNM_Veh *v;
  for (int i = 0; i < 4; ++i) {
    v = new MNM_Veh(i);
    v -> set_current_link(1);
    v -> set_destination(d);
    l -> m_incoming_array.push_back(v);
  }

  
  LOG(DEBUG) << v -> get_current_link();

  l -> clear_incoming_array();

  for (int i = 0; i < 8; ++i) {
    l -> evolve(i);
    printf("%d\n", int(l -> m_incoming_array.size()));
    l -> print_info();
    for (int i = 0; i < 3; ++i) {
      v = new MNM_Veh(i);
      v -> set_current_link(1);
      v -> set_destination(d);
      l -> m_incoming_array.push_back(v);
    }
    l -> clear_incoming_array();
  }

  return 0;
}