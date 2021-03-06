#include "sys.h"

/*-----------------------------------------------------------------------------
 *函数名称  ： WFI_SET
 *函数介绍  ：使用内嵌汇编，
 *参数介绍  ：无
 *返回值      ：无
 *---------------------------------------------------------------------------*/
void WFI_SET(void) {
	asm volatile("wfi");
}

/*-----------------------------------------------------------------------------
 *函数名称  ： MSR_MSP
 *函数介绍  ：使用内嵌汇编， 设置栈顶地址
 *参数介绍  ：addr  :  要设置的地址
 *返回值      ：无
 *---------------------------------------------------------------------------*/
void MSR_MSP(u32 addr) {
	asm volatile("MSR MSP, r0");
	asm volatile("BX r14");
}

/*-----------------------------------------------------------------------------
 *函数名称  ： INTX_ENABLE
 *函数介绍  ：使用内嵌汇编， 开启所有中断
 *参数介绍  ：无
 *返回值      ：无
 *---------------------------------------------------------------------------*/
void INTX_ENABLE(void) {
	asm volatile("cpsie i");
}

/*-----------------------------------------------------------------------------
 *函数名称  ： INTX_DISABLE
 *函数介绍  ：使用内嵌汇编， 关闭所有中断
 *参数介绍  ：无
 *返回值      ：无
 *---------------------------------------------------------------------------*/
void INTX_DISABLE(void) {
	asm volatile("cpsid i");
}
