/*******************************************************************************
 * @Author      : wangming
 * @wechat      : DeepCoderMing
 * @qq          : 3201935299
 * @Date        : 2025-05-01
 * @Copyright   : For learning reference only. Commercial use prohibited.
 ********************************************************************************/
#include "OS_System.h"

volatile OS_TaskTypeDef OS_Task[OS_TASK_SUM];

CPUInterrupt_CallBack_t CPUInterrupptCtrlCBS;

/********************************************************************************************************
 *  @FunctionName   OS_CPUInterruptCBSRegister
 *  @Description    Register CPU interrupt control function
 *  @Parameter      pCPUInterruptCtrlCBS - CPU interrupt control callback function address
 *  @ReturnValue    None
 *  @Note           None
 ********************************************************************************************************/
void OS_CPUInterruptCBSRegister(CPUInterrupt_CallBack_t pCPUInterruptCtrlCBS)
{
	if (CPUInterrupptCtrlCBS == 0)
	{
		CPUInterrupptCtrlCBS = pCPUInterruptCtrlCBS;
	}
}

/********************************************************************************************************
 *  @FunctionName   OS_TaskInit
 *  @Description    Initialize system tasks
 *  @Parameter      None
 *  @ReturnValue    None
 *  @Note           None
 ********************************************************************************************************/
void OS_TaskInit(void)
{
	unsigned char i;
	for (i = 0; i < OS_TASK_SUM; i++)
	{
		OS_Task[i].task = 0;
		OS_Task[i].RunFlag = OS_SLEEP;
		OS_Task[i].RunPeriod = 0;
		OS_Task[i].RunTimer = 0;
	}
}

/*******************************************************************************
 * Function Name  : void OS_CreatTask(unsigned char ID, void (*proc)(void), OS_TIME_TYPEDEF TimeDly, bool flag)
 * Description    : Create a task
 * Input          : - ID: Task ID
 *					- (*proc)() User task function address
 *					- TimeDly: Task execution frequency, unit ms
 * 					- flag: Task running status  OS_SLEEP-Sleep OS_RUN-Run
 * Output         : None
 * Return         : None
 * Attention      : None
 *******************************************************************************/
void OS_CreatTask(unsigned char ID, void (*proc)(void), unsigned short Period, OS_TaskStatusTypeDef flag)
{
	if (!OS_Task[ID].task)
	{
		OS_Task[ID].task = proc;
		OS_Task[ID].RunFlag = OS_SLEEP;
		OS_Task[ID].RunPeriod = Period;
		OS_Task[ID].RunTimer = 0;
	}
}

/********************************************************************************************************
 *  @FunctionName   OS_ClockInterruptHandle
 *  @Description    System task scheduler
 *  @Parameter      None
 *  @ReturnValue    None
 *  @Note           To ensure task real-time performance, place this in the 10ms timer system tick ISR
 ********************************************************************************************************/
void OS_ClockInterruptHandle(void)
{
	unsigned char i;
	for (i = 0; i < OS_TASK_SUM; i++) // This loop iterates through all tasks once to update parameters
	{
		if (OS_Task[i].task) // Check if task has been created by verifying the task function pointer is non-zero
		{
			OS_Task[i].RunTimer++;
			if (OS_Task[i].RunTimer >= OS_Task[i].RunPeriod) // Check if the interrupt timer value has reached the execution period
			{
				OS_Task[i].RunTimer = 0;
				OS_Task[i].RunFlag = OS_RUN; // Set task status to run. The main loop keeps polling this flag. When OS_RUN is set, the task function pointer is executed.
			}
		}
	}
}

/*******************************************************************************
 * Function Name  : void OS_Start(void)
 * Description    : Start task scheduling
 * Input          : None
 * Output         : None
 * Return         : None
 * Attention      : None
 *******************************************************************************/
void OS_Start(void)
{
	unsigned char i;
	while (1)
	{
		for (i = 0; i < OS_TASK_SUM; i++)
		{
			if (OS_Task[i].RunFlag == OS_RUN)
			{
				OS_Task[i].RunFlag = OS_SLEEP;

				(*(OS_Task[i].task))();
			}
		}
	}
}

/*******************************************************************************
 * Function Name  : void OS_TaskGetUp(OS_TaskIDTypeDef taskID)
 * Description    : Wake up a task
 * Input          : - taskID: ID of the task to wake up
 * Output         : None
 * Return         : None
 * Attention      : None
 *******************************************************************************/
void OS_TaskGetUp(OS_TaskIDTypeDef taskID)
{
	unsigned char IptStatus;
	if (CPUInterrupptCtrlCBS != 0)
	{
		CPUInterrupptCtrlCBS(CPU_ENTER_CRITICAL, &IptStatus);
	}
	OS_Task[taskID].RunFlag = OS_RUN;
	if (CPUInterrupptCtrlCBS != 0)
	{
		CPUInterrupptCtrlCBS(CPU_EXIT_CRITICAL, &IptStatus);
	}
}

/*******************************************************************************
 * Function Name  : void OS_TaskSleep(OS_TaskIDTypeDef taskID)
 * Description    : Put a task to sleep. This function is currently unused.
 * Input          : - taskID: ID of the task to put to sleep
 * Output         : None
 * Return         : None
 * Attention      : None
 *******************************************************************************/
void OS_TaskSleep(OS_TaskIDTypeDef taskID)
{
	unsigned char IptStatus;
	if (CPUInterrupptCtrlCBS != 0)
	{
		CPUInterrupptCtrlCBS(CPU_ENTER_CRITICAL, &IptStatus);
	}
	OS_Task[taskID].RunFlag = OS_SLEEP;
	if (CPUInterrupptCtrlCBS != 0)
	{
		CPUInterrupptCtrlCBS(CPU_EXIT_CRITICAL, &IptStatus);
	}
}

/********************************************************************************************************
 *  @FunctionName   S_QueueEmpty
 *  @Description    Clear a queue
 *  @Parameter      Head - Queue head address, Tail - Queue tail address, HBuff - Queue buffer
 *  @ReturnValue    None
 *  @Note           None
 ********************************************************************************************************/
void S_QueueEmpty(unsigned char **Head, unsigned char **Tail, unsigned char *HBuff)
{
	*Head = HBuff;
	*Tail = HBuff;
}

/********************************************************************************************************
 *  @FunctionName   S_QueueDataIn
 *  @Description    Write one or more bytes of data into the queue
 *  @Parameter      Head - Queue head address, Tail - Queue tail address, HBuff - Queue buffer
 *  @ReturnValue    None
 *  @Note           None
 ********************************************************************************************************/
void S_QueueDataIn(unsigned char **Head, unsigned char **Tail, unsigned char *HBuff, unsigned short Len, unsigned char *HData, unsigned short DataLen)
{
	unsigned short num;
	unsigned char IptStatus;

	if (CPUInterrupptCtrlCBS != 0)
	{
		CPUInterrupptCtrlCBS(CPU_ENTER_CRITICAL, &IptStatus);
	}
	for (num = 0; num < DataLen; num++, HData++)
	{
		**Tail = *HData;
		(*Tail)++;
		if (*Tail == HBuff + Len)
			*Tail = HBuff;
		if (*Tail == *Head)
		{
			if (++(*Head) == HBuff + Len)
				*Head = HBuff;
		}
	}
	if (CPUInterrupptCtrlCBS != 0)
	{
		CPUInterrupptCtrlCBS(CPU_EXIT_CRITICAL, &IptStatus);
	}
}

/********************************************************************************************************
 *  @FunctionName   S_QueueDataOut
 *  @Description    Read one byte of data from the queue
 *  @Parameter      Head - Queue head address, Tail - Queue tail address, HBuff - Queue buffer
 *  @ReturnValue    Retrieved data
 *  @Note           None
 ********************************************************************************************************/
unsigned char S_QueueDataOut(unsigned char **Head, unsigned char **Tail, unsigned char *HBuff, unsigned short Len, unsigned char *Data)
{
	unsigned char back = 0;
	unsigned char IptStatus;
	if (CPUInterrupptCtrlCBS != 0)
	{
		CPUInterrupptCtrlCBS(CPU_ENTER_CRITICAL, &IptStatus);
	}
	*Data = 0;
	if (*Tail != *Head)
	{
		*Data = **Head;
		back = 1;
		if (++(*Head) == HBuff + Len)
			*Head = HBuff;
	}
	if (CPUInterrupptCtrlCBS != 0)
	{
		CPUInterrupptCtrlCBS(CPU_EXIT_CRITICAL, &IptStatus);
	}
	return back;
}

/********************************************************************************************************
 *  @FunctionName   S_QueueDataLen
 *  @Description    Get the length of data in the queue
 *  @Parameter      Head - Queue head address, Tail - Queue tail address, HBuff - Queue buffer
 *  @ReturnValue    Number of data elements in the queue
 *  @Note           None
 ********************************************************************************************************/
unsigned short S_QueueDataLen(unsigned char **Head, unsigned char **Tail, unsigned short Len)
{
	if (*Tail > *Head)
		return *Tail - *Head;
	if (*Tail < *Head)
		return *Tail + Len - *Head;
	return 0;
}
