#include "dlink.h"

#include "g3log/g3log.hpp"
#include "g3log/logworker.hpp"

int main()
{

  auto worker = g3::LogWorker::createLogWorker();
  g3::initializeLogging(worker.get());

  LOG(DEBUG) << "Test SLOG INFO";
  // MNM_Dlink *a = new MNM_Dlink(1,2.0,3.0);
  MNM_Dlink_Ctm *b = new MNM_Dlink_Ctm(1,2.0,3.0, 1000,2.0, 5,1);


  return 0;
}