

#ifndef eci2ecef_h
	#define eci2ecef_h
#endif

#include   "matrix.h"


double mod(double, double);


int round_num(double);


void eci2ecef(double, VEC *);