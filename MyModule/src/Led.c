//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

#include "Led.h"

#define LED0 PEout(6)
#define LED1 PEout(5)
// ----------------------------------------------------------------------------

/*******************************************************************************
 * 函数名称 : led_init
 * 函数介绍 : 初始化LED灯的引脚
 * 参数介绍 : 无
 * 返回值   :无
 ******************************************************************************/
void led_init()
{
   GPIO_InitTypeDef  GPIO_InitStructure;

   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE, ENABLE);  //ʹ��PB,PE�˿�ʱ��

   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(GPIOE, &GPIO_InitStructure);
   GPIO_SetBits(GPIOE,GPIO_Pin_6);

   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
   GPIO_Init(GPIOE, &GPIO_InitStructure);
   GPIO_SetBits(GPIOE,GPIO_Pin_5);
}


/*******************************************************************************
 * 函数名称 : led_on
 * 函数介绍 : 点亮指定的LED灯
 * 参数介绍 : ledNum : 要点亮的小灯编号
 * 返回值   :无
 ******************************************************************************/
void led_on(unsigned char ledNum)
{
  if( ledNum == 0 )
  {
    LED0 = 1;
  }
  else
  {
    LED1 = 1;
  }


}

/*******************************************************************************
 * 函数名称 : led_off
 * 函数介绍 : 熄灭指定的LED灯
 * 参数介绍 : ledNum : 将要熄灭的LED灯的编号
 * 返回值   :无
 ******************************************************************************/
void led_off(unsigned char ledNum)
{
  if( ledNum == 0 )
  {
    LED0 = 0;
  }
  else
  {
    LED1 = 0;
  }
}

/*******************************************************************************
 * 函数名称 : led_toggle
 * 函数介绍 : 翻转指定的LED灯
 * 参数介绍 : ledNum : 要翻转LED的编号
 * 返回值   :无
 ******************************************************************************/
void led_toggle(unsigned char ledNum)
{
  if( ledNum == 0 )
  {
    LED0 = ~LED0;
  }
  else
  {
    LED1 = ~LED1;
  }
}
// ----------------------------------------------------------------------------
