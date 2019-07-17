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


bool advect_rk4(const Data &dat ,const float* pt, float *Y, const double h )
{
  int num_dims = 2;

  float p0[num_dims]; 
  memcpy(p0, pt, sizeof(float)*num_dims); 
  
  float v[num_dims]; 

  // 1st rk step
  if (!inside(dat, pt)) return false; 
  interp2d(dat, pt, v);
  float k1[num_dims]; 
  for (int i=0; i<num_dims; i++) k1[i] = h*v[i]; 
  for (int i=0; i<num_dims; i++) Y[i] = p0[i] + 0.5*k1[i]; 
  
  // 2nd rk step
  if (!inside(dat, pt)) return true; 
  interp2d(dat, pt, v);
  float k2[num_dims]; 
  for (int i=0; i<num_dims; i++) k2[i] = h*v[i]; 
  for (int i=0; i<num_dims; i++) Y[i] = p0[i] + 0.5*k2[i]; 

  // 3rd rk step
  if (!inside(dat, pt)) return true; 
  interp2d(dat, pt, v);
  float k3[num_dims]; 
  for (int i=0; i<num_dims; i++) k3[i] = h*v[i]; 
  for (int i=0; i<num_dims; i++) Y[i] = p0[i] + k3[i]; 

  // 4th rk step
  if (!inside(dat, pt)) return true; 
  interp2d(dat, pt, v);
  for (int i=0; i<num_dims; i++) 
    Y[i] = p0[i] + (k1[i] + 2.0*(k2[i]+k3[i]) + h*v[i])/6.0; 

  return true; 
}