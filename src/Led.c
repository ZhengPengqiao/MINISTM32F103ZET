//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

#include "Led.h"

#define LED0 PEout(6)
#define LED1 PEout(5)
// ----------------------------------------------------------------------------

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

void led_on(unsigned char LedNum)
{
  if( LedNum == 0 )
  {
    LED0 = 1;
  }
  else
  {
    LED1 = 1;
  }


}

void led_off(unsigned char LedNum)
{
  if( LedNum == 0 )
  {
    LED0 = 0;
  }
  else
  {
    LED1 = 0;
  }
}

void led_toggle(unsigned char LedNum)
{
  if( LedNum == 0 )
  {
    LED0 = ~LED0;
  }
  else
  {
    LED1 = ~LED1;
  }
}
// ----------------------------------------------------------------------------
