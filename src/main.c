//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"
#include "stm32f10x_conf.h"
#include "Led.h"
#include "Lcd.h"
#include "Timer.h"
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

int main(void)
{
  timer_init(); //初始化系统滴答定时器
	led_init();  //初始化LED引脚
  LCD_Init();  //初始化LCD
  LCD_ShowString(30,40,210,24,24,(u8*)"hello world");
  LCD_ShowString(30,70,200,16,16,(u8*)"TFTLCD TEST");
  LCD_ShowString(30,90,200,16,16,(u8*)"Look Here");
  LCD_ShowString(30,130,200,12,12,(u8*)"2016/10/05");
	while(1)
	{
	  led_on(0);  //点亮LED0
	  led_off(1); //熄灭LED1

	  timer_ms(1000);

		led_off(0);  //熄灭LED0
		led_on(1);   //点亮LED1

		timer_ms(1000);

	}
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
