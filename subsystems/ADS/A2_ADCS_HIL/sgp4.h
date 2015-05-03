

#ifndef sgp4_h
	#define sgp4_h
#endif


typedef struct _Vec3 {
    double x;
    double y;
    double z;
} Vec3;


void sgp4(float, float, float, float, float, double, double, float, float, double,double ,double, double *, double);

// at lower orbits
#define SGP4_SIMPLE	0x00000001


    unsigned int sgp4_flags;
    unsigned int pad;
    double AODP;
    double AYCOF;
    double C1;
    double C4;
    double C5;
    double COSIO;
    double D2;
    double D3;
    double D4;
    double DELMO;
    double ETA;
    double OMGCOF;
    double OMGDOT;
    double SINIO;
    double SINMO;
    double T2COF;
    double T3COF;
    double T4COF;
    double T5COF;
    double X1MTH2;
    double X3THM1;
    double X7THM1;
    double XLCOF;
    double XMCOF;
    double XMDOT;
    double XNODCF;
    double XNODOT;
    double XNODP;







