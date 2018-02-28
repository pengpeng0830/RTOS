#define NVIC_INT_CTRL   0xE000ED04
#define NVIC_SYSPRI14   0xE000ED22
#define NVIC_PENDSV_PRI 0xFF
#define NVIC_PENDSVSET  0x10000000

#define MEM32(addr)   *(volatile unsigned long *)(addr)
#define MEM8(addr)    *(volatile unsigned char *)(addr)

unsigned char flag;

void triggerPendSVC(void)
{
    MEM8(NVIC_SYSPRI14)  = NVIC_PENDSV_PRI;
    MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}

typedef struct _T_BLOCK
{
    unsigned long *stackPtr;
}T_BLOCK;

void delay(unsigned int u16Count)
{
    while(u16Count--);
}

unsigned long stackBuffer[1024];
T_BLOCK *blockPtr;
T_BLOCK block;

int main(void)
{
    blockPtr = &block;
    
    for (;;)
    {
        flag = 0;
        delay(500);
        flag = 1;
        delay(500);
			
        block.stackPtr = &stackBuffer[1024];
        triggerPendSVC();
    }
}
