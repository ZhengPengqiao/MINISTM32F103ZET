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
#include "Led.h"
#include "Lcd.h"
#include "math.h"
#include "touch.h"
#include "usart.h"
#include "Key.h"
#include "PWM.h"
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
	int oldx = 0, oldy = 0;
	int status = 0;
	delay_init(); //初始化系统滴答定时器
	led_init();  //初始化LED引脚
	LCD_Init();  //初始化LCD
	key_init();
	uart_init(115200);
	tp_dev.init();
	while (1) {
		tp_dev.scan(0);

		if ((tp_dev.sta) & (1 << 7)) {
			if (status == 0) {
				oldx = tp_dev.x;
				oldy = tp_dev.y;
				status = 1;
			} else {
				LCD_DrawLine(oldx, oldy, tp_dev.x, tp_dev.y);
				oldx = tp_dev.x;
				oldy = tp_dev.y;
			}
		}
	}

}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
