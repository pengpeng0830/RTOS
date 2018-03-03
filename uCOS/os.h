#ifndef __OS_H
#define __OS_H

#include "stdint.h"

typedef uint32_t tTaskStack;

typedef struct _tTask
{
    tTaskStack *stack;
}tTask;

#endif
