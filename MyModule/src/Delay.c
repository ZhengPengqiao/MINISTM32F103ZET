/*
 * 文件名称 : delay.c
 * 文件介绍 : 实现睡眠函数
 */

#include <Delay.h>
#include "cortexm/ExceptionHandlers.h"

//函数声明
void delay_tick (void);

// ----------------------------------------------------------------------------

volatile delay_ticks_t delay_InterruptCount;
static u32  fac_us=0;         //us延时倍乘数
static u32  fac_ms=0;         //ms延时倍乘数

/*******************************************************************************
 * 函数名称 : delay_init
 * 函数介绍 : 定时器初始化
 * 参数介绍 : 无
 * 返回值  : 无
 ******************************************************************************/
void delay_init()
{
  SysTick->CTRL |= SysTick_CLKSource_HCLK;  //选择外部时钟  HCLK
  SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;  //开启中断
  fac_us=SystemCoreClock/1000000;       //要定义微妙时的时钟脉冲数
  fac_ms=(u16)fac_us*1000;            //要定义毫秒时的时钟脉冲数
}

/*******************************************************************************
 * 函数名称 : delay_us
 * 函数介绍 : 延时US的时间,定时范围 < 233ms
 * 参数介绍 : nus : 要延时的微妙数
 * 返回值   :无
 ******************************************************************************/
void delay_us(u32 nus)
{
  SysTick->LOAD=nus*fac_us;           //时间加载,将要定时的微妙数直接进行定时
  SysTick->VAL=0x00;                  //清空计数器
  SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;  //开始倒数

  delay_InterruptCount = 1;                   //因为直接定时所需时间,所以中断一次就可以了

  // Busy wait until the SysTick decrements the counter to zero.
  while (delay_InterruptCount != 0u)
  ;

  SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;  //关闭计数器
  SysTick->VAL =0X00;                //清空计数器
}

/*******************************************************************************
 * 函数名称 : delay_ms
 * 函数介绍 : 延时MS的时间
 * 参数介绍 : nms : 要延时的毫秒数
 * 返回值   :无
 ******************************************************************************/
void delay_ms(u32 nms)
{
  SysTick->LOAD=(u32)fac_ms;
  SysTick->VAL =0x00;             //清空计数器
  SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;  //开始倒数

  delay_InterruptCount = nms;  //中断的次数

  // Busy wait until the SysTick decrements the counter to zero.
  while (delay_InterruptCount != 0u)
  ;

  SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;  //关闭计数器
  SysTick->VAL =0X00;                 //清空计数器
}


/*******************************************************************************
 * 函数名称 : delay_tick
 * 函数介绍 : 中断服务函数中将会调用的函数
 ******************************************************************************/
void delay_tick (void)
{
    // 在中断服务函数中没调用一次,就较少一次,知道减少到0
    if (delay_InterruptCount != 0u)
    {
        --delay_InterruptCount;
    }
}


/*******************************************************************************
 * 函数名称 : SysTick_Handler
 * 函数介绍 : 时钟函数,定时器每到一个系统时钟,就会调用这个函数
 ******************************************************************************/
void SysTick_Handler (void)
{
    delay_tick ();
}
