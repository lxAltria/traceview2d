#include <vector>
#include "Data.h"

bool advect(const Data &dat, const float p[2], float nxt[2], const double step);
bool advect_rk4(const Data &dat ,const float* pt, float *Y, const double h );