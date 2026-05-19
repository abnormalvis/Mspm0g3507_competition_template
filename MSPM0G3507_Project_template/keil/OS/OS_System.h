

typedef enum
{
	CPU_ENTER_CRITICAL,		// CPU enter critical section
	CPU_EXIT_CRITICAL,		// CPU exit critical section
}CPU_EA_TYPEDEF;

// Define a CPU interrupt control callback function pointer type
typedef void (*CPUInterrupt_CallBack_t)(CPU_EA_TYPEDEF cmd,unsigned char *pSta);


// System task IDs
typedef enum
{
	OS_TASK1,
	OS_TASK2,
	OS_TASK3,
	OS_TASK4,
//	OS_TASK5,
//	OS_TASK6,
	OS_TASK_SUM	
}OS_TaskIDTypeDef;


// System task running status (currently unused)
typedef enum
{
	OS_SLEEP,			// Task sleep
	OS_RUN=!OS_SLEEP	// Task run
}OS_TaskStatusTypeDef;

// System task struct
typedef struct
{
	void (*task)(void);					// Task function pointer
	OS_TaskStatusTypeDef RunFlag;		// Task running status
	unsigned short	RunPeriod;			// Task run period
	unsigned short RunTimer;			// Task run timer
}OS_TaskTypeDef;


/* Function declarations */ 
/*******************************************************************************/
void OS_CPUInterruptCBSRegister(CPUInterrupt_CallBack_t pCPUInterruptCtrlCBS);
void OS_ClockInterruptHandle(void);
void OS_TaskInit(void);
void OS_CreatTask(unsigned char ID, void (*proc)(void), unsigned short TimeDly, OS_TaskStatusTypeDef flag);
void OS_Start(void);
void OS_ClockInterruptHandle(void);
void OS_TaskGetUp(OS_TaskIDTypeDef taskID);	
void OS_TaskSleep(OS_TaskIDTypeDef taskID);
