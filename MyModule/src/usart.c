/*
 * usart.c
 *
 *  Created on: 2016年10月8日
 *      Author: Administrator
 */
/*******************************************************************************
 *
 ******************************************************************************/
#include "usart.h"

/*******************************************************************************
 *定义全局变量
 ******************************************************************************/
u8 USART_RX_BUF[USART_REC_LEN];
u16 USART_RX_STA;

/*******************************************************************************
 * 函数名称 : sendChar
 * 函数介绍 : 发送一个字符
 * 参数介绍 : ch : 要发送的字符
 * 返回值   : 要发送的ch
 ******************************************************************************/
int sendChar(int ch)
{
	USART_SendData(USART1, (unsigned char)ch);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET)
		;
	return ch;
}

/*******************************************************************************
 * 函数名称 : sendString
 * 函数介绍 : 发送一个字符串
 * 参数介绍 : str : 要发送的字符串
 * 			len : 字符串的长度
 * 返回值   : 无
 ******************************************************************************/
void sendString(unsigned char *str, int len)
{
	int i;
	for(i = 0; i < len; i++)
	{
		sendChar(str[i]);
	}
}

/*******************************************************************************
 * 函数名称 : uart_init
 * 函数介绍 : 串口初始化
 * 参数介绍 : bound : 波特率
 * 返回值   :无
 ******************************************************************************/
void uart_init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitTypeDefStruct;
	NVIC_InitTypeDef NVIC_InitTypeDefStruct;
	USART_InitTypeDef USART_InitTypeDefStruct;
	RCC_APB1PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA,
			ENABLE);

	//USART_TX : GPIOA.9
	GPIO_InitTypeDefStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitTypeDefStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitTypeDefStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitTypeDefStruct);  //初始化引脚GPIOA.9

	GPIO_InitTypeDefStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitTypeDefStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitTypeDefStruct);  //初始化引脚PIOA.10

	NVIC_InitTypeDefStruct.NVIC_IRQChannel = USART1_IRQn;
	//抢占优先级
	NVIC_InitTypeDefStruct.NVIC_IRQChannelPreemptionPriority = 3;
	//子优先级
	NVIC_InitTypeDefStruct.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitTypeDefStruct.NVIC_IRQChannelCmd = ENABLE;  //使能串口通道
	NVIC_Init(&NVIC_InitTypeDefStruct);   //初始化USART1的中断

	USART_InitTypeDefStruct.USART_BaudRate = bound;  //波特率
	//硬件流控制
	USART_InitTypeDefStruct.USART_HardwareFlowControl =
	USART_HardwareFlowControl_None;
	//模式
	USART_InitTypeDefStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	//无校验位
	USART_InitTypeDefStruct.USART_Parity = USART_Parity_No;
	//一个停止位
	USART_InitTypeDefStruct.USART_StopBits = USART_StopBits_1;
	//数据长度为8
	USART_InitTypeDefStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitTypeDefStruct);  //初始化USART1
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);  //使能接收
	USART_Cmd(USART1, ENABLE);                     //使能USART1
}

void USART1_IRQHandler(void)
{
	u8 Res;

	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		Res = USART_ReceiveData(USART1);

		if ((USART_RX_STA & 0x8000) == 0)
		{
			if (USART_RX_STA & 0x4000)
			{
				if (Res != 0x0a)
					USART_RX_STA = 0;
				else
					USART_RX_STA |= 0x8000;
			}
			else
			{
				if (Res == 0x0d)
					USART_RX_STA |= 0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA & 0X3FFF] = Res;  //将Res存入缓冲区
					USART_RX_STA++;                             //个数加1
					//如果一读取数据超过缓冲区大小,就清空缓冲区,其实就是将下一个字节的存放位
					//置指向0
					if (USART_RX_STA > (USART_REC_LEN - 1))
						USART_RX_STA = 0;
				}
			}
		}
	}
}
