#include "Key.h"
#include "stdio.h"
#include "Lcd.h"
#include "sdio_sdcard.h"
#include "ff.h"
int sec = 0;
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
	int i;
	if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
		//短暂延时，然后查看引脚状态，防止干扰
		//向内存卡写入数据
		for(i = 0; i < 100000; i++);
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) == 1)
		{

/*====按键用户处理部分          ==========================================================*/

/*====按键用户处理部分结束位置      ======================================================*/
		}
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}

//Key3按键
void EXTI4_IRQHandler(void) {
	int i = 1;

	int res;  //读写文件的返回值
	int a;
	FIL  fd;  //文件系统结构体，包含文件指针等成员。
	UINT br ;     //File R/W count
	BYTE buffer[512];  //file copy buffer
	if (EXTI_GetITStatus(EXTI_Line4) != RESET) {

		//短暂延时，然后查看引脚状态，防止干扰
		//向内存卡写入数据
		for(i = 0; i < 100000; i++);

		if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4) == 0)
		{

/*==== 按键用户处理部分     ==========================================================*/
			res = f_open(&fd, "0:/Dem1.txt", FA_OPEN_EXISTING | FA_READ);

			if(res == FR_OK){
				printf("File open OK\r\n");
			} else {
				printf("file open err  %d\r\n",res);
			}

			/*打开文件*/
			br = 1;
			a = 0;
			for(;;){
				/*清空缓冲区*/
				for(a = 0; a < 512; a++){
					buffer[a] = 0;
				}
				/*将文件内容读到缓冲区*/
				res = f_read(&fd,buffer,sizeof(buffer),&br);
				/*输出到控制台*/
				printf("%d Line: %s \r\n",i++,buffer);
				if(res || br == 0){
					break;
				}
			}
			f_close(&fd);
			led_toggle(1);
/*====  按键用户处理部分结束位置      ====================================================*/

		}
		EXTI_ClearITPendingBit(EXTI_Line4);
	}
}
