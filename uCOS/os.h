#ifndef __OS_H
#define __OS_H

#include "stdint.h"
#include "tConfig.h"

#include "tEvent.h"

typedef enum _tError {
	tErrorNoError = 0,
}tError;

typedef uint32_t tTaskStack;

typedef struct _tTask
{
    tTaskStack *stack;
    uint32_t delayTicks;
    tNode delayNode;
    uint32_t prio;
    uint32_t state;
    
    tEvent * waitEvent;
	void * eventMsg;
	uint32_t waitEventResult;
}tTask;

extern tTask *pTCurrentTask;
extern tTask *pTNextTask;

void tTaskRunFirst(void);
void tTaskSwitch(void);
void taskSched(void);

#endif

