#ifndef ACS_H
#define ACS_H

#include "matrix.h"
#include "TaskManaging.h"

#define PRIORITY_NORMAL 2
#define PRIORITY_HIGH 3

void ADCSCommands(void *pvParams);
void InitACS();
void CalculateSignal(void *pvParams);
void TorqueControl(void *pvParams);

//static BaseType_t prvACSSetPeriod(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

extern TaskDescriptor_t tdACS;

#endif
