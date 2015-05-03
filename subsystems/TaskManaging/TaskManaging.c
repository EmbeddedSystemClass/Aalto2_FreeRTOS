#include "TaskManaging.h"

#include "cli.h"
#include "main.h"
#include "ACS.h"
#include "ADS.h"
#include "test.h"
#include "SensorReading.h"
#include "TorqueControl.h"

#define QUEUE_NUMBER 10
#define EVENT_NUMBER 10
#define SEMAPHORE_NUMBER 10
static QueueHandle_t queue_array[QUEUE_NUMBER];
static SemaphoreHandle_t semaphore_array[SEMAPHORE_NUMBER];
static EventGroupHandle_t event_array[EVENT_NUMBER];

#define TASK_NUMBER 10
static TaskDescriptor_t *TaskArray[TASK_NUMBER];


/*struct TaskManagingListElement
{
	struct TaskManagingListElement *next;
	TaskDescriptor_t *td;
};

static struct TaskManagingList
{
	unsigned int task_number;
	struct TaskManagingListElement *first;
	struct TaskManagingListElement *last;
} TaskList = {0, 0, 0};*/

void TaskManaging(void)
{
	//initTest();


	queue_array[0] = xQueueCreate(1, 13*sizeof(Real));
	/*queue_array[1] = xQueueCreate(1, 8*sizeof(Real));
	queue_array[2] = xQueueCreate(1, 3*sizeof(Real));

	// Create event group
	event_array[0] = xEventGroupCreate();
*/

	cliInit();
	xTaskCreate(cliTask, "CLI", 200, NULL, 5, NULL);



	TaskArray[0] = &tdTest;
	//TaskArray[0] = &tdSR;
	//TaskArray[1] = &tdADS;
	//TaskArray[0] = &tdTC;
	//TaskArray[3] = &tdACS;


	TaskManagingStartFromArray();

	/*tdACS.event[0] = event_array[0];
	tdSR.event[0] = event_array[0];
	tdADS.event[0] = event_array[0];*/

	///TaskCreateFromDescriptor(&tdACS);
	//TaskMannagingConnectToQueue(&tdADS, 1, 0);
	//TaskMannagingConnectToQueue(&tdTC, 0, 0);
	//TaskMannagingConnectToQueue(&tdACS, 2, 2);

	//TaskCreateFromDescriptor(&tdADS);
	/*TaskMannagingConnectToQueue(&tdADS, 0, 0);
	TaskMannagingConnectToQueue(&tdADS, 1, 1);

	TaskMannagingConnectToQueue(&tdSR, 0, 0);*/

}

void TaskManagingStartFromArray(void)
{
	int i;
	for(i = 0; i < TASK_NUMBER; i++)
	{
		if(TaskArray[i] != 0)
		{
			TaskCreateFromDescriptor(TaskArray[i]);
		}

	}
}

void TaskCreateFromDescriptor(TaskDescriptor_t *td)
{
	TaskFunction_t func;
	//struct TaskManagingListElement *element;

	td->event = (EventGroupHandle_t *)pvPortMalloc(sizeof(EventGroupHandle_t)*td->num_e);
	td->queue = (QueueHandle_t *)pvPortMalloc(sizeof(QueueHandle_t)*td->num_q);
	td->semaphore = (SemaphoreHandle_t *)pvPortMalloc(sizeof(SemaphoreHandle_t)*td->num_s);

	if((func = td->init_func) != NULL)
	{
		func((void *)&td);
	}

	if((func = td->cmd_reg_func) != NULL)
	{
		func((void *)&td);
	}

	if(xTaskCreate(td->work_func, "", td->stack_size, (void *)&td, td->priority, &(td->handle)) == pdFAIL)
	{
		// deinitialize, report error
	}
	td->active = pdTRUE;
}



void TaskMannagingConnectToQueue(TaskDescriptor_t *td, unsigned int task_queue_input, unsigned int queue_num)
{
	td->queue[task_queue_input] = queue_array[queue_num];
}

/*CLI_Command_Definition_t xTaskDestroy =
{
	"acsSetPeriod",
	"\r\nacsSetPeriod T:\r\n Set control signal calculation period T [ms]\r\n\r\n",
	prvACSSetPeriod,
	1
};*/

/*void ADCSCommands(void *pvParams)
{
	FreeRTOS_CLIRegisterCommand(&xACSSetPeriod);
}*/

/*static BaseType_t prvTaskDestroy( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
	int T;
	BaseType_t len;
	char *param_p;
	param_p = (char *)FreeRTOS_CLIGetParameter(pcCommandString, 1, &len);
	sscanf(param_p, "%d", &T);



	return pdFALSE;
}*/
