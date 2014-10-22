/*
 * test.c
 *
 *  Created on: 13.08.2014.
 *      Author: Nemanja
 */

#include "FreeRTOS.h"
#include "os_task.h"
#include "os_queue.h"
#include "os_event_groups.h"
#include "os_semphr.h"

#include "sci.h"
#include "i2c.h"

#include "fat_sl.h"
#include "api_mdriver_ram.h"


#define PRIORITY_NORMAL 3

void i2cTest1(void *pvParams);
void i2cTest2(void *pvParams);

void sciTest1(void *pvParams);
void sciTest2(void *pvParams);

void fatTest(void *pvParams);

SemaphoreHandle_t test_mutex;

void initTest()
{

	//xTaskCreate(sciTest1, "sciTest1", 2000, NULL, PRIORITY_NORMAL, NULL);
	//xTaskCreate(sciTest2, "sciTest2", 2000, NULL, PRIORITY_NORMAL, NULL);
	//xTaskCreate(i2cTest1, "i2cTest1", 2000, NULL, PRIORITY_NORMAL, NULL);
	//xTaskCreate(i2cTest2, "i2cTest2", 2000, NULL, PRIORITY_NORMAL+1, NULL);
	//xTaskCreate(fatTest, "fat", 2000, NULL, PRIORITY_NORMAL, NULL);

	test_mutex = xSemaphoreCreateMutex();


/*	fn_initvolume(ram_initfunc);
	fs_init();

	fn_hardformat ( F_FAT16_MEDIA );*/

}

void i2cTest1(void *pvParams)
{
	uint8 text[6];

	while(1)
	{


		i2cSendMessageOS(6, 1, (uint8*)"123456", 6);
		i2cSendMessageOS(6, 2, (uint8*)"78910", 5);
		i2cReceiveMessageOS(6, 2, (uint8*)text, 5);

		vTaskDelay(500);
	}
}

void i2cTest2(void *pvParams)
{
	uint8 text[6];
	while(1)
	{
		i2cReceiveMessageOS(6, 2, (uint8*)text, 3);
		vTaskDelay(150);
	}
}

void sciTest1(void *pvParams)
{
	uint8 t[5];
	while(1)
	{
		if(sciReceiveOS(scilinREG, 3, t) == pdTRUE)
		{
			sciSendOS(scilinREG, 3, (uint8*)"bbb");
			sciSendOS(scilinREG, 3, t);
		}
		else
		{
			sciSendOS(scilinREG, 3, (uint8*)"222");
		}

		vTaskDelay(10);
	}
}

void sciTest2(void *pvParams)
{
	uint8 t[5];
	while(1)
	{
		if(sciReceiveOS(scilinREG, 3, (uint8*)t) == pdTRUE)
		{
			sciSendOS(scilinREG, 3, (uint8*)"aaa");
			sciSendOS(scilinREG, 3, t);
		}
		else
		{
			sciSendOS(scilinREG, 3, (uint8*)"111");
		}

		vTaskDelay(1200);
	}
}

