#ifndef DTA_GRADIENT_UTLS_H
#define DTA_GRADIENT_UTLS_H

#include "Snap.h"
#include "dlink.h"
#include "path.h"
#include "factory.h"

#include <unordered_map>
#include <set>

#include <Eigen/Sparse>

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
TFlt get_path_travel_time(MNM_Path* path, TFlt start_time, MNM_Link_Factory* link_factory);

int add_dar_records(std::vector<dar_record*> &record, MNM_Dlink* link, 
                    std::unordered_map<MNM_Path*, int> path_map, TFlt start_time, TFlt end_time);
int add_dar_records_eigen(std::vector<Eigen::Triplet<double>> &record, MNM_Dlink* link, 
                    std::unordered_map<MNM_Path*, int> path_map, TFlt start_time, TFlt end_time,
                    int link_ind, int interval_ind, int num_e_link, int num_e_path,
                    double *f_ptr);
};
#endif