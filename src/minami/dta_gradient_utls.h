#ifndef DTA_GRADIENT_UTLS_H
#define DTA_GRADIENT_UTLS_H

#include "Snap.h"
#include "dlink.h"
#include "path.h"
#include "factory.h"

namespace MNM_DTA_GRADIENT
{
TFlt get_link_inflow(MNM_Dlink* link, 
                    TFlt start_time, TFlt end_time);
TFlt get_link_inflow(MNM_Dlink* link, 
                    TInt start_time, TInt end_time);

TFlt get_travel_time(MNM_Dlink* link, TFlt start_time);

int get_dar_matrix(double **output, std::vector<MNM_Dlink*> links, 
                    std::vector<MNM_Path*> paths, MNM_Veh_Factory *veh_factory);
};
#endif