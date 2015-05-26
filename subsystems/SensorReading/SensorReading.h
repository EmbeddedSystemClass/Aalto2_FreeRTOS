/*
 * SensorReading.h
 *
 *  Created on: Feb 22, 2015
 *      Author: nemanja
 */

#ifndef SENSORREADING_H_
#define SENSORREADING_H_

#include "TaskManaging.h"

#define SensorDataQ (tdSR.queue[0])
#define torqueSignalEG (tdSR.event[0])

void InitSR(void *pvParams);
void SensorReading(void *pvParams);

/*struct SensorDataS
{


};*/

extern TaskDescriptor_t tdSR;

#endif /* SENSORREADING_H_ */
