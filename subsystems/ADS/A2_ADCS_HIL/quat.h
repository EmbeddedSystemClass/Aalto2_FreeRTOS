#ifndef quat_h
	#define quat_h
#endif

#include "matrix.h"
        
void quat_mul(VEC *, VEC *, VEC *);

void quat_inv(VEC *, VEC *);

void quat_normalize(VEC *);

void vec2quat(VEC *, VEC *);

void quat_rot_vec(VEC *, VEC*);

void quat2vec(VEC *, VEC *);

void get_dyad(VEC* , VEC* , MAT*);