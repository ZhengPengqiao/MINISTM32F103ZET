//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <Delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "diag/Trace.h"
#include "stm32f10x_conf.h"
#include "stm32f10x.h"
#include "Led.h"
#include "Lcd.h"
#include "math.h"
#include "touch.h"
#include "usart.h"
#include "Key.h"
#include "PWM.h"
#include "Delay.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "sdio_sdcard.h"
#include "ff.h"

// ----------------------------------------------------------------------------
//
// Standalone STM32F1 led blink sample (trace via DEBUG).
//
// In debug configurations, demonstrate how to print a greeting message
// on the trace device. In release configurations the message is
// simply discarded.
//
// Then demonstrates how to blink a led with 1 Hz, using a
// continuous loop and SysTick delays.
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the DEBUG output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//
// The external clock frequency is specified as a preprocessor definition
// passed to the compiler via a command line option (see the 'C/C++ General' ->
// 'Paths and Symbols' -> the 'Symbols' tab, if you want to change it).
// The value selected during project creation was HSE_VALUE=8000000.
//
// Note: The default clock settings take the user defined HSE_VALUE and try
// to reach the maximum possible system clock. For the default 8 MHz input
// the result is guaranteed, but for other values it might not be possible,
// so please adjust the PLL settings in system/src/cmsis/system_stm32f10x.c
//

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

extern SD_CardInfo SDCardInfo;
/*******************************************************************************
 * 函数名称 : show_sdcard_info
 * 函数介绍 : 通过串口打印SD卡相关信息
 * 参数介绍 : 无
 * 返回值     : 无
 ******************************************************************************/
void show_sdcard_info(void) {
	switch (SDCardInfo.CardType) {
	case SDIO_STD_CAPACITY_SD_CARD_V1_1:
		printf("Card Type:SDSC V1.1\r\n");
		break;
	case SDIO_STD_CAPACITY_SD_CARD_V2_0:
		printf("Card Type:SDSC V2.0\r\n");
		break;
	case SDIO_HIGH_CAPACITY_SD_CARD:
		printf("Card Type:SDHC V2.0\r\n");
		break;
	case SDIO_MULTIMEDIA_CARD:
		printf("Card Type:MMC Card\r\n");
		break;
	}
	printf("Card ManufacturerID:%d\r\n", SDCardInfo.SD_cid.ManufacturerID);	//制造商ID
	printf("Card RCA:%d\r\n", SDCardInfo.RCA);							//卡相对地址
	//显示容量   %ld位长类型
	printf("Card Capacity:%ld MB\r\n", (u32) (SDCardInfo.CardCapacity >> 20));
	//显示块大小  %ld位长类型
	printf("Card BlockSize:%ld\r\n\r\n", SDCardInfo.CardBlockSize);
}

int res;  //读写文件的返回值
int a;
int i = 0;
FIL  fd;  //文件系统结构体，包含文件指针等成员。
FATFS fs;       //记录文件系统盘符结构体信息的结构体
UINT br, bw;     //File R/W count
BYTE buffer[512];  //file copy buffer
BYTE textFileBuffer[] = "这是测试文字，只是用来测试，看看乱码不！ >_<\r\naaaaaaaaa\r\n"
		"bbbbbbbbbbbbb\r\nccccccccccccccccc\r\nddddddddddd\r\n";

int main(void) {
	delay_init(); //初始化系统滴答定时器
	led_init();  //初始化LED引脚
	LCD_Init();  //初始化LCD
	key_init();
	uart_init(115200);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级

	MPU_Init();					//初始化MPU6050
	POINT_COLOR = RED;			//设置字体为红色
	LCD_ShowString(30, 50, 200, 16, 16, (u8 *) "My SD Card Test");
	LCD_ShowString(30, 70, 200, 16, 16, (u8 *) "KEY0:Mult Sector Test");

	res = f_mount(0,&fs);
	if (res == FR_OK) {
		printf("File mount OK \r\n");
	} else {
		printf("File mount ERR  %d\r\n",res);
	}

	show_sdcard_info();	//打印SD卡相关信息
	POINT_COLOR = BLUE;	//设置字体为蓝色
	//检测SD卡成功,显示SD卡信息
	LCD_ShowString(30, 150, 200, 16, 16, (u8 *) "SD Card OK    ");
	LCD_ShowString(30, 170, 200, 16, 16, (u8 *) "SD Card Size:        GB");
	LCD_ShowNum(30 + 13 * 8, 170, (SDCardInfo.CardCapacity >> 30), 5, 16);//显示SD卡容量
	LCD_ShowString(30, 190, 220, 16, 16, (u8 *) "Card BlockSize:");
	LCD_ShowNum(30 + 16 * 8, 190, SDCardInfo.CardBlockSize, 5, 16);	//显示SD卡容量
	LCD_ShowString(30, 210, 220, 16, 16, (u8 *) "Card CardType:");
	LCD_ShowNum(30 + 16 * 8, 210, SDCardInfo.CardType, 5, 16);


	res = f_open(&fd, "0:/Dem2.txt", FA_CREATE_NEW | FA_WRITE);

	if (res == FR_OK) {
		/*将缓冲区的内容写入到文件中*/
		res = f_write(&fd, textFileBuffer, sizeof(textFileBuffer), &bw);
		printf("File create OK\r\n");
		f_close(&fd);
	} else {
		printf("file exit... %d\r\n",res);
	}

	/*以只读方式打开刚刚创建的文件*/
	res = f_open(&fd, "0:/Dem2.txt", FA_OPEN_EXISTING | FA_READ);

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

	while (1) {
		delay_ms(300);
		led_toggle(0);
	}
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
