#ifndef __TOUCH_H_
#define __TOUCH_H_
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "PIN.h"
#include "Lcd.h"
#include "math.h"
#include "Delay.h"
//针对触摸屏引脚定义
#define  DOUT PFin(8)   //主机输入从机输出
#define  TDIN PFout(9)  //主机输出从机输入
#define  PEN  PFin(10)  //INT
#define  TCLK  PBout(1)  //SPI时钟引脚
#define  TCS   PBout(2)  //片选芯片

#define TP_PRES_DOWN 0x80  		//触摸屏被按下了
#define TP_CATH_PRES 0x40  		//有按键按下

//触摸屏控制器
typedef struct {
	u8 (*init)(void);     //触摸屏初始化函数
	u8 (*scan)(u8);       //触摸屏扫描函数
	void (*adjust)(void); //触摸屏校准函数
	u16 x;
	u16 y;

	u8 sta;  //笔的状态，
			 //b7:按下1、松开0
			 //b6:没有按键按下0，有按键按下1
	float xfac;   //x轴斜率
	float yfac;   //y轴斜率
	short xoff;   //x轴偏移
	short yoff;   //y轴偏移
	u8 touchtype;   //屏幕类型
} _m_tp_dev;

extern _m_tp_dev tp_dev;
/*-----------------------------------------------------------------------------
 *函数名称 ：  TP_Adj_Info_Show
 *函数介绍 ： 显示校准信息
 *参数介绍 ： x0:
 *返回自     ：无
 *----------------------------------------------------------------------------*/

extern void TP_Adj_Info_Show(u16 x0, u16 y0, u16 x1, u16 y1, u16 x2, u16 y2,
		u16 x3, u16 y3, u16 fac);

/*-----------------------------------------------------------------------------
 *函数名称 ：  TP_Drow_Touch_Point
 *函数介绍 ：  画出校准图标点
 *参数介绍 ： x:x坐标轴  y:y坐标轴    color:颜色
 *返回自     ：无
 *----------------------------------------------------------------------------*/
extern void TP_Drow_Touch_Point(u16 x, u16 y, u16 color);

/*-----------------------------------------------------------------------------
 *函数名称 ：  TP_Write_Byte
 *函数介绍 ：  向控制芯片写入一个数据
 *参数介绍 ：  num : 要写入的数据
 *返回自     ：无
 *----------------------------------------------------------------------------*/
extern void TP_Write_Byte(u8 num);

/*-----------------------------------------------------------------------------
 *函数名称 ：  TP_Read_AD
 *函数介绍 ：  读取AD转换值
 *参数介绍 ：  CMD : 读取命令，支出读取的是x轴还是y轴
 *返回自     ： 返回读到的值
 *----------------------------------------------------------------------------*/
extern u16 TP_Read_AD(u8 CMD);

/*-----------------------------------------------------------------------------
 *函数名称 ：  TP_Read_XOY
 *函数介绍 ：  带滤波的坐标读取（x/y）
 *参数介绍 ：
 *返回自     ：
 *----------------------------------------------------------------------------*/
extern u16 TP_Read_XOY(u8 xy);

/*-----------------------------------------------------------------------------
 *函数名称 ：  TP_Read_XY
 *函数介绍 ：  双方向读取
 *参数介绍 ：
 *返回自     ：
 *----------------------------------------------------------------------------*/
extern u8 TP_Read_XY(u16 *x, u16 *y);
/*-----------------------------------------------------------------------------
 *函数名称 ：  TP_Read_XY2
 *函数介绍 ：  带加强滤波的双方向读取
 *参数介绍 ：
 *返回自     ：
 *----------------------------------------------------------------------------*/
extern u8 TP_Read_XY2(u16 *x, u16 *y);

/*-----------------------------------------------------------------------------
 * 函数名称 ： TP_Adjust
 * 函数介绍 ：电阻屏校准函数
 * 参数介绍 ： 无
 * 返回值    ： 无
 *----------------------------------------------------------------------------*/
extern void TP_Adjust(void);
/*-----------------------------------------------------------------------------
 * 函数名称 ： TP_Init
 * 函数介绍 ：电阻屏初始化函数
 * 参数介绍 ： 无
 * 返回值    ：
 *----------------------------------------------------------------------------*/
extern u8 TP_Init(void);
/*-----------------------------------------------------------------------------
 * 函数名称 ： TP_Scan
 * 函数介绍 ：电阻屏扫描函数
 * 参数介绍 ： 无
 * 返回值    ：  当前屏幕状态
 *----------------------------------------------------------------------------*/
extern u8 TP_Scan(u8 tp);

#endif //__TOUCH_H_

