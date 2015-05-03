#include "TorqueControl.h"

#include "FreeRTOS.h"
#include "os_task.h"
#include "os_queue.h"
#include "os_semphr.h"
#include "os_event_groups.h"


#include "matrix2.h"
#include "i2c.h"



TaskDescriptor_t tdTC = {
		1, // Queue number
		0, // Semaphore number
		1, // Events group number
		2, // Priority
		TorqueControl, // Work function
		InitTC, // Initialization function
		500, // Stack Size
		NULL, // Register commmands function
		NULL,
		pdFALSE
};


#define controlSignalQ (tdTC.queue[0])
#define torqueSignalEG (tdTC.event[0])

void InitTC(void *pvParams)
{

}

void TorqueControl(void *pvParams)
{
	uint8 buff[10];
	int i = 0;
	int d = 0;
	EventBits_t uxBits;
	const TickType_t xTicksToWait = 5000;

	enum {ON, OFF} torquer_state = OFF;

	Real control_signal_r[3];
	uint8 control_signal[3];

	double state[13];
	char output[20];

	while(1)
	{
		if(xQueueReceive(controlSignalQ, state, 5000) == pdTRUE)
		{
			snprintf(output,50,"%f %f %f\n\0", state[0], state[1], state[2]);
			cliPrintf(output);
		}
		else
		{
			vTaskDelay(20);
		}
		/*d=0;
		uxBits = xEventGroupWaitBits(torqueSignalEG, 1<<0 | 1<<1 | 1<<2, pdTRUE, pdFALSE, xTicksToWait);

		if( ( uxBits & 1<<0 ) != 0 )	// update signal
		{
			if(xQueueReceive(controlSignalQ, control_signal_r, 0) == pdTRUE)
			{
				// scale in range of 0 to 100 % of 0.2 torquer maximum
				if(control_signal_r[0] > 0.2)
				{
					control_signal_r[0] = 0.2;
				}
				if(control_signal_r[1] > 0.2)
				{
					control_signal_r[1] = 0.2;
				}
				if(control_signal_r[2] > 0.2)
				{
					control_signal_r[2] = 0.2;
				}
				if(control_signal_r[0] < -0.2)
				{
					control_signal_r[0] = -0.2;
				}
				if(control_signal_r[1] < -0.2)
				{
					control_signal_r[1] = -0.2;
				}
				if(control_signal_r[2] < -0.2)
				{
					control_signal_r[2] = -0.2;
				}
				control_signal[0] = (signed char)(control_signal_r[0]*600);
				control_signal[1] = (signed char)(control_signal_r[1]*600);
				control_signal[2] = (signed char)(control_signal_r[2]*600);

				if(torquer_state == ON)
				{
					i++;

					taskENTER_CRITICAL();
					i2cSetSlaveAdd(i2cREG1, 4);
					buff[0] = 1;
					buff[1] = ((uint8)control_signal[0]);
					buff[2] = ((uint8)control_signal[1]);
					buff[3] = ((uint8)control_signal[2]);
					i2cSetMode(i2cREG1, I2C_MASTER | I2C_TRANSMITTER | I2C_STOP_COND);
					i2cSetCount(i2cREG1, 4);
					i2cSetStart(i2cREG1);
					i2cSend(i2cREG1, 4, buff);
					taskEXIT_CRITICAL();
				}
			}
		}
		else if( ( uxBits & 1<<1 ) != 0 )	// OFF signal
		{
			torquer_state = OFF;
			taskENTER_CRITICAL();
			i2cSetSlaveAdd(i2cREG1, 4);
			buff[0] = 1;
			buff[1] = 0;
			buff[2] = 0;
			buff[3] = 0;
			i2cSetMode(i2cREG1, I2C_MASTER | I2C_TRANSMITTER | I2C_STOP_COND);
			i2cSetCount(i2cREG1, 4);
			i2cSetStart(i2cREG1);
			i2cSend(i2cREG1, 4, buff);
			taskEXIT_CRITICAL();
		}
		else if( ( uxBits & 1<<2 ) != 0 )	// ON signal
		{
			torquer_state = ON;
			taskENTER_CRITICAL();
			i2cSetSlaveAdd(i2cREG1, 4);
			buff[0] = 1;
			buff[1] = ((uint8)control_signal[0]);
			buff[2] = ((uint8)control_signal[1]);
			buff[3] = ((uint8)control_signal[2]);
			i2cSetMode(i2cREG1, I2C_MASTER | I2C_TRANSMITTER | I2C_STOP_COND);
			i2cSetCount(i2cREG1, 4);
			i2cSetStart(i2cREG1);
			i2cSend(i2cREG1, 4, buff);
						taskEXIT_CRITICAL();
		}
		while(d < 50)
		{
			d++;
		}*/
	}
}

