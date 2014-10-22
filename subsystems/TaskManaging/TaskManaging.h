/*
 * TaskManaging.h
 *
 *  Created on: Oct 20, 2014
 *      Author: nemanja
 */

#ifndef TASKMANAGING_H_
#define TASKMANAGING_H_

#include "FreeRTOS.h"
#include "os_queue.h"
#include "os_semphr.h"
#include "os_event_groups.h"

typedef struct TaskArgument
{
	QueueHandle_t *queue;
	SemaphoreHandle_t *semaphore;
	EventGroupHandle_t *event;
} TaskArgument_t;

void MallocTaskArgument(TaskArgument_t *ta, unsigned int num_queues, unsigned int num_semaphores, unsigned int num_events);

void TaskManaging(void);


#endif /* TASKMANAGING_H_ */
