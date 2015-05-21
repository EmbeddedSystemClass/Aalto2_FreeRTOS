/*
 * test.c
 *
 *  Created on: 13.08.2014.
 *      Author: Nemanja
 */

#include "test.h"
#include "FreeRTOS.h"
#include "os_task.h"
#include "os_queue.h"
#include "os_event_groups.h"
#include "os_semphr.h"

#include "FreeRTOS_CLI.h"
#include "TaskManaging.h"

#include "sci.h"
#include "i2c.h"
#include "spi.h"
#include "can.h"

void canTest1(void *pvParams);

void i2cTest1(void *pvParams);
void i2cTest2(void *pvParams);

void sciTest1(void *pvParams);
void sciTest2(void *pvParams);

void spiTest1(void *pvParams);
void spiTest2(void *pvParams);

void fatTest(void *pvParams);


void ReallocTest(void *pvParams);


TaskDescriptor_t tdTest = {
		0, // Queue number
		0, // Semaphore number
		0, // Events group number
		2, // Priority
		ReallocTest, // Work function
		initTest, // Initialization function
		20000, // Stack Size
		TestCommands // Register commmands function
		//NULL,
		//pdFALSE
};

static BaseType_t prvTestCmd( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
	int i;
	int X;
	BaseType_t len;
	char *param_p;
	param_p = (char *)FreeRTOS_CLIGetParameter(pcCommandString, 1, &len);
	//sscanf(param_p, "%d", &X);

	//for(i = 0; i < 2; i++)
	//{
		sciSendOS(scilinREG, len, param_p);

		//sciSendOS(scilinREG, len, param_p);
	//}
	pcWriteBuffer[0] = '\0';


	return pdFALSE;
}


CLI_Command_Definition_t xTestCmd =
{
	"testCmd",
	"\r\ntestCmd X:\r\n Test command addition feature\r\n\r\n",
	prvTestCmd,
	1
};

void TestCommands(void *pvParams)
{
	FreeRTOS_CLIRegisterCommand(&xTestCmd);
}





SemaphoreHandle_t test_mutex;

void initTest()
{

	//xTaskCreate(sciTest1, "sciTest1", 2000, NULL, PRIORITY_NORMAL, NULL);
	//xTaskCreate(sciTest2, "sciTest2", 2000, NULL, PRIORITY_NORMAL, NULL);
	//xTaskCreate(i2cTest1, "i2cTest1", 2000, NULL, PRIORITY_NORMAL, NULL);
	//xTaskCreate(i2cTest2, "i2cTest2", 2000, NULL, PRIORITY_NORMAL+1, NULL);
	//xTaskCreate(spiTest1, "spiTest1", 2000, NULL, PRIORITY_NORMAL, NULL);
	//xTaskCreate(spiTest2, "spiTest2", 2000, NULL, PRIORITY_NORMAL+1, NULL);
	//xTaskCreate(fatTest, "fat", 2000, NULL, PRIORITY_NORMAL, NULL);

	//test_mutex = xSemaphoreCreateMutex();

}


void ReallocTest(void *pvParams)
{
	char *a = NULL;
	char *b = NULL;
	char *c = NULL;
	char *d = NULL;

	int i;

	typedef struct A_BLOCK_LINK
	{
		struct A_BLOCK_LINK *pxNextFreeBlock;	/*<< The next free block in the list. */
		size_t xBlockSize;						/*<< The size of the free block. */
	} BlockLink_t;

	size_t xHeapStructSize	= ( ( sizeof( BlockLink_t ) + ( portBYTE_ALIGNMENT - 1 ) ) & ~portBYTE_ALIGNMENT_MASK );
	BlockLink_t *pxBlock;

	char output[20];

	while(1)
	{
		a = (char *)pvPortRealloc(a, sizeof(char)*120);
		pxBlock = (void *)(a - xHeapStructSize);

		b = (char *)pvPortMalloc(sizeof(char)*20);
		c = (char *)pvPortMalloc(sizeof(char)*20);
		d = (char *)pvPortMalloc(sizeof(char)*20);
		a = (char *)pvPortRealloc(a, sizeof(char)*500);
		vPortFree(a);
		vPortFree(b);
		vPortFree(c);
		vPortFree(d);
		a = NULL;
		b = NULL;
		c = NULL;
		d = NULL;

		vTaskDelay(3000);
	}
}


void canTest1(void *pvParams)
{
	uint8 buff[8] = {2, 3, 0, 1, 5, 6, 7, 8};
	uint8 buff2[8];


	while(1)
	{
		//canTransmit(canREG1, 2, 4, buff);
		can1SendOS(1, 4, buff);
		vTaskDelay(200);
		can1ReceiveOS(1, 4, buff2);

		vTaskDelay(3000);
	}
}

void spiTest1(void *pvParams)
{
	uint16 buff[5] = {1, 2, 3, 4, 5};
	uint16 buff2[5];
	spiDAT1_t data1;

	data1.CS_HOLD = true;
	data1.WDEL = false;
	data1.DFSEL = SPI_FMT_0;
	data1.CSNR = SPI_CS_0;

	while(1)
	{
		//spiTransmitData(spiREG4, &data1, 5, buff);
		//spiSendDataOS(spiREG4, &data1, 5, buff);
		//spiSendDataOS(spiREG4, &data1, 5, buff);
		//spiSendDataOS(spiREG4, &data1, 5, buff);
		vTaskDelay(1);
		spiSendAndGetDataOS(spiREG4, &data1, 5, buff, buff2);

		//vTaskDelay(10);
	}
}

void spiTest2(void *pvParams)
{
	uint16 buff[3] = {6, 7, 8};
	uint16 buff2[5];
	spiDAT1_t data1;

	data1.CS_HOLD = true;
	data1.WDEL = false;
	data1.DFSEL = SPI_FMT_0;
	data1.CSNR = SPI_CS_0;

	while(1)
	{
		//spiTransmitData(spiREG4, &data1, 5, buff);
		//spiSendDataOS(spiREG4, &data1, 5, buff);
		//spiSendDataOS(spiREG4, &data1, 5, buff);
		//spiSendDataOS(spiREG4, &data1, 5, buff);
		vTaskDelay(3);
		spiSendAndGetDataOS(spiREG4, &data1, 3, buff, buff2);

		//vTaskDelay(10);
	}
}

void i2cTest1(void *pvParams)
{
	uint8 text[6] = {1, 2, 3, 4, 5, 6};

	while(1)
	{
		i2cSendMessageOS(2, 1, (uint8*)text, 2);
		i2cSendMessageOS(2, 2, (uint8*)text, 2);
		i2cSendMessageOS(2, 3, (uint8*)text, 2);
		i2cSendMessageOS(2, 4, (uint8*)text, 2);
		i2cSendMessageOS(2, 5, (uint8*)text, 2);
		i2cSendMessageOS(2, 6, (uint8*)text, 2);
		i2cSendMessageOS(2, 7, (uint8*)text, 2);
		i2cSendMessageOS(2, 8, (uint8*)text, 2);
		vTaskDelay(5);

		i2cReceiveMessageOS(2, 1, (uint8*)text, 3);
		i2cReceiveMessageOS(2, 3, (uint8*)text, 3);
		vTaskDelay(5);
	}
}

void i2cTest2(void *pvParams)
{
	uint8 text[6];
	while(1)
	{
		i2cReceiveMessageOS(2, 2, (uint8*)text, 3);
		vTaskDelay(10);
	}
}

void sciTest1(void *pvParams)
{
	uint8 t[5];
	while(1)
	{
		/*if(sciReceiveOS(scilinREG, 3, t) == pdTRUE)
		{
			sciSendOS(scilinREG, 3, (uint8*)"bbb");
			sciSendOS(scilinREG, 3, t);
		}
		else*/
		{
			sciSendOS(sciREG, 3, (uint8*)"222");
		}

		vTaskDelay(150);
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

