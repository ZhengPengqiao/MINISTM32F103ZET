#include "Key.h"
#include "stdio.h"
#include "Lcd.h"
#include "sdio_sdcard.h"
void key_init() {
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	//使能时钟,和引脚复用
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	/*初始化 KEY2，中断4通道*/
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);

	/*初始化 KEY3,中断0通道*/
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

	//下降沿中断
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource4);
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  //下降沿中断
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	//上降沿中断
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  //上降沿中断
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}

void EXTI0_IRQHandler(void) {
	u8 buf[1024];
	int sd_size;
	int status;
	if (EXTI_GetITStatus(EXTI_Line0) != RESET) {

		status = SD_ReadDisk(buf, 0, 2);
		if (status == SD_OK) {
			LCD_ShowString(30, 190, 200, 16, 16,
					(u8*) "USART1 Sending Data...");
			printf("SECTOR 0 DATA:\r\n");

			/*将数据输出*/
			for (sd_size = 0; sd_size < 1024; sd_size++)
			printf("%x ", buf[sd_size]);
			printf("\r\nDATA ENDED\r\n");
			LCD_ShowString(30, 190, 200, 16, 16,
					(u8*) "USART1 Send Data Over!");
		}

		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}

//Key3按键
void EXTI4_IRQHandler(void) {
	u8 buf[1024];
	int sd_size;
	int status;
	int i;
	static int val = 0;
	if (EXTI_GetITStatus(EXTI_Line4) != RESET) {

		//短暂延时，然后查看引脚状态，防止干扰
		//向内存卡写入数据
		for(i = 0; i < 100000; i++);
		if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4) == 0)
		{
			//初始化数组
			for (sd_size = 0; sd_size < 1024; sd_size++)
			{
				buf[sd_size] = val++;
			}
			//将数据写入到分区
			status = SD_WriteDisk(buf, 0, 2);
			if (status == SD_OK) {
				LCD_ShowString(30, 230, 200, 16, 16, (u8*) "write OK");
			}

			led_toggle(1);
		}
		EXTI_ClearITPendingBit(EXTI_Line4);
	}
}
