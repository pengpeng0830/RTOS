#include "os.h"

tTask *pTCurrentTask;
tTask *pTNextTask;
tTask *pTTaskTable[2];

unsigned char Task1Flag,Task2Flag;

void tTaskInit (tTask *task, void (*entry)(void *), void *parm, tTaskStack *stack)
{
    *(--stack) = (unsigned long)(1 << 24);
    *(--stack) = (unsigned long)entry;
    *(--stack) = (unsigned long)0x14141414;
    *(--stack) = (unsigned long)0x12121212;
    *(--stack) = (unsigned long)0x03030303;
    *(--stack) = (unsigned long)0x02020202;
    *(--stack) = (unsigned long)0x01010101;
    *(--stack) = (unsigned long)parm;

    *(--stack) = (unsigned long)0x11111111;
    *(--stack) = (unsigned long)0x10101010;
    *(--stack) = (unsigned long)0x09090909;
    *(--stack) = (unsigned long)0x08080808;
    *(--stack) = (unsigned long)0x07070707;
    *(--stack) = (unsigned long)0x06060606;
    *(--stack) = (unsigned long)0x05050505;
    *(--stack) = (unsigned long)0x04040404;
    
    task->stack = stack;
}



void delay(unsigned int u16Count)
{
    while(u16Count--);
}

unsigned long stackBuffer[1024];

tTask tTask1;
tTask tTask2;

tTaskStack task1Env[1024];
tTaskStack task2Env[1024];

void task1(void *para)
{
    for (;;)
    {
        Task1Flag = 0;
        delay(100);
        Task1Flag = 1;
        delay(100);

        taskSched();
    }
}

void task2(void *para)
{
    for (;;)
    {
        Task2Flag = 0;
        delay(100);
        Task2Flag = 1;
        delay(100);

        taskSched();
    }
}

void taskSched(void)
{
    if (pTCurrentTask == pTTaskTable[0])
    {
        pTNextTask = pTTaskTable[1];
    }
    else
    {
        pTNextTask = pTTaskTable[0];
    }
    tTaskSwitch();
}

int main(void)
{
    tTaskInit(&tTask1, task1, (void *)0x11111111, &task1Env[1024]);
    tTaskInit(&tTask2, task2, (void *)0x22222222, &task2Env[1024]);
    
    pTTaskTable[0] = &tTask1;
    pTTaskTable[1] = &tTask2;

    pTNextTask = pTTaskTable[0];
    tTaskRunFirst();
    
    return 0;
}

