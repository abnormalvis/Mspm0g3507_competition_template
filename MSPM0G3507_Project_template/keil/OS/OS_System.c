/*******************************************************************************
  * @brief     : OS task scheduler
  * @author   : wangming
  * @wechat   : DeepCoderMing
  * @qq       : 3201935299
  * @date     : 2025-05-01
  * @copyright: Confidential - for demo purposes only
********************************************************************************/
#include "OS_System.h"

volatile OS_TaskTypeDef OS_Task[OS_TASK_SUM];

CPUInterrupt_CallBack_t CPUInterrupptCtrlCBS;


/********************************************************************************************************
*  @������   OS_CPUInterruptCBSRegister						                                                           
*  @����     ע��CPU�жϿ��ƺ���								                                     
*  @����     pCPUInterruptCtrlCBS-CPU�жϿ��ƻص�������ַ
*  @����ֵ   ��   
*  @ע��     ��
********************************************************************************************************/
void OS_CPUInterruptCBSRegister(CPUInterrupt_CallBack_t pCPUInterruptCtrlCBS)
{
	if(CPUInterrupptCtrlCBS == 0)
	{
		CPUInterrupptCtrlCBS = pCPUInterruptCtrlCBS;
	}
}

/********************************************************************************************************
*  @������   OS_TaskInit					                                                           
*  @����     ϵͳ�����ʼ��							                                     
*  @����     ��
*  @����ֵ   ��
*  @ע��     ��
********************************************************************************************************/
void OS_TaskInit(void)
{
	unsigned char i;
	for(i=0; i<OS_TASK_SUM; i++)
	{
		OS_Task[i].task = 0;
		OS_Task[i].RunFlag = OS_SLEEP;
		OS_Task[i].RunPeriod = 0;
		OS_Task[i].RunTimer = 0;
	}	
}


/*******************************************************************************
* Function Name  : void OS_CreatTask(unsigned char ID, void (*proc)(void), OS_TIME_TYPEDEF TimeDly, bool flag)
* Description    : �������� 
* Input          : - ID������ID
*					- (*proc)() �û�������ڵ�ַ 
*					- TimeDly ����ִ��Ƶ�ʣ���λms
* 					- flag �������״̬  OS_SLEEP-���� OS_RUN-���� 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void OS_CreatTask(unsigned char ID, void (*proc)(void), unsigned short Period, OS_TaskStatusTypeDef flag)
{	
	if(!OS_Task[ID].task)
	{
		OS_Task[ID].task = proc;
		OS_Task[ID].RunFlag = OS_SLEEP;
		OS_Task[ID].RunPeriod = Period;
		OS_Task[ID].RunTimer = 0;
	}
}


/********************************************************************************************************
*  @������   OS_ClockInterruptHandle						                                                           
*  @����     ϵͳ������Ⱥ���								                                     
*  @����     ��
*  @����ֵ   ��   
*  @ע��     Ϊ�˱�֤����ʵʱ�ԣ�����������10ms�Ķ�ʱ����ϵͳʱ���жϺ�����
********************************************************************************************************/
void OS_ClockInterruptHandle(void)
{
	unsigned char i;
	for(i=0; i<OS_TASK_SUM; i++)	//���ѭ���Ƕ����е�����ִ��һ�����²�����
	{
		if(OS_Task[i].task)	//ͨ��task����ָ��ָ�򲻵���0���ж������Ƿ񱻴���
		{					
			OS_Task[i].RunTimer++;
			if(OS_Task[i].RunTimer >= OS_Task[i].RunPeriod)	//�жϼ�ʱ��ֵ�Ƿ񵽴�������Ҫִ�е�ʱ��
			{
				OS_Task[i].RunTimer = 0;
				OS_Task[i].RunFlag = OS_RUN;//�������״̬���ó�ִ�У�������Ⱥ�����һֱ�ж����������ֵ�������OS_RUN�ͻ�ִ��taskָ��ĺ�����
			}
			
		}
	}
	
}

/*******************************************************************************
* Function Name  : void OS_Start(void)
* Description    : ��ʼ���� 
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void OS_Start(void)
{
	unsigned char i;
	while(1)
	{
		for(i=0; i<OS_TASK_SUM; i++)
		{
			if(OS_Task[i].RunFlag == OS_RUN)
			{
				OS_Task[i].RunFlag = OS_SLEEP;
		 
				(*(OS_Task[i].task))();	
			}
		}	
	}
}

/*******************************************************************************
* Function Name  : void OS_TaskGetUp(OS_TaskIDTypeDef taskID)
* Description    : ����һ������
* Input          : - taskID����Ҫ�����������ID
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void OS_TaskGetUp(OS_TaskIDTypeDef taskID)
{	
	unsigned char IptStatus;
	if(CPUInterrupptCtrlCBS != 0)
	{
		CPUInterrupptCtrlCBS(CPU_ENTER_CRITICAL,&IptStatus);
	}
	OS_Task[taskID].RunFlag = OS_RUN;	
	if(CPUInterrupptCtrlCBS != 0)
	{
		CPUInterrupptCtrlCBS(CPU_EXIT_CRITICAL,&IptStatus);
	}
}

/*******************************************************************************
* Function Name  : void OS_TaskSleep(OS_TaskIDTypeDef taskID)
* Description    : ����һ��������һ���������˯��״̬���ú�����ʱû�õ�
* Input          : - taskID����Ҫ�����������ID
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void OS_TaskSleep(OS_TaskIDTypeDef taskID)
{
	unsigned char IptStatus;
	if(CPUInterrupptCtrlCBS != 0)
	{
		CPUInterrupptCtrlCBS(CPU_ENTER_CRITICAL,&IptStatus);
	}
	OS_Task[taskID].RunFlag = OS_SLEEP;
	if(CPUInterrupptCtrlCBS != 0)
	{
		CPUInterrupptCtrlCBS(CPU_EXIT_CRITICAL,&IptStatus);
	}
}

/********************************************************************************************************
*  @������   S_QueueEmpty						                                                           
*  @����     ���һ������								                                     
*  @����     Head-����ͷ��ַ,  Tail-����β��ַ,   HBuff-���л���
*  @����ֵ   ��   
*  @ע��    ��
********************************************************************************************************/
void S_QueueEmpty(unsigned char **Head, unsigned char **Tail, unsigned char *HBuff)
{
		*Head = HBuff;
		*Tail = HBuff;
}

/********************************************************************************************************
*  @������   S_QueueDataIn						                                                           
*  @����     ����һ���ֽ����ݽ�����								                                     
*  @����     Head-����ͷ��ַ,  Tail-����β��ַ,   HBuff-���л���
*  @����ֵ   ��   
*  @ע��     ��
********************************************************************************************************/
void S_QueueDataIn(unsigned char **Head, unsigned char **Tail, unsigned char *HBuff, unsigned short Len, unsigned char *HData, unsigned short DataLen)
{	
	unsigned short num;
	unsigned char IptStatus;
	
	if(CPUInterrupptCtrlCBS != 0)
	{
		CPUInterrupptCtrlCBS(CPU_ENTER_CRITICAL,&IptStatus);
	}
	for(num = 0; num < DataLen; num++, HData++)
	{
			**Tail = *HData;
			(*Tail)++;
			if(*Tail == HBuff+Len)
				*Tail = HBuff;
			if(*Tail == *Head)
			{
					if(++(*Head) == HBuff+Len)
						*Head = HBuff;		
			}
	}	
	if(CPUInterrupptCtrlCBS != 0)
	{
		CPUInterrupptCtrlCBS(CPU_EXIT_CRITICAL,&IptStatus);
	}
}

/********************************************************************************************************
*  @������   S_QueueDataOut						                                                           
*  @����     �Ӷ�����ȡ��һ������								                                     
*  @����     Head-����ͷ��ַ,  Tail-����β��ַ,   HBuff-���л���
*  @����ֵ   ȡ��������   
*  @ע��     ��
********************************************************************************************************/
unsigned char S_QueueDataOut(unsigned char **Head, unsigned char **Tail, unsigned char *HBuff, unsigned short Len, unsigned char *Data)
{					   
	unsigned char back = 0;
	unsigned char IptStatus;
	if(CPUInterrupptCtrlCBS != 0)
	{
		CPUInterrupptCtrlCBS(CPU_ENTER_CRITICAL,&IptStatus);
	}
	*Data = 0;
	if(*Tail != *Head)
	{
			*Data = **Head;
			back = 1; 				
			if(++(*Head) == HBuff+Len)
				*Head = HBuff;
	}
	if(CPUInterrupptCtrlCBS != 0)
	{
		CPUInterrupptCtrlCBS(CPU_EXIT_CRITICAL,&IptStatus);
	}
	return back;	
}

/********************************************************************************************************
*  @������   S_QueueDataLen						                                                           
*  @����     �ж϶��������ݵĳ���							                                     
*  @����     Head-����ͷ��ַ,  Tail-����β��ַ,   HBuff-���л���
*  @����ֵ   �����������ݸ���
*  @ע��     ��
********************************************************************************************************/
unsigned short S_QueueDataLen(unsigned char **Head, unsigned char **Tail, unsigned short Len)
{
		if(*Tail > *Head)
			return *Tail-*Head;
		if(*Tail < *Head)
			return *Tail+Len-*Head;
		return 0;
}

