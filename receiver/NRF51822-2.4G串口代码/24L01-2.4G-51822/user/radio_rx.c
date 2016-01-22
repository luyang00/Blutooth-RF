//arm_startup_nrf51.s是启动文件，包含中断定义
//#include "nrf51.h"//包含51822各个寄存器结构地址。以上2文件就可以控制51822了。
//nrf51_bitfields.h包含各个寄存器设置值的作用
#include "project_nrf51822.c"//包含51822工程需要用到的头文件（晶振，延时，io脚什么的

#include <stdbool.h>//调用bool
#include "stdio.h"//标准输入输出
#include <stdint.h>//声明已知大小的整数或显示特征的整数
#include "PCA10001.H"
#include "led.h"

__IO char first = 0;
__IO char link_num =0;

static uint8_t 	TXBUF[32]={0,};
static uint8_t 	RXBUF[32]={0,};
static uint8_t index = 0;
uint8_t delay_cnt=10;
unsigned char result=0;//设置宏变量方便调试观察
uint8_t Get_RNG()
{
	uint8_t cnt=1;
    NRF_RNG->TASKS_START = 1; // start the RNG peripheral.
		// Clear the VALRDY EVENT.
		NRF_RNG->EVENTS_VALRDY = 0;
		// Wait until the value ready event is generated.
		while (NRF_RNG->EVENTS_VALRDY == 0)
		{
			nrf_delay_ms(1);
			if(cnt++)
				 break ;
		}
		result = (uint8_t)NRF_RNG->VALUE %20 ; //0-19
		if(result%2==1)//奇数
			result +=1;//偶数
		
		NRF_RNG->TASKS_START = 0; // stop the RNG peripheral.
		
		return result;
}

void init(void)//51822时钟初始化
{ 
	 //判断电源时钟状态
	 if ((((*(uint32_t *)0xF0000FE0) & 0xFF) == 1) && (((*(uint32_t *)0xF0000FE4) & 0xF) == 0))
    {
        if ((((*(uint32_t *)0xF0000FE8) & 0xF0) == 0x0) && (((*(uint32_t *)0xF0000FEC) & 0xF0) == 0x0))
        {
					//解决radio无法写入  
          *(uint32_t *)0x40000504 = 0xC007FFDF;//电源时钟设置
          *(uint32_t *)0x40006C18 = 0x00008000;//GPIOTE设置    
        }
				else if ((((*(uint32_t *)0xF0000FE8) & 0xF0) == 0x10) && (((*(uint32_t *)0xF0000FEC) & 0xF0) == 0x0))
        {
          //解决radio无法写入  
          *(uint32_t *)0x40000504 = 0xC007FFDF;//电源时钟设置
          *(uint32_t *)0x40006C18 = 0x00008000;//GPIOTE设置  
        }
        else if ((((*(uint32_t *)0xF0000FE8) & 0xF0) == 0x30) && (((*(uint32_t *)0xF0000FEC) & 0xF0) == 0x0))
        {
          //解决radio无法写入  
          *(uint32_t *)0x40000504 = 0xC007FFDF;//电源时钟设置
          *(uint32_t *)0x40006C18 = 0x00008000;//GPIOTE设置    
        }
    }
	NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;//HFCLK晶振运行状态0关闭，1运行
  NRF_CLOCK->TASKS_HFCLKSTART = 1;//高外晶振HFCLK 0关闭，1启动
  while(!NRF_CLOCK->EVENTS_HFCLKSTARTED);//等待HFCLK晶振运行状态为1
}


//packet[3]中QxC(Q开始，C结束，x是数据char)
static uint8_t volatile packet[32];  //< 数据传输的缓冲包packet[1]是数据
//nrf51822????
uint8_t NRF51_TX()//从UART接收到数据 然后通过Radio发送到上位机
{   
  uint8_t i=0;  
	LED0_Close();
	//数据准备发送
	NRF_RADIO->EVENTS_READY = 0U;//收发模式切换完成标志位。复位
	NRF_RADIO->TASKS_TXEN = 1U;//启动无线电为发射模式
	
	for(i=0;i<32;i++)
	packet[i] = TXBUF[i];
	
  while (NRF_RADIO->EVENTS_READY == 0U);//等待收发模式切换完成
			// 启动数据传输
	NRF_RADIO->EVENTS_END  = 0U;//传输结束标志位，复位    
	NRF_RADIO->TASKS_START = 1U;//开始传输		
	while(NRF_RADIO->EVENTS_END == 0U) //等待传输结束
	{ 					
			;//
	}
	//nrf_gpio_pin_clear(18);//led0灭，表示数据已经发出
	NRF_RADIO->EVENTS_DISABLED = 0U;//传输关闭标志位  复位
	NRF_RADIO->TASKS_DISABLE   = 1U; // 关闭传输
	while(NRF_RADIO->EVENTS_DISABLED == 0U)//等待传输关闭
	{
			// Do nothing.
	}
	
  	return 1;
}
//nrf51822????
uint8_t NRF51_RX()//从Radio接收到数据 然后通过UART发送到下位机
{
	uint32_t  RF_timeOver_flag = 0;
	uint8_t i=0;
	bool  CRC_SUCCESS = 0 ;
	NRF_RADIO->EVENTS_READY = 0U; //收发模式转换完成  标志位       
	NRF_RADIO->TASKS_RXEN   = 1U; //接收模式
	while(NRF_RADIO->EVENTS_READY == 0U) //等待收发模式转换完成(接收模式)标志位
	{
	// Do nothing.等待
	}
	NRF_RADIO->EVENTS_END  = 0U;//传输完成  标志位     
	NRF_RADIO->TASKS_START = 1U; // 开始传输

	while(NRF_RADIO->EVENTS_END == 0U)//等待传输完成  标志位
	{
		// Do nothing.等待
		RF_timeOver_flag++;
		nrf_delay_ms(1);
		if(RF_timeOver_flag > 1000)
		{
			break;
			//goto EER ;//如果长时间没有接收到无线数据包，跳出while循环。。。不至于影响其他程序运行！！！
		}
	}
	for(i=0;i<32;i++)
	{
	 RXBUF[i]=packet[i];
		packet[i]=0;
	}
	if (NRF_RADIO->CRCSTATUS == 1U)//如果CRC校验正确
	{					
		if(RXBUF[0]==0xAA && RXBUF[1]==0xAF)
		{
			for(char i=0;i<32;i++)
			{
				simple_uart_put(RXBUF[i]);
			}	
			LED0_Open(); 
			LED1_Close(); 	
			CRC_SUCCESS = 1 ;	
			first =10;		
      link_num = 10;			
		}						
	}
	// EER:
	NRF_RADIO->EVENTS_DISABLED = 0U;//无线关闭   标志位
	NRF_RADIO->TASKS_DISABLE   = 1U;// 关闭无线设备
	while(NRF_RADIO->EVENTS_DISABLED == 0U)//等待设备关闭
	{
		// Do nothing.
		//	LED0_Open(); 
		//	LED1_Close(); 					
	}
	if( CRC_SUCCESS) //
		return 0;					
	else 
	{	  
		if(link_num !=0) link_num --;
		return 1;
	}
		
} 
/**@brief  Function for initializing the UART module.
 */
static void uart_init(void)
{
    /**@snippet [UART Initialization] */
	  simple_uart_config(RTS_PIN_NUMBER, TX_PIN_NUMBER, CTS_PIN_NUMBER, RX_PIN_NUMBER, HWFC);
    
    NRF_UART0->INTENSET = UART_INTENSET_RXDRDY_Enabled << UART_INTENSET_RXDRDY_Pos;
    
    NVIC_SetPriority(UART0_IRQn,3);
    NVIC_EnableIRQ(UART0_IRQn);
    /**@snippet [UART Initialization] */
}

void UART0_IRQHandler(void)//从UART接收到数据后 
{		
    TXBUF[index] = simple_uart_get();
    index++;
		delay_cnt=10;
}
void UART_To_Radio()
{
			if(index!=0)
			{
				while(delay_cnt--)
				{
				 nrf_delay_us(100);
				}
        NRF51_TX();
				delay_cnt=10;
        index = 0;			 
			}	
}
uint32_t adc_result = 0 ;
#define FQ_CHK_EN 0  //0:不对频  1:对频
int main(void)//main主程序开始
{
	uint8_t FQ = 24;
	uint8_t led_Blink_Cnt=0;
	init();//时钟初始化
	LED_Init();//引脚初始化
	uart_init();
	radio_configure();//无线电配置（也可用于和nrf2401通讯）
//#if FQ_CHK_EN
//	nrf_adc_init_vbat();
//  FQ =Get_RNG();//随机数
//#else 
	FQ = 24;
//#endif
	// 设置有效载荷指针
	NRF_RADIO->PACKETPTR    = (uint32_t)packet;//指针指向数据缓冲packet
	NRF_RADIO->FREQUENCY =(uint32_t)(FQ);//设置频率  2424M
	while(1)
	{
		EXIT:
		LED0_Close();
		LED1_Open(); 
//		#if FQ_CHK_EN
//		TXBUF[0]=0xAA;//包头
//		TXBUF[1]=0XFF;
//		TXBUF[2]=FQ;//频率
//		for(uint8_t i=3;i<32;i++)//其余位填充0
//		{
//					 TXBUF[i]=FQ ;
//		}
//		while(1)//发送频率数据，对频，
//		{
//			NRF51_TX();//发送频率				
//			if(led_Blink_Cnt++ <=1)
//			{
//			 	LED0_Open();
//				nrf_delay_ms(1);
//			}
//			else if(led_Blink_Cnt >=2)
//			{
//				LED0_Close();
//				nrf_delay_ms(1);
//				led_Blink_Cnt=0;
//			}
//			
//			if(!NRF51_RX())//有接收,说明对频成功，退出发射模式改为接收模式
//				break;
//		}
//		#endif
		while(1)
		{
			NRF51_RX();//从Radio接收到数据 然后通过UART发送到下位机
			//UART_To_Radio();//UART 接收到数据发送到Radio
			if(link_num ==0)
			{
			 goto EXIT;
			}
		}
	}   
}

