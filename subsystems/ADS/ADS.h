/*
 * ADS.h
 *
 *  Created on: Jun 19, 2014
 *      Author: nemanja
 */

#ifndef ADS_H
#define ADS_H

#include "TaskManaging.h"


/*struct DeterminationData
{

};*/



void InitADS(void *pvParams);
//void SensorRead(void *pvParams);
void StateDetermination(void *pvParams);

extern TaskDescriptor_t tdADS;

#endif /* ADS_H_ */
