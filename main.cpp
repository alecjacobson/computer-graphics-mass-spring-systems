#ifdef SPARSE
#include "fast_mass_springs_precomputation_sparse.h"
#include "fast_mass_springs_step_sparse.h"
#else
#include "fast_mass_springs_precomputation_dense.h"
#include "fast_mass_springs_step_dense.h"
#endif
#include "read_json.h"
#include <igl/get_seconds.h>
#include <igl/opengl/glfw/Viewer.h>
#include <Eigen/Sparse>


int main(int argc, char * argv[])
{
  Eigen::MatrixXd V,VT;
  Eigen::MatrixXi F,FT,E;
  double k;
  Eigen::VectorXd m;
  Eigen::VectorXi b;
  Eigen::MatrixXd bc;
  Eigen::Matrix<unsigned char,Eigen::Dynamic,Eigen::Dynamic> R,G,B;
  if(!read_json(argv[1],V,F,E,k,m,b,bc,VT,FT,R,G,B))
  {
    std::cerr<<"Error: Failed to load "<<argv[1]<<std::endl;
    return EXIT_FAILURE;
  }

  const double delta_t = 1.0/30.0;
  Eigen::VectorXd r;
#ifdef SPARSE
  Eigen::SparseMatrix<double> M,A,C;
  Eigen::SimplicialLLT <Eigen::SparseMatrix<double > > prefactorization;
  if(!fast_mass_springs_precomputation_sparse(
    V,E,k,m,b,delta_t,r,M,A,C,prefactorization))
  {
    std::cerr<<"precomputation failed."<<std::endl;
  }
#else
  Eigen::MatrixXd M,A,C;
  Eigen::LLT<Eigen::MatrixXd> prefactorization;
  if(!fast_mass_springs_precomputation_dense(
    V,E,k,m,b,delta_t,r,M,A,C,prefactorization))
  {
    std::cerr<<"precomputation failed."<<std::endl;
  }
#endif

  // Point colors
  Eigen::MatrixXd CM = Eigen::MatrixXd::Zero(V.rows(),V.cols());
  const Eigen::RowVector3d silver(0.8,0.8,0.8);
  for(int i = 0;i<b.rows();i++) { CM.row(b(i)) = silver; }

  int graph_id,mesh_id;
  igl::opengl::glfw::Viewer viewer;
  std::cout<<R"(
  [space]  (turn off animation and) step forward one time step
  A,a  toggle animation 
  L,l  show/hide edges (springs) and verties (point masses)
  R,r  reset to rest configuration
  W,w  toggle horizontal wind
)";

  // Initial conditions
  const Eigen::MatrixXd U0 = V;
  const Eigen::MatrixXd Udot0 = Eigen::MatrixXd::Zero(V.rows(),V.cols());

  Eigen::MatrixXd U = U0;
  Eigen::MatrixXd Uprev = U0 - delta_t*Udot0;

  const auto update = [&]()
  {
    viewer.data_list[mesh_id].set_vertices(U);
    viewer.data_list[mesh_id].compute_normals();
    viewer.data_list[graph_id].set_points(U,CM);
    viewer.data_list[graph_id].set_edges(U,E,Eigen::RowVector3d(0,0,0));
  };
  bool first = true;
  bool wind = false;
  const double bbd = (V.colwise().maxCoeff()-V.colwise().minCoeff()).norm();
  const auto step = [&]()
  {
    Eigen::MatrixXd fext = Eigen::MatrixXd::Zero(V.rows(),V.cols());
    // gravity
    fext.col(1).setConstant(-9.8);
    if(wind)
    {
      for(int i = 0;i<V.rows();i++)
      {
        const double x = U(i,0)/bbd;
        const double phi = x+igl::get_seconds();
        fext(i,0) = 3.0+3.0*0.5*(1.0+cos(5.*phi));
        fext(i,2) = 2.0*0.5*(sin(phi));
        fext(i,1) += 1.0*(1.0+cos(0.3+phi));
      }
    }
    const Eigen::MatrixXd U_stash = U;
#ifdef SPARSE
    fast_mass_springs_step_sparse(
        V,E,k,b,delta_t,fext,
        r,M,A,C,prefactorization,
        Uprev,U_stash,
        U);
#else
    fast_mass_springs_step_dense(
        V,E,k,b,delta_t,fext,
        r,M,A,C,prefactorization,
        Uprev,U_stash,
        U);
#endif
    Uprev = U_stash;
    update();
  };
  viewer.callback_pre_draw = [&](igl::opengl::glfw::Viewer &)->bool
  {
    if(viewer.core().is_animating)
    {
      if(!first)
      {
        step();
      }
      first = false;
    }
    return false;
  };

  viewer.callback_key_pressed = [&](
    igl::opengl::glfw::Viewer & v,
    unsigned char key,
    int /*modifier*/
    )->bool
  {
    switch(key)
    {
      default:
        return false;
      case ' ':
        viewer.core().is_animating = false;
        step();
        return true;
      case 'L':
      case 'l':
        viewer.data_list[0].is_visible ^= 1;
        return true;
      case 'W':
      case 'w':
        wind ^= 1;
        return true;
      case 'R':
      case 'r':
        first = true;
        U = U0;
        Uprev = U0 - delta_t*Udot0;
        update();
        return true;
    }
  };

  graph_id = viewer.selected_data_index;
  viewer.data_list[graph_id].set_points(V,CM);
  viewer.data_list[graph_id].set_edges(V,E,Eigen::RowVector3d(0,0,0));
  viewer.append_mesh();
  mesh_id = viewer.selected_data_index;

  viewer.data_list[mesh_id].set_mesh(V,F);
  viewer.data_list[mesh_id].set_face_based(true);
  if(R.size() > 0 && VT.rows()>0 && FT.rows() >0)
  {
    viewer.data_list[mesh_id].set_uv(VT,FT);
    viewer.data_list[mesh_id].set_texture(R,G,B);
    viewer.data_list[mesh_id].set_colors(Eigen::RowVector3d(1,1,1));
    viewer.data_list[mesh_id].show_texture = true;
    viewer.data_list[mesh_id].set_face_based(false);
  }
  viewer.data_list[mesh_id].show_lines = false;
  viewer.core().is_animating = false;

  viewer.launch_init(true,false);
  viewer.data().meshgl.init();
  // Ooof. All this to turn on double-sided lighting...
  igl::opengl::destroy_shader_program(viewer.data().meshgl.shader_mesh);
  {
    std::string mesh_vertex_shader_string =
R"(#version 150
  uniform mat4 view;
  uniform mat4 proj;
  uniform mat4 normal_matrix;
  in vec3 position;
  in vec3 normal;
  out vec3 position_eye;
  out vec3 normal_eye;
  in vec4 Ka;
  in vec4 Kd;
  in vec4 Ks;
  in vec2 texcoord;
  out vec2 texcoordi;
  out vec4 Kai;
  out vec4 Kdi;
  out vec4 Ksi;

  void main()
  {
    position_eye = vec3 (view * vec4 (position, 1.0));
    normal_eye = vec3 (normal_matrix * vec4 (normal, 0.0));
    normal_eye = normalize(normal_eye);
    gl_Position = proj * vec4 (position_eye, 1.0); //proj * view * vec4(position, 1.0);"
    Kai = Ka;
    Kdi = Kd;
    Ksi = Ks;
    texcoordi = texcoord;
  }
)";

    std::string mesh_fragment_shader_string =
R"(#version 150
  uniform mat4 view;
  uniform mat4 proj;
  uniform vec4 fixed_color;
  in vec3 position_eye;
  in vec3 normal_eye;
  uniform vec3 light_position_eye;
  vec3 Ls = vec3 (1, 1, 1);
  vec3 Ld = vec3 (1, 1, 1);
  vec3 La = vec3 (1, 1, 1);
  in vec4 Ksi;
  in vec4 Kdi;
  in vec4 Kai;
  in vec2 texcoordi;
  uniform sampler2D tex;
  uniform float specular_exponent;
  uniform float lighting_factor;
  uniform float texture_factor;
  out vec4 outColor;
  void main()
  {
    vec3 Ia = La * vec3(Kai);    // ambient intensity

    vec3 vector_to_light_eye = light_position_eye - position_eye;
    vec3 direction_to_light_eye = normalize (vector_to_light_eye);
    float dot_prod = dot (direction_to_light_eye, normalize(normal_eye));
    float clamped_dot_prod = abs(dot_prod);
    vec3 Id = Ld * vec3(Kdi) * clamped_dot_prod;    // Diffuse intensity

    vec3 reflection_eye = reflect (-direction_to_light_eye, normalize(normal_eye));
    vec3 surface_to_viewer_eye = normalize (-position_eye);
    float dot_prod_specular = dot (reflection_eye, surface_to_viewer_eye);
    dot_prod_specular = float(abs(dot_prod)==dot_prod) * max (dot_prod_specular, 0.0);
    float specular_factor = pow (dot_prod_specular, specular_exponent);
    vec3 Kfi = 0.5*vec3(Ksi);
    vec3 Lf = Ls;
    float fresnel_exponent = 2*specular_exponent;
    float fresnel_factor = 0;
    {
      float NE = max( 0., dot( normalize(normal_eye), surface_to_viewer_eye));
      fresnel_factor = pow (max(sqrt(1. - NE*NE),0.0), fresnel_exponent);
    }
    vec3 Is = Ls * vec3(Ksi) * specular_factor;    // specular intensity
    vec3 If = Lf * vec3(Kfi) * fresnel_factor;     // fresnel intensity
    vec4 color = vec4(lighting_factor * (If + Is + Id) + Ia + (1.0-lighting_factor) * vec3(Kdi),(Kai.a+Ksi.a+Kdi.a)/3);
    outColor = mix(vec4(1,1,1,1), texture(tex, texcoordi), texture_factor) * color;
    if (fixed_color != vec4(0.0)) outColor = fixed_color;
  }
)";

    igl::opengl::create_shader_program(
      mesh_vertex_shader_string,
      mesh_fragment_shader_string,
      {},
      viewer.data().meshgl.shader_mesh);
  }
  viewer.launch_rendering(true);
  viewer.launch_shut();
}
