#include <pybind11/pybind11.h>
#include "dta_api.h"

#include "dta_gradient_utls.h"

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

int Dta_Api::run_whole()
{

  m_dta -> loading(false);
  return 0;
}

int Dta_Api::register_links(py::array_t<int> links)
{
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
  return 0;
}

py::array_t<double> Dta_Api::get_link_inflow()
{
  auto result = py::array_t<double>(m_link_vec.size() * 5);
  auto result_buf = result.request();
  double *result_prt = (double *) result_buf.ptr;
  for (int t = 0; t < 5; ++t){
    for (size_t i = 0; i<m_link_vec.size(); ++i){
      result_prt[i + 5 * t] = MNM_DTA_GRADIENT::get_link_inflow(m_link_vec[i], TFlt(t), TFlt(t+1));
    }
  }
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
            .def("register_links", &Dta_Api::register_links)
            .def("get_link_inflow", &Dta_Api::get_link_inflow);

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}
