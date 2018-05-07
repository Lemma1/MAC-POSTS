#ifndef DTA_API_H
#define DTA_API_H

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include "Snap.h"
#include "dta.h"

#include <set>

namespace py = pybind11;

int run_dta(std::string folder);

class Dta_Api
{
public:
  Dta_Api();
  ~Dta_Api();
  int initialize(std::string folder);
  int install_cc();
  int install_cc_tree();
  int run_once();
  int run_whole();
  int register_links(py::array_t<int> links);
  int register_paths(py::array_t<int> paths);
  int get_cur_loading_interval();
  py::array_t<double> get_link_inflow(py::array_t<int>start_intervals, 
                                        py::array_t<int>end_intervals);
  py::array_t<double> get_link_tt(py::array_t<int>start_intervals);
  py::array_t<double> get_link_in_cc(int link_ID);
  py::array_t<double> get_link_out_cc(int link_ID);
  py::array_t<double> get_dar_matrix(py::array_t<int>link_start_intervals, py::array_t<int>link_end_intervals);
  MNM_Dta *m_dta;
  std::vector<MNM_Dlink*> m_link_vec;
  std::vector<MNM_Path*> m_path_vec;
  std::set<MNM_Path*> m_path_set; 
  std::unordered_map<TInt, MNM_Path*> m_ID_path_mapping;
};


#endif