//-------------------------------------------------------------------------
//
// Sun
//
// Purpose:
//
//   Computes the Sun's geocentric position using a low precision 
//   analytical series
//
// Input/Output:
//
//   Mjd_TT    Terrestrial Time (Modified Julian Date) 
//   <return>  Solar position vector [m] with respect to the 
//             mean equator and equinox of J2000 (EME2000, ICRF)
//
//-------------------------------------------------------------------------

#include "global.h"
#include "matrix.h"
#include "matrix2.h"


// Elementary rotations

void R_x(double Angle, MAT *A )
{
  
  const double C = cos(Angle);
  const double S = sin(Angle);
  
  A->me[0][0] = 1.0;  A->me[0][1] = 0.0;  A->me[0][2] = 0.0;
  A->me[1][0] = 0.0;  A->me[1][1] =  +C;  A->me[1][2] =  +S;
  A->me[2][0] = 0.0;  A->me[2][1] =  -S;  A->me[2][2] =  +C;
  
}
/*
Matrix R_y(double Angle)
{
  const double C = cos(Angle);
  const double S = sin(Angle);
  Matrix U(3,3);
  U.M[0][0] =  +C;  U.M[0][1] = 0.0;  U.M[0][2] =  -S;
  U.M[1][0] = 0.0;  U.M[1][1] = 1.0;  U.M[1][2] = 0.0;
  U.M[2][0] =  +S;  U.M[2][1] = 0.0;  U.M[2][2] =  +C;
  return U;
}

Matrix R_z(double Angle)
{
  const double C = cos(Angle);
  const double S = sin(Angle);
  Matrix U(3,3);
  U.M[0][0] =  +C;  U.M[0][1] =  +S;  U.M[0][2] = 0.0;
  U.M[1][0] =  -S;  U.M[1][1] =  +C;  U.M[1][2] = 0.0;
  U.M[2][0] = 0.0;  U.M[2][1] = 0.0;  U.M[2][2] = 1.0;
  return U;
}
*/


void Sun_monten(double jd_TT, VEC *r_Sun)
{
  // Variables
  double L, M, r, eps, T, norm;
  MAT *A;
  VEC *x, *r_Sun_local;
  
   A = m_get(3,3);
  x = v_get(3);
  r_Sun_local = v_get(3);
  
  eps = 23.43929111 * Rad;             // Obliquity of J2000 ecliptic
  //this algo uses the julian date.
  T   = (jd_TT-2451545.0)/36525.0;  // Julian cent. since J2000
  
  
  
  // Mean anomaly, ecliptic longitude and radius

  M = twopi * Fraction ( 0.9931267 + 99.9973583*T);                    // [rad]
  L = twopi * Fraction ( 0.7859444 + M/twopi + (6892.0*sin(M)+72.0*sin(2.0*M)) / 1296.0e3); // [rad]
  r = 149.619e9 - 2.499e9*cos(M) - 0.021e9*cos(2*M);             // [m]
  
  // Equatorial position vector

  R_x(-eps,A);
  
  x->ve[0] = r*cos(L);
  x->ve[1] = r*sin(L);      
  x->ve[2] = 0.0;
  
  //r_Sun = R_x(-eps) * Vector(r*cos(L),r*sin(L),0.0);
  mv_mlt(A, x, r_Sun_local);
  
  norm = v_norm2(r_Sun_local);
  
  sv_mlt((1/norm), r_Sun_local, r_Sun);
  
  /*r_Sun.X = R_x(-eps,A) * r*cos(L);
  r_Sun.Y = R_x(-eps) * r*sin(L);
  r_Sun.Z = R_x(-eps) * 0.0;*/
  
  //TODO 
 V_FREE(x);
 V_FREE(r_Sun_local);
 M_FREE(A);
}
