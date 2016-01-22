#include <stdint.h>//声明已知大小的整数或显示特征的整数
#include "nrf51.h"//包含51822各个寄存器结构地址
#include "nrf_gpio.h"
//输入io配置  x起始 y结束
void gpio_init_in(uint8_t x,uint8_t y,uint8_t z)
{
  uint8_t i;//计算多少个io脚
	i=(y-x)+1;
	while(i--)
	{	
	 NRF_GPIO->PIN_CNF[x++]=(0 << 0)//引脚作用0输入，1输出 
                        | (0 << 1)//输入缓冲 0接入，1断开
                        | (z << 2)//内置13k电阻  01下拉，11上拉
                        | (2 << 8)//驱动电流强度
                        | (3 <<16);//电平沿感知  10上升沿11下降沿
	}
}

//输出io配置  x起始 y结束
void gpio_init_out(uint8_t x,uint8_t y)
{
  uint8_t i;//计算多少个io脚
	i=(y-x)+1;
	while(i--)
	{	
	 NRF_GPIO->PIN_CNF[x++]=(1 << 0)//引脚作用0输入，1输出 
                        | (1 << 1)//输入缓冲 0接入，1断开
                        | (0 << 2)//内置13k电阻  01下拉，11上拉
                        | (1 << 8)//驱动电流强度
                        | (0 <<16);//电平沿感知  10上升沿11下降沿
	}
}

