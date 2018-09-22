#ifndef TYPECAST_GROUND_H
#define TYPECAST_GROUND_H

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/eigen.h>

namespace py = pybind11;

using DenseMatrixR = Eigen::Matrix<double, Eigen::Dynamic, 
                        Eigen::Dynamic, Eigen::RowMajor>;
using SparseMatrixR = Eigen::SparseMatrix<double, Eigen::RowMajor>;


class Test_Types
{
public:
  Test_Types();
  ~Test_Types();
  py::list get_list();
  DenseMatrixR get_matrix();
  SparseMatrixR get_sparse_matrix();
  SparseMatrixR get_sparse_matrix2(int num);
};

#endif
