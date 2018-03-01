TaskSwitch.c实现了CORTEM-M3内核系统产生异常，保存和恢复R4-R11寄存器的值。这是任务切换必须的操作。  
在函数名前面加上__asm，可以在C语言代码中嵌入汇编语言。当发生PendSV异常时，进入PendSV_Handler中断。  
IMPORT blockPtr  声明blockPtr变量  
LDR R0, =blockPtr  将blockPtr的地址给到R0寄存器  
LDR R0, [R0]       将R0寄存器内容指向的内容给到R0  
LDR R0, [R0]  
通过上面3个LDR的操作，最终R0的值为stackBuffer的地址。  
STMDB R0!, {R4-R11}  将R4-R11的内容存放到stackBuffer里面，从stackBuffer[1023]开始存放  
LDR R1, =blockPtr  
LDR R1, [R1]  
STR R0, [R1]  

LDMIA R0!, {R4-R11}  将stackBuffer里面的内容恢复到R4-R11寄存器  
 
BX LR  退出异常中断  
 
