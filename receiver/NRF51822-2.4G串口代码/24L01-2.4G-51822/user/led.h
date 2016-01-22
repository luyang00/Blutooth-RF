#ifndef __LED_H
#define	__LED_H

#include "nrf51.h"

#define LED_START      1
#define LED_0          1
#define LED_1          30
#define LED_STOP       30

void LED_Init(void);
void LED1_Open(void);
void LED1_Close(void);
void LED1_Toggle(void);
void LED0_Open(void);
void LED0_Close(void);
void LED0_Toggle(void);



#endif /* __LED_H */

