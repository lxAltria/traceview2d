#include "widget.h"
#include <OpenGL/glu.h>
#include <cfloat>
#include <netcdf.h>

#include <math.h>
#include <iostream>

#include <cmath>




CGLWidget::CGLWidget(const QGLFormat& fmt) : 
QGLWidget(fmt),
fovy(30.f), znear(0.1f), zfar(10.f),
eye(0, 0, 2.5), center(0, 0, 0), up(0, 1, 0)
{
}

CGLWidget::~CGLWidget()
{
}




void CGLWidget::initializeGL()
{
    glewInit();

    trackball.init();

    sphere = gluNewQuadric();
    gluQuadricDrawStyle(sphere, GLU_FILL);
    gluQuadricTexture(sphere, TRUE);
    gluQuadricNormals(sphere, GLU_SMOOTH);
    
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1, 1);
    
    glEnable(GL_DEPTH_TEST);

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





void CGLWidget::paintGL()
{
    glClearColor(1, 1, 1, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    projmatrix.setToIdentity();
    projmatrix.perspective(fovy, (float)width()/height(), znear, zfar);
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


    glColor3f(0, 0, 0);
    glPointSize(1.0);









    float vx,vy;
    float alpha = 0.001;

    float x, y;

    glLineWidth(1.0);
    for (size_t i=0; i<d->nu; i+=10){

        for (size_t j=0; j<d->nv; j+=10){

            
            vx = d->u[j*d->nu+i];
            vy = d->v[j*d->nu+i];
            x = float(i)/d->nu-0.5;
            y = float(j)/d->nu-0.5;

            glBegin(GL_LINES);
            glVertex3f(x, y, 0);
            // fprintf(stderr, "%f %f \n", x, y);
            glVertex3f(x + alpha*vx, y + alpha*vy, 0);
            glEnd();

            glBegin(GL_POINTS);
            glVertex3f(x,y,0);
            glEnd();

        }
    }



    

   

    float x0, y0, x1, y1;
    x0 = d->trace_xy[0]/d->nu-0.5;
    y0 = d->trace_xy[1]/d->nv-0.5;

    glColor3f(0, 0.5, 0);
    glPointSize(8.0);
    glBegin(GL_POINTS);
    glVertex3f(x0,y0,0);
    glEnd();

    glColor3f(1, 0, 0);
    glLineWidth(2.0);
    for (size_t i=0; i<d->trace_xy.size()-2; i+=2){

        x0 = d->trace_xy[i]/d->nu-0.5;
        y0 = d->trace_xy[i+1]/d->nv-0.5;
        x1 = d->trace_xy[i+2]/d->nu-0.5;
        y1 = d->trace_xy[i+3]/d->nv-0.5;

         glBegin(GL_LINES);
         glVertex3f(x0, y0, 0);
         glVertex3f(x1, y1, 0);
         glEnd();
    }

     
    

    CHECK_GLERROR();
}


void CGLWidget::mousePressEvent(QMouseEvent* e)
{
    trackball.mouse_rotate(e->x(), e->y());
}

void CGLWidget::mouseMoveEvent(QMouseEvent* e)
{
    trackball.motion_rotate(e->x(), e->y());
    updateGL();
}

void CGLWidget::wheelEvent(QWheelEvent* e)
{
    trackball.wheel(e->delta());
    updateGL();
}


