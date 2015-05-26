/*
 * ADS.c
 *
 *  Created on: Jun 19, 2014
 *      Author: nemanja
 */

#include "ADS.h"
#include "sci.h"
#include "ACS.h"

#include "FreeRTOS.h"
#include "os_task.h"
#include "os_queue.h"
#include "os_semphr.h"
#include "os_event_groups.h"

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

TaskDescriptor_t tdADS = {
		2, // Queue number
		0, // Semaphore number
		1, // Events group number
		2, // Priority
		StateDetermination, // Work function
		InitADS, // Initialization function
		5500, // Stack Size
		NULL, // Register commands function
		NULL,
		pdFAIL
};


/*extern QueueHandle_t stateQ;
extern QueueHandle_t BfieldQ;
extern EventGroupHandle_t torqueSignalEG;*/

static VEC *state = VNULL, *omega = VNULL, *sun_vec_m = VNULL, *mag_vec_m = VNULL, *Torq_ext = VNULL, *state_error = VNULL, *residual = VNULL;

void a_delay()
{
	int d = 0;
	while(d < 50)
	{
		d++;
	}
}

void InitADS(void *pvParams)
{
	// Create the tasks
	//xTaskCreate(SensorRead, "SensorRead", 500, NULL, PRIORITY_NORMAL, NULL);
	//xTaskCreate(StateDetermination, "StateDetermination", 200, NULL, PRIORITY_NORMAL, NULL);
}

void StateDetermination(void *pvParams)
{
	// Initialize timer, 2Hz period
	TickType_t readFrequency = 1000;
	TickType_t wakeUpTime = xTaskGetTickCount();

	// state variable
	//double state[8];
	uint8 buff[16];

	// added
	double *curr_time = pvPortMalloc(sizeof(double));
	curr_time[0] = 2455153.1954318;
	int *eclipse = pvPortMalloc(sizeof(int));
	eclipse[0] = 0;
	double kf = 0;
	double rkst;
	//double *m_v_m = pvPortMalloc(sizeof(double));
	//double *mag_v_m = pvPortMalloc(sizeof(double));
	//double *s_v_m = pvPortMalloc(sizeof(double));
	//double *om = pvPortMalloc(sizeof(double));
	double *sim_time = pvPortMalloc(sizeof(double));

	double *result1 = pvPortMalloc(sizeof(double)*6);
	double *st_out = pvPortMalloc(sizeof(double)*13);
	//double st_out[13];
	//double *sun_v_m = pvPortMalloc(sizeof(double));
	//double *T_ext = pvPortMalloc(sizeof(double)*3);
	double *t = pvPortMalloc(sizeof(double));
	t[0] = 0.0;
	// end added

	double XNO,XNDT2O, EPOCH;
	float XMO,XNODEO,OMEGAO,EO,XINCL,XNDD6O,BSTAR;
	double IEXP, IBEXP;
	int i;
	double elapsed_time;

	// *******for igrf ** //
	double Nmax, coords[3], jd; //year; clash between igrf and sol2JD year type so commented igrf one out

	int year;
	VEC *Q_eci_ecef, *Q_ecef_eci;
	VEC *v1, *B;
	// ****************** //

	// *******for sunpos*** //
	VEC *r_Sun_mont, *r_Sun_aalb;
	// ****************** //

	// *******for UKF*** //

	int P_flag;
	static float tslukfe = 0; //time since last ukf execution
	// ****************** //

	Q_eci_ecef = v_get(4);
	Q_ecef_eci = v_get(4);

	v1 = v_get(3);
	B = v_get(3);

	r_Sun_mont = v_get(3);
	r_Sun_aalb = v_get(3);

	omega = v_get(3);
	mag_vec_m = v_get(3);
	sun_vec_m = v_get(3);
	Torq_ext = v_get(3);
	residual = v_get(9);
	state = v_get(13);
	state_error = v_get(12);


	//sensor settings fixed
	omega->ve[0] = 0.05;
	omega->ve[1] = 0.0;
	omega->ve[2] = 0.0;
	mag_vec_m->ve[0] = 0.0;
	mag_vec_m->ve[1] = 0.5;
	mag_vec_m->ve[2] = 0.2;
	sun_vec_m->ve[0] = 0.0;
	sun_vec_m->ve[1] = 0.0;
	sun_vec_m->ve[2] = 1.0;
	Torq_ext->ve[0] = 0.01;
	Torq_ext->ve[1] = 0.0;
	Torq_ext->ve[2] = 0.01;


	unsigned int free_space = xPortGetFreeHeapSize();

	// extract TLE data
	// Card 1
	  EPOCH = 09321.69542252;
	  XNDT2O = 0.00000555;
	  XNDD6O = 0.0;
	  IEXP = 0;
	  BSTAR = 0.76898e-4;
	  IBEXP = 0;
	// Card 2
	  XINCL = 97.9251*0+98.6;
	  XNODEO = 24.1597;
	  EO = 0.000147;
	  OMEGAO = 254.1774;
	  XMO = 105.7862;
	  XNO = 14.81823818*0+15.65;


	while(1)
	{
		  sim_time[0] = (double)xTaskGetTickCount();

		  //not required yet as the input is already converted to JD format by simulator
		  //However, it works and will be neded in the final version.
		  tle_epoch2JD(EPOCH, &year, &jd);

		  //printf("jd is: %lf", jd);//should be 2.455153195422520e+06 its correct


		//*******************************SGP4 START******************************

		  elapsed_time = (curr_time[0]-jd)*1440;

		  // ts is -1 if tSinceEp is not ready.
		  // epoch, first element in TLEdata, must be greater than zero
		    //printf("got the result: \n");
		    sgp4( XMO, XNODEO, OMEGAO, EO, XINCL, XNO, XNDT2O, XNDD6O, BSTAR, EPOCH, IEXP, IBEXP, result1, elapsed_time);

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


		// 	coords[0] = v1.X;
		// 	coords[1] = v1.Y;
		// 	coords[2] = v1.Z;

		    coords[0] = v1->ve[0]/(6371.2);
		    coords[1] = v1->ve[1]/(6371.2);
		    coords[2] = v1->ve[2]/(6371.2);

		    //result_ECEF[0] = v1->ve[0];
		    //result_ECEF[1] = v1->ve[1];
		    //result_ECEF[2] = v1->ve[2];

			igrfmodel(B, coords, &Nmax);
		    //convert the field vector to gauss
		    B->ve[0] = 1e4*B->ve[0];
		    B->ve[1] = 1e4*B->ve[1];
		    B->ve[2] = 1e4*B->ve[2];
		    quat_rot_vec(Q_ecef_eci, B);
		    //convert the field vector to nT
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


		    if(sim_time[0] >= (tslukfe + kf))
		    {


				Ukf(omega, mag_vec_m, B, sun_vec_m, r_Sun_aalb, Torq_ext, t[0], rkst, eclipse[0], state, state_error, residual, &P_flag, sim_time[0]);
		        tslukfe = sim_time[0];

		        for(i=0; i<13; i++){
		            st_out[i] = state->ve[i];
		        }
		    }

		    xQueueOverwrite(HIL_send, st_out);

		vTaskDelayUntil(&wakeUpTime, readFrequency);
	}
}


/*vTaskDelay(5);
taskENTER_CRITICAL();
buff[0] = i2cRegisterRead(4, 4);
buff[1] = i2cRegisterRead(4, 5);
buff[2] = i2cRegisterRead(4, 6);
buff[3] = i2cRegisterRead(4, 7);
buff[4] = i2cRegisterRead(4, 8);
buff[5] = i2cRegisterRead(4, 9);
buff[6] = i2cRegisterRead(4, 10);
buff[7] = i2cRegisterRead(4, 11);
buff[8] = i2cRegisterRead(4, 12);
buff[9] = i2cRegisterRead(4, 13);
buff[10] = i2cRegisterRead(4, 14);
buff[11] = i2cRegisterRead(4, 15);
buff[12] = i2cRegisterRead(4, 16);
buff[13] = i2cRegisterRead(4, 17);
buff[14] = i2cRegisterRead(4, 18);
buff[15] = i2cRegisterRead(4, 19);
taskEXIT_CRITICAL();
vTaskDelay(5);

state[0] = ((double)((buff[0]<<8) + buff[1] - 32768))*((double)1/32768);
state[1] = ((double)((buff[2]<<8) + buff[3] - 32768))*((double)1/32768);
state[2] = ((double)((buff[4]<<8) + buff[5] - 32768))*((double)1/32768);
state[3] = ((double)((buff[6]<<8) + buff[7] - 32768))*((double)1/32768);

state[4] = ((double)((buff[8]<<8) + buff[9] - 32768))*((double)0.4/32768);
state[5] = ((double)((buff[10]<<8) + buff[11] - 32768))*((double)0.4/32768);
state[6] = ((double)((buff[12]<<8) + buff[13] - 32768))*((double)0.4/32768);

state[7] = ((double)((buff[14]<<8) + buff[15]))*((double)0.1/65536);

// Dispatch the state values
xQueueOverwrite(stateQ, state);*/
