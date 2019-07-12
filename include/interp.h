
#include <vector>
#include "Data.h"

void interp2d(const Data &dat, const float p[2], float v[2]);

double triarea(double a, double b, double c);
double dist(double x0, double y0, double z0, double x1, double y1, double z1);
void barycent2d(double *p0, double *p1, double *p2, const float *v, double *lambda );