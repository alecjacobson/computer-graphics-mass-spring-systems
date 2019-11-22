#ifndef SIGNED_INCIDENCE_MATRIX_SPARSE_H
#define SIGNED_INCIDENCE_MATRIX_SPARSE_H
#include <Eigen/Core>
#include <Eigen/Sparse>
// Same as signed_incidence_matrix_dense but with sparse matrices.
void signed_incidence_matrix_sparse(
  const int n,
  const Eigen::MatrixXi & E,
  Eigen::SparseMatrix<double>  & A);
#endif
