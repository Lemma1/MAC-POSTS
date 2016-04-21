#ifndef SHORTEST_PATH_H
#define SHORTEST_PATH_H

#include "path.h"

#include "Snap.h"

class MNM_Shortestpath
{
public:
  MNM_Shortestpath();
  ~MNM_Shortestpath(); 
  int one2one(const MNM_Path& p, const PNGraph& g, const THash<TInt, TFlt>& c);
  // int one2all(THash<TInt, MNM_Path> &, PNGraph);
  // int all2all(THash<TInt, THash<TInt, MNM_Path>> &);
private:
  int selector();
};

#endif
