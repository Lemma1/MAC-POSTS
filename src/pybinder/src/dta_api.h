#ifndef DTA_API_H
#define DTA_API_H

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include "Snap.h"
#include "dta.h"

namespace py = pybind11;

int run_dta(std::string folder);

class Dta_Api
{
public:
  Dta_Api();
  ~Dta_Api();
  int initialize(std::string);
  int run_once();
  int run_whole();
  int register_links(py::array_t<int> links);
  int register_paths(py::array_t<int> paths);
  py::array_t<double> get_link_inflow(); 
  MNM_Dta *m_dta;
};


#endif