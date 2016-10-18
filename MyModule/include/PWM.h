#ifndef __PWM_H_
#define __PWM_H_
#include "stm32f10x.h"
#include "stm32f10x_conf.h"

//使用定时器3完成PWM的功能
extern void Timer3PWMInit(int pwmPreiod,int pwmVal);


#endif  //__PWM_H_
