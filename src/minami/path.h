#ifndef PATH_H
#define PATH_H

#include "Snap.h"

class MNM_Path
{
public:
  MNM_Path();
  ~MNM_Path();
  int AddLinkID(TInt linkID);
  int GetSize();
  TInt *BegLinkID();
  TInt *EndLinkID();
private:
  TVec<TInt> m_path_vec;
};

#endif