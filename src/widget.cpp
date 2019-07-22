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
using namespace Eigen;



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


void CGLWidget::set_lic_size(double lic_size, int lic_nmax, float step, double rate){

    this->lic_nmax = lic_nmax;
    this->lic_size = lic_size;
    this->step = step;
    this->rate = rate;

    
   


}



void CGLWidget::generate_seeds(double px, double py){
    double scale = trackball.getScale();
  



    double step = rate/scale;
    this->lsize = 0;
    for (double i=px-lic_size/scale; i<px+lic_size/scale; i+=step){
        for (double j=py-lic_size/scale; j<py+lic_size/scale; j+=step){
            lic_x.push_back(i);
            lic_y.push_back(j);
        }
        this->lsize++;
    }
    scx = px-lic_size/scale;
    scy = py-lic_size/scale;
    lic_res = step;
    lic_vals.resize(lic_x.size());
    // fprintf(stderr, "lic_x %ld %d\n", lic_x.size(), lsize);
    // fprintf(stderr, "Checking assertion lic_x %ld %d\n", lic_x.size(), lsize);
    assert(lsize*lsize==lic_x.size());

    /* Generate noise image */
    srand(5);
    noise.resize(lsize*lsize);
     for (int i = 0; i < lsize; i++){
        for (int j = 0; j< lsize; j++){
            noise[i*lsize + j] = float((rand() % 255))/255;
            // fprintf(stderr, "noise %f \n", noise[i*lsize + j]);

        }
    }

}

void CGLWidget::generate_lic(){

    float p[2];
    float nx[2];
    double lic_val;
    double  idx_x, idx_y;
    int ind_i, ind_j;
    double maxval = 0;

    for (size_t i=0; i<lic_x.size(); i++){

        lic_val = 0;
        p[0] = lic_x[i];
        p[1] = lic_y[i];
        // forward advect
        for (int j=0; j<lic_nmax; j++){
            if(advect(*d, p, nx, step)){


                p[0] = nx[0];
                p[1] = nx[1];

                // get index on noise field
                idx_x = (p[0]-scx)/lic_res;
                idx_y = (p[1]-scy)/lic_res;

                ind_i = floor(idx_x);
                ind_j = floor(idx_y);


                // get noise contribution from clamped positions
                if (idx_x > 0 && idx_y >0 && idx_x < lsize && idx_y <lsize)
                    lic_val += noise[ind_i * lsize  + ind_j] * (lic_nmax-j);
                else
                    break;

            }else{
                break;
            }

        }
        // exit(0);
        p[0] = lic_x[i];
        p[1] = lic_y[i];
        // backward advect
        for (int j=1; j<lic_nmax; j++){
            if(advect(*d, p, nx, -step)){

                p[0] = nx[0];
                p[1] = nx[1];

                 // get index on noise field
                idx_x = (p[0]-scx)/lic_res;
                idx_y = (p[1]-scy)/lic_res;

                ind_i = floor(idx_x);
                ind_j = floor(idx_y);

                // get noise contribution from clamped positions
                if (idx_x > 0 && idx_y >0 && idx_x < lsize && idx_y <lsize)
                    lic_val += noise[ind_i * lsize  + ind_j] * (lic_nmax-j);
                else
                    break;

            }else{
                break;
            }
        }

        lic_vals[i] = lic_val;
        if (maxval < lic_val)
            maxval = lic_val;

        

    }

    
    for (size_t i=0; i<lic_vals.size(); i++){

        lic_vals[i] /= maxval;
        // fprintf(stderr, "licvals %d %f\n", i, lic_vals[i]);
    }

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

    // Import the viewPort
      GLint *params = new GLint(4);
      glGetIntegerv(GL_VIEWPORT, params);
      QRect vp = QRect(*params, *(params + 1), *(params + 2), *(params + 3));  


    glColor3f(0, 0, 0);
    glPointSize(1.0);

                                                                                                                                                                                                                                                   

    float x, y;
    seed_offset_x = d->px/d->nu;
    seed_offset_y = d->py/d->nv;

    QVector3D vec(400,200,1);
    QVector3D tmp = unproject(vec, mvmatrix, projmatrix, vp);
    // fprintf(stderr, "%f %f %f, %f %f\n", tmp.x(), tmp.y(), tmp.z(), znear, zfar);
    // fprintf(stderr, "%d %d\n", width(), height());

     /* drawing lic */
    // fprintf(stderr, "len %ld\n", lic_x.size());
    glColor3f(1, 0, 0);
    glPointSize(1.0);
    float colr;
    int ind_i, ind_j;
    for (size_t i=0; i<lic_x.size(); i++){
        // ind_i = i/lsize;
        // ind_j = i%lsize;
        // colr = noise[ind_i*lsize + ind_j];
        colr = lic_vals[i];
        glColor3f(colr, colr, colr);
        // fprintf(stderr, "colr %f\n", colr);
        x = (float)lic_x[i]/d->nu - seed_offset_x;
        y = (float)lic_y[i]/d->nv - seed_offset_y;
        glBegin(GL_POINTS);
        glVertex3f(x,y,0);
        glEnd();
    }

    /* drawing vector field */

    glColor3f(0, 0, 0);
    glPointSize(1.0);

   
    float alpha = 0.001;
    float vx, vy;
    glLineWidth(1.0);
    for (size_t i=0; i<d->nu; i+=10){
        for (size_t j=0; j<d->nv; j+=10){

            
            vx = d->u[j*d->nu+i];
            vy = d->v[j*d->nu+i];
            x = float(i)/d->nu-seed_offset_x;
            y = float(j)/d->nu-seed_offset_y;

            glBegin(GL_LINES);
            glVertex3f(x, y, 0);
            // fprintf(stderr, "%f %f \n", x, y);
            glVertex3f(x + alpha*vx, y + alpha*vy, 0.001);
            glEnd();

            glBegin(GL_POINTS);
            glVertex3f(x,y,0.001);
            glEnd();

        }
    }

   
    /* drawing streamline  */
    float x0, y0, x1, y1;
    x0 = d->trace_xy[0]/d->nu-seed_offset_x;
    y0 = d->trace_xy[1]/d->nv-seed_offset_y;

    glColor3f(0, 0.5, 0);
    glPointSize(8.0);
    glBegin(GL_POINTS);
    glVertex3f(x0,y0,0);
    glEnd();

    glColor3f(1, 0, 0);
    glLineWidth(2.0);
    for (size_t i=0; i<d->trace_xy.size()-2; i+=2){

        x0 = d->trace_xy[i]/d->nu-seed_offset_x;
        y0 = d->trace_xy[i+1]/d->nv-seed_offset_y;
        x1 = d->trace_xy[i+2]/d->nu-seed_offset_x;
        y1 = d->trace_xy[i+3]/d->nv-seed_offset_y;

         glBegin(GL_LINES);
         glVertex3f(x0, y0, 0.001);
         glVertex3f(x1, y1, 0.001);
         glEnd();
    }

    /* drawing interactive streamline  */

    if (d->itrace_xy.size()>0){
        x0 = d->itrace_xy[0]/d->nu-seed_offset_x;
        y0 = d->itrace_xy[1]/d->nv-seed_offset_y;

        glColor3f(0, 0.5, 0);
        glPointSize(8.0);
        glBegin(GL_POINTS);
        glVertex3f(x0,y0,0);
        glEnd();

        glColor3f(1, 0, 0);
        glLineWidth(2.0);
        for (size_t i=0; i<d->itrace_xy.size()-2; i+=2){

            x0 = d->itrace_xy[i]/d->nu-seed_offset_x;
            y0 = d->itrace_xy[i+1]/d->nv-seed_offset_y;
            x1 = d->itrace_xy[i+2]/d->nu-seed_offset_x;
            y1 = d->itrace_xy[i+3]/d->nv-seed_offset_y;

             glBegin(GL_LINES);
             glVertex3f(x0, y0, 0.001);
             glVertex3f(x1, y1, 0.001);
             glEnd();
        }
    }

    CHECK_GLERROR();
}

void CGLWidget::compute_interavtive_streamline(double sx, double sy){

     d->itrace_xy.clear();
     float p[2] = {(float)sx, (float)sy};

    // advect
    for (int i=0; i<d->nmax; i++){

        float nx[2];
        if (advect(*d, p, nx, step)){
            p[0] = nx[0]; p[1] = nx[1];
            d->itrace_xy.push_back(p[0]); d->itrace_xy.push_back(p[1]);
        }else{
            break;
        }

    }

}


void CGLWidget::mousePressEvent(QMouseEvent* e)
{
    trackball.mouse_rotate(e->x(), e->y());


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

    // Import the viewPort
      GLint *params = new GLint(4);
      glGetIntegerv(GL_VIEWPORT, params);
      QRect vp = QRect(*params, *(params + 1), *(params + 2), *(params + 3));  


    glColor3f(0, 0, 0);
    glPointSize(1.0);

                                                                                                                                                                                                                                                   

    float x, y;
    float seed_offset_x = d->px/d->nu, seed_offset_y = d->py/d->nv;
    QVector3D frnt(e->x(), e->y(), 0);
    QVector3D frnt_pt = unproject(frnt, mvmatrix, projmatrix, vp);

    QVector3D back(e->x(), e->y(), 1);
    QVector3D back_pt = unproject(back, mvmatrix, projmatrix, vp);

    Vector3d dir(back_pt.x() - frnt_pt.x(), back_pt.y() - frnt_pt.y(), back_pt.z() -frnt_pt.z());
    double magratio = frnt_pt.z()/abs((frnt_pt.z() - back_pt.z()));
    double mag = dir.norm();
    dir.normalize();

    Vector3d sd = magratio*mag*dir + Vector3d(frnt_pt.x(), frnt_pt.y(), frnt_pt.z());
   
    double sx = (d->nu)*(sd(0)+seed_offset_x);
    double sy = (d->nv)*(-sd(1)+seed_offset_y);

    compute_interavtive_streamline(sx, sy);
    updateGL();

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


void CGLWidget::keyPressEvent(QKeyEvent *ev)
{   

    lic_x.clear();
    lic_y.clear();

    if (ev->text().toStdString().c_str()[0]=='s'){

        generate_seeds(d->px, d->py);
        generate_lic();
        updateGL();
    }
}


