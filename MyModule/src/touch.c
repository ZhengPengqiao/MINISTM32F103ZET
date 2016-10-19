#include "touch.h"
//校准提示信息
unsigned char TP_REMIND_MSG_TBL[] =
		"Please use the stylus click the cross on the "
				"screen.The cross will always move until the screen adjustment is "
				"completed.";
_m_tp_dev tp_dev = { TP_Init, TP_Scan, TP_Adjust, 0, 0, 0, 0, 0, 0, 0, 0, };

//读取坐标命令
unsigned char CMD_RDX = 0xD0;  //读取X轴坐标
unsigned char CMD_RDY = 0x90;  //读取Y轴坐标
#define READ_TIMES 5 	//读取次数
#define LOST_VAL 1	  	//丢弃值
/*-----------------------------------------------------------------------------
 *函数名称 ：  TP_Adj_Info_Show
 *函数介绍 ： 显示校准信息
 *参数介绍 ： x0:
 *返回自     ：无
 *----------------------------------------------------------------------------*/
void TP_Adj_Info_Show(u16 x0, u16 y0, u16 x1, u16 y1, u16 x2, u16 y2, u16 x3,
		u16 y3, u16 fac) {
	POINT_COLOR = RED;
	LCD_ShowString(40, 160, lcddev.width, lcddev.height, 16, (u8 *) "x1:");
	LCD_ShowString(40 + 80, 160, lcddev.width, lcddev.height, 16, (u8 *) "y1:");
	LCD_ShowString(40, 180, lcddev.width, lcddev.height, 16, (u8 *) "x2:");
	LCD_ShowString(40 + 80, 180, lcddev.width, lcddev.height, 16, (u8 *) "y2:");
	LCD_ShowString(40, 200, lcddev.width, lcddev.height, 16, (u8 *) "x3:");
	LCD_ShowString(40 + 80, 200, lcddev.width, lcddev.height, 16, (u8 *) "y3:");
	LCD_ShowString(40, 220, lcddev.width, lcddev.height, 16, (u8 *) "x4:");
	LCD_ShowString(40 + 80, 220, lcddev.width, lcddev.height, 16, (u8 *) "y4:");
	LCD_ShowString(40, 240, lcddev.width, lcddev.height, 16, (u8 *) "fac is:");
	LCD_ShowNum(40 + 24, 160, x0, 4, 16);       //显示数值
	LCD_ShowNum(40 + 24 + 80, 160, y0, 4, 16);  //显示数值
	LCD_ShowNum(40 + 24, 180, x1, 4, 16);       //显示数值
	LCD_ShowNum(40 + 24 + 80, 180, y1, 4, 16);  //显示数值
	LCD_ShowNum(40 + 24, 200, x2, 4, 16);       //显示数值
	LCD_ShowNum(40 + 24 + 80, 200, y2, 4, 16);  //显示数值
	LCD_ShowNum(40 + 24, 220, x3, 4, 16);       //显示数值
	LCD_ShowNum(40 + 24 + 80, 220, y3, 4, 16);  //显示数值
	LCD_ShowNum(40 + 56, 240, fac, 3, 16);      //显示数值,该数值必须在95~105范围之间

}

/*-----------------------------------------------------------------------------
 *函数名称 ：  TP_Drow_Touch_Point
 *函数介绍 ：  画出校准图标点
 *参数介绍 ： x:x坐标轴  y:y坐标轴    color:颜色
 *返回自     ：无
 *----------------------------------------------------------------------------*/
void TP_Drow_Touch_Point(u16 x, u16 y, u16 color) {
	POINT_COLOR = color;
	LCD_DrawLine(x - 12, y, x + 13, y);  //ºáÏß
	LCD_DrawLine(x, y - 12, x, y + 13);  //ÊúÏß
	LCD_DrawPoint(x + 1, y + 1);
	LCD_DrawPoint(x - 1, y + 1);
	LCD_DrawPoint(x + 1, y - 1);
	LCD_DrawPoint(x - 1, y - 1);
	LCD_Draw_Circle(x, y, 6);  //»­ÖÐÐÄÈ¦
}

/*-----------------------------------------------------------------------------
 *函数名称 ：  TP_Write_Byte
 *函数介绍 ：  向控制芯片写入一个数据
 *参数介绍 ：  num : 要写入的数据
 *返回自     ：无
 *----------------------------------------------------------------------------*/
void TP_Write_Byte(u8 num) {
	u8 count = 0;
	for (count = 0; count < 8; count++) {
		if (num & 0x80)
			TDIN = 1;
		else
			TDIN = 0;
		num <<= 1;
		TCLK = 0;
		delay_us(1);
		TCLK = 1;  //上升沿有效
	}
}

/*-----------------------------------------------------------------------------
 *函数名称 ：  TP_Read_AD
 *函数介绍 ：  从触摸屏IC读取adc值
 *参数介绍 ：  CMD : 读取命令
 *返回自     ： 返回读到的值
 *----------------------------------------------------------------------------*/
u16 TP_Read_AD(u8 CMD) {
	u8 count = 0;
	u16 Num = 0;
	TCLK = 0;   //先拉低时钟
	TDIN = 0;   //拉低数据线
	TCS = 0;    //选中触摸屏IC
	TP_Write_Byte(CMD);  //发送命令字
	delay_us(6);    //延时，等待转换时间
	TCLK = 0;
	delay_us(1);
	TCLK = 1;         //给1个时钟，清除BUSY
	delay_us(1);
	TCLK = 0;
	for (count = 0; count < 16; count++) { //读出16位数据，只有高12位有效
		Num <<= 1;
		TCLK = 0;    //下降沿有效
		delay_us(1);
		TCLK = 1;
		if (DOUT)
			Num++;
	}
	Num >>= 4;       //只有高12位有效
	TCS = 1;         //释放片选
	return (Num);
}

/*-----------------------------------------------------------------------------
 *函数名称 ：  TP_Read_XOY
 *函数介绍 ：  读取一个坐标值（x或者y）,连续读取READ_TIMES次数据，对这些数据升序排列，然后去掉
 *			最低和最高LOST_VAL个数，取平均值
 *参数介绍 ： xy : 指令（MCD_RDX/CMD_RDY）
 *返回自     ： 读取到的数值
 *----------------------------------------------------------------------------*/
u16 TP_Read_XOY(u8 xy) {
	u16 i, j;
	u16 buf[READ_TIMES];
	u16 sum = 0;
	u16 temp;
	for (i = 0; i < READ_TIMES; i++)  //排序
		buf[i] = TP_Read_AD(xy);
	for (i = 0; i < READ_TIMES - 1; i++) {
		for (j = i + 1; j < READ_TIMES; j++) {
			if (buf[i] > buf[j]) {   //升序排列
				temp = buf[i];
				buf[i] = buf[j];
				buf[j] = temp;
			}
		}
	}
	sum = 0;
	for (i = LOST_VAL; i < READ_TIMES - LOST_VAL; i++)
		sum += buf[i];
	temp = sum / (READ_TIMES - 2 * LOST_VAL);
	return temp;
}

/*-----------------------------------------------------------------------------
 *函数名称 ：  TP_Read_XY
 *函数介绍 ：  双方向读取,最小值不能小于100,
 *参数介绍 ： x ,y :读取到的坐标值
 *返回自     ：0:失败   1：成功
 *----------------------------------------------------------------------------*/
u8 TP_Read_XY(u16 *x, u16 *y) {
	u16 xtemp, ytemp;
	xtemp = TP_Read_XOY(CMD_RDX);
	ytemp = TP_Read_XOY(CMD_RDY);
	//if(xtemp<100||ytemp<100)return 0;
	*x = xtemp;
	*y = ytemp;
	return 1;  //读取成功
}

/*-----------------------------------------------------------------------------
 *函数名称 ：  TP_Read_XY2
 *函数介绍 ：  带加强滤波的双方向读取，连续两次读取触摸屏IC,且这两次的偏差不能超过ERR_RANGE，
 *			满足条件，则认为读数正确，否则读取错误，该函数大大提高精度，条用两次TP_Read_XY2
 *参数介绍 ：  x,y ：读取到的坐标值
 *返回自     ：  0：失败，  1：成功
 *----------------------------------------------------------------------------*/
#define ERR_RANGE 50   //误差范围
u8 TP_Read_XY2(u16 *x, u16 *y) {
	u16 x1, y1;
	u16 x2, y2;
	u8 flag;
	flag = TP_Read_XY(&x1, &y1);
	if (flag == 0)
		return (0);
	flag = TP_Read_XY(&x2, &y2);
	if (flag == 0)
		return (0);
	//前后两次采样在+-50内
	if (((x2 <= x1 && x1 < x2 + ERR_RANGE) || (x1 <= x2 && x2 < x1 + ERR_RANGE))
			&& ((y2 <= y1 && y1 < y2 + ERR_RANGE)
					|| (y1 <= y2 && y2 < y1 + ERR_RANGE))) {
		*x = (x1 + x2) / 2;
		*y = (y1 + y2) / 2;
		return 1;
	} else
		return 0;
}

/*-----------------------------------------------------------------------------
 * 函数名称 ： TP_Scan
 * 函数介绍 ：电阻屏扫描函数
 * 参数介绍 ： 无
 * 返回值    ：  当前屏幕状态
 *----------------------------------------------------------------------------*/
u8 TP_Scan(u8 tp) {
	if (PEN == 0) {		//有按键被按下
		if (tp)
			TP_Read_XY2(&tp_dev.x, &tp_dev.y);		//读取物理坐标
		else if (TP_Read_XY2(&tp_dev.x, &tp_dev.y)) {		//读取屏幕坐标
			tp_dev.x = tp_dev.xfac * tp_dev.x + tp_dev.xoff;		//将结果转换为屏幕坐标
			tp_dev.y = tp_dev.yfac * tp_dev.y + tp_dev.yoff;
		}
		if ((tp_dev.sta & TP_PRES_DOWN) == 0) {		//之前没有被按下
			tp_dev.sta = TP_PRES_DOWN | TP_CATH_PRES;		//按键按下
		}
	} else {
		if (tp_dev.sta & TP_PRES_DOWN) {	//之前是被按下的
			tp_dev.sta &= ~(1 << 7);		//标记按键松开
		} else { //之前就没有被按下
			tp_dev.x = 0;
			tp_dev.y = 0;
		}
	}
	return tp_dev.sta & TP_PRES_DOWN;		//返回当前屏幕状态
}

/*-----------------------------------------------------------------------------
 * 函数名称 ： TP_Adjust
 * 函数介绍 ：电阻屏校准函数
 * 参数介绍 ： 无
 * 返回值    ： 无
 *----------------------------------------------------------------------------*/
void TP_Adjust(void) {
	u16 pos_temp[4][2];  //坐标缓存值
	u8 cnt = 0;
	u16 d1, d2;
	u32 tem1, tem2;
	double fac;
	u16 outtime = 0;
	cnt = 0;
	POINT_COLOR = BLUE;
	BACK_COLOR = WHITE;
	LCD_Clear(WHITE);  //清屏
	POINT_COLOR = RED;   //红色
	LCD_Clear(WHITE);  //清屏
	POINT_COLOR = BLACK;
	LCD_ShowString(40, 40, 160, 100, 16, (u8*) TP_REMIND_MSG_TBL); //显示提示信息
	TP_Drow_Touch_Point(20, 20, RED); //画点1
	tp_dev.sta = 0; //清楚触发信号
	tp_dev.xfac = 0; //xfac用来标记是否校准过，所以校准之前清楚，避免错误
	while (1) //如果连续10s没有按下，则直接退出
	{
		tp_dev.scan(1); //扫描物理坐标
		if ((tp_dev.sta & 0xc0) == TP_CATH_PRES) { //标记按键已经被处理过了
			outtime = 0;
			tp_dev.sta &= ~(1 << 6); //标记按下已被处理过了

			pos_temp[cnt][0] = tp_dev.x;
			pos_temp[cnt][1] = tp_dev.y;
			cnt++;
			switch (cnt) {
			case 1:
				TP_Drow_Touch_Point(20, 20, WHITE);				//清除点1
				TP_Drow_Touch_Point(lcddev.width - 20, 20, RED);	//画点2
				break;
			case 2:
				TP_Drow_Touch_Point(lcddev.width - 20, 20, WHITE);	//清除点2
				TP_Drow_Touch_Point(20, lcddev.height - 20, RED);	//画点3
				break;
			case 3:
				TP_Drow_Touch_Point(20, lcddev.height - 20, WHITE);	//清除点3
				TP_Drow_Touch_Point(lcddev.width - 20, lcddev.height - 20, RED);//画点4
				break;
			case 4:	//全部四个已经得到
				//对边相等
				tem1 = abs(pos_temp[0][0] - pos_temp[1][0]);	    	//x1-x2
				tem2 = abs(pos_temp[0][1] - pos_temp[1][1]);	    	//y1-y2
				tem1 *= tem1;
				tem2 *= tem2;
				d1 = sqrt(tem1 + tem2);	    		    //得到1，2的距离

				tem1 = abs(pos_temp[2][0] - pos_temp[3][0]);	    	//x3-x4
				tem2 = abs(pos_temp[2][1] - pos_temp[3][1]);	    	//y3-y4
				tem1 *= tem1;
				tem2 *= tem2;
				d2 = sqrt(tem1 + tem2);	    		    //得到3,4的距离
				fac = (float) d1 / d2;
				if (fac < 0.95 || fac > 1.05 || d1 == 0 || d2 == 0)	    //不合格
						{
					cnt = 0;
					TP_Drow_Touch_Point(lcddev.width - 20, lcddev.height - 20,
					WHITE);	//清除点4
					TP_Drow_Touch_Point(20, 20, RED);					//画点1
					TP_Adj_Info_Show(pos_temp[0][0], pos_temp[0][1],
							pos_temp[1][0], pos_temp[1][1], pos_temp[2][0],
							pos_temp[2][1], pos_temp[3][0], pos_temp[3][1],
							fac * 100);								//显示数据
					continue;
				}
				tem1 = abs(pos_temp[0][0] - pos_temp[2][0]);			//x1-x3
				tem2 = abs(pos_temp[0][1] - pos_temp[2][1]);			//y1-y3
				tem1 *= tem1;
				tem2 *= tem2;
				d1 = sqrt(tem1 + tem2);							//得到1,3的距离

				tem1 = abs(pos_temp[1][0] - pos_temp[3][0]);			//x2-x4
				tem2 = abs(pos_temp[1][1] - pos_temp[3][1]);			//y2-y4
				tem1 *= tem1;
				tem2 *= tem2;
				d2 = sqrt(tem1 + tem2);							//得到2，4的距离
				fac = (float) d1 / d2;
				if (fac < 0.95 || fac > 1.05)							//不合格
						{
					cnt = 0;
					TP_Drow_Touch_Point(lcddev.width - 20, lcddev.height - 20,
					WHITE);	//清除点4
					TP_Drow_Touch_Point(20, 20, RED);					//画点1
					TP_Adj_Info_Show(pos_temp[0][0], pos_temp[0][1],
							pos_temp[1][0], pos_temp[1][1], pos_temp[2][0],
							pos_temp[2][1], pos_temp[3][0], pos_temp[3][1],
							fac * 100);								//显示数据
					continue;
				}								//正确了

				//对角线相等
				tem1 = abs(pos_temp[1][0] - pos_temp[2][0]);			//x1-x3
				tem2 = abs(pos_temp[1][1] - pos_temp[2][1]);			//y1-y3
				tem1 *= tem1;
				tem2 *= tem2;
				d1 = sqrt(tem1 + tem2);								//得到1,4距离

				tem1 = abs(pos_temp[0][0] - pos_temp[3][0]);			//x2-x4
				tem2 = abs(pos_temp[0][1] - pos_temp[3][1]);			//y2-y4
				tem1 *= tem1;
				tem2 *= tem2;
				d2 = sqrt(tem1 + tem2);								//得到2,3距离
				fac = (float) d1 / d2;
				if (fac < 0.95 || fac > 1.05)							//不合格
						{
					cnt = 0;
					TP_Drow_Touch_Point(lcddev.width - 20, lcddev.height - 20,
					WHITE);	//清除点4
					TP_Drow_Touch_Point(20, 20, RED);					//画点1
					TP_Adj_Info_Show(pos_temp[0][0], pos_temp[0][1],
							pos_temp[1][0], pos_temp[1][1], pos_temp[2][0],
							pos_temp[2][1], pos_temp[3][0], pos_temp[3][1],
							fac * 100);								//显示数据
					continue;
				}								//正确了
												//计算结果
				tp_dev.xfac = (float) (lcddev.width - 40)
						/ (pos_temp[1][0] - pos_temp[0][0]);			//得到xfac
				tp_dev.xoff = (lcddev.width
						- tp_dev.xfac * (pos_temp[1][0] + pos_temp[0][0])) / 2;	//得到xoff

				tp_dev.yfac = (float) (lcddev.height - 40)
						/ (pos_temp[2][1] - pos_temp[0][1]);			//得到yfac
				tp_dev.yoff = (lcddev.height
						- tp_dev.yfac * (pos_temp[2][1] + pos_temp[0][1])) / 2;	//得到off
				if (abs(tp_dev.xfac) > 2 || abs(tp_dev.yfac) > 2)	//触屏和预设的相反了
						{
					cnt = 0;
					TP_Drow_Touch_Point(lcddev.width - 20, lcddev.height - 20,
					WHITE);	//清楚点4
					TP_Drow_Touch_Point(20, 20, RED);					//画点1
					LCD_ShowString(40, 26, lcddev.width, lcddev.height, 16,
							(u8 *) "TP Need readjust!");
					tp_dev.touchtype = !tp_dev.touchtype;	//修改触摸屏类型
					if (tp_dev.touchtype)					//X,Y方向与屏幕相反
					{
						CMD_RDX = 0x90;
						CMD_RDY = 0xD0;
					} else				   //X,Y方向与屏幕方向相同
					{
						CMD_RDX = 0xD0;
						CMD_RDY = 0x90;
					}
					continue;
				}
				POINT_COLOR = BLUE;
				LCD_Clear(WHITE);				   //清屏
				LCD_ShowString(35, 110, lcddev.width, lcddev.height, 16,
						(u8 *) "Touch Screen Adjust OK!");			 //校准完成
				LCD_Clear(WHITE);				   //清屏
				return;				   //校准完成
			}
		}
		delay_ms(10);
		outtime++;

	}
}

/*-----------------------------------------------------------------------------
 * 函数名称 ： TP_Init
 * 函数介绍 ：电阻屏初始化函数
 * 参数介绍 ： 无
 * 返回值    ：
 *----------------------------------------------------------------------------*/
u8 TP_Init(void) {

//#define  DOUT PFin(8)   //主机输入从机输出
//#define  TDIN PFout(9)  //主机输出从机输入
//#define  PEN  PFin(10)  //INT
//#define  TCLK  PBout(1)  //SPI时钟引脚
//#define  TCS   PBout(2)  //片选芯片

	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOF, ENABLE);

	//初始化引脚TCLK/TCS
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB, GPIO_Pin_1 | GPIO_Pin_2);

	//初始化引脚DOUT
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	GPIO_SetBits(GPIOF, GPIO_Pin_8);

	//初始化引脚TDIN
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	GPIO_SetBits(GPIOF, GPIO_Pin_9);

	//初始化引脚PEN
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	GPIO_SetBits(GPIOF, GPIO_Pin_10);

	//校准触摸屏
	LCD_Clear(WHITE);
	TP_Adjust();   //调用校准函数

	return 1;
}
