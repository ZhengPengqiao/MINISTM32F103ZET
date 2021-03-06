#include "mpuiic.h"
#include "Delay.h"


/*-----------------------------------------------------------------------------
 * 函数名称 ：MPU_IIC_Delay
 * 函数介绍 ：MPU IIC 延时函数
 * 参数介绍 ：无
 * 返回值     ：无
 *---------------------------------------------------------------------------*/
void MPU_IIC_Delay(void) {
	delay_us(2);
}


/*-----------------------------------------------------------------------------
 * 函数名称 ：MPU_IIC_Init
 * 函数介绍 ：初始化IIC
 * 参数介绍 ：无
 * 返回值     ：无
 *---------------------------------------------------------------------------*/
void MPU_IIC_Init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //先使能外设IO PORTB时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;	 // 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIO

	GPIO_SetBits(GPIOB, GPIO_Pin_10 | GPIO_Pin_11);				//PB10,PB11 输出高

}


/*-----------------------------------------------------------------------------
 * 函数名称 ：MPU_IIC_Start
 * 函数介绍 ：产生IIC起始信号
 * 参数介绍 ：无
 * 返回值     ：无
 *---------------------------------------------------------------------------*/
void MPU_IIC_Start(void) {
	MPU_SDA_OUT()
	;     //sda线输出
	MPU_IIC_SDA = 1;
	MPU_IIC_SCL = 1;
	MPU_IIC_Delay();
	MPU_IIC_SDA = 0;     //START:when CLK is high,DATA change form high to low
	MPU_IIC_Delay();
	MPU_IIC_SCL = 0;     //钳住I2C总线，准备发送或接收数据
}


/*-----------------------------------------------------------------------------
 * 函数名称 ：MPU_IIC_Stop
 * 函数介绍 ：产生IIC停止信号
 * 参数介绍 ：无
 * 返回值     ：无
 *---------------------------------------------------------------------------*/
void MPU_IIC_Stop(void) {
	MPU_SDA_OUT()
	;     //sda线输出
	MPU_IIC_SCL = 0;
	MPU_IIC_SDA = 0;     //STOP:when CLK is high DATA change form low to high
	MPU_IIC_Delay();
	MPU_IIC_SCL = 1;
	MPU_IIC_SDA = 1;     //发送I2C总线结束信号
	MPU_IIC_Delay();
}


/*-----------------------------------------------------------------------------
 * 函数名称 ：MPU_IIC_Wait_Ack
 * 函数介绍 ：等待应答信号到来
 * 参数介绍 ：无
 * 返回值     ：无
 *---------------------------------------------------------------------------*/
u8 MPU_IIC_Wait_Ack(void) {
	u8 ucErrTime = 0;
	MPU_SDA_IN()
	;      //SDA设置为输入
	MPU_IIC_SDA = 1;
	MPU_IIC_Delay();
	MPU_IIC_SCL = 1;
	MPU_IIC_Delay();
	while (MPU_READ_SDA) {
		ucErrTime++;
		if (ucErrTime > 250) {
			MPU_IIC_Stop();
			return 1;
		}
	}
	MPU_IIC_SCL = 0;      //时钟输出0
	return 0;
}


/*-----------------------------------------------------------------------------
 * 函数名称 ：MPU_IIC_Ack
 * 函数介绍 ：产生ACK应答
 * 参数介绍 ：无
 * 返回值     ：无
 *---------------------------------------------------------------------------*/
void MPU_IIC_Ack(void) {
	MPU_IIC_SCL = 0;
	MPU_SDA_OUT()
	;
	MPU_IIC_SDA = 0;
	MPU_IIC_Delay();
	MPU_IIC_SCL = 1;
	MPU_IIC_Delay();
	MPU_IIC_SCL = 0;
}


/*-----------------------------------------------------------------------------
 * 函数名称 ：MPU_IIC_NAck
 * 函数介绍 ：不产生ACK应答
 * 参数介绍 ：无
 * 返回值     ：无
 *---------------------------------------------------------------------------*/
void MPU_IIC_NAck(void) {
	MPU_IIC_SCL = 0;
	MPU_SDA_OUT()
	;
	MPU_IIC_SDA = 1;
	MPU_IIC_Delay();
	MPU_IIC_SCL = 1;
	MPU_IIC_Delay();
	MPU_IIC_SCL = 0;
}


/*-----------------------------------------------------------------------------
 * 函数名称 ：MPU_IIC_Send_Byte
 * 函数介绍 ：IIC发送一个字节
 * 参数介绍 ：无
 * 返回值     ：1：有应答，0：无应答
 *---------------------------------------------------------------------------*/
void MPU_IIC_Send_Byte(u8 txd) {
	u8 t;
	MPU_SDA_OUT()
	;
	MPU_IIC_SCL = 0;      //拉低时钟开始数据传输
	for (t = 0; t < 8; t++) {
		MPU_IIC_SDA = (txd & 0x80) >> 7;
		txd <<= 1;
		MPU_IIC_SCL = 1;
		MPU_IIC_Delay();
		MPU_IIC_SCL = 0;
		MPU_IIC_Delay();
	}
}


/*-----------------------------------------------------------------------------
 * 函数名称 ：MPU_IIC_Read_Byte
 * 函数介绍 ：读1个字节
 * 参数介绍 ：ack=1时，发送ACK，ack=0，发送nACK
 * 返回值     ：读取到的字节
 *---------------------------------------------------------------------------*/
u8 MPU_IIC_Read_Byte(unsigned char ack) {
	unsigned char i, receive = 0;
	MPU_SDA_IN()
	;      //SDA设置为输入
	for (i = 0; i < 8; i++) {
		MPU_IIC_SCL = 0;
		MPU_IIC_Delay();
		MPU_IIC_SCL = 1;
		receive <<= 1;
		if (MPU_READ_SDA)
			receive++;
		MPU_IIC_Delay();
	}
	if (!ack)
		MPU_IIC_NAck();      //发送nACK
	else
		MPU_IIC_Ack(); //发送ACK
	return receive;
}

