#include "ults.h"

TInt MNM_Ults::round(TFlt in)
{
  TFlt rdNum = TFlt(std::rand()/(1.0*RAND_MAX));
  TFlt floorN = TFlt(TInt(in));
  if((in - floorN) >rdNum)
    return TInt(floorN + 1);
  else
    return TInt(floorN);
}