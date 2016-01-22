#ifndef PROJECT_NRF51822_H//如果没定义过该文件
#define PROJECT_NRF51822_H//定义该文件

//C语言
#include <stdbool.h>//调用bool
#include "stdio.h"//标准输入输出
#include <stdint.h>//声明已知大小的整数或显示特征的整数

//nrf51工程头文件
#include "nrf51.h"//包含51822各个寄存器结构地址
#include "nrf51_bitfields.h"//包含51822各个寄存器设置值
#include "system_nrf51.h"//晶振频率设置
#include "boards.h"//设置开发板型号
#include "usbjtag.H"//设置USB开发板型号
#include "nrf_delay.h"//延时
#include "nrf_gpio.h"//引脚
#include "simple_uart.h"//串口
#include "spi_master.h"//spi传输
#include "nrf_ecb.h"//ecb
#include "nrf_gpiote.h"//
#include "nrf_nvmc.h"//
#include "twi_master.h"//
#include "nrf.h"//
#include "nrf_adc.h"
#include "pm.h"
#include "radio_config.h"

#endif 



