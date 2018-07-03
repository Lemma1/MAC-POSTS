#include <pybind11/pybind11.h>
#include "dta_api.h"


#include "dta_gradient_utls.h"
#include "multiclass.h"

#include <unordered_map>

namespace py = pybind11;

int run_dta(std::string folder) {
  printf("Current working directory is......\n");
  std::cout << folder << std::endl;

  MNM_Dta *test_dta = new MNM_Dta(folder);
  test_dta -> build_from_files();
  printf("Hooking......\n");
  test_dta -> hook_up_node_and_link();
  // printf("Checking......\n");
  // test_dta -> is_ok();
  test_dta -> loading(false);


  delete test_dta;

  return 0;
}


Dta_Api::Dta_Api()
{
  m_dta = NULL;
  m_link_vec = std::vector<MNM_Dlink*>();
  m_path_vec = std::vector<MNM_Path*>();
  m_path_set = std::set<MNM_Path*>(); 
  m_ID_path_mapping = std::unordered_map<TInt, MNM_Path*>();
}

Dta_Api::~Dta_Api()
{
  if (m_dta != NULL){
    delete m_dta;
  }
  m_link_vec.clear();
  m_path_vec.clear();
  
}

int Dta_Api::initialize(std::string folder)
{
  m_dta = new MNM_Dta(folder);
  m_dta -> build_from_files();
  m_dta -> hook_up_node_and_link();
  m_dta -> is_ok();
  // printf("start load ID path mapping 0\n");
  if (MNM_Routing_Fixed *_routing = dynamic_cast<MNM_Routing_Fixed *>(m_dta -> m_routing)){
    MNM::get_ID_path_mapping(m_ID_path_mapping, _routing -> m_path_table);
  }
  if (MNM_Routing_Hybrid *_routing = dynamic_cast<MNM_Routing_Hybrid *>(m_dta -> m_routing)){
    // printf("start load ID path mapping\n");
    MNM::get_ID_path_mapping(m_ID_path_mapping, _routing -> m_routing_fixed -> m_path_table);
    // printf("mapping size %d\n", m_ID_path_mapping.size());
  }
  return 0;
}

int Dta_Api::run_once()
{
  return 0;
}

int Dta_Api::install_cc()
{
  for (size_t i = 0; i<m_link_vec.size(); ++i){
    m_link_vec[i] -> install_cumulative_curve();
  }
  return 0;
}

int Dta_Api::install_cc_tree()
{
  for (size_t i = 0; i<m_link_vec.size(); ++i){
    m_link_vec[i] -> install_cumulative_curve_tree();
  }
  return 0;
}

int Dta_Api::run_whole()
{

  m_dta -> loading(false);
  return 0;
}

int Dta_Api::get_cur_loading_interval()
{
  return m_dta -> m_current_loading_interval();
}

int Dta_Api::register_links(py::array_t<int> links)
{
  if (m_link_vec.size() > 0){
    printf("Warning, Dta_Api::register_links, link exists\n");
    m_link_vec.clear();
  }
  auto links_buf = links.request();
  if (links_buf.ndim != 1){
    throw std::runtime_error("Number of dimensions must be one");
  }
  int *links_ptr = (int *) links_buf.ptr;
  MNM_Dlink *_link;
  for (int i = 0; i < links_buf.shape[0]; i++){
    _link = m_dta -> m_link_factory -> get_link(TInt(links_ptr[i]));
    if(std::find(m_link_vec.begin(), m_link_vec.end(), _link) != m_link_vec.end()) {
      throw std::runtime_error("Error, Dta_Api::register_links, link exists");
    } 
    else {
      m_link_vec.push_back(_link);
    }
  }
  return 0;
}

int Dta_Api::register_paths(py::array_t<int> paths)
{
  if (m_link_vec.size() > 0){
    printf("Warning, Dta_Api::register_paths, path exists\n");
    m_path_vec.clear();
    m_path_set.clear();
  }
  auto paths_buf = paths.request();
  if (paths_buf.ndim != 1){
    throw std::runtime_error("register_paths: Number of dimensions must be one");
  }
  int *paths_ptr = (int *) paths_buf.ptr; 
  TInt _path_ID;
  for (int i = 0; i < paths_buf.shape[0]; i++){
    _path_ID = TInt(paths_ptr[i]);
    printf("registering path %d, %d\n", _path_ID(), (int)m_ID_path_mapping.size());
    if (m_ID_path_mapping.find(_path_ID) == m_ID_path_mapping.end()){
      throw std::runtime_error("register_paths: No such path");
    }
    else {
      m_path_vec.push_back(m_ID_path_mapping[_path_ID]);
    }
  }
  m_path_set = std::set<MNM_Path*> (m_path_vec.begin(), m_path_vec.end());
  return 0;
}

py::array_t<double> Dta_Api::get_link_inflow(py::array_t<int>start_intervals, py::array_t<int>end_intervals)
{
  auto start_buf = start_intervals.request();
  auto end_buf = end_intervals.request();
  if (start_buf.ndim != 1 || end_buf.ndim != 1){
    throw std::runtime_error("Error, Dta_Api::get_link_inflow, input dismension mismatch");
  }
  if (start_buf.shape[0] != end_buf.shape[0]){
    throw std::runtime_error("Error, Dta_Api::get_link_inflow, input length mismatch");
  }
  int l = start_buf.shape[0];
  int new_shape [2] = { (int) m_link_vec.size(), l};
  auto result = py::array_t<double>(new_shape);
  auto result_buf = result.request();
  double *result_prt = (double *) result_buf.ptr;
  int *start_prt = (int *) start_buf.ptr;
  int *end_prt = (int *) end_buf.ptr;
  for (int t = 0; t < l; ++t){
    for (size_t i = 0; i<m_link_vec.size(); ++i){
      if (end_prt[t] < start_prt[t]){
        throw std::runtime_error("Error, Dta_Api::get_link_inflow, end time smaller than start time");
      }
      if (end_prt[t] > get_cur_loading_interval()){
        throw std::runtime_error("Error, Dta_Api::get_link_inflow, loaded data not enough");
      }

      result_prt[i * l + t] = MNM_DTA_GRADIENT::get_link_inflow(m_link_vec[i], TFlt(start_prt[t]), TFlt(end_prt[t]))();
      // printf("i %d, t %d, %f\n", i, t, result_prt[i * l + t]);
    }
  }
  return result;
}

py::array_t<double> Dta_Api::get_link_tt(py::array_t<int>start_intervals)
{
  auto start_buf = start_intervals.request();
  if (start_buf.ndim != 1){
    throw std::runtime_error("Error, Dta_Api::get_link_tt, input dismension mismatch");
  }
  int l = start_buf.shape[0];
  int new_shape [2] = { (int) m_link_vec.size(), l}; 

  auto result = py::array_t<double>(new_shape);
  auto result_buf = result.request();
  double *result_prt = (double *) result_buf.ptr;
  int *start_prt = (int *) start_buf.ptr;
  for (int t = 0; t < l; ++t){
    for (size_t i = 0; i<m_link_vec.size(); ++i){
      if (start_prt[t] > get_cur_loading_interval()){
        throw std::runtime_error("Error, Dta_Api::get_link_tt, loaded data not enough");
      }
      result_prt[i * l + t] = MNM_DTA_GRADIENT::get_travel_time(m_link_vec[i], TFlt(start_prt[t]))();
    }
  }
  return result;
}

py::array_t<double> Dta_Api::get_link_in_cc(int link_ID)
{
  if (m_dta -> m_link_factory -> get_link(TInt(link_ID)) -> m_N_in == NULL){
    throw std::runtime_error("Error, Dta_Api::get_link_in_cc, cc not installed");
  }
  std::deque<std::pair<TFlt, TFlt>> _record = m_dta -> m_link_factory -> get_link(TInt(link_ID)) -> m_N_in -> m_recorder;
  int new_shape [2] = { (int) _record.size(), 2}; 
  auto result = py::array_t<double>(new_shape);
  auto result_buf = result.request();
  double *result_prt = (double *) result_buf.ptr;
  for (size_t i=0; i< _record.size(); ++i){
    result_prt[i * 2 ] = _record[i].first();
    result_prt[i * 2 + 1 ] =  _record[i].second();
  }
  return result;
}


py::array_t<double> Dta_Api::get_link_out_cc(int link_ID)
{
  if (m_dta -> m_link_factory -> get_link(TInt(link_ID)) -> m_N_out == NULL){
    throw std::runtime_error("Error, Dta_Api::get_link_out_cc, cc not installed");
  }
  std::deque<std::pair<TFlt, TFlt>> _record = m_dta -> m_link_factory -> get_link(TInt(link_ID)) -> m_N_out -> m_recorder;
  int new_shape [2] = { (int) _record.size(), 2}; 
  auto result = py::array_t<double>(new_shape);
  auto result_buf = result.request();
  double *result_prt = (double *) result_buf.ptr;
  for (size_t i=0; i< _record.size(); ++i){
    result_prt[i * 2 ] = _record[i].first();
    result_prt[i * 2 + 1 ] =  _record[i].second();
  }
  return result;
}


py::array_t<double> Dta_Api::get_dar_matrix(py::array_t<int>start_intervals, py::array_t<int>end_intervals)
{
  auto start_buf = start_intervals.request();
  auto end_buf = end_intervals.request();
  if (start_buf.ndim != 1 || end_buf.ndim != 1){
    throw std::runtime_error("Error, Dta_Api::get_link_inflow, input dismension mismatch");
  }
  if (start_buf.shape[0] != end_buf.shape[0]){
    throw std::runtime_error("Error, Dta_Api::get_link_inflow, input length mismatch");
  }
  int l = start_buf.shape[0];
  int *start_prt = (int *) start_buf.ptr;
  int *end_prt = (int *) end_buf.ptr;
  std::vector<dar_record*> _record = std::vector<dar_record*>();
  // for (size_t i = 0; i<m_link_vec.size(); ++i){
  //   m_link_vec[i] -> m_N_in_tree -> print_out();
  // }
  for (int t = 0; t < l; ++t){
    for (size_t i = 0; i<m_link_vec.size(); ++i){
      if (end_prt[t] < start_prt[t]){
        throw std::runtime_error("Error, Dta_Api::get_dar_matrix, end time smaller than start time");
      }
      if (end_prt[t] > get_cur_loading_interval()){
        throw std::runtime_error("Error, Dta_Api::get_dar_matrix, loaded data not enough");
      }
        MNM_DTA_GRADIENT::add_dar_records(
                      _record, m_link_vec[i], m_path_set, TFlt(start_prt[t]), TFlt(end_prt[t]));
    }
  }
  // path_ID, assign_time, link_ID, start_int, flow
  int new_shape [2] = { (int) _record.size(), 5}; 
  auto result = py::array_t<double>(new_shape);
  auto result_buf = result.request();
  double *result_prt = (double *) result_buf.ptr;
  dar_record* tmp_record;
  for (size_t i = 0; i < _record.size(); ++i){
    tmp_record = _record[i];
    result_prt[i * 5 + 0] = (double) tmp_record -> path_ID();
    result_prt[i * 5 + 1] = (double) tmp_record -> assign_int();
    result_prt[i * 5 + 2] = (double) tmp_record -> link_ID();
    result_prt[i * 5 + 3] = (double) tmp_record -> link_start_int();
    result_prt[i * 5 + 4] = tmp_record -> flow();
  }
  for (size_t i = 0; i < _record.size(); ++i){
    delete _record[i];
  }
  _record.clear();
  return result;
}



/***************************************************************
***************************************************************
                        Multiclass
***************************************************************
***************************************************************/

Mcdta_Api::Mcdta_Api()
{
  m_mcdta = NULL;
  m_link_vec = std::vector<MNM_Dlink_Multiclass*>();
  m_path_vec = std::vector<MNM_Path*>();
  m_path_set = std::set<MNM_Path*>(); 
  m_ID_path_mapping = std::unordered_map<TInt, MNM_Path*>();
}

Mcdta_Api::~Mcdta_Api()
{
  if (m_mcdta != NULL){
    delete m_mcdta;
  }
  m_link_vec.clear();
  m_path_vec.clear();
  
}

int Mcdta_Api::initialize(std::string folder)
{
  m_mcdta = new MNM_Dta_Multiclass(folder);
  m_mcdta -> build_from_files();
  m_mcdta -> hook_up_node_and_link();
  m_mcdta -> is_ok();
  if (MNM_Routing_Fixed *_routing = dynamic_cast<MNM_Routing_Fixed *>(m_mcdta -> m_routing)){
    MNM::get_ID_path_mapping(m_ID_path_mapping, _routing -> m_path_table);
  }
  if (MNM_Routing_Hybrid *_routing = dynamic_cast<MNM_Routing_Hybrid *>(m_mcdta -> m_routing)){
    // printf("start load ID path mapping\n");
    MNM::get_ID_path_mapping(m_ID_path_mapping, _routing -> m_routing_fixed -> m_path_table);
    // printf("mapping size %d\n", m_ID_path_mapping.size());
  }
  return 0;
}

int Mcdta_Api::install_cc()
{
  for (size_t i = 0; i < m_link_vec.size(); ++i){
    m_link_vec[i] -> install_cumulative_curve_multiclass();
  }
  return 0;
}

int Mcdta_Api::install_cc_tree()
{
  for (size_t i = 0; i < m_link_vec.size(); ++i){
    m_link_vec[i] -> install_cumulative_curve_tree_multiclass();
  }
  return 0;
}

int Mcdta_Api::run_whole()
{
  m_mcdta -> pre_loading();
  m_mcdta -> loading(false);
  return 0;
}

int Mcdta_Api::register_links(py::array_t<int> links)
{
  if (m_link_vec.size() > 0){
    printf("Warning, Dta_Api::register_links, link exists\n");
    m_link_vec.clear();
  }
  auto links_buf = links.request();
  if (links_buf.ndim != 1){
    throw std::runtime_error("Number of dimensions must be one");
  }
  int *links_ptr = (int *) links_buf.ptr;
  MNM_Dlink *_link;
  for (int i = 0; i < links_buf.shape[0]; i++){
    _link = m_mcdta -> m_link_factory -> get_link(TInt(links_ptr[i]));
    if (MNM_Dlink_Multiclass * _mclink = dynamic_cast<MNM_Dlink_Multiclass *>(_link)){
      if(std::find(m_link_vec.begin(), m_link_vec.end(), _link) != m_link_vec.end()) {
        throw std::runtime_error("Error, Dta_Api::register_links, link exists");
      } 
      else {
        m_link_vec.push_back(_mclink);
      }
    }
    else{
      throw std::runtime_error("Mcdta_Api::register_links: link type is not multiclass");
    }
  }
  return 0;
}

int Mcdta_Api::get_cur_loading_interval()
{
  return m_mcdta -> m_current_loading_interval();
}

int Mcdta_Api::print_emission_stats()
{
    TInt _count_car = 0, _count_truck = 0;
    TFlt _tot_tt = 0.0;
    MNM_Veh_Multiclass* _veh;
    for (auto _map_it : m_mcdta -> m_veh_factory -> m_veh_map){
        if (_map_it.second -> m_finish_time > 0) {
            _veh = dynamic_cast<MNM_Veh_Multiclass *>(_map_it.second);
            if (_veh -> m_class == 0){
                _count_car += 1;
            }
            else {
                _count_truck += 1;
            }
            _tot_tt += (_veh -> m_finish_time - _veh -> m_start_time) * 5.0 / 3600.0;
        }
    }
    printf("\n\nTotal car: %d, Total truck: %d, Total tt: %.2f hours\n\n", 
           int(_count_car/m_mcdta -> m_flow_scalar), int(_count_truck/m_mcdta -> m_flow_scalar), 
           float(_tot_tt/m_mcdta -> m_flow_scalar));
    m_mcdta -> m_emission -> output();
}

// unit: m_mcdta -> m_unit_time (eg: 5 seconds)
py::array_t<double> Mcdta_Api::get_car_link_tt(py::array_t<double>start_intervals)
{
  auto start_buf = start_intervals.request();
  if (start_buf.ndim != 1){
    throw std::runtime_error("Error, Mcdta_Api::get_car_link_tt, input dismension mismatch");
  }
  int l = start_buf.shape[0];
  int new_shape [2] = { (int) m_link_vec.size(), l}; 

  auto result = py::array_t<double>(new_shape);
  auto result_buf = result.request();
  double *result_prt = (double *) result_buf.ptr;
  double *start_prt = (double *) start_buf.ptr;
  for (int t = 0; t < l; ++t){
    if (start_prt[t] > get_cur_loading_interval()){
      throw std::runtime_error("Error, Mcdta_Api::get_car_link_tt, loaded data not enough");
    }
    for (size_t i = 0; i < m_link_vec.size(); ++i){  
      result_prt[i * l + t] = MNM_DTA_GRADIENT::get_travel_time_car(m_link_vec[i], TFlt(start_prt[t]))();
    }
  }
  return result;
}

// unit: m_mcdta -> m_unit_time (eg: 5 seconds)
py::array_t<double> Mcdta_Api::get_truck_link_tt(py::array_t<double>start_intervals)
{
  auto start_buf = start_intervals.request();
  if (start_buf.ndim != 1){
    throw std::runtime_error("Error, Mcdta_Api::get_truck_link_tt, input dismension mismatch");
  }
  int l = start_buf.shape[0];
  int new_shape [2] = { (int) m_link_vec.size(), l}; 

  auto result = py::array_t<double>(new_shape);
  auto result_buf = result.request();
  double *result_prt = (double *) result_buf.ptr;
  double *start_prt = (double *) start_buf.ptr;
  for (int t = 0; t < l; ++t){
    if (start_prt[t] > get_cur_loading_interval()){
      throw std::runtime_error("Error, Mcdta_Api::get_truck_link_tt, loaded data not enough");
    }
    for (size_t i = 0; i < m_link_vec.size(); ++i){    
      result_prt[i * l + t] = MNM_DTA_GRADIENT::get_travel_time_truck(m_link_vec[i], TFlt(start_prt[t]))();
    }
  }
  return result;
}

// unit: mile per hour
py::array_t<double> Mcdta_Api::get_car_link_speed(py::array_t<double>start_intervals)
{
  auto start_buf = start_intervals.request();
  if (start_buf.ndim != 1){
    throw std::runtime_error("Error, Mcdta_Api::get_car_link_speed, input dismension mismatch");
  }
  int l = start_buf.shape[0];
  int new_shape [2] = { (int) m_link_vec.size(), l};

  auto result = py::array_t<double>(new_shape);
  auto result_buf = result.request();
  double *result_prt = (double *) result_buf.ptr;
  double *start_prt = (double *) start_buf.ptr;
  for (int t = 0; t < l; ++t){
    if (start_prt[t] > get_cur_loading_interval()){
      throw std::runtime_error("Error, Mcdta_Api::get_car_link_speed, loaded data not enough");
    }
    for (size_t i = 0; i < m_link_vec.size(); ++i){  
      int _tt = MNM_DTA_GRADIENT::get_travel_time_car(m_link_vec[i], TFlt(start_prt[t]))() * m_mcdta -> m_unit_time; //seconds
      result_prt[i * l + t] = (m_link_vec[i] -> m_length) / _tt * 3600;
    }
  }
  return result;
}

// unit: mile per hour
py::array_t<double> Mcdta_Api::get_truck_link_speed(py::array_t<double>start_intervals)
{
  auto start_buf = start_intervals.request();
  if (start_buf.ndim != 1){
    throw std::runtime_error("Error, Mcdta_Api::get_truck_link_speed, input dismension mismatch");
  }
  int l = start_buf.shape[0];
  int new_shape [2] = { (int) m_link_vec.size(), l}; 

  auto result = py::array_t<double>(new_shape);
  auto result_buf = result.request();
  double *result_prt = (double *) result_buf.ptr;
  double *start_prt = (double *) start_buf.ptr;
  for (int t = 0; t < l; ++t){
    if (start_prt[t] > get_cur_loading_interval()){
      throw std::runtime_error("Error, Mcdta_Api::get_truck_link_speed, loaded data not enough");
    }
    for (size_t i = 0; i < m_link_vec.size(); ++i){
      int _tt = MNM_DTA_GRADIENT::get_travel_time_truck(m_link_vec[i], TFlt(start_prt[t]))() * m_mcdta -> m_unit_time; //seconds
      result_prt[i * l + t] = (m_link_vec[i] -> m_length) / _tt * 3600;
    }
  }
  return result;
}

py::array_t<double> Mcdta_Api::get_link_car_inflow(py::array_t<int>start_intervals, py::array_t<int>end_intervals)
{
  auto start_buf = start_intervals.request();
  auto end_buf = end_intervals.request();
  if (start_buf.ndim != 1 || end_buf.ndim != 1){
    throw std::runtime_error("Error, Mcdta_Api::get_link_car_inflow, input dismension mismatch");
  }
  if (start_buf.shape[0] != end_buf.shape[0]){
    throw std::runtime_error("Error, Mcdta_Api::get_link_car_inflow, input length mismatch");
  }
  int l = start_buf.shape[0];
  int new_shape [2] = { (int) m_link_vec.size(), l};
  auto result = py::array_t<double>(new_shape);
  auto result_buf = result.request();
  double *result_prt = (double *) result_buf.ptr;
  int *start_prt = (int *) start_buf.ptr;
  int *end_prt = (int *) end_buf.ptr;
  for (int t = 0; t < l; ++t){
    if (end_prt[t] < start_prt[t]){
      throw std::runtime_error("Error, Mcdta_Api::get_link_car_inflow, end time smaller than start time");
    }
    if (end_prt[t] > get_cur_loading_interval()){
      throw std::runtime_error("Error, Mcdta_Api::get_link_car_inflow, loaded data not enough");
    }
    for (size_t i = 0; i < m_link_vec.size(); ++i){
      result_prt[i * l + t] = MNM_DTA_GRADIENT::get_link_inflow_car(m_link_vec[i], TFlt(start_prt[t]), TFlt(end_prt[t]))();
      // printf("i %d, t %d, %f\n", i, t, result_prt[i * l + t]);
    }
  }
  return result;
}

py::array_t<double> Mcdta_Api::get_link_truck_inflow(py::array_t<int>start_intervals, py::array_t<int>end_intervals)
{
  auto start_buf = start_intervals.request();
  auto end_buf = end_intervals.request();
  if (start_buf.ndim != 1 || end_buf.ndim != 1){
    throw std::runtime_error("Error, Mcdta_Api::get_link_truck_inflow, input dismension mismatch");
  }
  if (start_buf.shape[0] != end_buf.shape[0]){
    throw std::runtime_error("Error, Mcdta_Api::get_link_truck_inflow, input length mismatch");
  }
  int l = start_buf.shape[0];
  int new_shape [2] = { (int) m_link_vec.size(), l};
  auto result = py::array_t<double>(new_shape);
  auto result_buf = result.request();
  double *result_prt = (double *) result_buf.ptr;
  int *start_prt = (int *) start_buf.ptr;
  int *end_prt = (int *) end_buf.ptr;
  for (int t = 0; t < l; ++t){
    if (end_prt[t] < start_prt[t]){
      throw std::runtime_error("Error, Mcdta_Api::get_link_truck_inflow, end time smaller than start time");
    }
    if (end_prt[t] > get_cur_loading_interval()){
      throw std::runtime_error("Error, Mcdta_Api::get_link_truck_inflow, loaded data not enough");
    }
    for (size_t i = 0; i < m_link_vec.size(); ++i){
      result_prt[i * l + t] = MNM_DTA_GRADIENT::get_link_inflow_truck(m_link_vec[i], TFlt(start_prt[t]), TFlt(end_prt[t]))();
      // printf("i %d, t %d, %f\n", i, t, result_prt[i * l + t]);
    }
  }
  return result;
}

int Mcdta_Api::register_paths(py::array_t<int> paths)
{
  if (m_link_vec.size() > 0){
    printf("Warning, Mcdta_Api::register_paths, path exists\n");
    m_path_vec.clear();
    m_path_set.clear();
  }
  auto paths_buf = paths.request();
  if (paths_buf.ndim != 1){
    throw std::runtime_error("Mcdta_Api::register_paths: Number of dimensions must be one");
  }
  int *paths_ptr = (int *) paths_buf.ptr; 
  TInt _path_ID;
  for (int i = 0; i < paths_buf.shape[0]; i++){
    _path_ID = TInt(paths_ptr[i]);
    // printf("registering path %d, %d\n", _path_ID(), (int)m_ID_path_mapping.size());
    if (m_ID_path_mapping.find(_path_ID) == m_ID_path_mapping.end()){
      throw std::runtime_error("Mcdta_Api::register_paths: No such path");
    }
    else {
      m_path_vec.push_back(m_ID_path_mapping[_path_ID]);
    }
  }
  m_path_set = std::set<MNM_Path*> (m_path_vec.begin(), m_path_vec.end());
  return 0;
}

// py::array_t<double> Mcdta_Api::get_car_link_out_cc(int link_ID)
// {
//   MNM_Dlink_Multiclass *_link = (MNM_Dlink_Multiclass *) m_mcdta -> m_link_factory -> get_link(TInt(link_ID));
//   printf("%d\n", _link -> m_link_ID());
//   if (_link -> m_N_out_car == NULL){
//     throw std::runtime_error("Error, Mcdta_Api::get_car_link_out_cc, cc not installed");
//   }
//   printf("1\n");
//   std::deque<std::pair<TFlt, TFlt>> _record = _link -> m_N_out_car -> m_recorder;
//   int new_shape [2] = { (int) _record.size(), 2}; 
//   auto result = py::array_t<double>(new_shape);
//   printf("2\n");
//   auto result_buf = result.request();
//   double *result_prt = (double *) result_buf.ptr;
//   printf("3\n");
//   for (size_t i=0; i< _record.size(); ++i){
//     result_prt[i * 2 ] = _record[i].first();
//     result_prt[i * 2 + 1 ] =  _record[i].second();
//   }
//   printf("5\n");
//   return result;
// }

py::array_t<double> Mcdta_Api::get_enroute_and_queue_veh_stats_agg()
{
  int _tot_interval = get_cur_loading_interval();
  int new_shape[2] = {_tot_interval, 3};
  auto result = py::array_t<double>(new_shape);
  auto result_buf = result.request();
  double *result_prt = (double *) result_buf.ptr;

  if (m_mcdta -> m_enroute_veh_num.size() != get_cur_loading_interval()){
    throw std::runtime_error("Error, Mcdta_Api::get_enroute_and_queue_veh_stats_agg, enroute vehicle missed for some intervals");
  }
  else if (m_mcdta -> m_queue_veh_num.size() != get_cur_loading_interval()){
    throw std::runtime_error("Error, Mcdta_Api::get_enroute_and_queue_veh_stats_agg, queuing vehicle missed for some intervals");
  }
  else{
    for (size_t i = 0; i < _tot_interval; ++i){
      result_prt[i * 3] =  (m_mcdta -> m_enroute_veh_num[i]())/(m_mcdta -> m_flow_scalar);
      result_prt[i * 3 + 1] =  (m_mcdta -> m_queue_veh_num[i]())/(m_mcdta -> m_flow_scalar);
      result_prt[i * 3 + 2] =  (m_mcdta -> m_enroute_veh_num[i]() - m_mcdta -> m_queue_veh_num[i]())/(m_mcdta -> m_flow_scalar);
    }
  } 
  return result;
}

py::array_t<double> Mcdta_Api::get_queue_veh_each_link(py::array_t<int>useful_links, py::array_t<int>intervals)
{
  auto intervals_buf = intervals.request();
  if (intervals_buf.ndim != 1){
    throw std::runtime_error("Error, Mcdta_Api::get_queue_veh_each_link, input (intervals) dismension mismatch");
  }
  auto links_buf = useful_links.request();
  if (links_buf.ndim != 1){
    throw std::runtime_error("Error, Mcdta_Api::get_queue_veh_each_link, input (useful_links) dismension mismatch");
  }
  int num_intervals = intervals_buf.shape[0];
  int num_links = links_buf.shape[0];
  int new_shape[2] = {num_links, num_intervals};
  auto result = py::array_t<double>(new_shape);
  auto result_buf = result.request();
  double *result_prt = (double *) result_buf.ptr;
  double *intervals_prt = (double *) intervals_buf.ptr;
  double *links_prt = (double *) links_buf.ptr;
    
  for (int t = 0; t < num_intervals; ++t){
    if (intervals_prt[t] > get_cur_loading_interval()){
      throw std::runtime_error("Error, Mcdta_Api::get_queue_veh_each_link, too large interval number");
    }
    for (int i = 0; i < num_links; ++i){
      if (m_mcdta -> m_queue_veh_map.find(links_prt[i]) == m_mcdta -> m_queue_veh_map.end()){
        throw std::runtime_error("Error, Mcdta_Api::get_queue_veh_each_link, can't find link ID");
      }
      result_prt[i * num_intervals + t] = (*(m_mcdta -> m_queue_veh_map[links_prt[i]]))[intervals_prt[t]];
    }
  }
  return result;
}

double Mcdta_Api::get_car_link_out_num(int link_ID, double time)
{
  MNM_Dlink_Multiclass *_link = (MNM_Dlink_Multiclass *) m_mcdta -> m_link_factory -> get_link(TInt(link_ID));
  // printf("%d\n", _link -> m_link_ID());
  if (_link -> m_N_out_car == NULL){
    throw std::runtime_error("Error, Mcdta_Api::get_car_link_out_cc, cc not installed");
  }
  // printf("1\n");
  TFlt result = _link -> m_N_out_car -> get_result(TFlt(time));
  // printf("%lf\n", result());
  return result();
}

double Mcdta_Api::get_truck_link_out_num(int link_ID, double time)
{
  MNM_Dlink_Multiclass *_link = (MNM_Dlink_Multiclass *) m_mcdta -> m_link_factory -> get_link(TInt(link_ID));
  if (_link -> m_N_out_truck == NULL){
    throw std::runtime_error("Error, Mcdta_Api::get_truck_link_out_cc, cc not installed");
  }
  TFlt result = _link -> m_N_out_truck -> get_result(TFlt(time));
  return result();
}


py::array_t<double> Mcdta_Api::get_car_dar_matrix(py::array_t<int>start_intervals, py::array_t<int>end_intervals)
{
  auto start_buf = start_intervals.request();
  auto end_buf = end_intervals.request();
  if (start_buf.ndim != 1 || end_buf.ndim != 1){
    throw std::runtime_error("Error, Mcdta_Api::get_car_dar_matrix, input dismension mismatch");
  }
  if (start_buf.shape[0] != end_buf.shape[0]){
    throw std::runtime_error("Error, Mcdta_Api::get_car_dar_matrix, input length mismatch");
  }
  int l = start_buf.shape[0];
  int *start_prt = (int *) start_buf.ptr;
  int *end_prt = (int *) end_buf.ptr;
  std::vector<dar_record*> _record = std::vector<dar_record*>();
  // for (size_t i = 0; i<m_link_vec.size(); ++i){
  //   m_link_vec[i] -> m_N_in_tree -> print_out();
  // }
  for (int t = 0; t < l; ++t){
    // printf("Current processing time: %d\n", t);
    for (size_t i = 0; i<m_link_vec.size(); ++i){
      if (end_prt[t] < start_prt[t]){
        throw std::runtime_error("Error, Mcdta_Api::get_car_dar_matrix, end time smaller than start time");
      }
      if (end_prt[t] > get_cur_loading_interval()){
        throw std::runtime_error("Error, Mcdta_Api::get_car_dar_matrix, loaded data not enough");
      }
        MNM_DTA_GRADIENT::add_dar_records_car(
                      _record, m_link_vec[i], m_path_set, TFlt(start_prt[t]), TFlt(end_prt[t]));
    }
  }
  // path_ID, assign_time, link_ID, start_int, flow
  int new_shape [2] = { (int) _record.size(), 5}; 
  auto result = py::array_t<double>(new_shape);
  auto result_buf = result.request();
  double *result_prt = (double *) result_buf.ptr;
  dar_record* tmp_record;
  for (size_t i = 0; i < _record.size(); ++i){
    tmp_record = _record[i];
    result_prt[i * 5 + 0] = (double) tmp_record -> path_ID();
    result_prt[i * 5 + 1] = (double) tmp_record -> assign_int();
    result_prt[i * 5 + 2] = (double) tmp_record -> link_ID();
    result_prt[i * 5 + 3] = (double) tmp_record -> link_start_int();
    result_prt[i * 5 + 4] = tmp_record -> flow();
  }
  for (size_t i = 0; i < _record.size(); ++i){
    delete _record[i];
  }
  _record.clear();
  return result;
}

py::array_t<double> Mcdta_Api::get_truck_dar_matrix(py::array_t<int>start_intervals, py::array_t<int>end_intervals)
{
  auto start_buf = start_intervals.request();
  auto end_buf = end_intervals.request();
  if (start_buf.ndim != 1 || end_buf.ndim != 1){
    throw std::runtime_error("Error, Mcdta_Api::get_truck_dar_matrix, input dismension mismatch");
  }
  if (start_buf.shape[0] != end_buf.shape[0]){
    throw std::runtime_error("Error, Mcdta_Api::get_truck_dar_matrix, input length mismatch");
  }
  int l = start_buf.shape[0];
  int *start_prt = (int *) start_buf.ptr;
  int *end_prt = (int *) end_buf.ptr;
  std::vector<dar_record*> _record = std::vector<dar_record*>();
  // for (size_t i = 0; i<m_link_vec.size(); ++i){
  //   m_link_vec[i] -> m_N_in_tree -> print_out();
  // }
  for (int t = 0; t < l; ++t){
    for (size_t i = 0; i<m_link_vec.size(); ++i){
      if (end_prt[t] < start_prt[t]){
        throw std::runtime_error("Error, Mcdta_Api::get_truck_dar_matrix, end time smaller than start time");
      }
      if (end_prt[t] > get_cur_loading_interval()){
        throw std::runtime_error("Error, Mcdta_Api::get_truck_dar_matrix, loaded data not enough");
      }
        MNM_DTA_GRADIENT::add_dar_records_truck(
                      _record, m_link_vec[i], m_path_set, TFlt(start_prt[t]), TFlt(end_prt[t]));
    }
  }
  // path_ID, assign_time, link_ID, start_int, flow
  int new_shape [2] = { (int) _record.size(), 5}; 
  auto result = py::array_t<double>(new_shape);
  auto result_buf = result.request();
  double *result_prt = (double *) result_buf.ptr;
  dar_record* tmp_record;
  for (size_t i = 0; i < _record.size(); ++i){
    tmp_record = _record[i];
    result_prt[i * 5 + 0] = (double) tmp_record -> path_ID();
    result_prt[i * 5 + 1] = (double) tmp_record -> assign_int();
    result_prt[i * 5 + 2] = (double) tmp_record -> link_ID();
    result_prt[i * 5 + 3] = (double) tmp_record -> link_start_int();
    result_prt[i * 5 + 4] = tmp_record -> flow();
  }
  for (size_t i = 0; i < _record.size(); ++i){
    delete _record[i];
  }
  _record.clear();
  return result;
}


PYBIND11_MODULE(MNMAPI, m) {
    m.doc() = R"pbdoc(
        Pybind11 example plugin
        -----------------------

        .. currentmodule:: cmake_example

        .. autosummary::
           :toctree: _generate

           run_dta
    )pbdoc";

    m.def("run_dta", &run_dta, R"pbdoc(
        Run MAC-POSTS dta model

        Some other explanation about the add function.
    )pbdoc");

    // m.def("subtract", [](int i, int j) { return i - j; }, R"pbdoc(
    //     Subtract two numbers

    //     Some other explanation about the subtract function.
    // )pbdoc");
    py::class_<Dta_Api> (m, "dta_api")
            .def(py::init<>())
            .def("initialize", &Dta_Api::initialize)
            .def("run_whole", &Dta_Api::run_whole)
            .def("install_cc", &Dta_Api::install_cc)
            .def("install_cc_tree", &Dta_Api::install_cc_tree)
            .def("get_cur_loading_interval", &Dta_Api::get_cur_loading_interval)
            .def("register_links", &Dta_Api::register_links)
            .def("register_paths", &Dta_Api::register_paths)
            .def("get_link_tt", &Dta_Api::get_link_tt)
            .def("get_link_inflow", &Dta_Api::get_link_inflow)
            .def("get_link_in_cc", &Dta_Api::get_link_in_cc)
            .def("get_link_out_cc", &Dta_Api::get_link_out_cc)
            .def("get_dar_matrix", &Dta_Api::get_dar_matrix);

    py::class_<Mcdta_Api> (m, "mcdta_api")
            .def(py::init<>())
            .def("initialize", &Mcdta_Api::initialize)
            .def("run_whole", &Mcdta_Api::run_whole)
            .def("install_cc", &Mcdta_Api::install_cc)
            .def("install_cc_tree", &Mcdta_Api::install_cc_tree)
            .def("print_emission_stats", &Mcdta_Api::print_emission_stats)
            .def("get_cur_loading_interval", &Mcdta_Api::get_cur_loading_interval)
            .def("register_links", &Mcdta_Api::register_links)
            .def("register_paths", &Mcdta_Api::register_paths)
            .def("get_car_link_tt", &Mcdta_Api::get_car_link_tt)
            .def("get_truck_link_tt", &Mcdta_Api::get_truck_link_tt)
<<<<<<< HEAD
            .def("get_car_link_out_num", &Mcdta_Api::get_car_link_out_num)
            .def("get_truck_link_out_num", &Mcdta_Api::get_truck_link_out_num)
            // .def("get_car_link_out_cc", &Mcdta_Api::get_car_link_out_cc);
=======
            .def("get_car_link_speed", &Mcdta_Api::get_car_link_speed)
            .def("get_truck_link_speed", &Mcdta_Api::get_truck_link_speed)
>>>>>>> 6c30d713d25e85df02499d433850e604a8d26b4c
            .def("get_link_car_inflow", &Mcdta_Api::get_link_car_inflow)
            .def("get_link_truck_inflow", &Mcdta_Api::get_link_truck_inflow)
            .def("get_enroute_and_queue_veh_stats_agg", &Mcdta_Api::get_enroute_and_queue_veh_stats_agg)
            .def("get_queue_veh_each_link", &Mcdta_Api::get_queue_veh_each_link)
            .def("get_car_link_out_num", &Mcdta_Api::get_car_link_out_num)
            .def("get_truck_link_out_num", &Mcdta_Api::get_truck_link_out_num)
            //.def("get_car_link_out_cc", &Mcdta_Api::get_car_link_out_cc)
            .def("get_car_dar_matrix", &Mcdta_Api::get_car_dar_matrix)
            .def("get_truck_dar_matrix", &Mcdta_Api::get_truck_dar_matrix);
#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}
