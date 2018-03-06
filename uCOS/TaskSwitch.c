#include "OS.h"
#include "ARMCM3.h"

#define NVIC_INT_CTRL   0xE000ED04
#define NVIC_SYSPRI14   0xE000ED22
#define NVIC_PENDSV_PRI 0xFF
#define NVIC_PENDSVSET  0x10000000

#define MEM32(addr)   *(volatile unsigned long *)(addr)
#define MEM8(addr)    *(volatile unsigned char *)(addr)

__asm void PendSV_Handler (void)
{
    IMPORT pTCurrentTask
    IMPORT pTNextTask

    MRS R0, PSP    ;进入异常以后，PSP会自动压栈保存8个寄存器的值。然后PSP的值给R0寄存器
    CBZ R0, PendSVHander_nosave   ;判断R0的值若等于0，直接跳转到PendSVHander_nosave（只有第一次执行时，满足该条件，因为第一次没有寄存器的值需要保存）
    
    STMDB R0!, {R4-R11}   ;手动保存R4-R11寄存器，保存到地址值为R0的地方，R0继续压栈8个空间，此后不再压栈，到达栈底
    
    LDR R1, =pTCurrentTask  ;pTCurrentTask的地址保存到R1
    LDR R1, [R1]   ;pTCurrentTask的值保存到R1
    STR R0, [R1]   ;R0的值存到pTCurrentTask->stack，即保存了栈底的地址，下次可以根据该地址恢复数据
    
PendSVHander_nosave
    LDR R0, =pTCurrentTask  ;pTCurrentTask的地址保存到R0
    LDR R1, =pTNextTask     ;pTNextTask的地址保存到R1
    LDR R2, [R1]            ;pTNextTask的值保存到R2
    STR R2, [R0]            ;pTNextTask的值保存到pTCurrentTask。即实现了设置当前任务的指针为pTNextTask
 
    LDR R0, [R2]    ;pTCurrentTask的值保存到R0
    LDMIA R0!, {R4-R11}   ;通过R0的值，即pTCurrentTask->stack从栈恢复到寄存器R4-R11
 
    MSR PSP, R0     ;修改PSP的值为R0，这样出栈的时候自动恢复另外8个寄存器的起始位置是正确的
    ORR LR, LR, #0x04   ;确保使用的堆栈指针是PSP，而不是MSP
    BX LR     ;退出异常
}

void tTaskRunFirst(void)
{
    __set_PSP(0);

    MEM8(NVIC_SYSPRI14)  = NVIC_PENDSV_PRI;
    MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}

void tTaskSwitch(void)
{
    MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}

