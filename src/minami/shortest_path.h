#ifndef SHORTEST_PATH_H
#define SHORTEST_PATH_H

#include "path.h"

#include "Snap.h"

class MNM_Shortestpath
{
public:
  MNM_Shortestpath();
  ~MNM_Shortestpath();
  int selector();
  int one2one(MNM_Path &);
  int one2all(THash<TInt, MNM_Path> &);
  int all2all(THash<TInt, THash<TInt, MNM_Path>> &);
};

#endif
