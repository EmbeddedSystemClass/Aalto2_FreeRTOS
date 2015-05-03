#include "SensorReading.h"

#include "FreeRTOS.h"
#include "os_task.h"
#include "os_queue.h"
#include "os_semphr.h"
#include "os_event_groups.h"

#include "sci.h"
#include "i2c.h"

#define torqueSignalEG (tdSR.event[0])

TaskDescriptor_t tdSR = {
		1, // Queue number
		0, // Semaphore number
		1, // Events group number
		2, // Priority
		SensorReading, // Work function
		InitSR, // Initialization function
		500, // Stack Size
		NULL, // Register commands function
		pdFAIL
};

#define SensorDataQ (tdSR.queue[0])
#define torqueSignalEG (tdSR.event[0])

uint8 i2cRegisterRead(uint8 sadd, uint8 regadd)
{
	int d = 0;
	uint8 buff;
	buff = (regadd-1)*2;
	i2cSetSlaveAdd(i2cREG1, sadd);
	i2cSetCount(i2cREG1, 1);
	i2cSetMode(i2cREG1, I2C_MASTER | I2C_TRANSMITTER | I2C_START_COND);
	i2cSend(i2cREG1, 1,&buff);
	while((i2cREG1->STR &  (uint32)I2C_ARDY) == 0U)
	{}
	i2cSetMode(i2cREG1, I2C_MASTER | I2C_RECEIVER | I2C_START_COND | I2C_STOP_COND);
	i2cREG1->MDR = i2cREG1->MDR & (uint32)0xFFFFFDFF;
	i2cSetCount(i2cREG1, 1);
	i2cReceive(i2cREG1, 1, &buff);

	while(d < 50)
	{
		d++;
	}

	return buff;
}

void InitSR(void *pvParams)
{

}

void SensorReading(void *pvParams)
{
	uint8 buff[7];
	uint8 bfd[15];
	// Initialize timer, 1Hz period
	TickType_t readFrequency = 1000;
	TickType_t wakeUpTime = xTaskGetTickCount();

	// Sensor variables
	double Bfield[3];
	int tmp;

	while(1)
	{
		// Read magnetic field
		xEventGroupSetBits(torqueSignalEG, 1<<1);
		vTaskDelay(50);

		taskENTER_CRITICAL();

		/*buff[1] = i2cRegisterRead(4, 20);
		buff[2] = i2cRegisterRead(4, 21);
		buff[3] = i2cRegisterRead(4, 22);
		buff[4] = i2cRegisterRead(4, 23);
		buff[5] = i2cRegisterRead(4, 24);
		buff[6] = i2cRegisterRead(4, 25);*/

		taskEXIT_CRITICAL();

		Bfield[0] = ((double)((buff[1]<<8) + buff[2] - 32768))*((double)1.52587890625E-9);//0.00005/32768
		Bfield[1] = ((double)((buff[3]<<8) + buff[4] - 32768))*((double)1.52587890625E-9);
		Bfield[2] = ((double)((buff[5]<<8) + buff[6] - 32768))*((double)1.52587890625E-9);

		/*sprintf(bfd, "%.7f,", Bfield[0]);
		sciSend(scilinREG, 10, bfd);
		sprintf(bfd, "%.7f,", Bfield[1]);
		sciSend(scilinREG, 10, bfd);
		sprintf(bfd, "%.7f", Bfield[2]);
		sciSend(scilinREG, 10, bfd);
		bfd[0] = '\n';
		sciSend(scilinREG, 1, bfd);*/

		vTaskDelay(50);
		xEventGroupSetBits(torqueSignalEG, 1<<2);

		// Dispatch read values
		xQueueOverwrite(SensorDataQ, Bfield);

		vTaskDelayUntil(&wakeUpTime, readFrequency);
	}
}


