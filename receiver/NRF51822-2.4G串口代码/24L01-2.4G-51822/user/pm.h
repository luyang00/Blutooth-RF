 /*
 * pm.h - Power Management driver and functions.
 */
 #ifndef PM_H_
 #define PM_H_

 #include "nrf_adc.h"
 #include "nrf51.h"
#include "nrf_gpio.h"

#ifndef CRITICAL_LOW_VOLTAGE
  #define PM_BAT_CRITICAL_LOW_VOLTAGE   3.0
#else
  #define PM_BAT_CRITICAL_LOW_VOLTAGE   CRITICAL_LOW_VOLTAGE
#endif
#ifndef CRITICAL_LOW_TIMEOUT
  #define PM_BAT_CRITICAL_LOW_TIMEOUT   M2T(1000 * 5) // 5 sec default
#else
  #define PM_BAT_CRITICAL_LOW_TIMEOUT   CRITICAL_LOW_VOLTAGE
#endif

#ifndef LOW_VOLTAGE
  #define PM_BAT_LOW_VOLTAGE   3.2
#else
  #define PM_BAT_LOW_VOLTAGE   LOW_VOLTAGE
#endif
#ifndef LOW_TIMEOUT
  #define PM_BAT_LOW_TIMEOUT   M2T(1000 * 5) // 5 sec default
#else
  #define PM_BAT_LOW_TIMEOUT   LOW_TIMEOUT
#endif

#ifndef SYSTEM_SHUTDOWN_TIMEOUT
  #define PM_SYSTEM_SHUTDOWN_TIMEOUT    M2T(1000 * 60 * 5) // 5 min default
#else
  #define PM_SYSTEM_SHUTDOWN_TIMEOUT    M2T(1000 * 60 * SYSTEM_SHUTDOWN_TIMEOUT)
#endif

// Power managment pins

#define PM_GPIO_SYSOFF          30
#define PM_GPIO_EN1             24
#define PM_GPIO_EN2             25
#define PM_GPIO_IN_CHG          22
#define PM_GPIO_IN_PGOOD        23

// Power managment pins

//#define PM_GPIO_BAT             1



#define PM_BAT_CRITICAL_LOW_VOLTAGE   3.0
#define PM_BAT_CRITICAL_LOW_TIMEOUT   M2T(1000 * 5) // 5 sec
#define PM_BAT_LOW_VOLTAGE            3.2
#define PM_BAT_LOW_TIMEOUT            M2T(1000 * 5) // 5 sec

#define PM_BAT_DIVIDER                (float)(3.0)
#define PM_BAT_ADC_FOR_3_VOLT         (int32_t)(((3.0 / PM_BAT_DIVIDER) / 2.8) * 4096)
#define PM_BAT_ADC_FOR_1p2_VOLT       (int32_t)(((1.2 / PM_BAT_DIVIDER) / 2.8) * 4096)

#define PM_SYSTEM_SHUTDOWN_TIMEOUT    M2T(1000 * 60 * 5) // 5 min

#define PM_BAT_IIR_SHIFT     8
/**
 * Set PM_BAT_WANTED_LPF_CUTOFF_HZ to the wanted cut-off freq in Hz.
 */
#define PM_BAT_WANTED_LPF_CUTOFF_HZ   1

/**
 * Attenuation should be between 1 to 256.
 *
 * f0 = fs / 2*pi*attenuation.
 * attenuation = fs / 2*pi*f0
 */
#define PM_BAT_IIR_LPF_ATTENUATION (int)(ADC_SAMPLING_FREQ / (int)(2 * 3.1415 * PM_BAT_WANTED_LPF_CUTOFF_HZ))
#define PM_BAT_IIR_LPF_ATT_FACTOR  (int)((1<<PM_BAT_IIR_SHIFT) / PM_BAT_IIR_LPF_ATTENUATION)

typedef enum
{
  battery,
  charging,
  charged,
  lowPower,
  shutDown,
} PMStates;

typedef enum
{
  charge100mA,
  charge500mA,
  chargeMax,
} PMChargeStates;


void pmInit(void);

bool pmTest(void);

/**
 * Power management task
 */
void pmTask(void *param);

void pmSetChargeState(PMChargeStates chgState);

/**
 * Returns the battery voltage i volts as a float
 */
float pmGetBatteryVoltage(void);

/**
 * Returns the min battery voltage i volts as a float
 */
float pmGetBatteryVoltageMin(void);

/**
 * Returns the max battery voltage i volts as a float
 */
float pmGetBatteryVoltageMax(void);

/**
 * Updates and calculates battery values.
 * Should be called for every new adcValues sample.
 */
void pmBatteryUpdate(AdcGroup* adcValues);

/**
 * Returns true if the battery is currently in use
 */
bool pmIsDischarging(void);

PMStates pmUpdateState(void);

#endif /* PM_H_ */
