#include <stdio.h>

#define  PT_START  static uint16 u16lc = 0; switch(u16lc){   case 0: u16lc = 0
#define  PT_END    }; u16lc = 0 
#define  WaitX(u8id,s32timer)  settimer(&u16lc,__LINE__,u8id,s32timer); return ; case __LINE__:
#define  MAXTASKS 5

struct TASK
{
	sint32 td;
	void (*fp)();
};

struct TASK tasks[MAXTASKS];

void settimer(uint16 *pu16lc,uint16 u16line,uint8 u8id,sint32 s32time)
{
	*pu16lc = u16line;
	tasks[u8id].td = s32time;
}

void dectimers()
{ 
    uint8 i;   
    for (i=0;i<MAXTASKS;i++)
    { 
        if (tasks[i].td>0)  
            tasks[i].td--;  
    }
}

void runtasks() 
{
	uint8 i;    
	for(i=0;i<MAXTASKS;i++)        
	{   
		if (tasks[i].fp!=0)
		{    
			if (tasks[i].td==0)
			{
				tasks[i].td=-1;  
				tasks[i].fp();
			}  
		}         
	}
}

void InitSofttimer()
{
    /* ... */
}


void task1()
{
	PT_START;
    while(1)
    {
       WaitX(2,1000);
       printf("Task 1 Timeup");
    }
    PT_END;
}

void task2()
{
	PT_START;
    while(1)
    {
       WaitX(2,2000);
       printf("Task 2 Timeup");
    }
    PT_END;
}

void main()
{
	InitSofttimer();

	tasks[0].fp=task1; 
	tasks[1].fp=task2; 

	while(1)
	{
		runtasks();
	}
}
