#ifndef __NRF_ADC_H__
#define __NRF_ADC_H__

#include <stdbool.h>
#include <stdint.h>
#include "nrf51.h"
/******** Defines ********/

/**
 * \def ADC_MEAN_SIZE
 * Number of samples used in the mean value calculation.
 * Mean size should be evenly dividable by decimation bits.
 */
#define ADC_DECIMATE_TO_BITS  12
#define ADC_MEAN_SIZE         8

#define ADC_RESOLUTION        12
#define ADC_DECIMATE_DIVEDEND (ADC_MEAN_SIZE / (1 << (ADC_DECIMATE_TO_BITS - ADC_RESOLUTION)))

#if ADC_DECIMATE_TO_BITS < ADC_RESOLUTION
#  error "ADC_DECIMATE_TO_BITS must be bigger or equal to ADC_RESOLUTION"
#endif

#define ADC_SAMPLING_FREQ      100
#define ADC_OVERSAMPLING_FREQ  (ADC_SAMPLING_FREQ * ADC_MEAN_SIZE)

#define ADC_TRIG_PRESCALE       1
#define ADC_TRIG_PRESCALE_FREQ  (72000000 / (ADC_TRIG_PRESCALE + 1))
#define ADC_TRIG_PERIOD         (ADC_TRIG_PRESCALE_FREQ / (ADC_OVERSAMPLING_FREQ))

#define ADC_INTERNAL_VREF   1.20

/******** Types ********/

typedef struct __attribute__((packed))
{
  uint16_t vref;
  uint16_t val;
} AdcPair;

typedef struct __attribute__((packed))
{
  AdcPair vbat;
} AdcGroup;

typedef struct
{
  uint16_t vbat;
  uint16_t vbatVref;
} AdcDeciGroup;

/*** Public interface ***/

/**
 * Initialize analog to digital converter. Configures gyro and vref channels.
 * Configures DMA to transfer the result.
 */
void adcInit(void);

bool adcTest(void);

/**
 * Converts a 12 bit ADC value to battery voltage
 * @param vbat  12 bit adc value
 * @param vref  12 bit adc value of the internal voltage
 *              reference, 1.2V
 *
 * @return The voltage in a float value
 */
float adcConvertToVoltageFloat(uint16_t v, uint16_t vref);

/**
 * Starts converting ADC samples by activating the DMA.
 */
void adcDmaStart(void);

/**
 * Stop converting ADC samples.
 */
void adcDmaStop(void);

/**
 * ADC interrupt handler
 */
void adcInterruptHandler(void);

/**
 * ADC task
 */
void adcTask(void *param);

typedef enum
{
    ADC_RES_8bit = 0, 
    ADC_RES_9bit, 
    ADC_RES_10bit
} ADC_Res_t;

typedef enum 
{
    ADC_INPUT_AIN0_P26 = 0, 
    ADC_INPUT_AIN1_P27 = 1, 
    ADC_INPUT_AIN2_P01 = 2, 
    ADC_INPUT_AIN3_P02 = 3, 
    ADC_INPUT_AIN4_P03 = 4, 
    ADC_INPUT_AIN5_P04 = 5, 
    ADC_INPUT_AIN6_P05 = 6, 
    ADC_INPUT_AIN7_P06 = 7
} ADC_input_selection_t;

typedef enum 
{
    ADC_INT_DISABLED = 0,
    ADC_INT_ENABLED
} ADC_interrupt_enabled_t;

/**
 * @brief Configures the ADC to measure an analog input relative to VDD. 
 *
 * @param ADC_res specifies the resolution of the ADC
 *
 * @param ADC_input_selection specifies the analog input pin to the ADC
 *
 * @param ADC_interrupt_enabled enables the ADC interrupt
 *
 */

void nrf_adc_init(ADC_Res_t ADC_res, ADC_input_selection_t ADC_input_selection, ADC_interrupt_enabled_t ADC_interrupt_enabled);

/**
 * @brief Configures the ADC to measure VBAT. 
 *
 */
 
void nrf_adc_init_vbat(void);

/**
 * @brief Reads a single sample from the ADC without using interrupts, should only be used if the ADC is initialized without interrupts 
 *
 * @return Returns the ADC sample
 */

uint32_t nrf_adc_read(void);

/**
 * @brief Reads VBAT and converts it to a floating point number with a unit of 1.0 V
 *
 * @return VBAT [V]
 */

float nrf_adc_read_vbat_f(void);

/**
 * @brief Reads VBAT and converts it to an integer with a unit of 1 mV
 *
 * @return VBAT [mV]
 */

uint32_t nrf_adc_read_vbat_mv(void);

// ADC INTERRUPT EXAMPLE
// Remember to include something like this in the code if ADC_interrupt_enabled is set
//
// void ADC_IRQHandler()
// {
//     NRF_ADC->EVENTS_END = 0;
//     // The data can be read from NRF_ADC->RESULT
// }

#endif
