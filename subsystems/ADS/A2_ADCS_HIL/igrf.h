
#ifndef igrf_h
	#define igrf_h
#endif

#include<math.h>
#include"matrix.h"       


static double absv(double *);

static int cross(double *, double *, double *);

static int crossn (double *, double *, double *);

int com_read (FILE *, char *);

char *skip_in_str (int, char *);

void setigrf(double);

void igrfmodel(VEC *, double *, double *);
