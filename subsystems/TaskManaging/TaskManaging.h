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
	unsigned int num_q, num_s, num_e;
	QueueHandle_t *queue;
	SemaphoreHandle_t *semaphore;
	EventGroupHandle_t *event;

} TaskArgument_t;

typedef struct TaskDescriptor
{
	unsigned int num_q, num_s, num_e;

	unsigned int priority;
	TaskFunction_t work_func;
	TaskFunction_t init_func;
	unsigned int stack_size;

	TaskFunction_t cmd_reg_func;

	TaskHandle_t handle;

	BaseType_t active;

	QueueHandle_t *queue;
	SemaphoreHandle_t *semaphore;
	EventGroupHandle_t *event;
} TaskDescriptor_t;


void MallocTaskArgument(TaskArgument_t *ta, unsigned int num_queues, unsigned int num_semaphores, unsigned int num_events);

void TaskManagingStartFromArray(void);

void TaskCreateFromDescriptor(TaskDescriptor_t *td);

void TaskMannagingConnectToQueue(TaskDescriptor_t *td, unsigned int task_queue_input, unsigned int queue_num);

void TaskManaging(void);


#endif /* TASKMANAGING_H_ */
