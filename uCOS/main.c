#include "os.h"
#include "ARMCM3.h"

tTask *pTCurrentTask;
tTask *pTNextTask;
tList *pTTaskTable[TINYOS_PRO_COUNT];
tBitmap taskPrioBitmap;

uint8_t Task1Flag,Task2Flag;

uint8_t g_u8SchedLockCount;
tList tTaskDelayedList;

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
    
    task->slice = TINYOS_SLICE_MAX;
    task->stack = stack;
    task->delayTicks = 0;
    task->prio = prio;
	
    task->state = TINYOS_TASK_STATE_RDY;
    tNodeInit(&(task->delayNode));
    tNodeInit(&(task->linkNode));
    tListAddFirst(&taskTable[prio], &(task->linkNode));
	
    tBitmapSet(&taskPrioBitmap, prio);
}

tTask *tTaskHighestReady (void)
{
	uint32_t highestPrio = tBitmapGetFirstSet(&taskPrioBitmap);
	tNode * node = tListFirst(&taskTable[highestPrio]);
	return tNodeParent(node, tTask, linkNode);
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
	int i;
	
	schedLockCount = 0;
	tBitmapInit(&taskPrioBitmap);
	for (i = 0; i < TINYOS_PRO_COUNT; i++)
	{
		tListInit(&taskTable[i]);
	}
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

void tTaskSchedRdy (tTask * task)
{
	taskTable[task->prio] = task;
	tBitmapSet(&taskPrioBitmap, task->prio);
}

void tTaskSchedUnRdy (tTask * task)
{
	taskTable[task->prio] = (tTask *)0;
	tBitmapClear(&taskPrioBitmap, task->prio);
}

void tTaskDelayedInit (void)
{
	tListInit(&tTaskDelayedList);
}

void tTimeTaskWait (tTask * task, uint32_t ticks)
{
	task->delayTicks = ticks;
	tListAddLast(&tTaskDelayedList, &(task->delayNode));
	task->state |= TINYOS_TASK_STATE_DELAYED;
}

void tTimeTaskWakeUp (tTask * task)
{
	tListRemove(&tTaskDelayedList, &(task->delayNode));
	task->state &= ~TINYOS_TASK_STATE_DELAYED;
}

void SysTick_Handler()
{
	tNode * node;
	
	uint32_t status = tTaskEnterCritical();
	
	for (node = tTaskDelayedList.headNode.nextNode; node != &(tTaskDelayedList.headNode); node = node->nextNode)
	{
		tTask * task = tNodeParent(node, tTask, delayNode);
		if (--task->delayTicks == 0)
		{
			tTimeTaskWakeUp(task);
			
			tTaskSchedRdy(task);
		}
	}
		
	tTaskExitCritical(status);
	
	tTaskSched();
}

void delay(uint16_t u16Count)
{
    while(u16Count--);
}

void tTaskDelay (uint32_t delay)
{
	uint32_t status = tTaskEnterCritical();
	
	tTimeTaskWait(currentTask, delay);

	tTaskSchedUnRdy(currentTask);
	
	tTaskExitCritical(status);
	
	tTaskSched();
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
    tTask * tempTask;
    uint32_t status = tTaskEnterCritical();

    if (g_u8SchedLockCount > 0)
    {
        tTaskExitCritical(status);
        return;
    }
    tTaskExitCritical(status);
    
   tempTask = tTaskHighestReady();
   if (tempTask != currentTask)
   {
       nextTask = tempTask;
       tTaskSwitch();
   }
}

int main(void)
{
    tTaskSchedInit();
    tTaskDelayedInit();
	
    tTaskInit(&tTask1, task1, (void *)0x11111111,0, &task1Env[1024]);
    tTaskInit(&tTask2, task2, (void *)0x22222222,1, &task2Env[1024]);
    tTaskInit(&tTaskIdle, taskIdle, (void *)0, TINYOS_PRO_COUNT - 1,&taskIdleEnv[1024]);
    
    pTTaskTable[0] = &tTask1;
    pTTaskTable[1] = &tTask2;

    pTNextTask = tTaskHighestReady();
    tTaskRunFirst();
    
    return 0;
}

