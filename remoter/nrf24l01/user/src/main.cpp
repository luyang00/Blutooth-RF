#include "stm32f4xx.h"
#include "24l01.h"
#include "spi.h"
#include "SyncGen.h"
#include "hoop.h"
#include <stdio.h>
#include "adc.h"
#include <string.h>
u8 package_buf[32];	
u8 payload[25];
u8 hoop_sequence[1000];
u8 sync_sequence[3];
int hoop_tx=0;
using namespace HOOP;
NRF_HOOP hoop;
void delay_us(u16 us)
{
	int i,j;
	for(j=0;j<us;j++)
		for(i=0;i<14;i++);
}
void LED_GPIOInit(void)
 
{ 
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);     
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC,GPIO_Pin_4);
	GPIO_SetBits(GPIOC,GPIO_Pin_5);
//	GPIO_Bits(GPIOC,GPIO_Pin_4);
//	GPIO_SetBits(GPIOC,GPIO_Pin_5);
}
void TIM3_GPIOInit(void)
 
{ 
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);    
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
	
}


void TIM3_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  //使能TIM3时钟
	
    TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseInitStructure.TIM_Period = arr; 	//自动装载值
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	
//	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //允许定时器3中断更新
	
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //定时器中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占式优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}
void SwitchInit()
{
	GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);    
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
}
int flag;
int cnt;
extern "C" void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) 
	{
		hoop.set_interval2ms(true);
		//GPIO_ToggleBits(GPIOC,GPIO_Pin_1);
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //???????
}
int throttle_val,yaw_val,pitch_val,roll_val;
int mode_switch,safe_switch;
int main(void)
{  
	LED_GPIOInit();
	SwitchInit();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	NRF24L01_Init();
	while(NRF24L01_Check());//if not ok ,freeloop
	//TIM3_GPIOInit();
	NRF24L01_RX_Mode();
	/*TX mode:*/

	//TIM3_Int_Init(21000,7);//2ms Interrupt (PWMI8 OUTPUT a hole period about 4ms in osciloscope not a accurcy value) 
	
	
	F4ADC throttle(ADC1,ADC_Channel_8);
	F4ADC yaw(ADC1,ADC_Channel_9);
	F4ADC roll(ADC1,ADC_Channel_10);
	F4ADC pitch(ADC1,ADC_Channel_11);
	
	//hoop.init_hoop(20,hoop_sequence,1000,sync_sequence,3,package_buf,TXMODE);
	//hoop.set_tx_callback(NRF24L01_TxPacket);
	//hoop.set_channel(0);
	//TIM_Cmd(TIM3,ENABLE); //使能定时器
	//TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //允许定时器3中断更新
	NRF24L01_TX_Mode();
	while(1)
	{
		throttle_val = throttle.read();
		payload[0]=(unsigned char)(throttle_val>>8);
		payload[1]=(unsigned char)(throttle_val);
		yaw_val =yaw.read();
		payload[2]=(unsigned char)(yaw_val>>8);
		payload[3]=(unsigned char)(yaw_val);
		pitch_val = pitch.read();
		payload[4]=(unsigned char)(pitch_val>>8);
		payload[5]=(unsigned char)(pitch_val);
		roll_val = roll.read();
		payload[6]=(unsigned char)(roll_val>>8);
		payload[7]=(unsigned char)(roll_val);
		mode_switch = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4);
		payload[8]=(unsigned char)(mode_switch);
		safe_switch = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15);
		payload[9]=(unsigned char)(safe_switch);
		
		package_buf[0]=0xAA;
		package_buf[1]=0xAF;
		memcpy(package_buf+2,payload,10);
		NRF24L01_TxPacket(package_buf);
		delay_us(2000);
		
		//hoop.tx_loop(payload);
	}
}

struct __FILE { int handle; /* Add whatever you need here */ };
#define ITM_Port8(n)    (*((volatile unsigned char *)(0xE0000000+4*n)))
#define ITM_Port16(n)   (*((volatile unsigned short*)(0xE0000000+4*n)))
#define ITM_Port32(n)   (*((volatile unsigned long *)(0xE0000000+4*n)))
#define DEMCR           (*((volatile unsigned long *)(0xE000EDFC)))
#define TRCENA          0x01000000

extern "C" int fputc(int ch, FILE *f)
{
	if (DEMCR & TRCENA) 
	{
		while (ITM_Port32(0) == 0);
		ITM_Port8(0) = ch;
	}
	return (ch);
}

