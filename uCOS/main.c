#include "os.h"
#include "ARMCM3.h"

tTask *pTCurrentTask;
tTask *pTNextTask;
tTask *pTTaskTable[TINYOS_PRO_COUNT];
tBitmap taskPrioBitmap;

uint8_t Task1Flag,Task2Flag;

uint8_t g_u8SchedLockCount;

void tTaskInit (tTask *task, void (*entry)(void *), void *parm, uint32_t prio, tTaskStack *stack)
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
    task->delayTicks = 0;
    task->prio = prio;
	
    taskTable[prio] = task;
    tBitmapSet(&taskPrioBitmap, prio);
}

tTask *tTaskHighestReady (void)
{
	uint32_t highestPrio = tBitmapGetFirstSet(&taskPrioBitmap);
	return taskTable[highestPrio];
}

void SetSysTick(uint32_t ms)
{
    SysTick->LOAD = ms * SystemCoreClock / 1000 - 1;
    NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
    SysTick->VAL  = 0;
    SysTick->CTRL = (SysTick_CTRL_CLKSOURCE_Msk | \
                     SysTick_CTRL_TICKINT_Msk   | \
                     SysTick_CTRL_ENABLE_Msk);
}

void tTaskSchedInit(void)
{
    g_u8SchedLockCount = 0;
    tBitmapInit(&taskPrioBitmap);
}

void tTaskScheDisable(void)
{
    uint32_t status = tTaskEnterCritical();
    if (g_u8SchedLockCount < 255)
    {
        g_u8SchedLockCount++;
    }
    tTaskExitCritical(status);
}

void tTaskScheEnable(void)
{
    uint32_t status = tTaskEnterCritical();
    if (g_u8SchedLockCount > 0)
    {
        if (--g_u8SchedLockCount == 0)
        {
            taskSched();
        }
    }
    tTaskExitCritical(status);
}

void SysTick_Handler()
{
    uint8_t u8TaskNum;
    for (u8TaskNum = 0; u8TaskNum < 2; u8TaskNum++)
    {
        if (pTTaskTable[u8TaskNum]->delayTicks > 0)
        {
            pTTaskTable[u8TaskNum]->delayTicks--;
        }
    }
    
    taskSched();
}

void delay(uint16_t u16Count)
{
    while(u16Count--);
}

void taskDelay(uint16_t u16Count)
{
    pTCurrentTask->delayTicks = u16Count;
    taskSched();
}

tTask tTask1;
tTask tTask2;

tTaskStack task1Env[1024];
tTaskStack task2Env[1024];

tTask tTaskIdle;
tTaskStack taskIdleEnv[1024];

void task1(void *para)
{
    SetSysTick(10);
    for (;;)
    {
        Task1Flag = 0;
        taskDelay(100);
        Task1Flag = 1;
        taskDelay(100);
    }
}

void task2(void *para)
{
    for (;;)
    {
        Task2Flag = 0;
        taskDelay(100);
        Task2Flag = 1;
        taskDelay(100);
    }
}

void taskIdle(void *para)
{
    for (;;)
    {
    }
}

void taskSched(void)
{
    uint32_t status = tTaskEnterCritical();

    if (g_u8SchedLockCount > 0)
    {
        tTaskExitCritical(status);
        return;
    }
    tTaskExitCritical(status);
    
    if (pTCurrentTask == &tTaskIdle)
    {
        if (0 == pTTaskTable[0]->delayTicks)
        {
            pTNextTask = pTTaskTable[0];
        }
        else if (0 == pTTaskTable[1]->delayTicks)
        {
            pTNextTask = pTTaskTable[1];
        }
        else
        {
            return;
        }
    }
    else
    {
        if (pTCurrentTask == pTTaskTable[0])
        {
            if (0 == pTTaskTable[1]->delayTicks)
            {
                pTNextTask = pTTaskTable[1];
            }
            else if (0 != pTCurrentTask->delayTicks)
            {
                pTNextTask = &tTaskIdle;
            }
            else
            {
                return;
            }
        }
        else if (pTCurrentTask == pTTaskTable[1])
        {
            if (0 == pTTaskTable[0]->delayTicks)
            {
                pTNextTask = pTTaskTable[0];
            }
            else if (0 != pTCurrentTask->delayTicks)
            {
                pTNextTask = &tTaskIdle;
            }
            else
            {
                return;
            }
        }
    }
    
    tTaskSwitch();
}

int main(void)
{
    tTaskSchedInit();
    tTaskInit(&tTask1, task1, (void *)0x11111111, &task1Env[1024]);
    tTaskInit(&tTask2, task2, (void *)0x22222222, &task2Env[1024]);
    tTaskInit(&tTaskIdle, taskIdle, (void *)0, &taskIdleEnv[1024]);
    
    pTTaskTable[0] = &tTask1;
    pTTaskTable[1] = &tTask2;

    pTNextTask = pTTaskTable[0];
    tTaskRunFirst();
    
    return 0;
}

