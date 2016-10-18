#ifndef __KEY_H_
#define __KEY_H_
#include "PIN.h"
#include "Led.h"
#include "Delay.h"
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#define KEY2  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)
#define KEY3  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)

extern void key_init();

extern unsigned char key_scan();

extern unsigned char KEY2_Scan(void);

extern unsigned char KEY3_Scan(void);

#endif
