#include "nrf_adc.h"
#include "project_nrf51822.c"//包含51822工程需要用到的头文件（晶振，延时，io脚什么的

/** @brief Function for initializing the ADC peripheral.
*/
void nrf_adc_init_vbat(void)
{
	//NRF_ADC->CONFIG =0x4002;//ADC精度10bit，预分频2/3，使用内部带隙1.2V基准，AIN6引脚输入，模拟参考引脚输入禁用。
	//NRF_ADC ->ENABLE=1;//使能ADC
			//NRF_ADC->INTENSET=1;//打开ADC中断
			//NVIC_SetPriority(ADC_IRQn ,1);
			//NVIC_EnableIRQ(ADC_IRQn);


    // Configure ADC
    NRF_ADC->INTENSET   = ADC_INTENSET_END_Msk;
    NRF_ADC->CONFIG     = (ADC_CONFIG_RES_10bit                       << ADC_CONFIG_RES_Pos)     |
                          (ADC_CONFIG_INPSEL_AnalogInputNoPrescaling  << ADC_CONFIG_INPSEL_Pos)  |
                          (ADC_CONFIG_REFSEL_VBG                      << ADC_CONFIG_REFSEL_Pos)  |
                          (ADC_CONFIG_PSEL_AnalogInput7               << ADC_CONFIG_PSEL_Pos)    |
                          (ADC_CONFIG_EXTREFSEL_None                  << ADC_CONFIG_EXTREFSEL_Pos);
    NRF_ADC->EVENTS_END = 0;
    NRF_ADC->ENABLE     = ADC_ENABLE_ENABLE_Enabled;

    NRF_ADC->EVENTS_END  = 0;    // Stop any running conversions.
    NRF_ADC->TASKS_START = 1;
}

uint32_t nrf_adc_read(void)
{
  uint32_t adc_data;
  NRF_ADC->TASKS_START = 1;
  while(NRF_ADC->EVENTS_END == 0);
  NRF_ADC->EVENTS_END = 0;
  adc_data = NRF_ADC->RESULT;
  return adc_data;
}

float nrf_adc_read_vbat_f(void)
{
    return (float)nrf_adc_read() * 3.0f * 3.0f * 1.2f / 2.0f /1024.0f;
}

uint32_t nrf_adc_read_vbat_mv(void)
{
    // The non-optimized math is: (ADC / 1024 /2) * 3 * 3 * 1.2 * 1000
    return nrf_adc_read() / 1024 * (3 * 3 * 600) ;
}
