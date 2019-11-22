#ifndef SIGNED_INCIDENCE_MATRIX_DENSE_H
#define SIGNED_INCIDENCE_MATRIX_DENSE_H
#include <Eigen/Core>
// Construct the sparse incidence matrix for a given edge network.
//
// Inputs: 
//   n  number of vertices (#V)
//   E  #E by 2 list of edge indices into rows of V
// Outputs:
//   A  #E by n signed incidence matrix
void signed_incidence_matrix_dense(
  const int n,
  const Eigen::MatrixXi & E,
  Eigen::MatrixXd & A);
#endif
