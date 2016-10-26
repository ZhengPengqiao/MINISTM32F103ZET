#include "Key.h"
#include "DS1302.h"

/*定义按键事件*/
int key = 0;
void key_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	//使能时钟,和引脚复用
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE|RCC_APB2Periph_AFIO,ENABLE);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	//配置中断源
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);

	//配置中断源
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);


	/*初始化 KEY3-->GPIOE.4	  上拉输入*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	/*初始化 KEY2-->GPIOA.0	  下拉输入*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//设置上拉中断
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource4);
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  //下降沿中断
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	//设置下拉中断
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0);
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  //上降沿中断
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}


unsigned char getKey()
{
	unsigned char tem = key;
	key = 0;
	return tem;
}

void EXTI0_IRQHandler(void)
{
	int i;
	if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
		//短暂延时，然后查看引脚状态，防止干扰
		//向内存卡写入数据
		for(i = 0; i < 100000; i++);
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) == 1)
		{

/*====按键用户处理部分          ==========================================================*/
			key = 2;
			led_toggle(0);
/*====按键用户处理部分结束位置      ======================================================*/
		}
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}

void EXTI4_IRQHandler(void)
{
	int i;
	if(EXTI_GetITStatus(EXTI_Line4) != RESET)
	{
		//短暂延时，然后查看引脚状态，防止干扰
		for(i = 0; i < 100000; i++);

		if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4) == 0)
		{
/*==== 按键用户处理部分=========================================================*/
			key = 3;
			led_toggle(1);
/*==== 按键用户处理部分结束位置===================================================*/
		}
		EXTI_ClearITPendingBit(EXTI_Line4);
	}
}
