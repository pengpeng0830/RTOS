#define MAXTASKS 2
static uint32 u32timers[MAXTASKS];
#define PT_START static uint16 u16lc; switch(u16lc){default: 
#define PT_END ;}; u16lc=0; return 0;
#define WaitX(u32time)  do {u16lc=__LINE__; return u32time ;} while(0); case __LINE__: 
#define RunTask(TaskName,TaskID) do { if (u32timers[TaskID]==0) u32timers[TaskID]=TaskName(); }  while(0);

#define UpdateTimers() uint8 i; for(i=MAXTASKS;i>0 ;i--){if(u32timers[i-1]!=0) u32timers[i-1]--;}


void InitSofttimer()
{
    /* ... */
}

void Timer interrupt()
{
    UpdateTimers(); 
}


uint32 task1()
{
    PT_START
    while(1)
    {
        WaitX(50);
        dosomething();   
    }
    PT_END
}

uint32 task2()
{
    PT_START
    while(1)
    {
        WaitX(100);
        dosomething();   
    }
    PT_END
}


void main()
{
    InitSofttimer();
    while(1)
    {
        RunTask(task1,0);
        RunTask(task2,1);
    }
}

