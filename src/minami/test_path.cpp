#include "Snap.h"

#include "g3log/g3log.hpp"
#include "g3log/logworker.hpp"

#include "path.h"

int main(int argc, char **argv)
{
  // TFlt pi_d = 3.1415926535897932384626433832795;
  // TFlt pi_f = 3.1415926535897932384626433832795f;

  auto worker = g3::LogWorker::createLogWorker();
  // // auto handle= worker->addDefaultLogger(argv[0], "./");
  g3::initializeLogging(worker.get());

  // LOGF(INFO, "Hi log %d", 123);
  LOG(INFO) << "Test SLOG INFO";
  // LOG(DEBUG) << "Test SLOG DEBUG";


  MNM_Path p = MNM_Path();
  for (TInt i=0; i< 10; i++){
    p.AddLinkID(i);
    printf("adding %d\n", (int)i);
  }
  printf("The size is now %d\n", p.GetSize());

  for (TInt* eI = p.BegLinkID(); eI != p.EndLinkID(); eI++) {
    printf("Now is Link ID %d\n", (int) *eI);
  }

}