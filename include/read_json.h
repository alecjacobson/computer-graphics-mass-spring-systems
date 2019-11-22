#ifndef READ_JSON_H
#define READ_JSON_H
#include <string>
#include <Eigen/Core>
// Inputs:
//   filename  path to .json file
// Outputs:
//   V  #V by 3 list of vertex positions
//   F  #F by 3 list of triangle indices into rows of V
//   E  #E by 2 list of edge/spring indices into rows of V
//   k  spring stiffness
//   m  #V list of masses 
//   b  #b list of fixed indices into rows of V
//   bc  #b by 3 list of fixed positions
//   VT  #VT by 3 list of texture coordinates
//   FT  #FT by 3 list of texture faces
//   R,G,B  #w by #h by 1 list of texture color values
//
inline bool read_json(
  const std::string & filename,
  Eigen::MatrixXd & V,
  Eigen::MatrixXi & F,
  Eigen::MatrixXi & E,
  double & k,
  Eigen::VectorXd & m,
  Eigen::VectorXi & b,
  Eigen::MatrixXd & bc,
  Eigen::MatrixXd & VT,
  Eigen::MatrixXi & FT,
  Eigen::Matrix<unsigned char,Eigen::Dynamic,Eigen::Dynamic> & R,
  Eigen::Matrix<unsigned char,Eigen::Dynamic,Eigen::Dynamic> & G,
  Eigen::Matrix<unsigned char,Eigen::Dynamic,Eigen::Dynamic> & B
    );

// Implementation
#include <igl/readOBJ.h>
#include <igl/dirname.h>
#include <igl/massmatrix.h>
#include <igl/edges.h>
#include <igl/list_to_matrix.h>
#include <igl/polygon_mesh_to_triangle_mesh.h>
#include <igl/png/readPNG.h>
#include <json.hpp>
inline bool read_json(
  const std::string & filename,
  Eigen::MatrixXd & V,
  Eigen::MatrixXi & F,
  Eigen::MatrixXi & E,
  double & k,
  Eigen::VectorXd & m,
  Eigen::VectorXi & b,
  Eigen::MatrixXd & bc,
  Eigen::MatrixXd & VT,
  Eigen::MatrixXi & FT,
  Eigen::Matrix<unsigned char,Eigen::Dynamic,Eigen::Dynamic> & R,
  Eigen::Matrix<unsigned char,Eigen::Dynamic,Eigen::Dynamic> & G,
  Eigen::Matrix<unsigned char,Eigen::Dynamic,Eigen::Dynamic> & B
    )
{
  // Heavily borrowing from
  // https://github.com/yig/graphics101-raycasting/blob/master/parser.cpp
  using json = nlohmann::json;

  std::ifstream infile( filename );
  if( !infile ) return false;
  json j;
  infile >> j;

  // Load 3d model / spring network
#if defined(WIN32) || defined(_WIN32)
#define PATH_SEPARATOR std::string("\\")
#else
#define PATH_SEPARATOR std::string("/")
#endif
  const std::string obj_filename = j["obj"];
  {
    const std::string full_obj_filename = 
      igl::dirname(filename)+ PATH_SEPARATOR + obj_filename;
    Eigen::MatrixXd N;
    Eigen::MatrixXi FN;
    if(!igl::readOBJ(full_obj_filename, V,VT,N,F,FT,FN))
    {
      assert(false && "Failed to read obj");
      std::cerr<<"Error: Failed to read "<<full_obj_filename<<std::endl;
      return false;
    }
    std::cout<<"VT: "<<VT.rows()<<","<<VT.cols()<<std::endl;
    std::cout<<"FT: "<<FT.rows()<<","<<FT.cols()<<std::endl;
  }
  if(F.cols()>2)
  {
    igl::edges(F,E);
    igl::polygon_mesh_to_triangle_mesh(Eigen::MatrixXi(F),F);
    igl::polygon_mesh_to_triangle_mesh(Eigen::MatrixXi(FT),FT);
  }else
  {
    E = F;
    F.resize(0,3);
  }
  k = j["k"];

  // texture?
  if(j.count("png") )
  {
    const std::string png_filename = j["png"];
      const std::string full_png_filename = 
        igl::dirname(filename)+ PATH_SEPARATOR + png_filename;
    Eigen::Matrix<unsigned char,Eigen::Dynamic,Eigen::Dynamic> A;
    igl::png::readPNG(full_png_filename,R,G,B,A);
  }

  // mass
  if(j.count("m") )
  {
    m = Eigen::VectorXd::Constant(V.rows(),1,j["m"]);
  }else
  {
    assert(F.rows() > 0 && "if not m given then should be triangle mesh");
    Eigen::SparseMatrix<double> M;
    igl::massmatrix(V,F,igl::MASSMATRIX_TYPE_DEFAULT,M);
    m = M.diagonal();
  }

  std::vector<int> b_vec = j["b"];
  igl::list_to_matrix(b_vec,b);
  b.array() -= 1;
 

  if(j.count("bc"))
  {
    assert(false && "not supported");
    return false;
  }else
  {
    // use rest positions
    bc.resize(b.rows(),V.cols());
    for(int i = 0;i<b.rows();i++) { bc.row(i) = V.row(b(i)); }
  }

  // NOTE: if we add U0 to this, we should update bc's default value to use U0
  // instead of V
  return true;
}

#endif 
