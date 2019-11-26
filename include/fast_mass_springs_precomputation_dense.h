#ifndef FAST_MASS_SPRINGS_PRECOMPUTATION_DENSE_H
#include <Eigen/Core>
// Precompute matrices and factorizations necessary for the "Fast Simulation of
// Mass-Spring Systems" method.
//
// Inputs: 
//   V  #V by 3 list of vertex positions
//   E  #E by 2 list of edge indices into rows of V
//   k  spring stiffness
//   m  #V list of masses 
//   b  #b list of "pinned"/fixed vertices as indices into rows of V
//   delta_t  time step in seconds
// Outputs:
//   r  #E list of edge lengths
//   M  #V by #V mass matrix
//   A  #E by #V signed incidence matrix
//   C  #b by #V selection matrix
//   prefactorization  LLT prefactorization of energy's quadratic matrix
bool fast_mass_springs_precomputation_dense(
  const Eigen::MatrixXd & V,
  const Eigen::MatrixXi & E,
  const double k,
  const Eigen::VectorXd & m,
  const Eigen::VectorXi & b,
  const double delta_t,
  Eigen::VectorXd & r,
  Eigen::MatrixXd & M,
  Eigen::MatrixXd & A,
  Eigen::MatrixXd & C,
  Eigen::LLT<Eigen::MatrixXd> & prefactorization );
#endif

