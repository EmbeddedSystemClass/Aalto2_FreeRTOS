
#include "global.h"
#include "sgp4.h"
#include "igrf.h"
#include "eci2ecef.h"
#include "sol2JD.h"
#include "quat.h"
#include "sunpos1.h"
#include "sunpos.h"
#include "UKF.h"

#include "matrix.h"
#include "matrix2.h"


#ifndef TRUE
    #define TRUE (1)
#endif

#ifndef TRUE
    #define FALSE (0)
#endif

void det_main(t_jd, f,f,f,f, )
{
	//12 inputs
	/*TSINCE or current julian date
	TLE params
	ss_meas
	m_meas
	om
	T_
	eclipse_
	t_
	h_rk
	t_ukf_f
	*/
    /* Port 0 CURRENT time TSINCE*/
    ssSetInputPortWidth(S,  0, 1);
    ssSetInputPortDataType(S, 0, SS_DOUBLE);
    ssSetInputPortDirectFeedThrough(S, 0, TRUE);
    ssSetInputPortRequiredContiguous(S, 0, TRUE); /*direct input signal access*/
    
    /* Port 1 TLE parameters*/
    ssSetInputPortWidth(S,  1, 12);
    ssSetInputPortDataType(S, 1, SS_DOUBLE);
    ssSetInputPortDirectFeedThrough(S, 1, TRUE);
    ssSetInputPortRequiredContiguous(S, 1, TRUE); /*direct input signal access*/
    
    /* Port 2 sun_ vector measurement */
    ssSetInputPortWidth(S,  2, 3);
    ssSetInputPortDataType(S, 2, SS_DOUBLE);
    ssSetInputPortDirectFeedThrough(S, 2, TRUE);
    ssSetInputPortRequiredContiguous(S, 2, TRUE); /*direct input signal access*/
    
    /* Port 3 magnetic vector measurement */
    ssSetInputPortWidth(S,  3, 3);
    ssSetInputPortDataType(S, 3, SS_DOUBLE);
    ssSetInputPortDirectFeedThrough(S, 3, TRUE);
    ssSetInputPortRequiredContiguous(S, 3, TRUE); /*direct input signal access*/
    
    /* Port 4 omega */
    ssSetInputPortWidth(S,  4, 3);
    ssSetInputPortDataType(S, 4, SS_DOUBLE);
    ssSetInputPortDirectFeedThrough(S, 4, TRUE);
    ssSetInputPortRequiredContiguous(S, 4, TRUE); /*direct input signal access*/
    
    /* Port 5 T_ext*/
    ssSetInputPortWidth(S,  5, 3);
    ssSetInputPortDataType(S, 5, SS_DOUBLE);
    ssSetInputPortDirectFeedThrough(S, 5, TRUE);
    ssSetInputPortRequiredContiguous(S, 5, TRUE); /*direct input signal access*/
    
    /* Port 6 eclipse*/
    ssSetInputPortWidth(S, 6, 1);
    ssSetInputPortDataType(S, 6, SS_DOUBLE);
    ssSetInputPortDirectFeedThrough(S, 6, TRUE);
    ssSetInputPortRequiredContiguous(S, 6, TRUE); /*direct input signal access*/
    
    /* Port 7 t for ukf */
    ssSetInputPortWidth(S, 7, 1);
    ssSetInputPortDataType(S, 7, SS_DOUBLE);
    ssSetInputPortDirectFeedThrough(S, 7, TRUE);
    ssSetInputPortRequiredContiguous(S, 7, TRUE); /*direct input signal access*/

    /* Port 8 h(step size)*/
    ssSetInputPortWidth(S, 8, 1);
    ssSetInputPortDataType(S, 8, SS_DOUBLE);
    ssSetInputPortDirectFeedThrough(S, 8, TRUE);
    ssSetInputPortRequiredContiguous(S, 8, TRUE); /*direct input signal access*/
    
    /* Port 9 sim time for UKF*/
    ssSetInputPortWidth(S, 9, 1);
    ssSetInputPortDataType(S, 9, SS_DOUBLE);
    ssSetInputPortDirectFeedThrough(S, 9, TRUE);
    ssSetInputPortRequiredContiguous(S, 9, TRUE); /*direct input signal access*/

	/*Port 10 predicted magnetometer for UKF*/
	ssSetInputPortWidth(S, 10, 3);
	ssSetInputPortDataType(S, 10, SS_DOUBLE);
	ssSetInputPortDirectFeedThrough(S, 10, TRUE);
	ssSetInputPortRequiredContiguous(S, 10, TRUE); /*direct input signal access*/

	/*Port 11 predicted sun measurement for UKF*/
	ssSetInputPortWidth(S, 11, 3);
	ssSetInputPortDataType(S, 11, SS_DOUBLE);
	ssSetInputPortDirectFeedThrough(S, 11, TRUE);
	ssSetInputPortRequiredContiguous(S, 11, TRUE); /*direct input signal access*/
    
    if (!ssSetNumOutputPorts(S, 5)) return;
	/*outputs
	state
	state_err_vec
	residual

	
	
	
	*/
    ///* Port 0 pos+vel(ECI)*/
    //ssSetOutputPortWidth(S, 0, 6);
    //ssSetOutputPortDataType(S, 0, SS_DOUBLE);

    // /* Port 1 pos(ECEF)*/
    // ssSetOutputPortWidth(S, 1, 3);
    // ssSetOutputPortDataType(S, 1, SS_DOUBLE);
    // 
    // /* Port 2 IGRF Mag field*/
    // ssSetOutputPortWidth(S, 2, 3);
    // ssSetOutputPortDataType(S, 2, SS_DOUBLE);

    // /* Port 3 sunpos*/
    // ssSetOutputPortWidth(S, 3, 3);
    // ssSetOutputPortDataType(S, 3, SS_DOUBLE);
    // 
    // /* Port 4 sunpos_monten*/
    // ssSetOutputPortWidth(S, 4, 3);
    // ssSetOutputPortDataType(S, 4, SS_DOUBLE);
     
	/* Port 5 UKF state */
	ssSetOutputPortWidth(S, 0, 13);
	ssSetOutputPortDataType(S, 0, SS_DOUBLE);

	/* Port 6 UKF st_error vector*/
	ssSetOutputPortWidth(S, 1, 12);
	ssSetOutputPortDataType(S, 1, SS_DOUBLE);

	/* Port 7 UKF residual vector*/
	ssSetOutputPortWidth(S, 2, 9);
	ssSetOutputPortDataType(S, 2, SS_DOUBLE);

	/* Port 10 mag vec measured */
	ssSetOutputPortWidth(S, 3, 3);
	ssSetOutputPortDataType(S, 3, SS_DOUBLE);

	/* Port 11 sun vec measured */
	ssSetOutputPortWidth(S, 4, 3);
	ssSetOutputPortDataType(S, 4, SS_DOUBLE);


     
    ssSetNumSampleTimes(S, 1);
    ssSetNumRWork(S, 0);
    ssSetNumIWork(S, 0);
    ssSetNumPWork(S, 0);
    ssSetNumModes(S, 0);
    ssSetNumNonsampledZCs(S, 0);

    ssSetOptions(S, 0);
  
    double XNO,XNDT2O, EPOCH;
	float XMO,XNODEO,OMEGAO,EO,XINCL,XNDD6O,BSTAR;
	double IEXP, IBEXP;
    int i;
    static int initialize = 0;
    double elapsed_time;
    
	// *******for igrf ** //
	double Nmax, coords[3], day, jd; //year; clash between igrf and sol2JD year type so commented igrf one out
	
	int month, year;
	VEC *Q_eci_ecef, *Q_ecef_eci;
	VEC *v1, *B;
	// ****************** //
    
    // *******for sunpos*** //
    VEC *r_Sun_mont, *r_Sun_aalb;
    // ****************** //
    
    // *******for UKF*** //
    static VEC *state = VNULL, *omega = VNULL, *sun_vec_m = VNULL, *mag_vec_m = VNULL, *Torq_ext = VNULL, *state_error = VNULL, *residual = VNULL;
    int P_flag;
    static float tslukfe = 0; //time since last ukf execution
    // ****************** //
    
 
  //**********************START INPUTS TO BLOCK**********************
  real_T *curr_time   = (real_T*) ssGetInputPortSignal(S,0);   
  real_T *tle_data   =  (real_T*) ssGetInputPortSignal(S,1);    
  real_T *s_v_m   = (real_T*) ssGetInputPortSignal(S,2);    
  real_T *m_v_m   = (real_T*) ssGetInputPortSignal(S,3);    
  real_T *om   =     (real_T*) ssGetInputPortSignal(S,4); 
  real_T *T_ext   =     (real_T*) ssGetInputPortSignal(S,5); 
  real_T *eclipse  = (real_T*) ssGetInputPortSignal(S,6); 
  real_T *t   =     (real_T*) ssGetInputPortSignal(S,7); 
  real_T *h   =     (real_T*) ssGetInputPortSignal(S,8); 
  real_T *sim_time   =     (real_T*) ssGetInputPortSignal(S,9); 
  real_T *pred_magn_I = (real_T*)ssGetInputPortSignal(S, 10);
  real_T *pred_sun_I = (real_T*)ssGetInputPortSignal(S, 11);
  
  //**********************END INPUTS TO BLOCK**********************
  
  //**********************START OUTPUTS FROM BLOCK**********************
  //real_T *result   = ssGetOutputPortSignal(S,0);
  //real_T *result_ECEF   = ssGetOutputPortSignal(S,1);
  ////   real_T *Q_eci2ecef   = ssGetOutputPortSignal(S,2);
  //real_T *Bv   = ssGetOutputPortSignal(S,2);
  //real_T *sunpos_aalb   = ssGetOutputPortSignal(S,3);
  //real_T *sunpos_monten   = ssGetOutputPortSignal(S,4);
  real_T *st_out   = ssGetOutputPortSignal(S,0);
  real_T *st_error   = ssGetOutputPortSignal(S,1);
  real_T *residual_out   = ssGetOutputPortSignal(S,2);
  real_T *mag_v_m = ssGetOutputPortSignal(S, 3);
  real_T *sun_v_m = ssGetOutputPortSignal(S, 4);

  //**********************END OUTPUTS FROM BLOCK**********************
  
  double result1[6];
  float kf, rkst;

  pred_simsun_I = v_get(3);
  pred_simmag_I = v_get(3);


  Q_eci_ecef = v_get(4);
  Q_ecef_eci = v_get(4);
  
  v1 = v_get(3);
  B = v_get(3);

  r_Sun_mont = v_get(3);
  r_Sun_aalb = v_get(3);
  //temp = get_mat(3,3);
    if (initialize == 0)
    {
        omega = v_get(3);
        mag_vec_m = v_get(3);
        sun_vec_m = v_get(3);
        Torq_ext = v_get(3);
        residual = v_get(9);
//         if(state == VNULL){
            state = v_get(13);
//         }
        state_error = v_get(12);
        residual = v_get(9);
        
        initialize = 1;
    }
    
    
    
  for(i=0; i<3; i++){
        omega->ve[i] = om[i];
        sun_vec_m->ve[i] = s_v_m[i];
        mag_vec_m->ve[i] = m_v_m[i];  
        Torq_ext->ve[i] = T_ext[i];
		mag_v_m[i] = mag_vec_m->ve[i];
		sun_v_m[i] = sun_vec_m->ve[i];

		pred_simsun_I->ve[i] = pred_sun_I[i];
		pred_simmag_I->ve[i] = pred_magn_I[i];
    }
  v_output(omega);
  
  
  /* extract TLE data */
/* Card 1 */
  EPOCH = tle_data[0];
  XNDT2O = tle_data[1];
  XNDD6O = tle_data[2];
  IEXP = tle_data[3]; 
  BSTAR = tle_data[4];
  IBEXP = tle_data[5];
/* Card 2 */
  XINCL = tle_data[6];
  XNODEO = tle_data[7];
  EO = tle_data[8];
  OMEGAO = tle_data[9];
  XMO = tle_data[10];
  XNO = tle_data[11];
  
//   printf("Epoch: %f\n", EPOCH);
//   printf("XNDT2O: %f\n", XNDT2O);
//   printf("XNDD6O: %f\n", XNDD6O);
//   printf("IEXP: %f\n", IEXP);
//   printf("BSTAR: %f\n", BSTAR);
//   printf("IBEXP: %f\n", IBEXP);
//   printf("XINCL: %f\n", XINCL);
//   printf("XNODEO: %f\n", XNODEO);
//   printf("EO: %f\n", EO);
//   printf("OMEGAO: %f\n", OMEGAO);
//   printf("XMO: %f\n", XMO);
//   printf("XNO: %f\n", XNO);
//   printf("Ok_time since: %f\n", ts[0]);

  
  //not required yet as the input is already converted to JD format by simulator
  //However, it works and will be neded in the final version.
  tle_epoch2JD(EPOCH, &year, &jd); 
	
  //printf("jd is: %lf", jd);//should be 2.455153195422520e+06 its correct
  

//*******************************SGP4 START******************************
  
  elapsed_time = (curr_time[0]-jd)*1440;  
  
  /* ts is -1 if tSinceEp is not ready. */
  /* epoch, first element in TLEdata, must be greater than zero */ 
  if((elapsed_time == 0) || (tle_data[0] == 0)) { 
 /*   printf("noresult: \n");
    result[0] = -1;
    result[1] = -1;
    result[2] = -1;
    result[3] = -1;
    result[4] = -1;
    result[5] = -1;*/
  }
  else {
    //printf("got the result: \n");
    sgp4( XMO, XNODEO, OMEGAO, EO, XINCL, XNO, XNDT2O, XNDD6O, BSTAR, EPOCH, IEXP, IBEXP, result1, elapsed_time);

        
        /*result[0] = result1[0];
        result[1] = result1[1];
        result[2] = result1[2];
        result[3] = result1[3];
        result[4] = result1[4];
        result[5] = result1[5];*/
  }
  
   
    v1->ve[0] = result1[0];
	v1->ve[1] = result1[1];
	v1->ve[2] = result1[2];

    
//*************************SGP4 END********************************
    //quaternion for rotating from eci to ecef:
	eci2ecef(curr_time[0], Q_eci_ecef); //curr_time(jd format) is used here  since that gives the current transformation

    //quaternion for rotating from ecef to eci:
    quat_inv(Q_eci_ecef, Q_ecef_eci); 
    
//     Q_eci2ecef[0] = Q_eci_ecef.X;
//     Q_eci2ecef[1] = Q_eci_ecef.Y;
//     Q_eci2ecef[2] = Q_eci_ecef.Z;
//     Q_eci2ecef[3] = Q_eci_ecef.W;
    
//     printf("quat is: %lf %lf %lf %lf \n",Q_eci_ecef.X, Q_eci_ecef.Y, Q_eci_ecef.Z,Q_eci_ecef.W);
    
    //rotate vec from eci to ecef
    quat_rot_vec(Q_eci_ecef, v1);
    
//****************************IGRF START*********************************
    Nmax = 8;
	/*setigrf(year);*/
    year = 2009.843204467626;
    setigrf(year);

// 	coords[0] = v1.X;
// 	coords[1] = v1.Y;
// 	coords[2] = v1.Z;
    
    coords[0] = v1->ve[0]/(6371.2);
    coords[1] = v1->ve[1]/(6371.2);
    coords[2] = v1->ve[2]/(6371.2);
    
    /*result_ECEF[0] = v1->ve[0];
    result_ECEF[1] = v1->ve[1];
    result_ECEF[2] = v1->ve[2];*/

	igrfmodel(B, coords, &Nmax);
    /*convert the field vector to gauss*/
    B->ve[0] = 1e4*B->ve[0];
    B->ve[1] = 1e4*B->ve[1];
    B->ve[2] = 1e4*B->ve[2];
    quat_rot_vec(Q_ecef_eci, B);
    /*convert the field vector to nT*/
    B->ve[0] = 1e-4*B->ve[0];
    B->ve[1] = 1e-4*B->ve[1];
    B->ve[2] = 1e-4*B->ve[2];    
    
    //Bv[0] = B->ve[0];
    //Bv[1] = B->ve[1];
    //Bv[2] = B->ve[2];

//*******************************IGRF END*********************************
    
    
//********************************SUNPOS START****************************
    
    sunpos(curr_time[0] , r_Sun_aalb);
    
    //sunpos_aalb[0] = r_Sun_aalb->ve[0];
    //sunpos_aalb[1] = r_Sun_aalb->ve[1];
    //sunpos_aalb[2] = r_Sun_aalb->ve[2];
    
    Sun_monten(curr_time[0], r_Sun_mont);
    
    //sunpos_monten[0] = r_Sun_mont->ve[0];
    //sunpos_monten[1] = r_Sun_mont->ve[1];
    //sunpos_monten[2] = r_Sun_mont->ve[2];
    
    
//*********************************SUNPOS END*****************************
    if(tslukfe == 0){
        tslukfe = sim_time[0];
    }


	if(eclipse[0]==1)
	{
		kf = 0.05;//0.25;
		rkst = 0.05;//0.25;
	}
	else
	{
		kf = 0.05;//0.25;
		rkst = 0.05;//0.25;
	}
	

    if(sim_time[0] >= (tslukfe + kf)){//+0.25

		//printf("time since last ukf exec: %lf \n",tslukfe);
  
		Ukf(omega, mag_vec_m, /*pred_simmag_I*/B, sun_vec_m, /*pred_simsun_I*/r_Sun_aalb, Torq_ext, t[0], rkst/*h[0]*/, eclipse[0], state, state_error, residual, &P_flag, sim_time[0]);
        tslukfe = sim_time[0];
        
        for(i=0; i<13; i++){
            st_out[i] = state->ve[i];
        }
    
        for(i=0; i<12; i++){
            st_error[i] = state_error->ve[i];
        }
    
        for(i=0; i<9; i++){
            residual_out[i] = residual->ve[i];
        }

        //P_flag_out = P_flag;
    }


    /*********************************CALL UKF END*****************************/

    }



#define MDL_UPDATE  /* Change to #undef to remove function */
#if defined(MDL_UPDATE)
  /* Function: mdlUpdate ======================================================
   * Abstract:
   *    This function is called once for every major integration time step.
   *    Discrete states are typically updated here, but this function is useful
   *    for performing any tasks that should only take place once per
   *    integration step.
   */
static void mdlUpdate(SimStruct *S, int_T tid) {
}
#endif /* MDL_UPDATE */



#define MDL_DERIVATIVES  /* Change to #undef to remove function */
#if defined(MDL_DERIVATIVES)
  /* Function: mdlDerivatives =================================================
   * Abstract:
   *    In this function, you compute the S-function block's derivatives.
   *    The derivatives are placed in the derivative vector, ssGetdX(S).
   */
  static void mdlDerivatives(SimStruct *S)
  {

    
    }
#endif /* MDL_DERIVATIVES */



/* Function: mdlTerminate =====================================================
 * Abstract:
 *    In this function, you should perform any actions that are necessary
 *    at the termination of a simulation.  For example, if memory was
 *    allocated in mdlStart, this is the place to free it.
 */
static void mdlTerminate(SimStruct *S)
{
  //closefile();
}


/*======================================================*
 * See sfuntmpl_doc.c for the optional S-function methods *
 *======================================================*/

/*=============================*
 * Required S-function trailer *
 *=============================*/

#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif


