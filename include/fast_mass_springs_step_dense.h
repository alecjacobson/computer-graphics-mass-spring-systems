#ifndef FAST_MASS_SPRINGS_STEP_DENSE_H
#define FAST_MASS_SPRINGS_STEP_DENSE_H
#include <Eigen/Dense>
// Conduct a single step of the "Fast Simulation of Mass-Spring Systems" method.
//
// Inputs: 
//   V  #V by 3 list of **rest** vertex positions
//   E  #E by 2 list of edge indices into rows of V
//   k  spring stiffness
//   b  #b list of indices of fixed vertices as indices into rows of V
//   delta_t  time step in seconds
//   fext  #V by 3 list of external forces
//   r  #E list of edge lengths
//   M  #V by #V mass matrix
//   A  #E by #V signed incidence matrix
//   C  #b by #V selection matrix
//   prefactorization  LLT prefactorization of energy's quadratic matrix
//   Uprev  #V by 3 list of previous vertex positions (at time t-∆t)
//   Ucur  #V by 3 list of current vertex positions (at time t)
// Outputs:
//   Unext #V by 3 list of next vertex positions (at time t+∆t)
void fast_mass_springs_step_dense(
  const Eigen::MatrixXd & V,
  const Eigen::MatrixXi & E,
  const double k,
  const Eigen::VectorXi & b,
  const double delta_t,
  const Eigen::MatrixXd & fext,
  const Eigen::VectorXd & r,
  const Eigen::MatrixXd & M,
  const Eigen::MatrixXd & A,
  const Eigen::MatrixXd & C,
  const Eigen::LLT<Eigen::MatrixXd> & prefactorization,
  const Eigen::MatrixXd & Uprev,
  const Eigen::MatrixXd & Ucur,
  Eigen::MatrixXd & Unext);
#endif 
