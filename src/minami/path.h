#ifndef PATH_H
#define PATH_H

#include "Snap.h"

class MNM_Path
{
public:
  MNM_Path();
  ~MNM_Path();
  TVec<TInt> m_link_vec;
  TVec<TInt> m_node_vec;

};


class MNM_Path_Set
{
  MNM_Path_Set();
  ~MNM_Path_Set();
  TVec<MNM_Path*> m_path_vec
};
#endif