
#ifndef global_h
	#define global_h
#endif


static const double PI	= 3.141592653589793;
static const double twopi = 6.2832;
static const double Rad = 0.0175;
static const double Deg = 57.2958;
static const double Arcs = 206264.8062470964;

static double Fraction(double x)
{
    return(x-(int)x); //floor can be replaced by int
}