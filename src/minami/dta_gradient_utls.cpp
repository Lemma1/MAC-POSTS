#include "dta_gradient_utls.h"

#include "limits.h"
#include "path.h"

namespace MNM_DTA_GRADIENT
{

TFlt get_link_inflow(MNM_Dlink* link, 
                    TFlt start_time, TFlt end_time)
{
  if (link == NULL){
    throw std::runtime_error("Error, get_link_inflow link is null");
  }
  if (link -> m_N_in == NULL){
    throw std::runtime_error("Error, get_link_inflow link cummulative curve is not installed");
  }
  // printf("ss %d\n", link -> m_link_ID());
  // printf("%x\n", link -> m_N_in);
  return link -> m_N_in -> get_result(end_time) - link -> m_N_in -> get_result(start_time);
}

TFlt get_link_inflow(MNM_Dlink* link, 
                    TInt start_time, TInt end_time)
{
  if (link == NULL){
    throw std::runtime_error("Error, get_link_inflow link is null");
  }
  if (link -> m_N_in == NULL){
    throw std::runtime_error("Error, get_link_inflow link cummulative curve is not installed");
  }
  return link -> m_N_in -> get_result(TFlt(end_time)) - link -> m_N_in -> get_result(TFlt(start_time));
}

TFlt get_travel_time(MNM_Dlink* link, TFlt start_time)
{
  if (link == NULL){
    throw std::runtime_error("Error, get_travel_time link is null");
  }
  if (link -> m_N_in == NULL){
    throw std::runtime_error("Error, get_travel_time link cummulative curve is not installed");
  }
  TFlt _cc_flow = link -> m_N_in -> get_result(start_time);
  if (_cc_flow <= DBL_EPSILON){
    return link -> get_link_tt();
  }
  TFlt _end_time = link -> m_N_out -> get_time(_cc_flow);
  if (_end_time() < 0 || (_end_time - start_time < 0)){
    return _end_time - start_time;
  }
  else{
    return link -> get_link_tt();
  }
  return 0;
}


int get_dar_matrix(double **output, std::vector<MNM_Dlink*> links, 
                    std::vector<MNM_Path*> paths, MNM_Veh_Factory *veh_factory)
{
  return 0;
}


}//end namespace MNM_DTA_GRADIENT