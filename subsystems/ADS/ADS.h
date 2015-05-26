/*
 * ADS.h
 *
 *  Created on: Jun 19, 2014
 *      Author: nemanja
 */

#ifndef ADS_H
#define ADS_H

#include "TaskManaging.h"

#define HIL_recv (tdADS.queue[0])
#define HIL_send (tdADS.queue[1])

#define torqueSignalEG (tdADS.event[0])

/*struct DeterminationData
{

};*/



void InitADS(void *pvParams);
//void SensorRead(void *pvParams);
void StateDetermination(void *pvParams);

extern TaskDescriptor_t tdADS;

#endif /* ADS_H_ */
