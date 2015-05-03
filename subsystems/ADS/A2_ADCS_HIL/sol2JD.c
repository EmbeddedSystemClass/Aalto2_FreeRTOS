/* functions to manipulate the modified-julian-date used throughout xephem. */
#include "sol2JD.h"
#include <stdio.h>
#include <math.h>
//#include "global.h"



/* conversions among hours (of ra), degrees and radians. */
#define	degrad(x)	((x)*PI/180.)
#define	raddeg(x)	((x)*180./PI)
#define	hrdeg(x)	((x)*15.)
#define	deghr(x)	((x)/15.)
#define	hrrad(x)	degrad(hrdeg(x))
#define	radhr(x)	deghr(raddeg(x))

/* ratio of from synodic (solar) to sidereal (stellar) rate */
#define	SIDRATE		.9972695677

const double SPD = 24.0*3600.0;	// seconds per day

/* starting point for MJD calculations
 */
#define MJD0  2415020.0
#define J2000 (2451545.0 - MJD0)      /* let compiler optimise */


//#include "P_.h"
//#include "astro.h"

/* given a date in months, mn, days, dy, years, yr,
 * return the modified Julian date (number of days elapsed since 1900 jan 0.5),
 * *mjd.
 * The Modified Julian Day, on the other hand, was introduced by space scientists
 * in the late 1950's. It is defined as MJD = JD - 2400000.5
 * The half day is subtracted so that the day starts at midnight in conformance 
 * with civil time reckoning.
 */



// TODO: change to MJD.At the moment it uses jd not mjd
void cal_mjd (double dy,int mn, int *yr, double *mjd)
{
	static double last_mjd, last_dy;
	static int last_mn, last_yr;
	int b, d, m, y;
	long c;
	int a;
	//if (mn == last_mn && yr == last_yr && dy == last_dy) {
	//	  *mjd = last_mjd;
	//    return;
	//}


	//The months (M) January to December are 1 to 12. For the year (Y) 
	//astronomical year numbering is used, thus 1 BC is 0, 2 BC is −1,
	//and 4713 BC is −4712 (going backwards all the way). D is the day
	//of the month. JDN is the Julian Day Number, which pertains to the
	//noon occurring in the corresponding calendar date.

	m = mn;
	y = (*yr < 0) ? *yr + 1 : *yr;//if BC then add 1
	//For Jan and Feb, the 'a' is 1 else its '0' so if mn<3 then use prev year and generalize the routine

	a = (int) ((14-m)/12);
	y = y + 4800 - a;

	m = m + 12*a - 3;

	//Gregorian calendar was started in 1582 and was a reform to Julian calendar so, if the date is after 1582
	//then use the formula for the case where the start date is in Gregorian format
	if (*yr < 1582 || (*yr == 1582 && (mn < 10 || (mn == 10 && dy < 15))))
	    b = -32083;
	else {
	    c= (int)(y/400);
		d = (int)(y/100);
	    b =  c - d -32045;
	}
	//TODO: Dates between October 5 & 15, 1582 do not exist... apply some checks

	//d = (int)(30.6001*(m+1));

	*mjd = dy + (int)((153*m+2)/5) + 365*y + (int)(y/4) + b;

	//getch();
}




//epoch: 96198.95303667
//Specifically, the equatorial crossing was calculated for day 198 of the year 1996 
//at 22:52 UT [24 (hours) x 0.95303667 = 22.87288 hours, and 60 (minutes) x 0.87288 = 52.3728 minutes].
//Most tracking programs will inform the user how old the element is by using the epoch date element.
//This tells the user if an old and possibly unreliable TLE is being used. 


/*-------------------------------------------------
Gives the true Julian date when provided with 
the TLE epoch
--------------------------------------------------*/

//TODO: check the year-1 and months+12 for first 2 months of year

void tle_epoch2JD(double tle_epoch, int *year, double *jd)
{
	//09321.69542252
	double doy;
	*year = (int)(tle_epoch/1000);
	
	doy = (tle_epoch - *year * 1000);

	if(*year>=0 && *year <= 56){
		*year = *year + 2000;
	}
	else if(*year>56 && *year<100){
		*year = *year + 1900;
	}

	cal_mjd (0, 1, year, jd);
	
	//*jd = *jd + doy;
    *jd = *jd + doy - 0.5;//noon is considered as the start of the new day by subtracting 0.5

}
