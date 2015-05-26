#ifndef ACS_H
#define ACS_H

#include "matrix.h"
#include "TaskManaging.h"

#define PRIORITY_NORMAL 2
#define PRIORITY_HIGH 3

#define controlSignalQ (tdACS.queue[0])
#define stateQ (tdACS.queue[1])
#define BfieldQ (tdACS.queue[2])

#define torqueSignalEG (tdACS.event[0])

void ADCSCommands(void *pvParams);
void InitACS();
void CalculateSignal(void *pvParams);
void TorqueControl(void *pvParams);

//static BaseType_t prvACSSetPeriod(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

extern TaskDescriptor_t tdACS;

#endif
