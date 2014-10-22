/** @file sys_main.c 
*   @brief Application main file
*   @date 9.Sep.2014
*   @version 04.01.00
*
*   This file contains an empty main function,
*   which can be used for the application.
*/

/* (c) Texas Instruments 2009-2014, All rights reserved. */

/* USER CODE BEGIN (0) */
/* USER CODE END */

/* Include Files */

#include "sys_common.h"

/* USER CODE BEGIN (1) */

#include "FreeRTOS.h"
#include "os_task.h"
#include "sci.h"
#include "i2c.h"

#include "TaskManaging.h"


/* USER CODE END */

/** @fn void main(void)
*   @brief Application main function
*   @note This function is empty by default.
*
*   This function is called after startup.
*   The user can use this function to implement the application.
*/

/* USER CODE BEGIN (2) */
/* USER CODE END */

void main(void)
{
/* USER CODE BEGIN (3) */

	sciInit();
	i2cInit();
	//i2cEnableNotification(i2cREG1, I2C_RX_INT | I2C_TX_INT );
	//initTest();

	TaskManaging();

	vTaskStartScheduler();

	for( ;; );

/* USER CODE END */
}

/* USER CODE BEGIN (4) */

void vApplicationMallocFailedHook(void * pvParameters)
{

}

void vApplicationStackOverflowHook(void * pvParameters)
{

}

/* USER CODE END */
