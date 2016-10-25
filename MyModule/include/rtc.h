#ifndef __RTC_H_
#define __RTC_H_    

//时间结构体
typedef struct 
{
	vu8 hour; //时
	vu8 min;  //分
	vu8 sec;  //秒		
	vu16 w_year;  //年
	vu8  w_month; //月
	vu8  w_date;  //日
	vu8  week;    //星期 
}calendarStruct;					 
extern calendarStruct calendar;	//日历结构体

extern u8 const mon_table[12];	//月份日期数据表

/*-----------------------------------------------------------------------------
 * 函数名称 ：rtcInit
 * 函数介绍 ： 初始化rtc,实时时钟配置，初始化RTC时钟,同时检测时钟是否工作正常，BKP->DR1用于
 * 			保存是否第一次配置的设置
 * 参数介绍 ：无
 * 返回值     ： 0:正常;  其他:错误代码
 *---------------------------------------------------------------------------*/
u8 rtcInit(void);       

/*-----------------------------------------------------------------------------
 * 函数名称 ：isLeapYear
 * 函数介绍 ： 平年,闰年判断
 *  //判断是否是闰年函数
 *  //月份   1  2  3  4  5  6  7  8  9  10 11 12
 *  //闰年   31 29 31 30 31 30 31 31 30 31 30 31
 *  //非闰年 31 28 31 30 31 30 31 31 30 31 30 31
 * 参数介绍 ：year : 要判断的年份
 * 返回值     ： 该年份是不是闰年.1,是.0,不是
 *---------------------------------------------------------------------------*/
u8 isLeapYear(u16 year);
/*-----------------------------------------------------------------------------
 * 函数名称 ：rtcAlarmSet
 * 函数介绍 ： 设置时钟，把输入的时钟转换为秒钟，以1970年1月1日为基准，1970~2099年为合法年份
 * 参数介绍 ：参数依次是： 年月日时分秒
 * 返回值     ：0,成功;其他:错误代码.
 *---------------------------------------------------------------------------*/
u8 rtcAlarmSet(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);
/*-----------------------------------------------------------------------------
 * 函数名称 ：rtcGet
 * 函数介绍 ：  更新时间   
 * 参数介绍 ：无
 * 返回值     ：0,成功;其他:错误代码.
 *---------------------------------------------------------------------------*/
u8 rtcGet(void);        
/*-----------------------------------------------------------------------------
 * 函数名称 ：rtcGetWeek
 * 函数介绍 ： 得到星期
 * 参数介绍 ：year : 年份     mouth　：　月份　　　　day : 天
 * 返回值     ： 星期号
 *---------------------------------------------------------------------------*/
u8 rtcGetWeek(u16 year,u8 month,u8 day);
/*-----------------------------------------------------------------------------
 * 函数名称 ：rtcSet
 * 函数介绍 ： 设置时钟，把输入的时钟转换为秒钟，以1970年1月1日为基准，1970~2099年为合法年份
 * 参数介绍 ：参数依次是：年月日时分秒
 * 返回值     ：0,成功;其他:错误代码.
 *---------------------------------------------------------------------------*/
u8 rtcSet(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);//设置时间			 
/*-----------------------------------------------------------------------------
 * 函数名称 ：isLeapYear
 * 函数介绍 ： 平年,闰年判断
 * 参数介绍 ：year : 要判断的年份
 * 返回值     ： 1,是.0,不是
 *---------------------------------------------------------------------------*/
#endif   //__RTC_H_


