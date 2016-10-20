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

int event_id = 0;
//绘制出画图板
void draw_plan() {
	LCD_Clear(WHITE);
	LCD_Fill(0, 0, 20, 30, BLACK);
	LCD_Fill(20, 0, 40, 30, BLUE);
	LCD_Fill(40, 0, 60, 30, BRED);
	LCD_Fill(60, 0, 80, 30, GRED);
	LCD_Fill(80, 0, 100, 30, GBLUE);
	LCD_Fill(100, 0, 120, 30, RED);
	LCD_Fill(120, 0, 140, 30, MAGENTA);
	LCD_Fill(140, 0, 160, 30, GREEN);
	LCD_Fill(160, 0, 180, 30, CYAN);
	LCD_Fill(180, 0, 200, 30, YELLOW);
	LCD_Fill(200, 0, 220, 30, BROWN);
	LCD_DrawRectangle(220, 0, 240, 30);
	POINT_COLOR = BLACK;
	LCD_ShowString(223, 5, 20, 25, 16, (u8 *) "RE");
	LCD_DrawRectangle(0, 30, 240, 60);
	LCD_ShowString(0, 35, 100, 25, 16, (u8 *) "Color:BLACK");
	LCD_ShowString(0, 35, 100, 25, 16, (u8 *) "Color:BLACK");
}

void event_msg() {
	if ((tp_dev.sta) & (1 << 7)) {  //触摸屏被按下
		if ( (tp_dev.y < 30) && (tp_dev.y >= 0) ) {
			event_id = tp_dev.x / 20 + 1;
		} else if ((tp_dev.y < 60) && (tp_dev.y >= 30)) {
			event_id = 13;
		} else {
			event_id = 14;
		}
	}
}

int main(void) {
	int oldx = 0, oldy = 0;
	int status = 0;
	delay_init(); //初始化系统滴答定时器
	led_init();  //初始化LED引脚
	LCD_Init();  //初始化LCD
	key_init();
	uart_init(115200);

	POINT_COLOR = BLUE;
	LCD_ShowString(30, 30, 210, 50, 16, (u8 *) "you can drawing>_<");
	LCD_ShowString(30, 80, 210, 50, 16, (u8 *) "(*'_'*)good luck");
	delay_ms(2000);
	tp_dev.init();
	draw_plan();
	while (1) {
		tp_dev.scan(0);
		event_msg();  //将坐标转化为事件
		switch (event_id) {
		case 1:
			POINT_COLOR = BLACK;
			LCD_ShowString(0, 35, 100, 25, 16, (u8 *) "Color:BLACK  ");
			break;
		case 2:
			POINT_COLOR = BLUE;
			LCD_ShowString(0, 35, 100, 25, 16, (u8 *) "Color:BLUE   ");
			break;
		case 3:
			POINT_COLOR = BRED;
			LCD_ShowString(0, 35, 100, 25, 16, (u8 *) "Color:BRED   ");
			break;
		case 4:
			POINT_COLOR = GRED;
			LCD_ShowString(0, 35, 100, 25, 16, (u8 *) "Color:GRED   ");
			break;
		case 5:
			POINT_COLOR = GBLUE;
			LCD_ShowString(0, 35, 100, 25, 16, (u8 *) "Color:GBLUE  ");
			break;
		case 6:
			POINT_COLOR = RED;
			LCD_ShowString(0, 35, 100, 25, 16, (u8 *) "Color:RED    ");
			break;
		case 7:
			POINT_COLOR = MAGENTA;
			LCD_ShowString(0, 35, 100, 25, 16, (u8 *) "Color:MAGENTA");
			break;
		case 8:
			POINT_COLOR = GREEN;
			LCD_ShowString(0, 35, 100, 25, 16, (u8 *) "Color:GREEN  ");
			break;
		case 9:
			POINT_COLOR = CYAN;
			LCD_ShowString(0, 35, 100, 25, 16, (u8 *) "Color:CYAN   ");
			break;
		case 10:
			POINT_COLOR = YELLOW;
			LCD_ShowString(0, 35, 100, 25, 16, (u8 *) "Color:YELLOW ");
			break;
		case 11:
			POINT_COLOR = BROWN;
			LCD_ShowString(0, 35, 100, 25, 16, (u8 *) "Color:BROWN  ");
			break;
		case 12:
			LCD_Fill(0, 61, lcddev.width, lcddev.height, WHITE);
			break;  //点击了RT
		case 14:
			if (status == 0) {
				oldx = tp_dev.x;
				oldy = tp_dev.y;
				status = 1;
			} else {
				LCD_DrawLine(oldx, oldy, tp_dev.x, tp_dev.y);
				oldx = tp_dev.x;
				oldy = tp_dev.y;
			}
			break;
		default:break;
		}
		event_id = 0;
		 //如果触摸屏松开，就标记下次画的时候，要重新记录起点
		if ( !((tp_dev.sta) & (1 << 7)) ) {
			oldx = 0;
			oldy = 0;
			status = 0;
		}
		delay_ms(10);
	}

}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
