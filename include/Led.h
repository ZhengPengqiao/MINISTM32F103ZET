//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

#ifndef BLINKLED_H_
#define BLINKLED_H_

#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "PIN.h"

/*
 * 函数名称 : blink_led_init
 * 函数介绍 : 初始化LED
 */
extern void led_init(void);

// ----------------------------------------------------------------------------

/*
 * 函数名称 : led_on
 * 函数介绍 : 开LED
 * 参数介绍 : LedNum : 操作的LED灯编号
 */
extern void led_on(unsigned char LedNum);
/*
 * 函数名称 : led_off
 * 函数介绍 : 关LED
 * 参数介绍 : LedNum : 操作的LED灯编号
 */
extern void led_off(unsigned char LedNum);
/*
 * 函数名称 : led_toggle
 * 函数介绍 : 翻转LED
 * 参数介绍 : LedNum : 操作的LED灯编号
 */
extern void led_toggle(unsigned char LedNum);

// ----------------------------------------------------------------------------

#endif // BLINKLED_H_
