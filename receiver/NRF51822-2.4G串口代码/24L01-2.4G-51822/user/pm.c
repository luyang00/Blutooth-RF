 /*
 * pm.c - Power Management driver and functions.
 */
#include "pm.h"
#include "nrf_delay.h"

#define ACTIVATE_AUTO_SHUTDOWN
static float    batteryVoltage;
static float    batteryVoltageMin = 6.0;
static float    batteryVoltageMax = 0.0;
static int32_t  batteryVRawFilt = PM_BAT_ADC_FOR_3_VOLT;
static int32_t  batteryVRefRawFilt = PM_BAT_ADC_FOR_1p2_VOLT;
static uint32_t batteryLowTimeStamp;
static uint32_t batteryCriticalLowTimeStamp;
static bool isInit;
static PMStates pmState;
static PMChargeStates pmChargeState;
static void pmSetBatteryVoltage(float voltage);

const static float bat671723HS25C[10] =
{
  3.00, // 00%
  3.78, // 10%
  3.83, // 20%
  3.87, // 30%
  3.89, // 40%
  3.92, // 50%
  3.96, // 60%
  4.00, // 70%
  4.04, // 80%
  4.10  // 90%
};

//Power Management pins
//SYSOFF->P0.30
//EN1->P0.24
//EN2->P0.25
//PGOOD->P0.23
//CHG->P0.22
//VBAT->P0.1_AIN2
void pmInit(void)
{
  if(isInit)
    return;
  
  // Configure PM PGOOD pin (Power good)
  NRF_GPIO->PIN_CNF[PM_GPIO_IN_PGOOD]=0;//输入，不上下拉，标准驱动0、1
  
	// Configure PM CHG pin (Charge)
  NRF_GPIO->PIN_CNF[PM_GPIO_IN_CHG]=3<<2;//输入，上拉，标准驱动0、1
  // Configure PM EN2 pin
  NRF_GPIO->PIN_CNF[PM_GPIO_EN2]=0x301;//输出，高驱动0，高驱动1，不上下拉
  
	// Configure PM EN1 pin
  NRF_GPIO->PIN_CNF[PM_GPIO_EN1]=0x301;//输出，高驱动0，高驱动1，不上下拉

  // Configure PM SYSOFF pin
  NRF_GPIO->PIN_CNF[PM_GPIO_SYSOFF]=0x301;//输出，高驱动0，高驱动1，不上下拉

  // Configure battery ADC pin
  //adc引脚初始化在nrf_adc.c中完成！！！

  isInit = true;
}

bool pmTest(void)
{
  return isInit;
}

// /**
//  * IIR low pass filter the samples.
//  */
// static int16_t pmBatteryIIRLPFilter(uint16_t in, int32_t* filt)
// {
//   int32_t inScaled;
//   int32_t filttmp = *filt;
//   int16_t out;

//   // Shift to keep accuracy
//   inScaled = in << PM_BAT_IIR_SHIFT;
//   // Calculate IIR filter
//   filttmp = filttmp + (((inScaled-filttmp) >> 8) * PM_BAT_IIR_LPF_ATT_FACTOR);
//   // Scale and round
//   out = (filttmp >> 8) + ((filttmp & (1 << (PM_BAT_IIR_SHIFT - 1))) >> (PM_BAT_IIR_SHIFT - 1));
//   *filt = filttmp;

//   return out;
// }

/**
 * Sets the battery voltage and its min and max values
 */
static void pmSetBatteryVoltage(float voltage)
{
  batteryVoltage = voltage;
  if (batteryVoltageMax < voltage)
  {
    batteryVoltageMax = voltage;
  }
  if (batteryVoltageMin > voltage)
  {
    batteryVoltageMin = voltage;
  }
}


/**
 * Shutdown system
 */
static void pmSystemShutdown(void)
{
#ifdef ACTIVATE_AUTO_SHUTDOWN
  nrf_gpio_pin_set(PM_GPIO_SYSOFF);
#endif
}

/**
 * Returns a number from 0 to 9 where 0 is completely discharged
 * and 9 is 90% charged.
 */
static int32_t pmBatteryChargeFromVoltage(float voltage)
{
  int charge = 0;

  if (voltage < bat671723HS25C[0])
  {
    return 0;
  }
  if (voltage > bat671723HS25C[9])
  {
    return 9;
  }
  while (voltage >  bat671723HS25C[charge])
  {
    charge++;
  }

  return charge;
}

float pmGetBatteryVoltage(void)
{
  return batteryVoltage;
}

float pmGetBatteryVoltageMin(void)
{
  return batteryVoltageMin;
}

float pmGetBatteryVoltageMax(void)
{
  return batteryVoltageMax;
}

// void pmBatteryUpdate(AdcGroup* adcValues)
// {
//   float vBat;
//   int16_t vBatRaw;
//   int16_t vBatRefRaw;

//   vBatRaw = pmBatteryIIRLPFilter(adcValues->vbat.val, &batteryVRawFilt);
//   vBatRefRaw = pmBatteryIIRLPFilter(adcValues->vbat.vref, &batteryVRefRawFilt);

//   vBat = adcConvertToVoltageFloat(vBatRaw, vBatRefRaw) * PM_BAT_DIVIDER;
//   pmSetBatteryVoltage(vBat);
// }

void pmSetChargeState(PMChargeStates chgState)
{
  pmChargeState = chgState;

  switch (chgState)
  {
    case charge100mA:
      nrf_gpio_pin_clear(PM_GPIO_EN1);
      nrf_gpio_pin_clear(PM_GPIO_EN2);
      break;
    case charge500mA:
      nrf_gpio_pin_set(PM_GPIO_EN1);
      nrf_gpio_pin_clear(PM_GPIO_EN2);
      break;
    case chargeMax:
      nrf_gpio_pin_clear(PM_GPIO_EN1);
      nrf_gpio_pin_set(PM_GPIO_EN2);
      break;
  }
}

PMChargeStates pmGetChargeState(void)
{
  return pmChargeState;
}

PMStates pmUpdateState(void)
{
  PMStates state;
  bool isCharging = !nrf_gpio_pin_read(PM_GPIO_IN_CHG);
  bool isPgood = !nrf_gpio_pin_read(PM_GPIO_IN_PGOOD);
  uint32_t batteryLowTime;

  //batteryLowTime = xTaskGetTickCount() - batteryLowTimeStamp;

  if (isPgood && !isCharging)
  {
    state = charged;
		nrf_gpio_pin_clear(17);
		nrf_gpio_pin_set(18);//充电完成
  }
  else if (isPgood && isCharging)
  {
    state = charging;
		nrf_gpio_pin_set(17);//正在充电
		nrf_gpio_pin_clear(18);
  }
  else if (!isPgood && !isCharging)
  {
    state = lowPower;
		nrf_gpio_pin_clear (18);
		nrf_gpio_pin_clear (17);
		nrf_delay_ms(500);
		nrf_gpio_pin_set (17);//没插USB充电线
  }
  else
  {
    state = battery;
		nrf_gpio_pin_clear (18);
		nrf_delay_ms(500);
		nrf_gpio_pin_set (18);
  }

  return state;
}

// return true if battery discharging
bool pmIsDischarging(void) {
    PMStates pmState;
    pmState = pmUpdateState();
    return (pmState == lowPower )|| (pmState == battery);
}

