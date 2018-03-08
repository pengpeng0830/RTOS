#ifndef __OS_H
#define __OS_H

#include "stdint.h"

typedef uint32_t tTaskStack;

typedef struct _tTask
{
    tTaskStack *stack;
    uint32_t delayTicks;
}tTask;

extern tTask *pTCurrentTask;
extern tTask *pTNextTask;

void tTaskRunFirst(void);
void tTaskSwitch(void);
void taskSched(void);

#endif

