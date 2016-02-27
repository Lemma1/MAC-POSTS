// #include "base.h"
// // #include "spdlog.h"

// #include "Snap.h"

#include "g3log/g3log.hpp"
#include "g3log/logworker.hpp"

#include <iomanip>
#include <thread>
#include <iostream>
int main(int argc, char **argv)
{
  double pi_d = 3.1415926535897932384626433832795;
  float pi_f = 3.1415926535897932384626433832795f;

  auto worker = g3::LogWorker::createLogWorker();
  // auto handle= worker->addDefaultLogger(argv[0], "./");
  g3::initializeLogging(worker.get());
  // std::future<std::string> log_file_name = handle->call(&g3::FileSink::fileName);
  std::cout << "*   This is an example of g3log. It WILL exit by a failed CHECK(...)" << std::endl;
  std::cout << "*   that acts as a FATAL trigger. Please see the generated log and " << std::endl;
  std::cout << "*   compare to the code at:\n*  \t g3log/test_example/main_contract.cpp" << std::endl;
  // std::cout << "*\n*   Log file: [" << log_file_name.get() << "]\n\n" << std::endl;

  LOGF(INFO, "Hi log %d", 123);
  LOG(INFO) << "Test SLOG INFO";
  LOG(DEBUG) << "Test SLOG DEBUG";
  LOG(INFO) << "one: " << 1;
  LOG(INFO) << "two: " << 2;
  LOG(INFO) << "one and two: " << 1 << " and " << 2;
  LOG(DEBUG) << "float 2.14: " << 1000 / 2.14f;
  LOG(DEBUG) << "pi double: " << pi_d;
  LOG(DEBUG) << "pi float: " << pi_f;
  LOG(DEBUG) << "pi float (width 10): " << std::setprecision(10) << pi_f;
  LOGF(INFO, "pi float printf:%f", pi_f);



  // float d;
  // int a;
  //// what type of graph do you want to use?
  // typedef PUNGraph PGraph; // undirected graph
  //typedef PNGraph PGraph;  //   directed graph
  //typedef PNEGraph PGraph;  //   directed multigraph
  // typedef TPt<TNodeNet<TInt> > PGraph;
  // typedef TPt<TNodeEdgeNet<TInt, TInt> > PGraph;

  // // this code is independent of what particular graph implementation/type we use
  // printf("Creating graph:\n");
  // PGraph G = PGraph::TObj::New();
  // for (int n = 0; n < 10; n++) {
  //   G->AddNode(); // if no parameter is given, node ids are 0,1,...,9
  // }


  // G->AddEdge(0, 1);
  // for (int e = 0; e < 10; e++) {
  //   const int NId1 = G->GetRndNId();
  //   const int NId2 = G->GetRndNId();
  //   if (G->AddEdge(NId1, NId2) != -2) {
  //     printf("  Edge %d -- %d added\n", NId1,  NId2); }
  //   else {
  //     printf("  Edge %d -- %d already exists\n", NId1, NId2); }
  // }


  // IAssert(G->IsOk());
  // //G->Dump();
  // // delete
  // PGraph::TObj::TNodeI NI = G->GetNI(0);
  // printf("Delete edge %d -- %d\n", NI.GetId(), NI.GetOutNId(0));
  // G->DelEdge(NI.GetId(), NI.GetOutNId(0));
  // const int RndNId = G->GetRndNId();
  // printf("Delete node %d\n", RndNId);
  // G->DelNode(RndNId);
  // IAssert(G->IsOk());
  // // dump the graph
  // printf("Graph (%d, %d)\n", G->GetNodes(), G->GetEdges());
  // for (PGraph::TObj::TNodeI NI = G->BegNI(); NI < G->EndNI(); NI++) {
  //   printf("  %d: ", NI.GetId());
  //   for (int e = 0; e < NI.GetDeg(); e++) {
  //     printf(" %d", NI.GetNbrNId(e)); }
  //   printf("\n");
  // }
  // // dump subgraph
  // TIntV NIdV;
  // for (PGraph::TObj::TNodeI NI = G->BegNI(); NI < G->EndNI(); NI++) {
  //   if (NIdV.Len() < G->GetNodes()/2) { NIdV.Add(NI.GetId()); }
  // }
  // PGraph SubG = TSnap::GetSubGraph(G, NIdV);
  // //SubG->Dump();
  // // get UNGraph
  // { PUNGraph UNG = TSnap::ConvertGraph<PUNGraph>(SubG);
  // UNG->Dump();
  // IAssert(UNG->IsOk());
  // TSnap::ConvertSubGraph<PNGraph>(G, NIdV)->Dump(); }
  // // get NGraph
  // { PNGraph NG = TSnap::ConvertGraph<PNGraph>(SubG);
  // NG->Dump();
  // IAssert(NG->IsOk());
  // TSnap::ConvertSubGraph<PNGraph>(G, NIdV)->Dump(); }
  // // get NEGraph
  // { PNEGraph NEG = TSnap::ConvertGraph<PNEGraph>(SubG);
  // NEG->Dump();
  // IAssert(NEG->IsOk());
  // TSnap::ConvertSubGraph<PNGraph>(G, NIdV)->Dump(); }

  // TSnap::TestAnf<PUNGraph>();
  return 0;
}

// void TestEigSvd() {
//   PNGraph G = TSnap::GenRndGnm<PNGraph>(100,1000, true);
//   PUNGraph UG = TSnap::ConvertGraph<PUNGraph>(G);

//   TSnap::SaveMatlabSparseMtx(G, "test1.mtx");
//   TSnap::SaveMatlabSparseMtx(UG, "test2.mtx");

//   TFltV SngValV; TVec<TFltV> LeftV, RightV;
//   TSnap::GetSngVec(G, 20, SngValV, LeftV, RightV);
//   printf("Singular Values:\n");
//   for (int i =0; i < SngValV.Len(); i++) {
//     printf("%d\t%f\n", i, SngValV[i]()); }
//   printf("LEFT Singular Vectors:\n");
//   for (int i=0; i < LeftV[0].Len(); i++) {
//     printf("%d\t%f\t%f\t%f\t%f\t%f\n", i, LeftV[0][i](), LeftV[1][i](), LeftV[2][i](), LeftV[3][i](), LeftV[4][i]());
//   }
//   printf("RIGHT Singular Vectors:\n");
//   for (int i=0; i < RightV[0].Len(); i++) {
//     printf("%d\t%f\t%f\t%f\t%f\t%f\n", i, RightV[0][i](), RightV[1][i](), RightV[2][i](), RightV[3][i](), RightV[4][i]());
//   }
//   TFltV EigValV;
//   TVec<TFltV> EigV;
//   TSnap::GetEigVec(UG, 20, EigValV, EigV);
//   printf("Eigen Values:\n");
//   for (int i =0; i < EigValV.Len(); i++) {
//     printf("%d\t%f\n", i, EigValV[i]()); }
//   printf("Eigen Vectors %d:\n", EigV.Len());
//   for (int i =0; i < EigV[0].Len(); i++) {
//     printf("%d\t%f\t%f\t%f\t%f\t%f\n", i, EigV[0][i](), EigV[1][i](), EigV[2][i](), EigV[3][i](), EigV[4][i]());
//   }
  
// }