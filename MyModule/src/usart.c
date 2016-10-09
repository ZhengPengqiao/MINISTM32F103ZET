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
/*
 * newlib_stubs.c
 *
 *  Created on: 2 Nov 2010
 *      Author: nanoage.co.uk
 */
#include <errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>
#include "stm32f10x_usart.h"
/*******************************************************************************
 *定义全局变量
 ******************************************************************************/
u8 USART_RX_BUF[USART_REC_LEN];
u16 USART_RX_STA;

PUTCHAR_PROTOTYPE
{
	/* Place your implementation of fputc here */
	USART_SendData(USART1, (uint8_t) ch);
	/* Loop until the end of transmission */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	{
	}
	return ch;
}

/*******************************************************************************
 * 函数名称 : sendChar
 * 函数介绍 : 发送一个字符
 * 参数介绍 : ch : 要发送的字符
 * 返回值   : 要发送的ch
 ******************************************************************************/
int sendChar(int ch)
{
	USART_SendData(USART1, (unsigned char) ch);
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
	for (i = 0; i < len; i++)
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



#ifndef STDOUT_USART
#define STDOUT_USART 1
#endif

#ifndef STDERR_USART
#define STDERR_USART 1
#endif

#ifndef STDIN_USART
#define STDIN_USART 1
#endif

#undef errno
extern int errno;

/*
 environ
 A pointer to a list of environment variables and their values.
 For a minimal environment, this empty list is adequate:
 */
char *__env[1] =
{ 0 };
char **environ = __env;

int _write(int file, char *ptr, int len);

void _exit(int status)
{
	_write(1, "exit", 4);
	while (1)
	{
		;
	}
}

int _close(int file)
{
	return -1;
}
/*
 execve
 Transfer control to a new process. Minimal implementation (for a system without processes):
 */
int _execve(char *name, char **argv, char **env)
{
	errno = ENOMEM;
	return -1;
}
/*
 fork
 Create a new process. Minimal implementation (for a system without processes):
 */

int _fork()
{
	errno = EAGAIN;
	return -1;
}
/*
 fstat
 Status of an open file. For consistency with other minimal implementations in these examples,
 all files are regarded as character special devices.
 The `sys/stat.h' header file required is distributed in the `include' subdirectory for this C library.
 */
int _fstat(int file, struct stat *st)
{
	st->st_mode = S_IFCHR;
	return 0;
}

/*
 getpid
 Process-ID; this is sometimes used to generate strings unlikely to conflict with other processes. Minimal implementation, for a system without processes:
 */

int _getpid()
{
	return 1;
}

/*
 isatty
 Query whether output stream is a terminal. For consistency with the other minimal implementations,
 */
int _isatty(int file)
{
	switch (file)
	{
	case STDOUT_FILENO:
	case STDERR_FILENO:
	case STDIN_FILENO:
		return 1;
	default:
		//errno = ENOTTY;
		errno = EBADF;
		return 0;
	}
}

/*
 kill
 Send a signal. Minimal implementation:
 */
int _kill(int pid, int sig)
{
	errno = EINVAL;
	return (-1);
}

/*
 link
 Establish a new name for an existing file. Minimal implementation:
 */

int _link(char *old, char *new)
{
	errno = EMLINK;
	return -1;
}

/*
 lseek
 Set position in a file. Minimal implementation:
 */
int _lseek(int file, int ptr, int dir)
{
	return 0;
}

/*
 sbrk
 Increase program data space.
 Malloc and related functions depend on this
 */
/*
caddr_t _sbrk(int incr)
{

	extern char _ebss; // Defined by the linker
	static char *heap_end;
	char *prev_heap_end;

	if (heap_end == 0)
	{
		heap_end = &_ebss;
	}
	prev_heap_end = heap_end;

	char * stack = (char*) __get_MSP();
	if (heap_end + incr > stack)
	{
		_write(STDERR_FILENO, "Heap and stack collision\n", 25);
		errno = ENOMEM;
		return (caddr_t) -1;
		//abort ();
	}

	heap_end += incr;
	return (caddr_t) prev_heap_end;

}
*/
/*
 read
 Read a character to a file. `libc' subroutines will use this system routine for input from all files, including stdin
 Returns -1 on error or blocks until the number of characters have been read.
 */

int _read(int file, char *ptr, int len)
{
	int n;
	int num = 0;
	switch (file)
	{
	case STDIN_FILENO:
		for (n = 0; n < len; n++)
		{
#if   STDIN_USART == 1
			while ((USART1->SR & USART_FLAG_RXNE) == (uint16_t)RESET)
			{}
			char c = (char)(USART1->DR & (uint16_t)0x01FF);
#elif STDIN_USART == 2
			while ((USART2->SR & USART_FLAG_RXNE) == (uint16_t) RESET)
			{
			}
			char c = (char) (USART2->DR & (uint16_t) 0x01FF);
#elif STDIN_USART == 3
			while ((USART3->SR & USART_FLAG_RXNE) == (uint16_t)RESET)
			{}
			char c = (char)(USART3->DR & (uint16_t)0x01FF);
#endif
			*ptr++ = c;
			num++;
		}
		break;
	default:
		errno = EBADF;
		return -1;
	}
	return num;
}

/*
 stat
 Status of a file (by name). Minimal implementation:
 int    _EXFUN(stat,( const char *__path, struct stat *__sbuf ));
 */

int _stat(const char *filepath, struct stat *st)
{
	st->st_mode = S_IFCHR;
	return 0;
}

/*
 times
 Timing information for current process. Minimal implementation:
 */

clock_t _times(struct tms *buf)
{
	return -1;
}

/*
 unlink
 Remove a file's directory entry. Minimal implementation:
 */
int _unlink(char *name)
{
	errno = ENOENT;
	return -1;
}

/*
 wait
 Wait for a child process. Minimal implementation:
 */
int _wait(int *status)
{
	errno = ECHILD;
	return -1;
}

/*
 write
 Write a character to a file. `libc' subroutines will use this system routine for output to all files, including stdout
 Returns -1 on error or number of bytes sent
 */
int _write(int file, char *ptr, int len)
{
	int n;
	switch (file)
	{
	case STDOUT_FILENO: /*stdout*/
		for (n = 0; n < len; n++)
		{
#if STDOUT_USART == 1
			while ((USART1->SR & USART_FLAG_TC) == (uint16_t)RESET)
			{}
			USART1->DR = (*ptr++ & (uint16_t)0x01FF);
#elif  STDOUT_USART == 2
			while ((USART2->SR & USART_FLAG_TC) == (uint16_t) RESET)
			{
			}
			USART2->DR = (*ptr++ & (uint16_t) 0x01FF);
#elif  STDOUT_USART == 3
			while ((USART3->SR & USART_FLAG_TC) == (uint16_t)RESET)
			{}
			USART3->DR = (*ptr++ & (uint16_t)0x01FF);
#endif
		}
		break;
	case STDERR_FILENO: /* stderr */
		for (n = 0; n < len; n++)
		{
#if STDERR_USART == 1
			while ((USART1->SR & USART_FLAG_TC) == (uint16_t)RESET)
			{}
			USART1->DR = (*ptr++ & (uint16_t)0x01FF);
#elif  STDERR_USART == 2
			while ((USART2->SR & USART_FLAG_TC) == (uint16_t) RESET)
			{
			}
			USART2->DR = (*ptr++ & (uint16_t) 0x01FF);
#elif  STDERR_USART == 3
			while ((USART3->SR & USART_FLAG_TC) == (uint16_t)RESET)
			{}
			USART3->DR = (*ptr++ & (uint16_t)0x01FF);
#endif
		}
		break;
	default:
		errno = EBADF;
		return -1;
	}
	return len;
}
