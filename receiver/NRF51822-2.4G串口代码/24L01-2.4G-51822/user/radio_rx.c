//arm_startup_nrf51.s�������ļ��������ж϶���
//#include "nrf51.h"//����51822�����Ĵ����ṹ��ַ������2�ļ��Ϳ��Կ���51822�ˡ�
//nrf51_bitfields.h���������Ĵ�������ֵ������
#include "project_nrf51822.c"//����51822������Ҫ�õ���ͷ�ļ���������ʱ��io��ʲô��

#include <stdbool.h>//����bool
#include "stdio.h"//��׼�������
#include <stdint.h>//������֪��С����������ʾ����������
#include "PCA10001.H"
#include "led.h"

__IO char first = 0;
__IO char link_num =0;

static uint8_t 	TXBUF[32]={0,};
static uint8_t 	RXBUF[32]={0,};
static uint8_t index = 0;
uint8_t delay_cnt=10;
unsigned char result=0;//���ú����������Թ۲�
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
		if(result%2==1)//����
			result +=1;//ż��
		
		NRF_RNG->TASKS_START = 0; // stop the RNG peripheral.
		
		return result;
}

void init(void)//51822ʱ�ӳ�ʼ��
{ 
	 //�жϵ�Դʱ��״̬
	 if ((((*(uint32_t *)0xF0000FE0) & 0xFF) == 1) && (((*(uint32_t *)0xF0000FE4) & 0xF) == 0))
    {
        if ((((*(uint32_t *)0xF0000FE8) & 0xF0) == 0x0) && (((*(uint32_t *)0xF0000FEC) & 0xF0) == 0x0))
        {
					//���radio�޷�д��  
          *(uint32_t *)0x40000504 = 0xC007FFDF;//��Դʱ������
          *(uint32_t *)0x40006C18 = 0x00008000;//GPIOTE����    
        }
				else if ((((*(uint32_t *)0xF0000FE8) & 0xF0) == 0x10) && (((*(uint32_t *)0xF0000FEC) & 0xF0) == 0x0))
        {
          //���radio�޷�д��  
          *(uint32_t *)0x40000504 = 0xC007FFDF;//��Դʱ������
          *(uint32_t *)0x40006C18 = 0x00008000;//GPIOTE����  
        }
        else if ((((*(uint32_t *)0xF0000FE8) & 0xF0) == 0x30) && (((*(uint32_t *)0xF0000FEC) & 0xF0) == 0x0))
        {
          //���radio�޷�д��  
          *(uint32_t *)0x40000504 = 0xC007FFDF;//��Դʱ������
          *(uint32_t *)0x40006C18 = 0x00008000;//GPIOTE����    
        }
    }
	NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;//HFCLK��������״̬0�رգ�1����
  NRF_CLOCK->TASKS_HFCLKSTART = 1;//���⾧��HFCLK 0�رգ�1����
  while(!NRF_CLOCK->EVENTS_HFCLKSTARTED);//�ȴ�HFCLK��������״̬Ϊ1
}


//packet[3]��QxC(Q��ʼ��C������x������char)
static uint8_t volatile packet[32];  //< ���ݴ���Ļ����packet[1]������
//nrf51822????
uint8_t NRF51_TX()//��UART���յ����� Ȼ��ͨ��Radio���͵���λ��
{   
  uint8_t i=0;  
	LED0_Close();
	//����׼������
	NRF_RADIO->EVENTS_READY = 0U;//�շ�ģʽ�л���ɱ�־λ����λ
	NRF_RADIO->TASKS_TXEN = 1U;//�������ߵ�Ϊ����ģʽ
	
	for(i=0;i<32;i++)
	packet[i] = TXBUF[i];
	
  while (NRF_RADIO->EVENTS_READY == 0U);//�ȴ��շ�ģʽ�л����
			// �������ݴ���
	NRF_RADIO->EVENTS_END  = 0U;//���������־λ����λ    
	NRF_RADIO->TASKS_START = 1U;//��ʼ����		
	while(NRF_RADIO->EVENTS_END == 0U) //�ȴ��������
	{ 					
			;//
	}
	//nrf_gpio_pin_clear(18);//led0�𣬱�ʾ�����Ѿ�����
	NRF_RADIO->EVENTS_DISABLED = 0U;//����رձ�־λ  ��λ
	NRF_RADIO->TASKS_DISABLE   = 1U; // �رմ���
	while(NRF_RADIO->EVENTS_DISABLED == 0U)//�ȴ�����ر�
	{
			// Do nothing.
	}
	
  	return 1;
}
//nrf51822????
uint8_t NRF51_RX()//��Radio���յ����� Ȼ��ͨ��UART���͵���λ��
{
	uint32_t  RF_timeOver_flag = 0;
	uint8_t i=0;
	bool  CRC_SUCCESS = 0 ;
	NRF_RADIO->EVENTS_READY = 0U; //�շ�ģʽת�����  ��־λ       
	NRF_RADIO->TASKS_RXEN   = 1U; //����ģʽ
	while(NRF_RADIO->EVENTS_READY == 0U) //�ȴ��շ�ģʽת�����(����ģʽ)��־λ
	{
	// Do nothing.�ȴ�
	}
	NRF_RADIO->EVENTS_END  = 0U;//�������  ��־λ     
	NRF_RADIO->TASKS_START = 1U; // ��ʼ����

	while(NRF_RADIO->EVENTS_END == 0U)//�ȴ��������  ��־λ
	{
		// Do nothing.�ȴ�
		RF_timeOver_flag++;
		nrf_delay_ms(1);
		if(RF_timeOver_flag > 1000)
		{
			break;
			//goto EER ;//�����ʱ��û�н��յ��������ݰ�������whileѭ��������������Ӱ�������������У�����
		}
	}
	for(i=0;i<32;i++)
	{
	 RXBUF[i]=packet[i];
		packet[i]=0;
	}
	if (NRF_RADIO->CRCSTATUS == 1U)//���CRCУ����ȷ
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
	NRF_RADIO->EVENTS_DISABLED = 0U;//���߹ر�   ��־λ
	NRF_RADIO->TASKS_DISABLE   = 1U;// �ر������豸
	while(NRF_RADIO->EVENTS_DISABLED == 0U)//�ȴ��豸�ر�
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

void UART0_IRQHandler(void)//��UART���յ����ݺ� 
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
#define FQ_CHK_EN 0  //0:����Ƶ  1:��Ƶ
int main(void)//main������ʼ
{
	uint8_t FQ = 24;
	uint8_t led_Blink_Cnt=0;
	init();//ʱ�ӳ�ʼ��
	LED_Init();//���ų�ʼ��
	uart_init();
	radio_configure();//���ߵ����ã�Ҳ�����ں�nrf2401ͨѶ��
//#if FQ_CHK_EN
//	nrf_adc_init_vbat();
//  FQ =Get_RNG();//�����
//#else 
	FQ = 24;
//#endif
	// ������Ч�غ�ָ��
	NRF_RADIO->PACKETPTR    = (uint32_t)packet;//ָ��ָ�����ݻ���packet
	NRF_RADIO->FREQUENCY =(uint32_t)(FQ);//����Ƶ��  2424M
	while(1)
	{
		EXIT:
		LED0_Close();
		LED1_Open(); 
//		#if FQ_CHK_EN
//		TXBUF[0]=0xAA;//��ͷ
//		TXBUF[1]=0XFF;
//		TXBUF[2]=FQ;//Ƶ��
//		for(uint8_t i=3;i<32;i++)//����λ���0
//		{
//					 TXBUF[i]=FQ ;
//		}
//		while(1)//����Ƶ�����ݣ���Ƶ��
//		{
//			NRF51_TX();//����Ƶ��				
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
//			if(!NRF51_RX())//�н���,˵����Ƶ�ɹ����˳�����ģʽ��Ϊ����ģʽ
//				break;
//		}
//		#endif
		while(1)
		{
			NRF51_RX();//��Radio���յ����� Ȼ��ͨ��UART���͵���λ��
			//UART_To_Radio();//UART ���յ����ݷ��͵�Radio
			if(link_num ==0)
			{
			 goto EXIT;
			}
		}
	}   
}

