/*
 * TorqueControl.h
 *
 *  Created on: Feb 22, 2015
 *      Author: nemanja
 */

#ifndef TORQUECONTROL_H_
#define TORQUECONTROL_H_

#include "TaskManaging.h"

void InitTC(void *pvParams);
void TorqueControl(void *pvParams);

extern TaskDescriptor_t tdTC;

#endif /* TORQUECONTROL_H_ */
