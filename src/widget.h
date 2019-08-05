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

  void set_lic_size(double lic_size, int lic_nmax, float step, double rate);
  void generate_seeds(double px, double py);
  void generate_lic();
  void keyPressEvent(QKeyEvent *ev);

  // streamline related
  void compute_interavtive_streamline(double sx, double sy, int direction_factor); // direction

  void init(std::string view_filename);

//  inline void transformPoint(GLdouble out[4], const GLdouble m[16], const GLdouble in[4]);
//  inline GLint project(GLdouble objx, GLdouble objy, GLdouble objz,
//      const GLdouble model[16], const GLdouble proj[16],
//      const GLint viewport[4],
//      GLdouble * winx, GLdouble * winy, GLdouble * winz);
//  void renderText(D3DVECTOR &textPosWorld, QString text);

protected:
  void initializeGL();
  void resizeGL(int w, int h);
  void paintGL();
  
  void mousePressEvent(QMouseEvent*); 
  void mouseMoveEvent(QMouseEvent*);
  // void keyPressEvent(QKeyEvent*); 
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
