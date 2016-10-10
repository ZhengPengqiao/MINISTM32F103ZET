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
#include "usart.h"
#include "Key.h"
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
	unsigned char key;
	int t;
	int len;
	timer_init(); //初始化系统滴答定时器
	led_init();  //初始化LED引脚
	LCD_Init();  //初始化LCD
	key_init();
	uart_init(115200);
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
	LCD_ShowString(30, 40, 210, 24, 24, (u8*) "hello world");
	LCD_ShowString(30, 70, 200, 16, 16, (u8*) "TFTLCD TEST");
	LCD_ShowString(30, 90, 200, 16, 16, (u8*) "Look Here");
	LCD_ShowString(30, 130, 200, 12, 12, (u8*) "2016/10/05");
	while (1)
	{
		key = key_scan();
		if (key == 2)
		{
			led_toggle(0);
		}
		if (key == 3)
		{
			led_toggle(1);
		}

		if (USART_RX_STA & 0x8000)
		{
			len = USART_RX_STA & 0x3fff;
			printf("you imput is:\r\n");
			for (t = 0; t < len; t++)
			{
				USART_SendData(USART1, USART_RX_BUF[t]);
				while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET)
					;
			}
			printf("\r\n");
			LCD_ShowString(30, 230, 200, 12, 12, (u8*) USART_RX_BUF);
			USART_RX_STA = 0;
		}
	}
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
