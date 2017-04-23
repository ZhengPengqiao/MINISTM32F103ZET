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
#include "rtc.h"
#include "Adc.h"

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

int main(void) {
	//测试时间，和显示时间不相对，  但是几时是正确的，   用来计算测试时间长度
	calendarStruct startTest;  //测试开始时间
	calendarStruct startNow;   //测试到现在的时间
	int testCount = 0;  //到现在测试的总次数
	int testErrCount = 0; //到现在测试时出现的错误次数
	int startUpTime = 6;  //设置启动时，等待启动完成的时间 (单位s)
	int cutOutTime = 3;   //设置断电，等待完全断电需要等待的时间 (单位s)
	u16 adcVal = 0;       //存储得到的ADC电压数值 （0~4096）
	u32 timeCount = 0;    //用来记录循环的次数
	u32 ledTimeCount = 0; //led运行的时间间隔
	u32 lcdTimeCount = 0; //lcd显示的刷新
	u32 nextDoTimeCount = 0;   //下次事件的时间
	 //程序当前状态 , 0：上电  1：等待开机 ， 2：测试引脚状态  3：断电 4：等待断电
	u32 nowStatus = 0;
	float temp;
	u32 adcIntager = 0, adcPoint = 0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级

	delay_init(); //初始化系统滴答定时器
	led_init();  //初始化LED引脚
	LCD_Init();  //初始化LCD
	Adc_Init();  //初始化ADC
	key_init();
	uart_init(115200);
	rtcInit();
	LCD_ShowString(10,10,480,16,16,(u8*)" Blank Screen Test Program");
	POINT_COLOR=BLUE;
	while (1) {


		//按照一定时间间隔刷新LCD
		if ( timeCount >= lcdTimeCount ) {
			//显示和设置开始等待时间，显示和设置断电时间
			LCD_ShowString(10,40,200,16,16,(u8*)"startUpTime:     s  -- ++");
			LCD_ShowxNum(104,40,startUpTime,4,16,0);//显示上电等待时间
			LCD_ShowString(10,70,200,16,16,(u8*)"cutOutTime :     s  -- ++");
			LCD_ShowxNum(104,70,cutOutTime,4,16,0);//显示断电等待时间

			//显示ADC测试的数值
			LCD_ShowString(16,100,200,16,16,(u8*)"ADC VAL:");
			LCD_ShowxNum(160,100,adcVal,4,16,0);//显示ADC的值
			//显示ADC测试的电压
			LCD_ShowString(16,130,160,16,16,(u8*)"VOL VAL: .");
			LCD_ShowxNum(80,130,adcIntager,1,16,0);//显示电压值整数部分
			LCD_ShowxNum(96,130,adcPoint,3,16,0X80); //显示电压值小数部分

			//显示测试次数和错误次数
			LCD_ShowString(16,160,160,16,16,(u8*)"Test Times :");
			LCD_ShowxNum(160,160,testCount,3,16,0X80); //显示测试次数
			LCD_ShowString(16,192,160,16,16,(u8*)"Test ErrTimes :");
			LCD_ShowxNum(160,192,testErrCount,3,16,0X80); //显示测试错误次数

			lcdTimeCount += 100;  //间隔100次
		}

		 //程序当前状态 , 0：上电  1：等待开机 ， 2：测试引脚状态  3：断电 4：等待断电
		switch( nowStatus ){
		case 0:
			//上电
			led_off(0);  //这是引脚输出高电压
			 //将状态设置为等待上电完成， 并更新上电等待完成时的时间
			nextDoTimeCount = timeCount + startUpTime*100;
			nowStatus = 1;
			LCD_ShowString(50,288,160,16,16,(u8*)"Wait Power On....");
			break;
		case 1:
			if ( timeCount >= nextDoTimeCount ) {
				//当时间到达上电等待完成的时间时，  将状态改为测试ADC
				nowStatus = 2;

				LCD_ShowString(50,288,160,16,16,(u8*)"Read ADC And Test");
			}
			break;
		case 2:
			adcVal = Get_Adc_Average(ADC_Channel_1,20);  //测20次的平均数
			temp=(float)adcVal*(3.3/4096);  //计算电压
			adcIntager = temp;                  //得到整数部分
			temp-=adcIntager;                   //计算小数部分
			adcPoint = temp * 1000;             //换算成整数

			testCount++;  //测试次数增加

			if ( temp < 3 )
			{
				testErrCount++;
			}
			nowStatus = 3;

			LCD_ShowString(50,288,160,16,16,(u8*)"Power OFF");
			break;
		case 3:  //断电
			led_on(0);  //这是引脚输出低电压
			nextDoTimeCount = timeCount + cutOutTime * 100;
			nowStatus = 4;

			LCD_ShowString(50,288,160,16,16,(u8*)"Wait Power OFF...");
			break;
		case 4:  //等待断电完成
			if ( timeCount >= nextDoTimeCount ) {
				nowStatus = 0;

				LCD_ShowString(50,288,160,16,16,(u8*)"Power On");
			}
			break;
		default:
			break;
		}

		//按照间隔闪烁LED灯
		if ( timeCount >= ledTimeCount ) {
			led_toggle(1);
			ledTimeCount += 100;   //间隔100次
		}

		delay_ms(10);   //每次间隔延时10ms
		timeCount++;    //时间间隔加1
	};
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
