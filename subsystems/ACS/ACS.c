#include "ACS.h"
#include "FreeRTOS.h"
#include "os_task.h"
#include "os_queue.h"
#include "os_semphr.h"
#include "os_event_groups.h"

#include "matrix2.h"
#include "sci.h"
#include "i2c.h"
#include "FreeRTOS_CLI.h"

#include "TaskManaging.h"

typedef union
{
	double dbl;
	uint8 byte[8];
} DoubleAndBytes;

// Vector of diagonal inertia matrix coefficients
static VEC *Iv;

// orbit angular velocity
static Real w0;

// Q and R matrices for LQR
static MAT *Ql;
static MAT *Rl;
static MAT *invRl;
// Q and R matrices for SDRE
static MAT *Qs;
static MAT *Rs;
static MAT *invRs;

/*QueueHandle_t controlSignalQ;
QueueHandle_t stateQ;
QueueHandle_t BfieldQ;

EventGroupHandle_t torqueSignalEG;*/

TaskDescriptor_t tdACS = {
		3, // Queue number
		0, // Semaphore number
		1, // Events group number
		2, // Priority
		CalculateSignal, // Work function
		InitACS, // Initialization function
		20000//, // Stack Size
		//ADCSCommands // Register commmands function
		//NULL,
		//pdFALSE
};

/*CLI_Command_Definition_t xACSSetPeriod =
{
	"acsSetPeriod",
	"\r\nacsSetPeriod T:\r\n Set control signal calculation period T [ms]\r\n\r\n",
	prvACSSetPeriod,
	1
};

void ADCSCommands(void *pvParams)
{
	FreeRTOS_CLIRegisterCommand(&xACSSetPeriod);
}

static BaseType_t prvACSSetPeriod( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
	int T;
	BaseType_t len;
	char *param_p;
	param_p = (char *)FreeRTOS_CLIGetParameter(pcCommandString, 1, &len);
	sscanf(param_p, "%d", &T);



	return pdFALSE;
}*/

void InitACS(void *pvParams)
{
	// Set diagonal coefficients of inertia matrix
	Iv = v_get(3);
	Iv->ve[0] = 0.007;
	Iv->ve[1] = 0.014;
	Iv->ve[2] = 0.015;

	// Set orbit angular velocity
	// TODO include calculation from velocity and position w0 = (p/(||p||^2)) x v, or just w0 = v/r
	// 380km
	w0 = 0.001138195;

	// Q and R matrices setting
	Ql = m_get(6, 6);
	m_zero(Ql);
	Ql->me[0][0] = 10;
	Ql->me[1][1] = 100;
	Ql->me[2][2] = 100;
	Ql->me[3][3] = 10e-5;
	Ql->me[4][4] = 10e-5;
	Ql->me[5][5] = 10e-5;

	Rl = m_get(3, 3);
	m_zero(Rl);
	Rl->me[0][0] = 1000;
	Rl->me[1][1] = 1000;
	Rl->me[2][2] = 1000;

	invRl = m_get(3, 3);
	m_zero(invRl);
	invRl->me[0][0] = 1/Rl->me[0][0];
	invRl->me[1][1] = 1/Rl->me[1][1];
	invRl->me[2][2] = 1/Rl->me[2][2];

	Qs = m_get(6, 6);
	m_zero(Qs);
	Qs->me[0][0] = 10;
	Qs->me[1][1] = 100;
	Qs->me[2][2] = 100;
	Qs->me[3][3] = 10e-5;
	Qs->me[4][4] = 10e-5;
	Qs->me[5][5] = 10e-5;

	Rs = m_get(3, 3);
	m_zero(Rs);
	Rs->me[0][0] = 100;
	Rs->me[1][1] = 100;
	Rs->me[2][2] = 100;

	invRs = m_get(3, 3);
	m_zero(invRs);
	invRs->me[0][0] = 1/Rs->me[0][0];
	invRs->me[1][1] = 1/Rs->me[1][1];
	invRs->me[2][2] = 1/Rs->me[2][2];

	// Create queues
	/*controlSignalQ = xQueueCreate(1, 3*sizeof(Real));
	stateQ = xQueueCreate(1, 8*sizeof(Real));
	BfieldQ = xQueueCreate(1, 3*sizeof(Real));

	// Create event group
	torqueSignalEG = xEventGroupCreate();

	// Create the tasks
	xTaskCreate(CalculateSignal, "CalculateSignal", 20000, NULL, PRIORITY_NORMAL, NULL);*/
	//xTaskCreate(TorqueControl, "TorqueControl", 200, NULL, PRIORITY_HIGH, NULL);
}

void skew(VEC *v, MAT *M)
{
	M->me[0][0] = 0.0;
	M->me[0][1] = -v->ve[2];
	M->me[0][2] = v->ve[1];
	M->me[1][0] = v->ve[2];
	M->me[1][1] = 0.0;
	M->me[1][2] = -v->ve[0];
	M->me[2][0] = -v->ve[1];
	M->me[2][1] = v->ve[0];
	M->me[2][2] = 0.0;
}

void ReadMatrixSCI(int m, int n, MAT *M)
{
	int i, j;
	uint8 *buffer = pvPortMalloc(8*m*n*sizeof(uint8));
	sciReceive(scilinREG, n*m*8, buffer);

	for(i = 0; i < m; i++)
	{
		for(j = 0; j < n; j++)
		{
			M->me[i][j] = *((double *)(buffer+(i*n+j)*8));
		}
	}
	vPortFree(buffer);
}

void WriteMatrixSCI(int m, int n, MAT *M)
{
	int i, j, k;
	DoubleAndBytes tmp;

	for(i = 0; i < m; i++)
	{
		for(j = 0; j < n; j++)
		{
			tmp.dbl = M->me[i][j];
			for(k = 0; k < 8; k++)
			{
				sciSendByte(scilinREG, tmp.byte[k]);
			}
		}
	}
}

void InputMatrix(VEC *Bf, VEC *Iv, MAT *B)
{
	m_zero(B);
	B->me[3][1] = Bf->ve[2]/Iv->ve[0];
	B->me[3][2] = -Bf->ve[1]/Iv->ve[0];
	B->me[4][0] = -Bf->ve[2]/Iv->ve[1];
	B->me[4][2] = Bf->ve[0]/Iv->ve[1];
	B->me[5][0] = Bf->ve[1]/Iv->ve[2];
	B->me[5][1] = -Bf->ve[0]/Iv->ve[2];
}

void LinearSystemMatrix(VEC *Iv, Real w0, MAT *A)
{
	Real k1, k2, k3;
	Real Fd = 2.0*2.0*9.9550/1e12*(7670.95*7670.95)*0.02/2.0*0.1*0.1;

	k1 = (Iv->ve[1]-Iv->ve[2])/Iv->ve[0];
	k2 = (Iv->ve[2]-Iv->ve[0])/Iv->ve[1];
	k3 = (Iv->ve[0]-Iv->ve[1])/Iv->ve[2];
	m_zero(A);
	A->me[0][3] = 0.5;
	A->me[1][4] = 0.5;
	A->me[2][5] = 0.5;
	A->me[3][0] = -8.0*k1*w0*w0;
	A->me[3][5] = w0*(1.0-k1);
	A->me[4][1] = 6.0*k2*w0*w0 - Fd/Iv->ve[1];
	A->me[5][2] = 2.0*k3*w0*w0 - Fd/Iv->ve[2];
	A->me[5][3] = -w0*(1+k3);
}

void NonlinearSystemMatrix(VEC *Iv, Real w0, VEC *state, MAT *A)
{
	MAT *tmpM1 = m_get(3, 3);
	MAT *tmpM2 = m_get(3, 3);
	VEC *tmpV = v_get(3);
	MAT *As;
	Real r;
	Real tmpR;
	Real Fd = 2.0*2.0*9.9550/1e12*0.03*0.1*(7670.95*7670.95)*0.04/2.0;

	As = NEW(MAT);

	// submatrix A11
	As = sub_mat(A, 0, 0, 2, 2, As);
	As->me[0][0] = 0.0;
	As->me[0][1] = state->ve[6]/2.0;
	As->me[0][2] = -state->ve[5]/2.0;
	As->me[1][0] = -state->ve[6]/2.0;
	As->me[1][1] = 0.0;
	As->me[1][2] = state->ve[4]/2.0;
	As->me[2][0] = state->ve[5]/2.0;
	As->me[2][1] = -state->ve[4]/2.0;
	As->me[2][2] = 0.0;

	// submatrix A12
	As = sub_mat(A, 0, 3, 2, 5, As);
	As->me[0][0] = state->ve[3]/2.0;
	As->me[0][1] = 0.0;
	As->me[0][2] = 0.0;
	As->me[1][0] = 0.0;
	As->me[1][1] = state->ve[3]/2.0;
	As->me[1][2] = 0.0;
	As->me[2][0] = 0.0;
	As->me[2][1] = 0.0;
	As->me[2][2] = state->ve[3]/2.0;

	// submatrix A21
	r = 1.0/(1.0-state->ve[3]*state->ve[3]);
	As = sub_mat(A, 3, 0, 5, 2, As);

	tmpM1->me[0][0] = 2.0*state->ve[1];
	tmpM1->me[0][1] = 0.0;
	tmpM1->me[0][2] = 2.0*state->ve[3];
	tmpM1->me[1][0] = state->ve[0]*(r-2.0);
	tmpM1->me[1][1] = state->ve[1]*r;
	tmpM1->me[1][2] = state->ve[2]*(r-2.0);
	tmpM1->me[2][0] = -2.0*state->ve[3];
	tmpM1->me[2][1] = 2.0*state->ve[2];
	tmpM1->me[2][2] = 0.0;

	tmpM2->me[0][0] = Iv->ve[0];
	tmpM2->me[0][1] = 0.0;
	tmpM2->me[0][2] = 0.0;
	tmpM2->me[1][0] = 0.0;
	tmpM2->me[1][1] = Iv->ve[1];
	tmpM2->me[1][2] = 0.0;
	tmpM2->me[2][0] = 0.0;
	tmpM2->me[2][1] = 0.0;
	tmpM2->me[2][2] = Iv->ve[2];

	As = m_mlt(tmpM2, tmpM1, As);

	tmpV->ve[0] = 2.0*(state->ve[0]*state->ve[1] + state->ve[2]*state->ve[3]);
	tmpV->ve[1] = -state->ve[0]*state->ve[0]+state->ve[1]*state->ve[1]
			-state->ve[2]*state->ve[2]+state->ve[3]*state->ve[3];
	tmpV->ve[2] = 2.0*(state->ve[1]*state->ve[2] - state->ve[0]*state->ve[3]);

	skew(tmpV, tmpM1);
	tmpM2 = m_mlt(tmpM1, As, tmpM2);

	tmpR = -state->ve[0]*state->ve[0]-state->ve[1]*state->ve[1]+state->ve[2]*state->ve[2]+state->ve[3]*state->ve[3];
	tmpM1->me[0][0] = -6.0*(Iv->ve[1]-Iv->ve[2])*state->ve[3]*tmpR;
	tmpM1->me[0][1] = 0.0;
	tmpM1->me[0][2] = -6.0*(Iv->ve[1]-Iv->ve[2])*state->ve[1]*tmpR;
	tmpM1->me[1][0] = -6.0*(Iv->ve[2]-Iv->ve[0])*state->ve[2]*tmpR;
	tmpM1->me[1][1] = 6.0*(Iv->ve[2]-Iv->ve[0])*state->ve[3]*tmpR;
	tmpM1->me[1][2] = 0.0;
	tmpM1->me[2][0] = 0.0;
	tmpM1->me[2][1] = 12.0*(Iv->ve[0]-Iv->ve[1])*state->ve[3]*(state->ve[1]*state->ve[2]-state->ve[0]*state->ve[3]);
	tmpM1->me[2][2] = -12.0*(Iv->ve[0]-Iv->ve[1])*state->ve[0]*(state->ve[1]*state->ve[2]-state->ve[0]*state->ve[3]);

	tmpM2 = m_sub(tmpM1, tmpM2, tmpM2);

	tmpM1->me[0][0] = 1.0/Iv->ve[0];
	tmpM1->me[0][1] = 0.0;
	tmpM1->me[0][2] = 0.0;
	tmpM1->me[1][0] = 0.0;
	tmpM1->me[1][1] = 1.0/Iv->ve[1];
	tmpM1->me[1][2] = 0.0;
	tmpM1->me[2][0] = 0.0;
	tmpM1->me[2][1] = 0.0;
	tmpM1->me[2][2] = 1.0/Iv->ve[2];

	As = m_mlt(tmpM1, tmpM2, As);
	As = sm_mlt(w0*w0, As, As);

	m_zero(tmpM1);
	tmpM1->me[1][0] = -Fd*state->ve[2]/Iv->ve[1];
	tmpM1->me[1][1] = -Fd*state->ve[3]/Iv->ve[1];
	tmpM1->me[2][1] =  Fd*state->ve[0]/Iv->ve[2];
	tmpM1->me[2][2] = -Fd*state->ve[3]/Iv->ve[2];

	As = m_add(tmpM1, As, As);

	// submatrix A22
	As = sub_mat(A, 3, 3, 5, 5, As);
	tmpV->ve[0] = state->ve[4];
	tmpV->ve[1] = state->ve[5];
	tmpV->ve[2] = state->ve[6];
	skew(tmpV, tmpM1);

	tmpV->ve[0] = 2.0*(state->ve[0]*state->ve[1] + state->ve[2]*state->ve[3]);
	tmpV->ve[1] = -state->ve[0]*state->ve[0]+state->ve[1]*state->ve[1]-state->ve[2]*state->ve[2]+state->ve[3]*state->ve[3];
	tmpV->ve[2] = 2.0*(state->ve[1]*state->ve[2]-state->ve[0]*state->ve[3]);
	skew(tmpV, tmpM2);

	tmpM2 = sm_mlt(w0, tmpM2, tmpM2);

	tmpM1 = m_sub(tmpM2, tmpM1, tmpM1);

	tmpM1->me[0][1] = tmpM1->me[0][1]*Iv->ve[1]/Iv->ve[0];
	tmpM1->me[0][2] = tmpM1->me[0][2]*Iv->ve[2]/Iv->ve[0];
	tmpM1->me[1][0] = tmpM1->me[1][0]*Iv->ve[0]/Iv->ve[1];
	tmpM1->me[1][2] = tmpM1->me[1][2]*Iv->ve[2]/Iv->ve[1];
	tmpM1->me[2][0] = tmpM1->me[2][0]*Iv->ve[0]/Iv->ve[2];
	tmpM1->me[2][1] = tmpM1->me[2][1]*Iv->ve[1]/Iv->ve[2];

	As = m_add(tmpM2, tmpM1, As);

	tmpV->ve[0] = tmpV->ve[0]*Iv->ve[0];
	tmpV->ve[1] = tmpV->ve[1]*Iv->ve[1];
	tmpV->ve[2] = tmpV->ve[2]*Iv->ve[2];
	skew(tmpV, tmpM1);
	tmpM1->me[0][0] = tmpM1->me[0][0]/Iv->ve[0]*w0;
	tmpM1->me[0][1] = tmpM1->me[0][1]/Iv->ve[0]*w0;
	tmpM1->me[0][2] = tmpM1->me[0][2]/Iv->ve[0]*w0;
	tmpM1->me[1][0] = tmpM1->me[1][0]/Iv->ve[1]*w0;
	tmpM1->me[1][1] = tmpM1->me[1][1]/Iv->ve[1]*w0;
	tmpM1->me[1][2] = tmpM1->me[1][2]/Iv->ve[1]*w0;
	tmpM1->me[2][0] = tmpM1->me[2][0]/Iv->ve[2]*w0;
	tmpM1->me[2][1] = tmpM1->me[2][1]/Iv->ve[2]*w0;
	tmpM1->me[2][2] = tmpM1->me[2][2]/Iv->ve[2]*w0;

	As = m_sub(As, tmpM1, As);

	m_free(tmpM1);
	m_free(tmpM2);
	v_free(tmpV);
	free(As);
}

void Bdot(VEC* Bf, Real K, VEC *control_signal)
{
	static Real prevBf[3];
	control_signal->ve[0] = -(Bf->ve[0]-prevBf[0])*K;
	control_signal->ve[1] = -(Bf->ve[1]-prevBf[1])*K;
	control_signal->ve[2] = -(Bf->ve[2]-prevBf[2])*K;

	prevBf[0] = Bf->ve[0];
	prevBf[1] = Bf->ve[1];
	prevBf[2] = Bf->ve[2];
}

void PD(VEC* state, VEC* Bf, Real P, Real D, VEC *control_signal)
{
	Real T[3];
	Real Bs = Bf->ve[0]*Bf->ve[0]+Bf->ve[1]*Bf->ve[1]+Bf->ve[2]*Bf->ve[2];
	T[0] = -P*state->ve[0]-D*state->ve[4];
	T[1] = -P*state->ve[1]-D*state->ve[5];
	T[2] = -P*state->ve[2]-D*state->ve[6];

	control_signal->ve[0] = (Bf->ve[1]*T[2]-Bf->ve[2]*T[1])/Bs;
	control_signal->ve[1] = (Bf->ve[2]*T[0]-Bf->ve[0]*T[2])/Bs;
	control_signal->ve[2] = (Bf->ve[0]*T[1]-Bf->ve[1]*T[0])/Bs;
}

void Riccati2Control(MAT *X, MAT *invR, MAT *B, VEC *state, VEC *control_signal)
{
	MAT *tmpM1 = m_get(3,6);
	MAT *tmpM2 = m_get(3,6);
	VEC *s = v_get(6);

	s->ve[0] = -state->ve[0];
	s->ve[1] = -state->ve[1];
	s->ve[2] = -state->ve[2];
	s->ve[3] = -state->ve[4];
	s->ve[4] = -state->ve[5];
	s->ve[5] = -state->ve[6];

	tmpM1 = m_transp(B, tmpM1);
	tmpM2 = m_mlt(invR, tmpM1, tmpM2);
	tmpM1 = m_mlt(tmpM2, X, tmpM1);

	control_signal = mv_mlt(tmpM1, s, control_signal);

	m_free(tmpM1);
	m_free(tmpM2);
	v_free(s);
}

double CheckAMRE(MAT *A, MAT *B, MAT *Q, MAT *invR, MAT *X)
{
	double fn;

	MAT *T1 = m_get(6, 6);
	MAT *T2 = m_get(6, 6);
	MAT *T3 = m_get(6, 6);
	MAT *T4 = m_get(6, 6);

	T1 = m_transp(A, T1);
	T2 = m_mlt(X, A, T2);
	T3 = m_mlt(T1, X, T3);
	T1 = m_add(T2, T3, T1);
	T1 = m_add(T1, Q, T1);

	T2 = m_mlt(X, B, T2);
	T3 = m_mlt(T2, invR, T3);

	T4 = m_transp(B, T4);

	T2 = m_mlt(T3, T4, T2);
	T3 = m_mlt(T2, X, T3);
	T3 = sm_mlt(-1, T3, T3);

	T1 = m_add(T1, T3, T1);

	fn = m_norm_frob(T1);

	m_free(T1);
	m_free(T2);
	m_free(T3);
	m_free(T4);

	return fn;
}

void CalculateSignal(void *pvParams)
{
	VEC *control_signal = v_get(3);
	VEC *Bfield = v_get(3);
	VEC *state = v_get(8);

	double iter = 20;
	double ill;
	double check;

	MAT *A = m_get(6,6);
	MAT *B = m_get(6,3);
	MAT *X0 = m_get(6,6);
	MAT *X = m_get(6,6);

	TickType_t t1, t2, t3;

	uint8 buff[5];
	int amre;


	iter = 20;
	while(1)
	{
		if(uxQueueMessagesWaiting(BfieldQ) && uxQueueMessagesWaiting(stateQ))
		{
			xQueueReceive(BfieldQ, Bfield->ve, 0);
			xQueueReceive(stateQ, state->ve, 0);

			InputMatrix(Bfield, Iv, B);
			LinearSystemMatrix(Iv, w0, A);
			//NonlinearSystemMatrix(Iv, w0, state, A);
			t1 = xTaskGetTickCount();
			//if(iter > 5)
			//{
				schur_are(A, B, Ql, Rl, X0, &ill);
			//}
			//else
			//{
			//	m_copy(X, X0);
			//}
			t2 = xTaskGetTickCount();
			kleinman(A, B, Ql, Rl, X0, X, &iter);
			t3 = xTaskGetTickCount();
			/*if(iter > 5)
				schur_are(A, B, Ql, Rl, X, &ill);
			else
				m_copy(X, X0);*/

			check = CheckAMRE(A, B, Ql, invRl, X);
			amre = ((int)(check*100));
			if(amre > 255)
				amre = 255;
			if(check < 0.5)
			{
				Riccati2Control(X, invRl, B, state, control_signal);
			}
			else
			{
				control_signal->ve[0] = control_signal->ve[0]*0.8;
				control_signal->ve[1] = control_signal->ve[1]*0.8;
				control_signal->ve[2] = control_signal->ve[2]*0.8;
			}
			//Bdot(Bfield, 15000, control_signal);
			//PD(state, Bfield, 5E-7, 1E-4, control_signal);

			t1 = t2-t1;
			t2 = t3-t2;
			taskENTER_CRITICAL();
			i2cSetSlaveAdd(i2cREG1, 4);
			buff[0] = 51;
			buff[1] = ((uint8)t1);
			buff[2] = ((uint8)t2);
			buff[3] = ((uint8)ill);
			buff[4] = ((uint8)amre);
			i2cSetMode(i2cREG1, I2C_MASTER | I2C_TRANSMITTER | I2C_STOP_COND);
			i2cSetCount(i2cREG1, 5);
			i2cSetStart(i2cREG1);
			i2cSend(i2cREG1, 5, buff);
			taskEXIT_CRITICAL();
			xQueueOverwrite(controlSignalQ, control_signal->ve);
			xEventGroupSetBits(torqueSignalEG, 1<<0);
		}

	}
}


