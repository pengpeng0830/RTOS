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

    MRS R0, PSP
    CBZ R0, PendSVHander_nosave
    
    STMDB R0!, {R4-R11}
    
    LDR R1, =pTCurrentTask
    LDR R1, [R1]
    STR R0, [R1]
    
PendSVHander_nosave
    LDR R0, =pTCurrentTask
    LDR R1, =pTNextTask
    LDR R2, [R1]
    STR R2, [R0]

    LDR R0, [R2]
    LDMIA R0!, {R4-R11}

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

