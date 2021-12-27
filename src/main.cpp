#include <QApplication>
#include "widget.h"
#include "advect.h"
#include "Data.h"
#include <string>
#include "opts2.h"
#include <vector>

#include <mutex>
#include <ftk/numeric/print.hh>
#include <ftk/numeric/cross_product.hh>
#include <ftk/numeric/vector_norm.hh>
#include <ftk/numeric/linear_interpolation.hh>
#include <ftk/numeric/bilinear_interpolation.hh>
#include <ftk/numeric/inverse_linear_interpolation_solver.hh>
#include <ftk/numeric/inverse_bilinear_interpolation_solver.hh>
#include <ftk/numeric/gradient.hh>
#include <ftk/algorithms/cca.hh>
#include <ftk/geometry/cc2curves.hh>
#include <ftk/geometry/curve2tube.hh>
#include <hypermesh/ndarray.hh>
#include <hypermesh/regular_simplex_mesh.hh>
#include <unordered_map>
#include <queue>
#include <fstream>

using namespace std;

hypermesh::ndarray<double> grad;
hypermesh::regular_simplex_mesh m(2); // the 2D spatial mesh
std::mutex mut;
unordered_map<int, critical_point_t> critical_points, saddles;
vector<vector<double>> separatrices;

void check_simplex(const hypermesh::regular_simplex_mesh_element& s)
{
  if (!s.valid()) return; // check if the 3-simplex is valid

  const auto &vertices = s.vertices();
  double X[3][2], g[3][2];

  for (int i = 0; i < 3; i ++) {
    for (int j = 0; j < 2; j ++)
      g[i][j] = grad(j, vertices[i][0], vertices[i][1]);
    for (int j = 0; j < 2; j ++)
      X[i][j] = vertices[i][j];
  }
  // check intersection
  double mu[3];
  bool succ = ftk::inverse_lerp_s2v2(g, mu);
  if (!succ){
    return;
  }

  double x[2];
  ftk::lerp_s2v2(X, mu, x);
  // fprintf(stdout, "simplex_id=%d, corner=%d, %d, type=%d, mu=%f, %f, %f, x=%f, %f\n", s.to_integer(), s.corner[0], s.corner[1], s.type, mu[0], mu[1], mu[2], x[0], x[1]);
  double J[2][2]; // jacobian
  ftk::gradient_2dsimplex2_2(X, g, J);  
  int cp_type = 0;
  std::complex<double> eig[2];
  double delta = ftk::solve_eigenvalues2x2(J, eig);
  double eig_vec[2][2] = {0};
  if (delta >= 0) { // two real roots
    if (eig[0].real() * eig[1].real() < 0) {
      cp_type = 1;
      double eig_r[2];
      eig_r[0] = eig[0].real(), eig_r[1] = eig[1].real();
	  ftk::solve_eigenvectors2x2(J, 2, eig_r, eig_vec);
    }
  }
  critical_point_t cp(x, eig_vec, cp_type);
  {
    std::cout << cp.x[0] << " " << cp.x[1] << ": " << eig[0] << " " << eig[1] << std::endl;
    std::lock_guard<std::mutex> guard(mut);
    if(cp_type) saddles.insert(std::make_pair(s.to_integer(), cp));
    else critical_points.insert(std::make_pair(s.to_integer(), cp));
  }
}

void init_grad(const Data& data){
  grad.reshape({2, data.nv, data.nu});
  for (int i = 0; i < data.nu; i ++) {
    for (int j = 0; j < data.nv; j ++) {
      grad(0, j, i) = data.u[i*data.nu + j];
      grad(1, j, i) = data.v[i*data.nu + j];
    }
  }
}

void extract_critical_points_and_saddles(const Data& data)
{
  fprintf(stderr, "extracting critical points...\n");
  init_grad(data);
  m.set_lb_ub({1, 1}, {static_cast<int>(data.nv-2), static_cast<int>(data.nu-2)}); // set the lower and upper bounds of the mesh
  m.element_for(2, check_simplex, 1); // iterate over all 3-simplices
}

int main(int argc, char **argv){


	// read inputs	
	using namespace opts;

	Options ops(argc, argv);
	string ip_filename;
	ops >> PosOption(ip_filename);
	size_t offset = 0;

	double eps = 1e-3;
	double stepsize = 1e-3;
	size_t maxstep = 20000;
	size_t len = 200;
	ops >> PosOption(eps) >> PosOption(stepsize) >> PosOption(maxstep) >> PosOption(len);
	// initialize 
	Data dat;
	dat.nv = dat.nu = len;
	dat.read_nek5000_nc(ip_filename, offset);

	extract_critical_points_and_saddles(dat);
	cout << critical_points.size() << endl;
	cout << saddles.size() << endl;
	// set epsilon for eigen direction

	QApplication app(argc, argv); 

    QGLFormat fmt;
	fmt.setProfile( QGLFormat::CoreProfile ); // Requires >=Qt-4.8.0
	fmt.setSampleBuffers( true );
	fmt.setVersion(3,1);
	qDebug() << "OpenGL Versions Supported: " << QGLFormat::openGLVersionFlags();

    CGLWidget *widget = new CGLWidget(fmt);
    widget->resize(400, 400);
    // set stepsize
    widget->step = stepsize;
    dat.nmax = maxstep;

	widget->d = &dat;
	widget->init();
	for(const auto& iter:saddles){
		auto cp = iter.second;
		separatrices.push_back(widget->compute_interavtive_streamline(cp.x[0] + cp.eig_vec[0][0] * eps, cp.x[1] + cp.eig_vec[0][1] * eps, 1));
		separatrices.push_back(widget->compute_interavtive_streamline(cp.x[0] - cp.eig_vec[0][0] * eps, cp.x[1] - cp.eig_vec[0][1] * eps, -1));
		separatrices.push_back(widget->compute_interavtive_streamline(cp.x[0] + cp.eig_vec[1][0] * eps, cp.x[1] + cp.eig_vec[1][1] * eps, 1));
		separatrices.push_back(widget->compute_interavtive_streamline(cp.x[0] - cp.eig_vec[1][0] * eps, cp.x[1] - cp.eig_vec[1][1] * eps, -1));
		separatrices.push_back(widget->compute_interavtive_streamline(cp.x[0] + cp.eig_vec[0][0] * eps, cp.x[1] + cp.eig_vec[0][1] * eps, -1));
		separatrices.push_back(widget->compute_interavtive_streamline(cp.x[0] - cp.eig_vec[0][0] * eps, cp.x[1] - cp.eig_vec[0][1] * eps, 1));
		separatrices.push_back(widget->compute_interavtive_streamline(cp.x[0] + cp.eig_vec[1][0] * eps, cp.x[1] + cp.eig_vec[1][1] * eps, -1));
		separatrices.push_back(widget->compute_interavtive_streamline(cp.x[0] - cp.eig_vec[1][0] * eps, cp.x[1] - cp.eig_vec[1][1] * eps, 1));
		std::cout << cp.x[0] << " " << cp.x[1] << ", eigen vec = " << cp.eig_vec[0][0] << " " << cp.eig_vec[0][1] << ", " << cp.eig_vec[1][0] << " " << cp.eig_vec[1][1] << std::endl;
	}
	// for(const auto& s:separatrices){
	// 	cout << s.size() << endl;
	// }
	widget->show();
	app.exec();


	return 0;
}