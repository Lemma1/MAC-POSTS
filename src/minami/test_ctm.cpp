#include "dlink.h"

#include "g3log/g3log.hpp"
#include "g3log/logworker.hpp"

int main()
{

  auto worker = g3::LogWorker::createLogWorker();
  g3::initializeLogging(worker.get());

  LOG(DEBUG) << "Test SLOG INFO";
  // MNM_Dlink *a = new MNM_Dlink(1,2.0,3.0);
  MNM_Dlink_Ctm *b = new MNM_Dlink_Ctm(0.125, 0.611, 2, 800, 13.33, 5, 1);
  b -> print_info();
  LOG(DEBUG) << b -> get_link_supply();
  return 0;
}