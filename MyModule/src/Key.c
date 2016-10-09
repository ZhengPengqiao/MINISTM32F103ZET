#include "Key.h"
void key_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启按键端口（PA0/PA13/PA15）的时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	/*初始化KEY0-->GPIOA.13,KEY1-->GPIOA.15  上拉输入*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	/*初始化 WK_UP-->GPIOA.0	  下拉输入*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

}

unsigned char key_scan()
{
	if (KEY2_Scan())
	{
		return 2;
	}
	if (KEY3_Scan())
	{
		return 3;
	}
	return 0;
}

unsigned char KEY2_Scan(void)
{

	if (KEY2 == 1)
	{
		/*延时去抖动 */
		timer_ms(100);

		if (KEY2 == 1)
		{
			while (KEY2)
				;  //等待按键松开
			return 1;
		}
	}
	return 0;
}

unsigned char KEY3_Scan(void)
{
	if (KEY3 == 0)
	{
		/*延时去抖动 */
		timer_ms(100);

		if (KEY3 == 0)
		{
			while (!KEY3)
				;  //等待按键松开
			return 1;
		}
	}
	return 0;
}

