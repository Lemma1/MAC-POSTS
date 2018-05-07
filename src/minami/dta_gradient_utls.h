#ifndef DTA_GRADIENT_UTLS_H
#define DTA_GRADIENT_UTLS_H

#include "Snap.h"
#include "dlink.h"
#include "path.h"
#include "factory.h"

#include <set>

struct dar_record { 
  TInt path_ID; 
  TInt assign_int; 
  TInt link_ID;
  TFlt link_start_int;
  TFlt flow;
};

namespace MNM_DTA_GRADIENT
{
TFlt get_link_inflow(MNM_Dlink* link, 
                    TFlt start_time, TFlt end_time);
TFlt get_link_inflow(MNM_Dlink* link, 
                    TInt start_time, TInt end_time);

TFlt get_travel_time(MNM_Dlink* link, TFlt start_time);

int add_dar_records(std::vector<dar_record*> &record, MNM_Dlink* link, 
                    std::set<MNM_Path*> path_set, TFlt start_time, TFlt end_time);
};
#endif