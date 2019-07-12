#ifndef _WIDGET_H
#define _WIDGET_H

#include "def.h"
#include <GL/glew.h>
#include <QGLWidget>
#include <OpenGL/glu.h>
#include <QMouseEvent>
#include "trackball.h"
#include "Data.h"

class CGLWidget : public QGLWidget {
  Q_OBJECT

public:
  CGLWidget(const QGLFormat& fmt = QGLFormat::defaultFormat());
  ~CGLWidget();



public:

  Data *d;
  


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

private:
  CGLTrackball trackball;
  QMatrix4x4 projmatrix, mvmatrix; 
  
  const float fovy, znear, zfar; 
  const QVector3D eye, center, up;

protected:
  
  
  GLUquadricObj *sphere = NULL;
  GLuint texEarth = 0;






};

#endif
