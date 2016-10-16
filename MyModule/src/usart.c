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
u8 USART_RX_BUF[RX_BUFFER_SIZE];    //接受缓冲区的字符
u16 end,start;                     //接受缓冲区的使用情况

/*******************************************************************************
 * 函数名称 : sendChar
 * 函数介绍 : 发送一个字符
 * 参数介绍 : ch : 要发送的字符
 * 返回值   : 要发送的ch
 ******************************************************************************/
int sendChar(int ch) {
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
void sendString(unsigned char *str, int len) {
	int i;
	for (i = 0; i < len; i++) {
		sendChar(str[i]);
	}
}


/*******************************************************************************
 * 函数名称 : sendNewLine
 * 函数介绍 : 发送换行符
 * 参数介绍 : 无
 * 返回值  :  无
 ******************************************************************************/
void sendNewLine()
{
	sendString((unsigned char *)"\r\n",2);
}


/*******************************************************************************
 * 函数名称 : readChar
 * 函数介绍 : 读取字符
 * 参数介绍 : ch  :  读取的字符存放位置
 * 返回值  :  无
 ******************************************************************************/
char readChar()
{
	if(start != end)
	{
		char ch = USART_RX_BUF[start];
		USART_RX_BUF[start] = 0;
		start++;
		if(start >= RX_BUFFER_SIZE)
		{
			start = 0;
		}

		return ch;
	}
	else
	{
		return 0;
	}
}

/*******************************************************************************
 * 函数名称 : readString
 * 函数介绍 : 读取字符串
 * 参数介绍 : str  :  读取字符串暂存的位置
 * 			len  :   要读取的位数
 * 返回值  :  正真读取的位数
 ******************************************************************************/
int readString(char * str, int len)
{
	char ch = 0;
	int i = 0;
	if(len <= 0)
	{
		return -1;
	}
	ch = readChar();
	while( ch != 0 )
	{
		*(str+i) = ch;
		i++;
		if( i < len )
		{
			ch = readChar();
		}
		else
		{
			break;
		}
	}
	return i;
}


/*******************************************************************************
 * 函数名称 : readStringWait
 * 函数介绍 : 阻塞读取字符串
 * 参数介绍 : str  :  读取字符串暂存的位置
 * 			len  :   将会读取的位数
 * 返回值  :  无
 ******************************************************************************/
void readStringWait(char * str, int len)
{
	int i = 0;
	while(i < len)
	{
		i += readString(str+i,len-i);
	}
}

/*******************************************************************************
 * 函数名称 : readStringRaw
 * 函数介绍 : 阻塞读取一行，或者用户能存储的最大字符串
 * 参数介绍 : str  :  读取字符串暂存的位置
 * 			len  :   将会读取的位数
 * 返回值  :  无
 ******************************************************************************/
int readStringRaw(char * str, int len)
{
	int i = 0;
	char ch = 0;
	if(len <= 0)
	{
		return -1;
	}

	ch = readChar();
	while( (i < len)  &&  !( (i > 0) && (str[i-1] == 0x0d) && (str[i] != 0x0a)) )
	{
		if(ch != 0)
		{
			*(str+i) = ch;
			i++;
			ch = readChar();
		}
		else
		{
			ch = readChar();
		}
	}
	str[i] = '\0';

	return i;
}

/*******************************************************************************
 * 函数名称 : uart_init
 * 函数介绍 : 串口初始化
 * 参数介绍 : bound : 波特率
 * 返回值   :无
 ******************************************************************************/
void uart_init(u32 bound) {
	GPIO_InitTypeDef GPIO_InitTypeDefStruct;
	NVIC_InitTypeDef NVIC_InitTypeDefStruct;
	USART_InitTypeDef USART_InitTypeDefStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA,
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


/*******************************************************************************
 * 函数名称 : USART1_IRQHandler
 * 函数介绍 : 串口中断服务函数
 * 参数介绍 : 无
 * 返回值   :无
 ******************************************************************************/
void USART1_IRQHandler(void) {
	if(end >= RX_BUFFER_SIZE )
		end = 0;
	USART_RX_BUF[end++] = USART_ReceiveData(USART1);
}





/*******************************************************************************
 * 一下代码是为了使用newlib的printf而添加的函数，这些宏定义是为了告诉下面代码使用哪一个串口
 ******************************************************************************/


//重载fputc函数
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

extern PUTCHAR_PROTOTYPE;
PUTCHAR_PROTOTYPE {
	/* Place your implementation of fputc here */
	USART_SendData(USART1, (uint8_t) ch);
	/* Loop until the end of transmission */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {
	}
	return ch;
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
char *__env[1] = { 0 };
char **environ = __env;

int _write(int file, char *ptr, int len);

void _exit(int status) {
	//消除参数未使用的警告_start
	status = status + 1;
	status = status - 1;
	//消除参数未使用的警告_end
	_write(1, "exit", 4);
	while (1) {
		;
	}
}

int _close(int file) {
	//消除参数未使用的警告_start
	file = file + 1;
	file = file - 1;
	//消除参数未使用的警告_end
	return -1;
}
/*
 execve
 Transfer control to a new process. Minimal implementation (for a system without processes):
 */
int _execve(char *name, char **argv, char **env) {
	//消除参数未使用的警告_start
	name = name + 1;
	name = name - 1;
	argv = argv + 1;
	argv = argv - 1;
	env  = env + 1;
	env  = env - 1;
	//消除参数未使用的警告_end
	errno = ENOMEM;
	return -1;
}
/*
 fork
 Create a new process. Minimal implementation (for a system without processes):
 */

int _fork() {
	errno = EAGAIN;
	return -1;
}
/*
 fstat
 Status of an open file. For consistency with other minimal implementations in these examples,
 all files are regarded as character special devices.
 The `sys/stat.h' header file required is distributed in the `include' subdirectory for this C library.
 */
int _fstat(int file, struct stat *st) {
	//消除参数未使用的警告_start
	file = file + 1;
	file = file - 1;
	//消除参数未使用的警告_end
	st->st_mode = S_IFCHR;
	return 0;
}

/*
 getpid
 Process-ID; this is sometimes used to generate strings unlikely to conflict with other processes. Minimal implementation, for a system without processes:
 */

int _getpid() {
	return 1;
}

/*
 isatty
 Query whether output stream is a terminal. For consistency with the other minimal implementations,
 */
int _isatty(int file) {
	switch (file) {
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
int _kill(int pid, int sig) {
	//消除参数未使用的警告_start
	pid = pid + 1;
	pid = pid - 1;
	sig = sig + 1;
	sig = sig - 1;
	//消除参数未使用的警告_end

	errno = EINVAL;
	return (-1);
}

/*
 link
 Establish a new name for an existing file. Minimal implementation:
 */

int _link(char *old, char *new) {
	//消除参数未使用的警告_start
	old = old + 1;
	old = old - 1;
	new = new + 1;
	new = new - 1;
	//消除参数未使用的警告_end
	errno = EMLINK;
	return -1;
}

/*
 lseek
 Set position in a file. Minimal implementation:
 */
int _lseek(int file, int ptr, int dir) {
	//消除参数未使用的警告_start
	file = file + 1;
	file = file - 1;
	ptr = ptr + 1;
	ptr = ptr - 1;
	dir = dir + 1;
	dir = dir - 1;
	//消除参数未使用的警告_end
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

int _read(int file, char *ptr, int len) {
	int n;
	int num = 0;
	switch (file) {
	case STDIN_FILENO:
		for (n = 0; n < len; n++) {
#if   STDIN_USART == 1
			while ((USART1->SR & USART_FLAG_RXNE) == (uint16_t) RESET) {
			}
			char c = (char) (USART1->DR & (uint16_t) 0x01FF);
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

int _stat(const char *filepath, struct stat *st) {
	//消除参数未使用的警告_start
	filepath = filepath + 1;
	filepath = filepath - 1;
	st = st + 1;
	st = st - 1;
	//消除参数未使用的警告_end
	st->st_mode = S_IFCHR;
	return 0;
}

/*
 times
 Timing information for current process. Minimal implementation:
 */

clock_t _times(struct tms *buf) {
	//消除参数未使用的警告_start
	buf = buf + 1;
	buf = buf - 1;
	//消除参数未使用的警告_end
	return -1;
}

/*
 unlink
 Remove a file's directory entry. Minimal implementation:
 */
int _unlink(char *name) {
	//消除参数未使用的警告_start
	name = name + 1;
	name = name - 1;
	//消除参数未使用的警告_end
	errno = ENOENT;
	return -1;
}

/*
 wait
 Wait for a child process. Minimal implementation:
 */
int _wait(int *status) {
	//消除参数未使用的警告_start
	status = status + 1;
	status = status - 1;
	//消除参数未使用的警告_end
	errno = ECHILD;
	return -1;
}

/*
 write
 Write a character to a file. `libc' subroutines will use this system routine for output to all files, including stdout
 Returns -1 on error or number of bytes sent
 */
int _write(int file, char *ptr, int len) {
	int n;
	switch (file) {
	case STDOUT_FILENO: /*stdout*/
		for (n = 0; n < len; n++) {
#if STDOUT_USART == 1
			while ((USART1->SR & USART_FLAG_TC) == (uint16_t) RESET) {
			}
			USART1->DR = (*ptr++ & (uint16_t) 0x01FF);
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
		for (n = 0; n < len; n++) {
#if STDERR_USART == 1
			while ((USART1->SR & USART_FLAG_TC) == (uint16_t) RESET) {
			}
			USART1->DR = (*ptr++ & (uint16_t) 0x01FF);
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
