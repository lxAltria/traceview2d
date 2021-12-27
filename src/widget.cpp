#include "widget.h"
#include <OpenGL/glu.h>
#include <cfloat>
#include <netcdf.h>

#include <math.h>
#include <iostream>

#include <cmath>
#include <stdlib.h> 
#include <algorithm>

#include "advect.h"

#include <Eigen/Dense>

#include <QGLShaderProgram>
#include <QPixmap>

using namespace Eigen;

#include <fstream>
#include <iostream>
#include <ctime>


CGLWidget::CGLWidget(const QGLFormat& fmt) : 
QGLWidget(fmt),
fovy(30.f), znear(0.1f), zfar(10.f),
eye(0, 0, 2.5), center(0, 0, 0), up(0, 1, 0)
{
}

CGLWidget::~CGLWidget()
{
}

void CGLWidget::init(){

     // reset trace xy if view filename exists
    // set trackball rotation, trackball scale, d->px, d->py, seed_offset_x, seed_offset_y

    trackball.init();

}

void CGLWidget::initializeGL()
{
#ifndef __APPLE__
    glewInit();
#endif
    // trackball.init();

    sphere = gluNewQuadric();
    gluQuadricDrawStyle(sphere, GLU_FILL);
    gluQuadricTexture(sphere, TRUE);
    gluQuadricNormals(sphere, GLU_SMOOTH);
    
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1, 1);
    
    glEnable(GL_DEPTH_TEST);

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);


    seed_offset_x = d->px/d->nu;
    seed_offset_y = d->py/d->nv;
    

    CHECK_GLERROR();
}

void CGLWidget::resizeGL(int w, int h)
{
    trackball.reshape(w, h);
    glViewport(0, 0, w, h);

    CHECK_GLERROR();
}

double mag(double *x){
    return std::sqrt(x[0]*x[0] + x[1]*x[1] + x[2]*x[2]);
}


// https://git.merproject.org/mer-core/qtbase/commit/595ed595eabe01a1cf11c8b846fd777de8233721
// https://forum.qt.io/topic/69315/qt5-7-qvector3d-unproject-mouse-picking/7
// https://dondi.lmu.build/share/cg/unproject-explained.pdf
QVector3D unproject(const QVector3D & thisvec, const QMatrix4x4 &modelView, const QMatrix4x4 &projection, const QRect &viewport)
{
    QMatrix4x4 inverse = QMatrix4x4( projection * modelView ).inverted();

    QVector4D tmp(thisvec, 1.0f);
    tmp.setX((tmp.x() - float(viewport.x())) / float(viewport.width()));
    tmp.setY((tmp.y() - float(viewport.y())) / float(viewport.height()));
    tmp = tmp * 2.0f - QVector4D(1.0f, 1.0f, 1.0f, 1.0f);

    QVector4D obj = inverse * tmp;
    if (qFuzzyIsNull(obj.w()))
        obj.setW(1.0f);
    obj /= obj.w();
    return obj.toVector3D();
}

void CGLWidget::paintGL()
{


    glClearColor(1, 1, 1, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
        
    projmatrix.setToIdentity();
    // projmatrix.perspective(fovy, (float)width()/height(), znear, zfar);
    // projmatrix.ortho(-0.5, 0.5, -0.5, 0.5, znear, zfar);
    projmatrix.ortho(-0.5, 0.5, -float(height())/width()/2.0, float(height())/width()/2.0, znear, zfar);
    mvmatrix.setToIdentity();
    mvmatrix.lookAt(eye, center, up);
    mvmatrix.rotate(trackball.getRotation());
    mvmatrix.scale(trackball.getScale());

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glLoadMatrixd(projmatrix.data());
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glLoadMatrixd(mvmatrix.data());

    glColor3f(1, 0, 0);
    glPointSize(3.0);
    for(const auto& iter:critical_points){
        auto cp = iter.second;
        float x0 = (cp.x[0] - d->nu/2)/d->nu;
        float y0 = (cp.x[1] - d->nv/2)/d->nv;
        glBegin(GL_POINTS);
        glVertex3f(x0, y0, 0);
        glEnd();
        // std::cout << cp.type;
    }
    // std::cout << std::endl;
    glColor3f(0, 0, 1);
    for(const auto& iter:saddles){
        auto cp = iter.second;
        float x0 = (cp.x[0] - d->nu/2)/d->nu;
        float y0 = (cp.x[1] - d->nv/2)/d->nv;
        glBegin(GL_POINTS);
        glVertex3f(x0, y0, 0);
        glEnd();
        // std::cout << cp.type;
    }
    // std::cout << std::endl;
    glColor3f(0, 0, 0);
    for(const auto& s:separatrices){
        for(int i=0; i<s.size()-2; i+=2){
            float x0, y0, x1, y1;
            x0 = (s[i] - d->nu/2)/d->nu;
            y0 = (s[i+1] - d->nv/2)/d->nv;
            x1 = (s[i+2] - d->nu/2)/d->nu;
            y1 = (s[i+3] - d->nv/2)/d->nv;

            glBegin(GL_LINES);
            glVertex3f(x0, y0, 0);
            glVertex3f(x1, y1, 0);
            glEnd();
        }
    }

    CHECK_GLERROR();
}

std::vector<double> CGLWidget::compute_interavtive_streamline(double sx, double sy, int direction){

     // d->itrace_xy.clear(1);

     std::vector<double> itrace_xy;
     double p[2] = {sx, sy};
     itrace_xy.push_back(p[0]); itrace_xy.push_back(p[1]);
    // advect
    for (int i=0; i<d->nmax; i++){

        double nx[2];
        if (advect(*d, p, nx, direction*step)){
            p[0] = nx[0]; p[1] = nx[1];
            itrace_xy.push_back(p[0]); itrace_xy.push_back(p[1]);
        }else{
            break;
        }

    }
    return itrace_xy;
}

void CGLWidget::wheelEvent(QWheelEvent* e)
{
    trackball.wheel(e->delta());
    fprintf(stderr, "scale %f\n", trackball.getScale());
    updateGL();
}

std::string get_timestamp()
{
    auto now = std::time(nullptr);
    char buf[sizeof("YYYY-MM-DD-HH:MM:SS")];
    return std::string(buf,buf + 
        std::strftime(buf,sizeof(buf),"%F-%T",std::localtime(&now)));
}

int ctr=0;
void CGLWidget::load_texture(){

    value = 1/trackball.getScale();
    glBindTexture(GL_TEXTURE_2D, tex);

    float pixels[lsize*lsize];
    for (size_t i=0; i<lsize; i++){
        for (int j=0; j<lsize; j++){
            pixels[j*lsize + i ] = float(lic_vals[i*lsize + j]);
        }
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, lsize, lsize, 0, GL_LUMINANCE, GL_FLOAT, pixels);
    CHECK_GLERROR();

}
