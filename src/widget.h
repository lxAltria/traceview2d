#ifndef _WIDGET_H
#define _WIDGET_H

#include "def.h"
#include <GL/glew.h>
#include <QGLWidget>
#include <OpenGL/glu.h>
#include <QMouseEvent>
#include "trackball.h"
#include "Data.h"
#include <QVector2D>
#include <unordered_map>

typedef struct critical_point_t{
  double x[2];
  double eig_vec[2][2];
  int type;
  critical_point_t(double* x_, double eig_v[2][2], int t_){
    x[0] = x_[0];
    x[1] = x_[1];
    eig_vec[0][0] = eig_v[0][0];
    eig_vec[0][1] = eig_v[0][1];
    eig_vec[1][0] = eig_v[1][0];
    eig_vec[1][1] = eig_v[1][1];
    type = t_;
  }
  critical_point_t(){}
}critical_point_t;
extern std::vector<std::vector<double>> separatrices;
extern std::unordered_map<int, critical_point_t> critical_points, saddles;

class QGLShaderProgram;

class CGLWidget : public QGLWidget {
  Q_OBJECT

public:
  CGLWidget(const QGLFormat& fmt = QGLFormat::defaultFormat());
  ~CGLWidget();



public:

  Data *d;

  // LIC related
  double lic_size; // half of side of lic image
  int lsize; // int version of side of lic image
  int lic_nmax; // max advection steps on each side for lic 
  double step; // advection step size
  double scx, scy; // starting corner x and y
  double lic_res; // resolution of lic image
  double rate; // determines the resolution of lic
  bool licEnabled = false;
  double vvals[6]; // scene values from file
  float licScale = 1.0; // store the scale at which latest lic is generated 

  std::vector<double> lic_x;
  std::vector<double> lic_y;
  std::vector<double> lic_vals;
  std::vector<double> noise;

  // streamline related
  std::vector<double> compute_interavtive_streamline(double sx, double sy, int direction_factor); // direction

  void init();

protected:
  void initializeGL();
  void resizeGL(int w, int h);
  void paintGL();
  
  void wheelEvent(QWheelEvent*); 

  void load_texture();


private:
  CGLTrackball trackball;
  QMatrix4x4 projmatrix, mvmatrix; 
  
  const float fovy, znear, zfar; 
  const QVector3D eye, center, up;

  float seed_offset_x, seed_offset_y;

  GLuint texture;
  QVector<QVector3D> vertices;
  QVector<QVector2D> texCoords;

  QGLShaderProgram *program;

  float value=1;
  float xval, yval;
  float xoffset = 0, yoffset = 0;
protected:
  
  
  GLUquadricObj *sphere = NULL;
  GLuint texEarth = 0;

  GLuint tex;







};

#endif
