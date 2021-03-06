//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

#ifndef __LED_H_
#define __LED_H_

#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "PIN.h"

// ----------------------------------------------------------------------------

/*******************************************************************************
 * 函数名称 : led_toggle
 * 函数介绍 : 翻转指定的LED灯
 * 参数介绍 : ledNum : 要翻转LED的编号
 * 返回值   :无
 ******************************************************************************/
extern void led_init(void);


/*******************************************************************************
 * 函数名称 : led_on
 * 函数介绍 : 点亮指定的LED灯
 * 参数介绍 : ledNum : 要点亮的小灯编号
 * 返回值   :无
 ******************************************************************************/
extern void led_on(unsigned char LedNum);
/*******************************************************************************
 * 函数名称 : led_off
 * 函数介绍 : 熄灭指定的LED灯
 * 参数介绍 : ledNum : 将要熄灭的LED灯的编号
 * 返回值   :无
 ******************************************************************************/
extern void led_off(unsigned char LedNum);
/*******************************************************************************
 * 函数名称 : led_toggle
 * 函数介绍 : 翻转指定的LED灯
 * 参数介绍 : ledNum : 要翻转LED的编号
 * 返回值   :无
 ******************************************************************************/
extern void led_toggle(unsigned char LedNum);

// ----------------------------------------------------------------------------

#endif // __LED_H_
