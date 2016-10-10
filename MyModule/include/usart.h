/*
 * usart.h
 *
 *  Created on: 2016骞�10鏈�8鏃�
 *      Author: Administrator
 */

#ifndef USART_H_
#define USART_H_
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "PIN.h"

#define USART_REC_LEN  			200  	//接收缓冲区长度
extern u8 USART_RX_BUF[USART_REC_LEN];  //接受缓冲区
extern u16 USART_RX_STA;                 //接受状态
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

extern PUTCHAR_PROTOTYPE;
/*******************************************************************************
 * 函数名称 : sendString
 * 函数介绍 : 发送一个字符串
 * 参数介绍 : str : 要发送的字符串
 * 			len : 字符串的长度
 * 返回值   : 无
 ******************************************************************************/
extern void sendString(unsigned char *str, int len);

/*******************************************************************************
 * 函数名称 : sendChar
 * 函数介绍 : 发送一个字符
 * 参数介绍 : ch : 要发送的字符
 * 返回值   : 要发送的ch
 ******************************************************************************/
extern int sendChar(int ch);
/*******************************************************************************
 * 函数名称 : uart_init
 * 函数介绍 : 串口初始化
 * 参数介绍 : bound : 波特率
 * 返回值   :无
 ******************************************************************************/
extern void uart_init(u32 bound);

#endif /* USART_H_ */