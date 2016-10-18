#include <PWM.h>

void Timer3PWMGPIOConfig()
{
	  GPIO_InitTypeDef GPIO_InitStructure;

		/* 使能定时器TIM3时钟 */

	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	  /* 使能GPIOA and GPIOB 时钟 */
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	  /*GPIOA Configuration: TIM3  2 as alternate function push-pull */
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_6;
		/*GPIO设置为复用推挽输出*/
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	  GPIO_Init(GPIOA, &GPIO_InitStructure);

}

void Timer3PWMModeConfig(int pwmPreiod,int pwmVal)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

  /*定时器配置 */
  /*周期：pwmPreiod*/
  TIM_TimeBaseStructure.TIM_Period = pwmPreiod-1;
  /*设置预分频：不预分频，即为72MHz*/
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  /*设置时钟分频系数：不分频*/
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;
  /*设置定时器计数模式：向上计数模式*/
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* PWM1 Mode configuration: Channel2 */
	/*配置为PWM模式1*/
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  /*当定时器计数值小于CCR1_Val时为高电平*/
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	/*设置通道2的电平跳变值，输出另外一个占空比的PWM*/
  TIM_OCInitStructure.TIM_Pulse = pwmVal;
  /*使能通道2*/
  TIM_OC2Init(TIM3, &TIM_OCInitStructure);

  TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);

  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	/*设置通道2的电平跳变值，输出另外一个占空比的PWM*/
  TIM_OCInitStructure.TIM_Pulse = pwmVal;
  /*使能通道2*/
  TIM_OC1Init(TIM3, &TIM_OCInitStructure);

  TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);

  /*使能TIM3重载寄存器ARR*/
  TIM_ARRPreloadConfig(TIM3, ENABLE);

  /*使能定时器3*/
  TIM_Cmd(TIM3, ENABLE);
}

void Timer3PWMInit(int pwmPreiod,int pwmVal)
{
	Timer3PWMGPIOConfig();
	Timer3PWMModeConfig( pwmPreiod, pwmVal);
}

