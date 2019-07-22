#include <vector>
#include "Data.h"

bool advect(const Data &dat, const double p[2], double nxt[2], const double step);
bool advect_rk4(const Data &dat ,const double* pt, double *Y, const double h );