#include "advect.h"
#include "interp.h"


inline bool inside(const Data &dat, const float p[2])
{
    for (int i = 0; i < 2; i++)
        if (p[i] < dat.bounds[i][0] || p[i] > dat.bounds[i][1])
            return false;
    return true;
}


bool advect(const Data &dat ,const float p[2], float nxt[2], const double step){

	float v[2];

	// if inside domain advect and return true, else return false
	if (inside(dat, p)){

		interp2d(dat, p, v);

		nxt[0] = p[0] + step*v[0];
		nxt[1] = p[1] + step*v[1];

	}else {
		return false;
	}

	



	return true;
}