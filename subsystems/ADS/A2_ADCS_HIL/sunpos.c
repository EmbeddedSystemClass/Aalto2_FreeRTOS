#include <stdio.h>
#include <math.h>
#include "sunpos.h"
#include "global.h"
#include "matrix.h"
#include "matrix2.h"

/*-------------------------------------------------------------------------------
   Generate the sun vector in the earth-centered inertial frame. Will
   output the distance to the sun from the earth if two output
   arguments are given.
-------------------------------------------------------------------------------
   Form:
   [u, r] = SunV2( jD, rSc )
-------------------------------------------------------------------------------

   ------
   Inputs
   ------
   jD        (:)     Julian date of the time when the sun position is to be 
                     determined
   rSc       (3,:)   Spacecraft vector in the ECI frame (km) //not used...ok

   -------
   Outputs
   -------
   u         (3,:)   Unit sun vector
   r         (:)     Distance from origin to sun (km)

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
   References: Montenbruck, O., T.Pfleger, Astronomy on the Personal
               Computer, Springer-Verlag, Berlin, 1991, p. 36.
-------------------------------------------------------------------------------
   Copyright 1993 Princeton Satellite Systems, Inc. All rights reserved.
-------------------------------------------------------------------------------*/

void sunpos(double jD, VEC *u)
{
    double T, g, dLam, lam, obOfE, sLam, r, temp1,temp2;
    //twopi removed from here as it is defined in global.h
    
    //this algo uses the julian date.
    //------------------
    T = (jD - 2451545.0)/36525; //36525 is the julian century. This gives the epoch since 1st Jan 2000 as the julian century
    
    // Mean anomaly
    //-------------
    g = Fraction( (0.993133 + 99.997361*T) );// - (int)(0.993133 + 99.997361*T) ); 
    //problem here:
    temp1 = 2*PI*g;//(twopi * g);
    temp2= 2*g*2*PI;//(2*g*twopi);
    ////////////
    
    
    dLam = 6893 * sin(temp1) + 72 * sin(temp2);
    
//  lam = twopi*rem(0.7859453 + g + (6191.2*T + dLam)/1296e3,1)=
    temp1 = Fraction(0.7859453 + g + (6191.2 * T + dLam)/1296e3); //-(int)(0.7859453 + g + (6191.2 * T + dLam)/1296e3);
    
    //problem here:
    lam =2*PI*temp1;// twopi * temp;

    // Obliquity of ecliptic
    //----------------------
    obOfE = 0.408982 - 2.269e-04*T;

    sLam = sin(lam);
    
    u->ve[0] = cos(lam); //X
    u->ve[1] = cos(obOfE)*sLam; //Y
    u->ve[2] = sin(obOfE)*sLam; //Z

    r = (1.0014 - 0.01671*cos(g*PI/180) - 0.00014*cos(2*g*PI/180))*149600e3;
    

    /*
    // Account for parallax
    //---------------------
    if ( nargin == 2 )
             *{
      u = [r.*u(1,:) - rSc(1,:);...
           r.*u(2,:) - rSc(2,:);...
           r.*u(3,:) - rSc(3,:)];
      r = Mag(u);
      u = Unit(u);
     }*/
              
}
