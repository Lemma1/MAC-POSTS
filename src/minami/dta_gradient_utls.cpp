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
    return link -> get_link_tt();
  }
  else{
    return _end_time - start_time;
  }
  return 0;
}


int add_dar_records(std::vector<dar_record*> &record, MNM_Dlink* link, 
                    std::unordered_map<MNM_Path*, int> path_map, TFlt start_time, TFlt end_time)
{
  if (link == NULL){
    throw std::runtime_error("Error, add_dar_records link is null");
  }
  if (link -> m_N_in_tree == NULL){
    throw std::runtime_error("Error, add_dar_records link cummulative curve tree is not installed");
  }
  MNM_Path* _path;
  for (auto path_it : link -> m_N_in_tree -> m_record){
    _path = path_it.first;
    // if (std::find(pathset.begin(), pathset.end(), _path) != pathset.end()) {
    if (path_map.find(_path) != path_map.end()){
      for (auto depart_it : path_it.second){
        TFlt tmp_flow = depart_it.second -> get_result(end_time) - depart_it.second -> get_result(start_time);
        if (tmp_flow > DBL_EPSILON){
          auto new_record = new dar_record();
          new_record -> path_ID = path_it.first -> m_path_ID;
          new_record -> assign_int = depart_it.first;
          new_record -> link_ID = link -> m_link_ID;
          new_record -> link_start_int = start_time;
          new_record -> flow = tmp_flow;
          // printf("Adding record, %d, %d, %d, %f, %f\n", new_record -> path_ID(), new_record -> assign_int(), 
          //     new_record -> link_ID(), (float)new_record -> link_start_int(), (float) new_record -> flow());
          record.push_back(new_record);
        }
      }
    }
  }
  return 0;
}



int add_dar_records_eigen(std::vector<Eigen::Triplet<double>> &record, MNM_Dlink* link, 
                    std::unordered_map<MNM_Path*, int> path_map, 
                    TFlt start_time, TFlt end_time,
                    int link_ind, int interval_ind, int num_e_link, int num_e_path,
                    double *f_ptr)
{
  if (link == NULL){
    throw std::runtime_error("Error, add_dar_records link is null");
  }
  if (link -> m_N_in_tree == NULL){
    throw std::runtime_error("Error, add_dar_records link cummulative curve tree is not installed");
  }
  MNM_Path* _path;
  int _x, _y;
  for (auto path_it : link -> m_N_in_tree -> m_record){
    _path = path_it.first;
    // if (std::find(pathset.begin(), pathset.end(), _path) != pathset.end()) {
    auto _path_iter = path_map.find(_path);
    if (_path_iter != path_map.end()){
      for (auto depart_it : path_it.second){
        TFlt tmp_flow = depart_it.second -> get_result(end_time) - depart_it.second -> get_result(start_time);
        if (tmp_flow > DBL_EPSILON){
          _x = link_ind + num_e_link * interval_ind;
          _y = (*_path_iter).second + num_e_path * depart_it.first; 
          // printf("Adding record, %d, %d, %d, %f, %f\n", new_record -> path_ID(), new_record -> assign_int(), 
          //     new_record -> link_ID(), (float)new_record -> link_start_int(), (float) new_record -> flow());
          record.push_back(Eigen::Triplet<double>((double)_x, (double) _y, tmp_flow() / f_ptr[_y]));
        }
      }
    }
  } 
  return 0;
}


}//end namespace MNM_DTA_GRADIENT