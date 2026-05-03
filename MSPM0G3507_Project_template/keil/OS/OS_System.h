

typedef enum
{
	CPU_ENTER_CRITICAL,		//CPU进入临界
	CPU_EXIT_CRITICAL,		//CPU退出临界
}CPU_EA_TYPEDEF;

//定义一个CPU中断控制回调函数指针,别名CPUInterrupt_CallBack_t,
typedef void (*CPUInterrupt_CallBack_t)(CPU_EA_TYPEDEF cmd,unsigned char *pSta);


//系统任务ID
typedef enum
{
	OS_TASK1,
	OS_TASK2,
//	OS_TASK3,
//	OS_TASK4,
//	OS_TASK5,
//	OS_TASK6,
	OS_TASK_SUM	
}OS_TaskIDTypeDef;


//系统任务运行状态,暂时没用到
typedef enum
{
	OS_SLEEP,			//任务休眠
	OS_RUN=!OS_SLEEP	//任务运行
}OS_TaskStatusTypeDef;

//系统任务结构体
typedef struct
{
	void (*task)(void);					//任务函数指针
	OS_TaskStatusTypeDef RunFlag;		//任务运行状态
	unsigned short	RunPeriod;			//任务调度频率
	unsigned short RunTimer;			//任务调度计时器
}OS_TaskTypeDef;


/*	函数声明 */ 
/*******************************************************************************/
void OS_CPUInterruptCBSRegister(CPUInterrupt_CallBack_t pCPUInterruptCtrlCBS);
void OS_ClockInterruptHandle(void);
void OS_TaskInit(void);
void OS_CreatTask(unsigned char ID, void (*proc)(void), unsigned short TimeDly, OS_TaskStatusTypeDef flag);
void OS_Start(void);
void OS_ClockInterruptHandle(void);
void OS_TaskGetUp(OS_TaskIDTypeDef taskID);	
void OS_TaskSleep(OS_TaskIDTypeDef taskID);
