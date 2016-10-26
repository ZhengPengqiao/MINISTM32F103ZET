#include "DS1302.h"

void DS1302_IO_OUT()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOF, &GPIO_InitStructure);
}

void DS1302_IO_IN()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;

    GPIO_Init(GPIOF, &GPIO_InitStructure);
}
/*******************************************************************************
 * 函数名称 : initDS1302
 * 函数介绍 : ds1302模块初始化
 * 参数介绍 : 无
 * 返回值   : 无
 ******************************************************************************/
void initDS1302()
{
	unsigned char dat;
	GPIO_InitTypeDef GPIO_InitStructure;
	DataStruct time;
	/*默认时间*/
	time.year = 2016;
	time.month = 10;
	time.day = 26;
	time.hour = 20;
	time.minutes = 9;
	time.seconds = 0;
	time.week = 3;

	/*初始化引脚*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE );
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOF, &GPIO_InitStructure);


	DS1302_CE = 0; //初始化 DS1302 通信引脚
	DS1302_CK = 0;
	dat = ds1302SingleRead(0); //读取秒寄存器
	//由秒寄存器最高位 CH 的值判断 DS1302 是否已停止
	if ((dat & 0x80) != 0)
	{
		ds1302SingleWrite(7, 0x00); //撤销写保护以允许写入数据
		setDs1302Time(&time);
	}
}
/*******************************************************************************
 * 函数名称 : ds1302ByteWrite
 * 函数介绍 : 向1302中写一个字节
 * 参数介绍 : data : 将要写的字节
 * 返回值   : 无
 ******************************************************************************/
void ds1302ByteWrite(unsigned char data)
{
	unsigned char mask;
	DS1302_IO_OUT();
	for(mask = 0x01; mask != 0; mask <<= 1)   //低位在前
	{
		if( (mask & data) == 0)
		{
			DS1302_WIO = 0;    //发送低电平
		}
		else
		{
			DS1302_WIO = 1;    //发送高电平
		}
		timer_us(50);
		DS1302_CK = 1;   //上升沿
		timer_us(50);
		DS1302_CK = 0;   //下降沿
		timer_us(50);

	}
	DS1302_WIO = 1; //最后确保释放 IO 引脚
	timer_us(50);
}


/*******************************************************************************
 * 函数名称 : ds1302ByteRead
 * 函数介绍 : 从1302中读一个字节
 * 参数介绍 : data : 将要写的字节
 * 返回值   : 无
 ******************************************************************************/
unsigned char ds1302ByteRead()
{
	unsigned char data = 0;
	unsigned char mask;
	DS1302_IO_IN();
	for(mask = 0x01; mask != 0; mask <<= 1)   //低位在前
	{
		if( DS1302_RIO != 0)   //先上升沿之前读,这是因为模拟spi的问题
		{
			data |= mask;    //保存高电平
		}
		timer_us(50);
		DS1302_CK = 1;   //上升沿
		timer_us(50);
		DS1302_CK = 0;   //下降沿
		timer_us(50);

	}
	return data;
}

/*******************************************************************************
 * 函数名称 : ds1302SingleRead
 * 函数介绍 : 用单次读操作从某一寄存器中读取一个字节,
 * 参数介绍 : addr : 寄存器地址
 * 返回值   : 读取的值
 ******************************************************************************/
unsigned char ds1302SingleRead(unsigned char addr)
{
	unsigned char data;
	DS1302_CE = 1;   //使能定ds1302芯片
	ds1302ByteWrite((addr << 1) | 0x81);
	data = ds1302ByteRead();
	DS1302_CE = 0;   //禁止定ds1302芯片
	return data;
}

/*******************************************************************************
 * 函数名称 : ds1302SingleWrite
 * 函数介绍 : 用单次读操作向某一寄存器中写入一个字节,
 * 参数介绍 : addr : 寄存器地址
 * 			data : 要写入的内容
 * 返回值   : 无
 ******************************************************************************/
void ds1302SingleWrite(unsigned char addr, unsigned char data)
{
	DS1302_CE = 1;   //使能定ds1302芯片
	ds1302ByteWrite( (addr << 1) | 0x80);
	ds1302ByteWrite(data);
	DS1302_CE = 0;   //禁止定ds1302芯片
}


/*******************************************************************************
 * 函数名称 : ds1302BurstRead
 * 函数介绍 : 用突发模式连续写入8个寄存器数据
 * 参数介绍 : data : 将要写入的内容
 * 返回值   : 无
 ******************************************************************************/
void ds1302BurstRead(unsigned char *data)
{
	unsigned char i;
	DS1302_CE = 1;   //使能定ds1302芯片
	ds1302ByteWrite( 0xBF );
	for(i = 0; i < 8; i++)
	{
		data[i] = ds1302ByteRead();
	}
	DS1302_CE = 0;   //禁止定ds1302芯片
}

/*******************************************************************************
 * 函数名称 : ds1302BurstWrite
 * 函数介绍 : 用突发模式连续读取8个寄存器数据,
 * 参数介绍 : data : 读取的值的缓冲区
 * 返回值   : 无
 ******************************************************************************/
void ds1302BurstWrite(unsigned char *data)
{
	unsigned char i;
	DS1302_CE = 1;   //使能定ds1302芯片
	ds1302ByteWrite( 0xBE );
	for(i = 0; i < 8; i++)
	{
		ds1302ByteWrite(data[i]);
	}
	DS1302_CE = 0;   //禁止定ds1302芯片
}


/*******************************************************************************
 * 函数名称 : setDs1302Time
 * 函数介绍 : 设置ds1302的时钟模块时间
 * 参数介绍 : data : 包含要设置时间的结构体指针
 * 返回值   : 无
 ******************************************************************************/
void setDs1302Time(DataStruct *time)
{
	unsigned char buf[8];
	buf[7] = 0;
	buf[6] = ( (time->year) % 10 ) + ( (time->year / 10 % 10) << 4);  //年
	buf[5] = time->week & 0x0F;                                   //星期
	buf[4] = (time->month % 10) + ( (time->month / 10 % 10) << 4);//月
	buf[3] = (time->day % 10) + ( (time->day / 10 % 10) << 4);//日
	buf[2] = (time->hour % 10) + ( (time->hour / 10 % 10) << 4);//时
	buf[1] = (time->minutes % 10) + ( (time->minutes / 10 % 10) << 4);//分
	buf[0] = (time->seconds % 10) + ( (time->seconds / 10 % 10) << 4);//秒

	ds1302BurstWrite(buf);
}

/*******************************************************************************
 * 函数名称 : getDs1302Time
 * 函数介绍 : 获得ds1302的时钟模块时间
 * 参数介绍 : data : 用来保存时间的结构体指针
 * 返回值   : 无
 ******************************************************************************/
void getDs1302Time(DataStruct *time)
{
	unsigned char buf[8];
	ds1302BurstRead(buf);
	time->year = ( (buf[6]&0xF0) >> 4) * 10 + (buf[6]&0x0F) + 2000;  //年
	time->month = ( (buf[4]&0xF0) >> 4) * 10 + (buf[4]&0x0F) ;       //月
	time->day = ( (buf[3]&0xF0) >> 4) * 10 + (buf[3]&0x0F);          //日
	time->hour = ( (buf[2]&0x10) >> 4) * 10 + (buf[2]&0x0F);         //时
	time->minutes = ( (buf[1]&0xF0) >> 4) * 10 + (buf[1]&0x0F);      //分
	time->seconds = ( (buf[0]&0x70) >> 4) * 10 + (buf[0]&0x0F);      //秒
	time->week = buf[5]&0x0F;                                //星期
}
