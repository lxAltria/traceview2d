#include "interp.h"
#include "Data.h"
#include "math.h"

// barycentric adapted function from :https://www.gamedev.net/forums/topic/621445-barycentric-coordinates-c-code-check/

void interp2d(const Data &dat, const double p[2], double v[2]){


	// get indexes i,j of lower left corner

	int i = floor(p[0] / dat.res);
	int j = floor(p[1] / dat.res);

	// fprintf(stderr, "i %d j %d\n", i, j);
	// fprintf(stderr, "left %f right %d\n", p[0] - p[1] + (j - i), 0-511+ (j-i));
	int triangle = 1; // initially assuming upper 

	// determine if point is contained within lower or upper triangle
	if (((p[0] - p[1] + (j - i))>=0) == ((511+ (j-i)>=0)))
		triangle = 0;

	// barycentric coordinates lambda
	double lambda[3];

	// double pp[2] = {0,0.5};
	// double p0[2] = {double(0),double(0)};
	// double p1[2] = {double(1),double(1)};
	// double p2[2] = {double(0), double(1)};
	// barycent2d(p0, p1, p2, pp, lambda);
	// fprintf(stderr, "lambda %f %f %f\n", lambda[0], lambda[1], lambda[2]);

	double v00, v01, v10, v11, v20, v21;

	if (triangle == 0){

		double p0[2] = {double(i),double(j)};
		double p1[2] = {double(i+1),double(j)};
		double p2[2] = {double(i+1), double(j+1)};

		barycent2d(p0, p1, p2, p, lambda);

		v00 = dat.u[j*dat.nu+i];
		v01 = dat.v[j*dat.nu+i];
		v10 = dat.u[j*dat.nu+(i+1)];
		v11 = dat.v[j*dat.nu+(i+1)];
		v20 = dat.u[(j+1)*dat.nu+(i+1)];
		v21 = dat.v[(j+1)*dat.nu+(i+1)];

		// fprintf(stderr, "upper\n");

	}else{
		double p0[2] = {double(i),double(j)};
		double p1[2] = {double(i),double(j+1)};
		double p2[2] = {double(i+1), double(j+1)};
		// fprintf(stderr, "lower\n");

		barycent2d(p0, p1, p2, p, lambda);

		v00 = dat.u[j*dat.nu+i];
		v01 = dat.v[j*dat.nu+i];
		v10 = dat.u[(j+1)*dat.nu+i];
		v11 = dat.v[(j+1)*dat.nu+i];
		v20 = dat.u[(j+1)*dat.nu+(i+1)];
		v21 = dat.v[(j+1)*dat.nu+(i+1)];
	}

	

	// interpolate velocity using lambda

	v[0] = lambda[0] * v00 + lambda[1] * v10 + lambda[2] *v20;
	v[1] = lambda[0] * v01 + lambda[1] * v11 + lambda[2] *v21;

	// fprintf (stderr, "velocity %f %f\n", v[0], v[1]);

}



// compute the area of a triangle using Heron's formula

double triarea(double a, double b, double c)

{

    double s = (a + b + c)/2.0;

    double area=sqrt(fabs(s*(s-a)*(s-b)*(s-c)));

    return area;     

}



// compute the distance between two points

double dist(double x0, double y0, double z0, double x1, double y1, double z1)

{

    double a = x1 - x0;	  

    double b = y1 - y0;

    double c = z1 - z0;

    return sqrt(a*a + b*b + c*c);

}


void barycent2d(double *p0, double *p1, double *p2, const double *v, double *lambda )
{

	double x0 = p0[0], y0 = p0[1], z0 = 0;
	double x1 = p1[0], y1 = p1[1], z1 = 0;
	double x2 = p2[0], y2 = p2[1], z2 = 0;
	double vx = v[0], vy = v[1], vz = 0;

    // compute the area of the big triangle

    double a = dist(x0, y0, z0, x1, y1, z1);
    double b = dist(x1, y1, z1, x2, y2, z2);
    double c = dist(x2, y2, z2, x0, y0, z0);

    double totalarea = triarea(a, b, c);

	

    // compute the distances from the outer vertices to the inner vertex

    double length0 = dist(x0, y0, z0, vx, vy, vz);	  

    double length1 = dist(x1, y1, z1, vx, vy, vz);	  

    double length2 = dist(x2, y2, z2, vx, vy, vz);	  

    

    // divide the area of each small triangle by the area of the big triangle

    lambda[0] = triarea(b, length1, length2)/totalarea;

    lambda[1] = triarea(c, length0, length2)/totalarea;

    lambda[2] = triarea(a, length0, length1)/totalarea;	  

}
