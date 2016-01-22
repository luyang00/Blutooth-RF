#include "24l01.h"
#include "spi.h"
//////////////////////////////////////////////////////////////////////////////////	 
//±¾³ÌĞòÖ»¹©Ñ§Ï°Ê¹ÓÃ£¬Î´¾­×÷ÕßĞí¿É£¬²»µÃÓÃÓÚÆäËüÈÎºÎÓÃÍ¾
//ALIENTEK STM32F407¿ª·¢°å
//NRF24L01Çı¶¯´úÂë	   
//ÕıµãÔ­×Ó@ALIENTEK
//¼¼ÊõÂÛÌ³:www.openedv.com
//ĞŞ¸ÄÈÕÆÚ:2014/5/9
//°æ±¾£ºV1.0
//°æÈ¨ËùÓĞ£¬µÁ°æ±Ø¾¿¡£
//Copyright(C) ¹ãÖİÊĞĞÇÒíµç×Ó¿Æ¼¼ÓĞÏŞ¹«Ë¾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
    
const u8 TX_ADDRESS[TX_ADR_WIDTH]={0xE7,0xE7,0xE7,0xE7,0xE7}; //·¢ËÍµØÖ·
const u8 RX_ADDRESS[RX_ADR_WIDTH]={0xC2,0xC2,0xC2,0xC2,0xC2}; //·¢ËÍµØÖ·

void NRF24L01_SPI_Init(void)
{
	
 	SPI_InitTypeDef  SPI_InitStructure;
	
	SPI_Cmd(SPI1, DISABLE); //Ê§ÄÜSPIÍâÉè
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //ÉèÖÃSPIµ¥Ïò»òÕßË«ÏòµÄÊı¾İÄ£Ê½:SPIÉèÖÃÎªË«ÏßË«ÏòÈ«Ë«¹¤
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//ÉèÖÃSPI¹¤×÷Ä£Ê½:ÉèÖÃÎªÖ÷SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//ÉèÖÃSPIµÄÊı¾İ´óĞ¡:SPI·¢ËÍ½ÓÊÕ8Î»Ö¡½á¹¹
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		//´®ĞĞÍ¬²½Ê±ÖÓµÄ¿ÕÏĞ×´Ì¬ÎªµÍµçÆ½
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	//´®ĞĞÍ¬²½Ê±ÖÓµÄµÚ1¸öÌø±äÑØ£¨ÉÏÉı»òÏÂ½µ£©Êı¾İ±»²ÉÑù
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSSĞÅºÅÓÉÓ²¼ş£¨NSS¹Ü½Å£©»¹ÊÇÈí¼ş£¨Ê¹ÓÃSSIÎ»£©¹ÜÀí:ÄÚ²¿NSSĞÅºÅÓĞSSIÎ»¿ØÖÆ
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//¶¨Òå²¨ÌØÂÊÔ¤·ÖÆµµÄÖµ:²¨ÌØÂÊÔ¤·ÖÆµÖµÎª256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//Ö¸¶¨Êı¾İ´«Êä´ÓMSBÎ»»¹ÊÇLSBÎ»¿ªÊ¼:Êı¾İ´«Êä´ÓMSBÎ»¿ªÊ¼
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCÖµ¼ÆËãµÄ¶àÏîÊ½
	SPI_Init(SPI1, &SPI_InitStructure);  //¸ù¾İSPI_InitStructÖĞÖ¸¶¨µÄ²ÎÊı³õÊ¼»¯ÍâÉèSPIx¼Ä´æÆ÷
 
	SPI_Cmd(SPI1, ENABLE); //Ê¹ÄÜSPIÍâÉè
	
}
 
//³õÊ¼»¯24L01µÄIO¿Ú
void NRF24L01_Init(void)
{  
	GPIO_InitTypeDef  GPIO_InitStructure;


	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOA, ENABLE);//Ê¹ÄÜGPIOB,GÊ±ÖÓ
	
//  //GPIOB14³õÊ¼»¯ÉèÖÃ:ÍÆÍìÊä³ö
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//ÆÕÍ¨Êä³öÄ£Ê½
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//ÍÆÍìÊä³ö
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//ÉÏÀ­
//  GPIO_Init(GPIOB, &GPIO_InitStructure);//³õÊ¼»¯PB14
	
	//GPIOG6,7ÍÆÍìÊä³ö
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//ÆÕÍ¨Êä³öÄ£Ê½
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//ÍÆÍìÊä³ö
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//ÉÏÀ­
  GPIO_Init(GPIOC, &GPIO_InitStructure);//³õÊ¼»¯PG6,7
	
	//GPIOG.8ÉÏÀ­ÊäÈë
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//ÊäÈë
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//ÉÏÀ­
  GPIO_Init(GPIOA, &GPIO_InitStructure);//³õÊ¼»¯PG8

//  GPIO_SetBits(GPIOB,GPIO_Pin_14);//PB14Êä³ö1,·ÀÖ¹SPI FLASH¸ÉÈÅNRFµÄÍ¨ĞÅ 
  
 	SPI1_Init();    		//³õÊ¼»¯SPI1  
	
	NRF24L01_SPI_Init();//Õë¶ÔNRFµÄÌØµãĞŞ¸ÄSPIµÄÉèÖÃ

	NRF24L01_CE_LOW; 			//Ê¹ÄÜ24L01
	NRF24L01_CSN_HIGH;			//SPIÆ¬Ñ¡È¡Ïû	 		 	 
}
//¼ì²â24L01ÊÇ·ñ´æÔÚ
//·µ»ØÖµ:0£¬³É¹¦;1£¬Ê§°Ü	
u8 NRF24L01_Check(void)
{
	u8 buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
	u8 i;
	SPI1_SetSpeed(SPI_BaudRatePrescaler_8); //spiËÙ¶ÈÎª10.5Mhz£¨24L01µÄ×î´óSPIÊ±ÖÓÎª10Mhz£©   	 
	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,buf,5);//Ğ´Èë5¸ö×Ö½ÚµÄµØÖ·.	
	NRF24L01_Read_Buf(TX_ADDR,buf,5); //¶Á³öĞ´ÈëµÄµØÖ·  
	for(i=0;i<5;i++)if(buf[i]!=0XA5)break;	 							   
	if(i!=5)return 1;//¼ì²â24L01´íÎó	
	return 0;		 //¼ì²âµ½24L01
}	 	 
//SPIĞ´¼Ä´æÆ÷
//reg:Ö¸¶¨¼Ä´æÆ÷µØÖ·
//value:Ğ´ÈëµÄÖµ
u8 NRF24L01_Write_Reg(u8 reg,u8 value)
{
	u8 status;	
  NRF24L01_CSN_LOW;                 //Ê¹ÄÜSPI´«Êä
  status =SPI1_ReadWriteByte(reg);//·¢ËÍ¼Ä´æÆ÷ºÅ 
  SPI1_ReadWriteByte(value);      //Ğ´Èë¼Ä´æÆ÷µÄÖµ
  NRF24L01_CSN_HIGH;                 //½ûÖ¹SPI´«Êä	   
  return(status);       			//·µ»Ø×´Ì¬Öµ
}
//¶ÁÈ¡SPI¼Ä´æÆ÷Öµ
//reg:Òª¶ÁµÄ¼Ä´æÆ÷
u8 NRF24L01_Read_Reg(u8 reg)
{
	u8 reg_val;	    
  NRF24L01_CSN_LOW;          //Ê¹ÄÜSPI´«Êä		
  SPI1_ReadWriteByte(reg);   //·¢ËÍ¼Ä´æÆ÷ºÅ
  reg_val=SPI1_ReadWriteByte(0XFF);//¶ÁÈ¡¼Ä´æÆ÷ÄÚÈİ
  NRF24L01_CSN_HIGH;          //½ûÖ¹SPI´«Êä		    
  return(reg_val);           //·µ»Ø×´Ì¬Öµ
}	
//ÔÚÖ¸¶¨Î»ÖÃ¶Á³öÖ¸¶¨³¤¶ÈµÄÊı¾İ
//reg:¼Ä´æÆ÷(Î»ÖÃ)
//*pBuf:Êı¾İÖ¸Õë
//len:Êı¾İ³¤¶È
//·µ»ØÖµ,´Ë´Î¶Áµ½µÄ×´Ì¬¼Ä´æÆ÷Öµ 
u8 NRF24L01_Read_Buf(u8 reg,u8 *pBuf,u8 len)
{
  u8 status,u8_ctr;	       
  NRF24L01_CSN_LOW;           //Ê¹ÄÜSPI´«Êä
  status=SPI1_ReadWriteByte(reg);//·¢ËÍ¼Ä´æÆ÷Öµ(Î»ÖÃ),²¢¶ÁÈ¡×´Ì¬Öµ   	   
  for(u8_ctr=0;u8_ctr<len;u8_ctr++)pBuf[u8_ctr]=SPI1_ReadWriteByte(0XFF);//¶Á³öÊı¾İ
  NRF24L01_CSN_HIGH;       //¹Ø±ÕSPI´«Êä
  return status;        //·µ»Ø¶Áµ½µÄ×´Ì¬Öµ
}
//ÔÚÖ¸¶¨Î»ÖÃĞ´Ö¸¶¨³¤¶ÈµÄÊı¾İ
//reg:¼Ä´æÆ÷(Î»ÖÃ)
//*pBuf:Êı¾İÖ¸Õë
//len:Êı¾İ³¤¶È
//·µ»ØÖµ,´Ë´Î¶Áµ½µÄ×´Ì¬¼Ä´æÆ÷Öµ
u8 NRF24L01_Write_Buf(u8 reg, u8 *pBuf, u8 len)
{
  u8 status,u8_ctr;	    
 	NRF24L01_CSN_LOW;          //Ê¹ÄÜSPI´«Êä
  status = SPI1_ReadWriteByte(reg);//·¢ËÍ¼Ä´æÆ÷Öµ(Î»ÖÃ),²¢¶ÁÈ¡×´Ì¬Öµ
  for(u8_ctr=0; u8_ctr<len; u8_ctr++)SPI1_ReadWriteByte(*pBuf++); //Ğ´ÈëÊı¾İ	 
  NRF24L01_CSN_HIGH;       //¹Ø±ÕSPI´«Êä
  return status;          //·µ»Ø¶Áµ½µÄ×´Ì¬Öµ
}				   
//Æô¶¯NRF24L01·¢ËÍÒ»´ÎÊı¾İ
//txbuf:´ı·¢ËÍÊı¾İÊ×µØÖ·
//·µ»ØÖµ:·¢ËÍÍê³É×´¿ö
u8 NRF24L01_TxPacket(u8 *txbuf)
{
	u8 sta;
 	SPI1_SetSpeed(SPI_BaudRatePrescaler_8);//spiËÙ¶ÈÎª10.5Mhz£¨24L01µÄ×î´óSPIÊ±ÖÓÎª10Mhz£©   
	NRF24L01_CE_LOW;
	NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//Ğ´Êı¾İµ½TX BUF  32¸ö×Ö½Ú
 	NRF24L01_CE_HIGH;//Æô¶¯·¢ËÍ	   
	//while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15)!=0);//µÈ´ı·¢ËÍÍê³É //read IRQ
	sta=NRF24L01_Read_Reg(STATUS);  //¶ÁÈ¡×´Ì¬¼Ä´æÆ÷µÄÖµ	   
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //Çå³ıTX_DS»òMAX_RTÖĞ¶Ï±êÖ¾
	if(sta&MAX_TX)//´ïµ½×î´óÖØ·¢´ÎÊı
	{
		NRF24L01_Write_Reg(FLUSH_TX,0xff);//Çå³ıTX FIFO¼Ä´æÆ÷ 
		return MAX_TX; 
	}
	if(sta&TX_OK)//·¢ËÍÍê³É
	{
		return TX_OK;
	}
	return 0xff;//ÆäËûÔ­Òò·¢ËÍÊ§°Ü
}
//Æô¶¯NRF24L01·¢ËÍÒ»´ÎÊı¾İ
//txbuf:´ı·¢ËÍÊı¾İÊ×µØÖ·
//·µ»ØÖµ:0£¬½ÓÊÕÍê³É£»ÆäËû£¬´íÎó´úÂë
u8 NRF24L01_RxPacket(u8 *rxbuf)
{
	u8 sta;		    							   
	SPI1_SetSpeed(SPI_BaudRatePrescaler_8); //spiËÙ¶ÈÎª10.5Mhz£¨24L01µÄ×î´óSPIÊ±ÖÓÎª10Mhz£©   
	sta=NRF24L01_Read_Reg(STATUS);  //¶ÁÈ¡×´Ì¬¼Ä´æÆ÷µÄÖµ    	 
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //Çå³ıTX_DS»òMAX_RTÖĞ¶Ï±êÖ¾
	if(sta&RX_OK)//½ÓÊÕµ½Êı¾İ
	{
		NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//¶ÁÈ¡Êı¾İ
		NRF24L01_Write_Reg(FLUSH_RX,0xff);//Çå³ıRX FIFO¼Ä´æÆ÷ 
		return 0; 
	}	   
	return 1;//Ã»ÊÕµ½ÈÎºÎÊı¾İ
}					    
//¸Ãº¯Êı³õÊ¼»¯NRF24L01µ½RXÄ£Ê½
//ÉèÖÃRXµØÖ·,Ğ´RXÊı¾İ¿í¶È,Ñ¡ÔñRFÆµµÀ,²¨ÌØÂÊºÍLNA HCURR
//µ±CE±ä¸ßºó,¼´½øÈëRXÄ£Ê½,²¢¿ÉÒÔ½ÓÊÕÊı¾İÁË		   
void NRF24L01_RX_Mode(void)
{
  NRF24L01_CE_LOW;	  
//  NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(u8*)RX_ADDRESS,RX_ADR_WIDTH);//Ğ´RX½ÚµãµØÖ·
//	  
//  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x00);    //²»Ê¹ÄÜÍ¨µÀ0µÄ×Ô¶¯Ó¦´ğ  Ê¹ÄÜ0x01   
//  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x03);//Ê¹ÄÜÍ¨µÀ0µÄ½ÓÊÕµØÖ·  	 
//  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,24);	     //ÉèÖÃRFÍ¨ĞÅÆµÂÊ		  
//  NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//Ñ¡ÔñÍ¨µÀ0µÄÓĞĞ§Êı¾İ¿í¶È 	
//  NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P1,RX_PLOAD_WIDTH);//Ñ¡ÔñÍ¨µÀ1µÄÓĞĞ§Êı¾İ¿í¶È     
//  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);//ÉèÖÃTX·¢Éä²ÎÊı,0dbÔöÒæ,2Mbps,µÍÔëÉùÔöÒæ¿ªÆô   
//  NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG, 0x0f);//ÅäÖÃ»ù±¾¹¤×÷Ä£Ê½µÄ²ÎÊı;PWR_UP,EN_CRC,16BIT_CRC,½ÓÊÕÄ£Ê½ 
	NRF24L01_Write_Reg(0x20+0x00,0x0f);//ÅäÖÃ½ÓÊÕµÄ»ù±¾²ÎÊı£¬PWR_UP,EN_CRC,16BIT_CRC,  ½ÓÊÕÄ£Ê½
		NRF24L01_Write_Reg(0x20+0x01,0x00);//Ó¦´ğÉèÖÃ 0X00´ú±í54321 ÎŞÓ¦´ğ
		NRF24L01_Write_Reg(0x20+0x02,0x03);//½ÓÊÕÍ¨µÀÊ¹ÄÜ 0X03´ú±í 0 1 ½ÓÊÕ
		NRF24L01_Write_Reg(0x20+0x06,0x00F);	//ÉèÖÃTX²ÎÊı 0DB 2MPBS µÍÔëÉùÔöÒæ¿ª
		
		NRF24L01_Write_Buf(0X20+0X10,(u8*)RX_ADDRESS,5);//Ğ´TX·¢ËÍµØÖ·ip1
		NRF24L01_Write_Buf(0X20+0X0a,(u8*)TX_ADDRESS,5);//Ğ´RX0½ÓÊÕµØÖ·
		//	NRF24L01_Write_Buf(0X20+0X0b,(u8*)ip1,5);//Ğ´RX1½ÓÊÕµØÖ·
		//	NRF24L01_Write_Buf(0X20+0X0c,(u8*)ip2,1);//Ğ´RX2½ÓÊÕµØÖ·
		//	NRF24L01_Write_Buf(0X20+0X0d,(u8*)ip3,1);//Ğ´RX3½ÓÊÕµØÖ· 
		//	NRF24L01_Write_Buf(0X20+0X0e,(u8*)ip4,1);//Ğ´RX4½ÓÊÕµØÖ·
		//	NRF24L01_Write_Buf(0X20+0X0f,(u8*)ip5,1);//Ğ´RX4½ÓÊÕµØÖ·
		
		NRF24L01_Write_Reg(0x20+0x11,32);//0Í¨µÀÓĞĞ§Êı¾İ¿í¶È
		NRF24L01_Write_Reg(0x20+0x12,32);//0Í¨µÀÓĞĞ§Êı¾İ¿í¶È 
		//	NRF24L01_Write_Reg(0x20+0x13,32);//0Í¨µÀÓĞĞ§Êı¾İ¿í¶È
		//	NRF24L01_Write_Reg(0x20+0x14,32);//0Í¨µÀÓĞĞ§Êı¾İ¿í¶È 
		//	NRF24L01_Write_Reg(0x20+0x15,32);//0Í¨µÀÓĞĞ§Êı¾İ¿í¶È
		//	NRF24L01_Write_Reg(0x20+0x16,32);//0Í¨µÀÓĞĞ§Êı¾İ¿íÈ	
		NRF24L01_Write_Reg(0x20+0x05,24); //RFÆµÂÊÉèÖÃ	
  NRF24L01_CE_HIGH; //CEÎª¸ß,½øÈë½ÓÊÕÄ£Ê½ 
}						 
//¸Ãº¯Êı³õÊ¼»¯NRF24L01µ½TXÄ£Ê½
//ÉèÖÃTXµØÖ·,Ğ´TXÊı¾İ¿í¶È,ÉèÖÃRX×Ô¶¯Ó¦´ğµÄµØÖ·,Ìî³äTX·¢ËÍÊı¾İ,Ñ¡ÔñRFÆµµÀ,²¨ÌØÂÊºÍLNA HCURR
//PWR_UP,CRCÊ¹ÄÜ
//µ±CE±ä¸ßºó,¼´½øÈëRXÄ£Ê½,²¢¿ÉÒÔ½ÓÊÕÊı¾İÁË		   
//CEÎª¸ß´óÓÚ10us,ÔòÆô¶¯·¢ËÍ.	 
void NRF24L01_TX_Mode(void)
{														 
	NRF24L01_CE_LOW;	    
  NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,(u8*)RX_ADDRESS,TX_ADR_WIDTH);//Ğ´TX½ÚµãµØÖ· 
  NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(u8*)TX_ADDRESS,RX_ADR_WIDTH); //ÉèÖÃTX½ÚµãµØÖ·,Ö÷ÒªÎªÁËÊ¹ÄÜACK	  

//  //NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x00);     //²»Ê¹ÄÜÍ¨µÀ0µÄ×Ô¶¯Ó¦´ğ  Ê¹ÄÜ0x01
//  //NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01); //Ê¹ÄÜÍ¨µÀ0µÄ½ÓÊÕµØÖ·  
//  NRF24L01_Write_Reg(NRF_WRITE_REG+SETUP_RETR,0x1a);//ÉèÖÃ×Ô¶¯ÖØ·¢¼ä¸ôÊ±¼ä:500us + 86us;×î´ó×Ô¶¯ÖØ·¢´ÎÊı:10´Î
//  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,0x24);       //ÉèÖÃRFÍ¨µÀÎª24(2424Mhz)
//  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);  //ÉèÖÃTX·¢Éä²ÎÊı,0dbÔöÒæ,2Mbps,µÍÔëÉùÔöÒæ¿ªÆô   
//  NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG,0x0e);    //ÅäÖÃ»ù±¾¹¤×÷Ä£Ê½µÄ²ÎÊı;PWR_UP,EN_CRC,16BIT_CRC,½ÓÊÕÄ£Ê½,¿ªÆôËùÓĞÖĞ¶Ï
	NRF24L01_Write_Reg(0x20+0x04,0xff);//×Ô¶¯ÖØ·¢
  	NRF24L01_Write_Reg(0x20+0x05,24);    
  	//NRF24L01_Write_Reg(0x20+0x06,0x0f);  
	NRF24L01_Write_Reg(0x20+0x00,0x0e);//·¢ËÍÄ£Ê½  
	NRF24L01_CE_HIGH;//CEÎª¸ß,10usºóÆô¶¯·¢ËÍ
}
void NRF24L01_ChangeChannel(unsigned char channel)
{
	NRF24L01_CE_LOW;
	//Delay100us(0);
	 NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,channel);       //ÉèÖÃRFÍ¨µÀÎª40
	NRF24L01_CE_HIGH;
}









