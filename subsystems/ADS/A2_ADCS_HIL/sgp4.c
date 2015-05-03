
#include "sgp4.h"
#include <stdio.h>
#include <math.h>
#include "global.h"
// #include "actan.h"
/* compute position and velocity vectors for the satellite defined in sat->elem
 * at its epoch + TSINCE.
 */


const double CK2 = (5.413080e-04);
const double CK4 = (6.209887e-07);
const double QOMS2T = (1.880279e-09);
const double S = (1.012229e+00);
const double AE = (1.0);
const double DE2RA = (.174532925E-1);
const double E6A = (1.e-6);
//const double PI = (3.14159265);
const double PIO2 = (1.57079633);
const double QO = (120.0);
const double SO = (78.0);
const double TOTHRD = (0.66666667);
const double TWOPI = (6.2831853);
const double X3PIO2 = (4.71238898);
const double XJ2 = (1.082616e-3);
const double XJ3 = (-.253881e-5);
const double XJ4 = (-1.65597e-6);
const double XKE = (.743669161e-1);
const double XKMPER = (6378.135);
const double XMNPDA = (1440.0);


double actan2(double sinx, double cosx)
{
    double ret;

    ret = 0.0;
    if(cosx < 0.0) {
	ret = PI;
    } else if(cosx == 0.0) {
	if(sinx < 0.0) {
	    return 3.0 * PIO2;
	} else if(sinx == 0.0) {
	    return ret;
	} else /* sinx > 0.0 */ {
	    return PIO2;
	}
    } else /* cosx > 0.0 */ {
	if(sinx < 0.0) {
	    ret = 2.0 * PI;
	} else if(sinx == 0.0) {
	    return ret;
	}
    }

    return ret + atan(sinx / cosx);
}

// sgp4(           XMO,       XNODEO,       OMEGAO,       EO,       XINCL,        XNO,        XNDT2O,       XNDD6O,       BSTAR,        EPOCH,        &result,            ts[0]);
void sgp4(float XMO, float XNODEO, float OMEGAO, float EO, float XINCL, double XNO, double XNDT2O, float XNDD6O, float BSTAR, double EPOCH, double IEXP, double IBEXP,  double *pos_result, double TSINCE)
{
    int i;

    double A1, A3OVK2, AO, BETAO, BETAO2, C1SQ, C2, C3, COEF, COEF1,
	DEL1, DELO, EETA, EOSQ, ETASQ, PERIGE, PINVSQ, PSISQ, QOMS24,
	S4, TEMP, TEMP1, TEMP2, TEMP3, THETA2, THETA4, TSI, X1M5TH,
	XHDOT1;

//     COSEPW=cos(E+W)
    
    double A, AXN, AYN, AYNL, BETA, BETAL, CAPU, COS2U, COSEPW, COSIK,
	COSNOK, COSU, COSUK, DELM, DELOMG, E, ECOSE, ELSQ, EPW, ESINE,
	OMEGA, OMGADF, PL, R, RDOT, RDOTK, RFDOT, RFDOTK, RK, SIN2U,
	SINEPW, SINIK, SINNOK, SINU, SINUK, TCUBE, TEMP4, TEMP5, TEMP6,
	TEMPA, TEMPE, TEMPL, TFOUR, TSQ, U, UK, UX, UY, UZ, VX, VY, VZ,
	XINCK, XL, XLL, XLT, XMDF, XMP, XMX, XMY, XN, XNODDF, XNODE,
	XNODEK;
    
#if 0
    A1 = A3OVK2 = AO = BETAO = BETAO2 = C1SQ = C2 = C3 = COEF = COEF1 =
	DEL1 = DELO = EETA = EOSQ = ETASQ = PERIGE = PINVSQ = PSISQ = QOMS24 =
	S4 = TEMP = TEMP1 = TEMP2 = TEMP3 = THETA2 = THETA4 = TSI = X1M5TH =
	XHDOT1 = 0;

    A = AXN = AYN = AYNL = BETA = BETAL = CAPU = COS2U = COSEPW = COSIK =
	COSNOK = COSU = COSUK = DELM = DELOMG = E = ECOSE = ELSQ = EPW =
	ESINE =	OMEGA = OMGADF = PL = R = RDOT = RDOTK = RFDOT = RFDOTK =
	RK = SIN2U = SINEPW = SINIK = SINNOK = SINU = SINUK = TCUBE = TEMP4 =
	TEMP5 = TEMP6 =	TEMPA = TEMPE = TEMPL = TFOUR = TSQ = U = UK = UX =
	UY = UZ = VX = VY = VZ = XINCK = XL = XLL = XLT = XMDF = XMP = XMX =
	XMY = XN = XNODDF = XNODE = XNODEK = 0;
#endif

//     if(!sat->prop.sgp4) brace
// 	sat->prop.sgp4 = (struct sgp4_data *) malloc(sizeof(struct sgp4_data));

//   Condition the TLE elements to radian, min,km,...
  //TODO void arm_power_f32(float32_t *pSrc, uint32_t blockSize, float32_t * pResult );
  XNDD6O=XNDD6O*pow(10.0,IEXP);
  XNODEO=XNODEO*DE2RA;
  OMEGAO=OMEGAO*DE2RA;
  XMO=XMO*DE2RA;
  XINCL=XINCL*DE2RA;
  TEMP=TWOPI/XMNPDA/XMNPDA;
  XNO=XNO*TEMP*XMNPDA;
  XNDT2O=XNDT2O*TEMP;
  XNDD6O=XNDD6O*TEMP/XMNPDA;

  
  BSTAR=BSTAR*pow(10.0,IBEXP)/AE;
    
    
    /*
	 * RECOVER ORIGINAL MEAN MOTION (XNODP) AND SEMIMAJOR AXIS (AODP)
	 * FROM INPUT ELEMENTS
	 */
    
    
    
    
    //printf("In sgp4: \n");
    A1 = pow((XKE/XNO), TOTHRD);
	COSIO = cos(XINCL);
	THETA2 = COSIO * COSIO;
	X3THM1 = 3.0 * THETA2 - 1.0;
	EOSQ = EO * EO;
	BETAO2 = 1.0 - EOSQ;
	
	BETAO = sqrt(BETAO2);
	DEL1 = 1.5 * CK2 * X3THM1 / (A1 * A1 * BETAO * BETAO2);
	AO = A1 * (1.0 - DEL1 * (.5 * TOTHRD + DEL1 * (1.0 + 134.0 /81.0 * DEL1)));
	DELO = 1.5 * CK2 * X3THM1 / (AO * AO * BETAO * BETAO2);
	XNODP = XNO / (1.0 + DELO);
	AODP=AO / (1.0 - DELO);

    
    
	/*
	 * INITIALIZATION
	 *
	 * FOR PERIGEE LESS THAN 220 KILOMETERS, THE ISIMP FLAG IS SET AND
	 * THE EQUATIONS ARE TRUNCATED TO LINEAR VARIATION IN SQRT A AND
	 * QUADRATIC VARIATION IN MEAN ANOMALY.  ALSO, THE C3 TERM, THE
	 * DELTA OMEGA TERM, AND THE DELTA M TERM ARE DROPPED.
	 */

// 	sat->prop.sgp4->sgp4_flags = 0;
    sgp4_flags = 0;

	/* IF((AODP*(1.-EO)/AE) .LT. (220./XKMPER+AE)) ISIMP=1 */

	if((AODP * (1.0 - EO) / AE) < (220.0 / XKMPER + AE))
// 	    sat->prop.sgp4->sgp4_flags |= SGP4_SIMPLE;
        sgp4_flags |= SGP4_SIMPLE;

	/*
	 * FOR PERIGEE BELOW 156 KM, THE VALUES OF
	 * S AND QOMS2T ARE ALTERED
	 */

	S4 = S;
	QOMS24 = QOMS2T;
	PERIGE = (AODP * (1.0 - EO) - AE) * XKMPER;

	if(PERIGE < 156.0) {
	    S4 = PERIGE - 78.0;

	    if(PERIGE <= 98.0)
		S4 = 20.0;
		//TODO void arm_power_f32(float32_t *pSrc, uint32_t blockSize, float32_t * pResult );
	    QOMS24 = pow(((120.0 - S4) * AE / XKMPER), 4.0);
	    S4 = S4 / XKMPER + AE;
	}

	PINVSQ=1.0 / (AODP * AODP * BETAO2 * BETAO2);
	TSI = 1.0 / (AODP - S4);
	ETA = AODP * EO * TSI;
	ETASQ = ETA * ETA;
	EETA = EO * ETA;

	PSISQ = fabs(1.0 - ETASQ);
	COEF = QOMS24 * pow(TSI, 4.0);
	COEF1 = COEF / pow(PSISQ, 3.5);

	C2 = COEF1 * XNODP * (AODP * (1.0 + 1.5 * ETASQ +
				      EETA * (4.0 + ETASQ)) + .75 *
			      CK2 * TSI /
			      PSISQ * X3THM1 * (8.0 +
						3.0 * ETASQ * (8.0 + ETASQ)));


	C1 = BSTAR * C2;

	SINIO = sin(XINCL);
	
	A3OVK2 = -XJ3 / CK2 * pow(AE, 3.0);

	C3 = COEF * TSI * A3OVK2 * XNODP * AE * SINIO / EO;

	X1MTH2 = 1.0 - THETA2;
	C4 = 2.0 * XNODP * COEF1 * AODP * BETAO2 *
	    (ETA * (2.0 + .5 * ETASQ) + 
	     EO * (.5 + 2.0 * ETASQ) -
	     2.0 * CK2 * TSI / (AODP * PSISQ) *
	     (-3.0 * X3THM1 * (1.0 - 2.0 * EETA + ETASQ * (1.5 - .5 * EETA)) +
	      .75 * X1MTH2 * (2.0 * ETASQ - EETA * (1.0 + ETASQ)) * 
	      cos(2.0 * OMEGAO)));

	C5 = 2.0 * COEF1 * AODP * BETAO2 * (1.0 +
					    2.75 * (ETASQ + EETA) +
					    EETA * ETASQ);
	THETA4 = THETA2 * THETA2;
	TEMP1 = 3.0 * CK2 * PINVSQ * XNODP;
	TEMP2 = TEMP1 * CK2 * PINVSQ;
	TEMP3 = 1.25 * CK4 * PINVSQ * PINVSQ * XNODP;

	XMDOT = XNODP +
	    .5 * TEMP1 * BETAO * X3THM1 +
	    .0625 * TEMP2 * BETAO * (13.0 - 78.0 * THETA2 + 137.0 * THETA4);

	X1M5TH = 1.0 - 5.0 * THETA2;

	OMGDOT = -.5 * TEMP1 * X1M5TH +
	    .0625 * TEMP2 * (7.0 - 114.0 * THETA2 + 395.0 * THETA4) +
	    TEMP3 * (3.0 - 36.0 * THETA2 + 49.0 * THETA4);

	XHDOT1 = -TEMP1 * COSIO;

	XNODOT = XHDOT1 + (.5 * TEMP2 * (4.0 - 19.0 * THETA2) +
			   2.0 * TEMP3 * (3.0 - 7.0 * THETA2)) * COSIO;

	OMGCOF = BSTAR * C3 * cos(OMEGAO);

	XMCOF = -TOTHRD * COEF * BSTAR * AE / EETA;
	XNODCF = 3.5 * BETAO2 * XHDOT1 * C1;
	T2COF = 1.5 * C1;
	XLCOF = .125 * A3OVK2 * SINIO * (3.0 + 5.0 *COSIO) / (1.0 + COSIO);

	AYCOF = .25 * A3OVK2 * SINIO;
	DELMO = pow(1.0 + ETA * cos(XMO), 3.0);
	SINMO = sin(XMO);

	X7THM1 = 7.0 * THETA2 - 1.0;

/*      IF(ISIMP .EQ. 1) GO TO 90 */
// 	if(!(sat->prop.sgp4->sgp4_flags & SGP4_SIMPLE)) brace
	if(!(sgp4_flags & SGP4_SIMPLE)) {    
        C1SQ = C1 * C1;
	    D2 = 4.0 * AODP * TSI * C1SQ;
	    TEMP = D2 * TSI * C1 / 3.0;
	    D3 = (17.0 * AODP + S4) * TEMP;
	    D4 = .5 * TEMP * AODP * TSI * (221.0 * AODP + 31.0 * S4) * C1;
	    T3COF = D2 + 2.0 * C1SQ;
	    T4COF = .25 * (3.0 * D3 + C1 * (12.0 * D2 + 10.0 * C1SQ));
	    T5COF = .2 * (3.0 * D4 +
			  12.0 * C1 * D3 +
			  6.0 * D2 * D2 +
			  15.0 * C1SQ * (2.0 * D2 + C1SQ));
	}
//     brace

    /*
     * UPDATE FOR SECULAR GRAVITY AND ATMOSPHERIC DRAG
     */

    XMDF = XMO + XMDOT * TSINCE;
    OMGADF = OMEGAO + OMGDOT * TSINCE;
    XNODDF = XNODEO + XNODOT * TSINCE;
    OMEGA = OMGADF;
    XMP = XMDF;
    TSQ = TSINCE * TSINCE;
    XNODE = XNODDF + XNODCF * TSQ;
    TEMPA = 1.0 - C1 * TSINCE;
    TEMPE = BSTAR * C4 * TSINCE;
    TEMPL = T2COF * TSQ;

	if(!(sgp4_flags & SGP4_SIMPLE)) {
	DELOMG = OMGCOF * TSINCE;
	
	DELM = XMCOF * (pow(1.0 + ETA * cos(XMDF), 3) - DELMO);
	TEMP = DELOMG + DELM;
	XMP = XMDF + TEMP;
	OMEGA = OMGADF - TEMP;
	TCUBE = TSQ * TSINCE;
	TFOUR = TSINCE * TCUBE;
	TEMPA = TEMPA - D2 * TSQ - D3 * TCUBE - D4 * TFOUR;
	TEMPE = TEMPE + BSTAR * C5 * (sin(XMP) - SINMO);
	TEMPL = TEMPL + T3COF * TCUBE + TFOUR * (T4COF + TSINCE * T5COF);
    }

    A = AODP * TEMPA * TEMPA;
    E = EO - TEMPE;
    XL = XMP + OMEGA + XNODE + XNODP * TEMPL;
	
    BETA = sqrt(1.0 - E * E);
	
    XN = XKE / pow(A, 1.5);

    /*
     * LONG PERIOD PERIODICS
     */
	
    AXN = E * cos(OMEGA);
    TEMP = 1.0 / (A * BETA * BETA);
    XLL = TEMP * XLCOF * AXN;
    AYNL = TEMP * AYCOF;
    XLT = XL + XLL;
	
    AYN = E * sin(OMEGA) + AYNL;

    /*
     * SOLVE KEPLERS EQUATION
     */

    CAPU = fmod(XLT - XNODE, TWOPI);
    TEMP2 = CAPU;

    for(i = 0; i < 10; i++) {
	
	SINEPW = sin(TEMP2);
	
	COSEPW = cos(TEMP2);
	TEMP3 = AXN * SINEPW;
	TEMP4 = AYN * COSEPW;
	TEMP5 = AXN * COSEPW;
	TEMP6 = AYN * SINEPW;
	EPW = (CAPU - TEMP4 + TEMP3 - TEMP2) / (1.0 - TEMP5 - TEMP6) + TEMP2;

	if(fabs(EPW - TEMP2) <= E6A)
	    break;

	TEMP2 = EPW;
    }

    /*
     * SHORT PERIOD PRELIMINARY QUANTITIES
     */

    ECOSE = TEMP5 + TEMP6;
    ESINE = TEMP3 - TEMP4;
    ELSQ = AXN * AXN + AYN * AYN;
    TEMP = 1.0 - ELSQ;
    PL = A * TEMP;
    R = A * (1.0 - ECOSE);

    TEMP1 = 1.0 / R;
	
    RDOT = XKE * sqrt(A) * ESINE * TEMP1;
	
    RFDOT = XKE * sqrt(PL) * TEMP1;
    TEMP2 = A * TEMP1;
	
    BETAL = sqrt(TEMP);
    TEMP3 = 1.0 / (1.0 + BETAL);

    COSU = TEMP2 * (COSEPW - AXN + AYN * ESINE * TEMP3);
    SINU = TEMP2 * (SINEPW - AYN - AXN * ESINE * TEMP3);

    U = actan2(SINU, COSU);

    SIN2U = 2.0 * SINU * COSU;
    COS2U = 2.0 * COSU * COSU - 1.0;

    TEMP = 1.0 / PL;
    TEMP1 = CK2 * TEMP;
    TEMP2 = TEMP1 * TEMP;

    /*
     * UPDATE FOR SHORT PERIODICS
     */

    RK = R * (1.0 - 1.5 * TEMP2 * BETAL * X3THM1) +
	.5 * TEMP1 * X1MTH2 * COS2U;

    UK = U - .25 * TEMP2 * X7THM1 * SIN2U;
 
   XNODEK = XNODE + 1.5 * TEMP2 * COSIO * SIN2U;
   XINCK = XINCL + 1.5 * TEMP2 * COSIO * SINIO * COS2U;
   RDOTK = RDOT - XN * TEMP1 * X1MTH2 * SIN2U;
   RFDOTK = RFDOT + XN * TEMP1 * (X1MTH2 * COS2U + 1.5 * X3THM1);

   /*
    * ORIENTATION VECTORS
    */
   
   SINUK = sin(UK);
   COSUK = cos(UK);
   SINIK = sin(XINCK);
   COSIK = cos(XINCK);
   SINNOK = sin(XNODEK);
   COSNOK = cos(XNODEK);

   XMX = -SINNOK * COSIK;
   XMY = COSNOK * COSIK;
   UX = XMX * SINUK + COSNOK * COSUK;
   UY = XMY * SINUK + SINNOK * COSUK;
   UZ = SINIK * SINUK;
   VX = XMX * COSUK - COSNOK * SINUK;
   VY = XMY * COSUK - SINNOK * SINUK;
   VZ = SINIK * COSUK;

   /*
    * POSITION AND VELOCITY
    */
    
   pos_result[0] = RK * UX * XKMPER/AE;
   pos_result[1] = RK * UY* XKMPER/AE;
   pos_result[2] = RK * UZ* XKMPER/AE;
   
   
      
   pos_result[3] = RDOTK * UX + RFDOTK * VX * XKMPER/AE*XMNPDA/86400;
   pos_result[4] = RDOTK * UY + RFDOTK * VY * XKMPER/AE*XMNPDA/86400;
   pos_result[5] = RDOTK * UZ + RFDOTK * VZ * XKMPER/AE*XMNPDA/86400;
}


