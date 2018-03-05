#ifndef DTA_GRADIENT_UTLS_H
#define DTA_GRADIENT_UTLS_H

#include "Snap.h"
#include "dlink.h"

int get_link_inflow(double **output, std::vector<MNM_Dlink*> links, 
                    std::vector<TInt> intervals);

int get_travel_time(double **output, std::vector<MNM_Dlink*> links, 
                    std::vector<TInt> intervals);

int get_dar_matrix(double **output, std::vector<MNM_Dlink*> links, 
                    std::vector<MNM_Path*> paths, MNM_Veh_Factory *veh_factory)
#endif