#ifndef FAST_MASS_SPRINGS_PRECOMPUTATION_SPARSE_H
#define FAST_MASS_SPRINGS_PRECOMPUTATION_SPARSE_H
#include <Eigen/Core>
#include <Eigen/Sparse>
// Same as fast_mass_springs_precomputation_dense.h but with sparse matrices.
bool fast_mass_springs_precomputation_sparse(
  const Eigen::MatrixXd & V,
  const Eigen::MatrixXi & E,
  const double k,
  const Eigen::VectorXd & m,
  const Eigen::VectorXi & b,
  const double delta_t,
  Eigen::VectorXd & r,
  Eigen::SparseMatrix<double> & M,
  Eigen::SparseMatrix<double> & A,
  Eigen::SparseMatrix<double> & C,
  Eigen::SimplicialLLT<Eigen::SparseMatrix<double> > & prefactorization );
#endif

