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
#include "rtc.h"

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
	int t = -1;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级

	delay_init(); //初始化系统滴答定时器
	led_init();  //初始化LED引脚
	LCD_Init();  //初始化LCD
	key_init();
	uart_init(115200);
	rtcInit();
	POINT_COLOR=DARKBLUE;
	LCD_ShowString(60,70,200,16,16,(u8*)"RTC TEST");

	POINT_COLOR=BLUE;
	LCD_ShowString(60,130,200,16,16,(u8*)"    -  -  ");
	LCD_ShowString(60,162,200,16,16,(u8*)"  :  :  ");
	while (1) {
		if (t != calendar.sec) {
			t = calendar.sec;
			LCD_ShowNum(60, 130, calendar.w_year, 4, 16);
			LCD_ShowNum(100, 130, calendar.w_month, 2, 16);
			LCD_ShowNum(124, 130, calendar.w_date, 2, 16);
			switch (calendar.week) {
			case 0:
				LCD_ShowString(60, 148, 200, 16, 16, (u8*)"Sunday   ");
				break;
			case 1:
				LCD_ShowString(60, 148, 200, 16, 16, (u8*)"Monday   ");
				break;
			case 2:
				LCD_ShowString(60, 148, 200, 16, 16, (u8*)"Tuesday  ");
				break;
			case 3:
				LCD_ShowString(60, 148, 200, 16, 16, (u8*)"Wednesday");
				break;
			case 4:
				LCD_ShowString(60, 148, 200, 16, 16, (u8*)"Thursday ");
				break;
			case 5:
				LCD_ShowString(60, 148, 200, 16, 16, (u8*)"Friday   ");
				break;
			case 6:
				LCD_ShowString(60, 148, 200, 16, 16, (u8*)"Saturday ");
				break;
			}
			LCD_ShowNum(60, 162, calendar.hour, 2, 16);
			LCD_ShowNum(84, 162, calendar.min, 2, 16);
			LCD_ShowNum(108, 162, calendar.sec, 2, 16);
			led_toggle(0);
		}
		delay_ms(10);
	};
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
