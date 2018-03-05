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

    MRS R0, PSP    ;PSP的值给到R0寄存器，
    CBZ R0, PendSVHander_nosave   ;R0的值若等于0，直接跳转到PendSVHander_nosave（只有第一次执行时，满足该条件）
    
    STMDB R0!, {R4-R11}   ;手动保存R4-R11寄存器，保存到地址值为R0的地方。
    
    LDR R1, =pTCurrentTask  ;pTCurrentTask的地址保存到R1
    LDR R1, [R1]   ;pTCurrentTask的值保存到R1
    STR R0, [R1]   ;R0的值存到pTCurrentTask任务
    
PendSVHander_nosave
    LDR R0, =pTCurrentTask  ;pTCurrentTask的地址保存到R0
    LDR R1, =pTNextTask     ;pTNextTask的地址保存到R1
    LDR R2, [R1]            ;pTNextTask的值保存到R2
    STR R2, [R0]            ;pTNextTask的值保存到pTCurrentTask。即实现了设置当前任务的指针
 
    LDR R0, [R2]    ;pTNextTask的值保存到R0
    LDMIA R0!, {R4-R11}   ;恢复R4-R11的值
 
    MSR PSP, R0
    ORR LR, LR, #0x04
    BX LR
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

