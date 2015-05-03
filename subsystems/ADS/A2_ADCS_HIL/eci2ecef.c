
#include <math.h>
#include "eci2ecef.h"
#include "global.h"


//static double PI = 3.1416;


double mod_eci2ecef(double x, double y)
{
	return (x-x* (int)(x/y));
}

int round_num(double number)
{
    return ( (number >= 0) ? (int)(number + 0.5) : (int)(number - 0.5));
}




/*% Calculate ECI to ECEF rotation quartenion by Julian Date.

INPUTS: Julian date and address of the quaternion to be filled 
OUTPUTS: just the quaternion filled through (call by) reference

% q = eci2ecef(t)

% Calculates the ECI->ECEF (z-axis) quartenion given a Julian date.
% Output is within 0 - 2PI boundary.
% Originally based on eci2ecef.m by raf@control.auc.dk.

% Time of rotation of the Earth with respect to the mean equinox is referred to as
% mean sidereal time (ST). The Earth rotates 360 deg in one sidereal day
% which is 23h 56m 04.09053s [almanac, b6] in mean solar time. 
% The resulting Earth precession rate is therefore 2*PI/23h 56m 04.09053s
% Apart from inherent motion of the equinox, due to precession and nutation,
% ST is a direct measure of the diurnal rotation of the Earth. As a fixpoint
% for the Earth rotation the Greenwich median transit of the equinox at
% 0 Jan 1997 is used (JD2450448.5). According to [almanac, b15] the
% transit takes place at 17h 18m 21.8256s (JD0.7211).
% The Earth Precession is denoted "omega"
% The sidereal transit is denoted "s" */


void eci2ecef(double t, VEC *Qeci2ecef)
{
double daysec, t_rev, n_rev, s, psi, omega, t_into_rev;
//int psi, omega, t_into_rev;


// Seconds in a day
daysec = 86400;

//Earth precession [rad/sec]
omega = 2*PI/86164.09053; //This value has been correct (KV)

// Sidereal epoch (raf's)
s = 6.23018356e+04/daysec + 2450448.5; //JD fixpoint

// Time for a revolution
t_rev = (2*PI)/omega; //

// Number of revolutions since epoch
n_rev = round_num( ((t-s)*daysec) / t_rev);//int typecast removed for now

// Time into a revolution
t_into_rev = (t-s)*daysec - n_rev * t_rev;

// Calculate angle with 2PI-boundary
// psi = mod_eci2ecef((omega*t_into_rev), (2*PI));
psi = (omega*t_into_rev);

//TODO: algo given in the SPA doc in section 3.2.6

if(psi >= 2*PI){
    psi = psi-2*PI;
}
else if (psi < 0){
    psi = psi + 2*PI;
}
// printf("psi: %lf \n", psi);

// Represent result as a quartenion
// Qeci2ecef->W = 0;
// Qeci2ecef->X = 0;
// Qeci2ecef->Y = sin(psi/2);
// Qeci2ecef->Z = cos(psi/2);
psi = psi/2;

Qeci2ecef->ve[0] = 0.0;
Qeci2ecef->ve[1] = 0.0;
Qeci2ecef->ve[2] = sin(psi);
Qeci2ecef->ve[3] = cos(psi);

}
