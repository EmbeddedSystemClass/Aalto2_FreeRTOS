#include "TaskManaging.h"

#include "cli.h"
#include "main.h"

#define QUEUE_NUMBER 10
#define EVENT_NUMBER 10
#define SEMAPHORE_NUMBER 10
static QueueHandle_t queue_array[QUEUE_NUMBER];
static SemaphoreHandle_t semaphore_array[SEMAPHORE_NUMBER];
static EventGroupHandle_t event_array[EVENT_NUMBER];

TaskArgument_t mainArgs;

void TaskManaging(void)
{
	//initTest();

	queue_array[0] = xQueueCreate(1, sizeof(char));

	cliInit();
	xTaskCreate(cliTask, "CLI", 200, NULL, 5, NULL);


	/*MallocTaskArgument(&mainArgs, 1, 0, 0);
	mainArgs.queue[0] = queue_array[0];
	mainInit();
	xTaskCreate(mainTask, "mainTask", 200, NULL, 5, (void *)&mainArgs);*/
}


void MallocTaskArgument(TaskArgument_t *ta, unsigned int num_queues, unsigned int num_semaphores, unsigned int num_events)
{
	ta->event = (EventGroupHandle_t *)pvPortMalloc(sizeof(EventGroupHandle_t)*num_events);
	ta->queue = (QueueHandle_t *)pvPortMalloc(sizeof(QueueHandle_t)*num_queues);
	ta->semaphore = (SemaphoreHandle_t *)pvPortMalloc(sizeof(SemaphoreHandle_t)*num_semaphores);
}
