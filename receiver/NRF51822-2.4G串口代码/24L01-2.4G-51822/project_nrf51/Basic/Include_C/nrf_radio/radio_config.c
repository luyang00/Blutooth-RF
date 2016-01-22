/* Copyright (c) 2009 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */
/** @file
* @addtogroup nrf_dev_radio_rx_example_main nrf_dev_radio_tx_example_main
* @{
*/
 
#include "radio_config.h"
#include <stdint.h>//声明已知大小的整数或显示特征的整数
#include "nrf51.h"//包含51822各个寄存器结构地址
//#include "nrf_delay.h"

/* These are set to zero as Shockburst packets don't have corresponding fields. */
#define PACKET_S1_FIELD_SIZE             (0UL)  /**< Packet S1 field size in bits. */
#define PACKET_S0_FIELD_SIZE             (0UL)  /**< Packet S0 field size in bits. */
#define PACKET_LENGTH_FIELD_SIZE         (0UL)  /**< Packet length field size in bits. */

/**
 * @brief Function for swapping/mirroring bits in a byte.
 * 
 *@verbatim
 * output_bit_7 = input_bit_0
 * output_bit_6 = input_bit_1
 *           :
 * output_bit_0 = input_bit_7
 *@endverbatim
 *
 * @param[in] inp is the input byte to be swapped.
 *
 * @return
 * Returns the swapped/mirrored input byte.
 */
static uint32_t swap_bits(uint32_t inp);

/**
 * @brief Function for swapping bits in a 32 bit word for each byte individually.
 * 
 * The bits are swapped as follows:
 * @verbatim
 * output[31:24] = input[24:31] 
 * output[23:16] = input[16:23]
 * output[15:8]  = input[8:15]
 * output[7:0]   = input[0:7]
 * @endverbatim
 * @param[in] input is the input word to be swapped.
 *
 * @return
 * Returns the swapped input byte.
 */
static uint32_t bytewise_bitswap(uint32_t inp);

static uint32_t swap_bits(uint32_t inp)
{
    uint32_t i;
    uint32_t retval = 0;
    
    inp = (inp & 0x000000FFUL);
    
    for(i = 0; i < 8; i++)
    {
        retval |= ((inp >> i) & 0x01) << (7 - i);     
    }
    
    return retval;    
}


static uint32_t bytewise_bitswap(uint32_t inp)
{
      return (swap_bits(inp >> 24) << 24)
           | (swap_bits(inp >> 16) << 16)
           | (swap_bits(inp >> 8) << 8)
           | (swap_bits(inp));
}

// 无线地址设置
//地址一共40个add_bit是add0~add39
//在2401地址add0先写(add0,add1,add2......add39)

//那2401和51822的地址关系如下
// 2401[add0 :add7] =PREFIX0[bit7:bit0](add7,add6,add5,add4,add3,add2,add1,add0)
// 2401[add8 :add15]=BASE0[bit31:bit24](add15,add14,add13,add12,add11,add10,add9,add8)
// 2401[add16:add23]=BASE0[bit23:bit16](add23,add22,add21,add20,add19,add18,add17,add16)
// 2401[add24:add31]=BASE0[bit15:bit8] (add31,add30,add29,add28,add27,add26,add25,add24)
// 2401[add32:add39]=BASE0[bit7 :bit0] (add39,add38,add37,add36,add35,add34,add33,add32)

//以上结论得出的2401地址是每8个bit首尾对调。以下是官方用的地址
//const u8  ip[6]={0xE7,0xE7,0xE7,0xE7,0xE7,0x00};//发射地址  从[bit0~7] [bit8~15] [bit16~23] [bit24~31] [bit32~39]
//const u8 ip0[6]={0xE7,0xE7,0xE7,0xE7,0xE7,0x00};//接收地址0 和51822通讯我调了(bit0先写)

//const u8 ip1[6]={0xC2,0xC2,0xC2,0xC2,0xC2,0x00};//接收地址1 bit39~bit8为"0x43"(bit0先写)

//const u8 ip2[6]={0xC3,0x00};//接收地址2 bit39~8和通道1共用,只写bit0~bit7
//const u8 ip3[6]={0xC4,0x00};//接收地址3	通道2345只能设置char
//const u8 ip4[6]={0xC5,0x00};//接收地址4	写的时候顺序是bit0~bit39
//const u8 ip5[6]={0xC6,0x00};//接收地址5		只有bit0~7有用。接收地址6：0xC7 接收地址7：0xC8



//无线功率4分贝，2424MHz，通道0地址FUCK0，通道1~7地址FUCK 1~7
//速率2MHz，数据字节3字节QXC(Q开始，C结束，x是数据char)
void radio_configure()//无线配置，准备和nrf24L01通讯
{
  //无线功率04：+4分贝，0：0分贝，FC：-4分贝，F8：-8分贝
	//    F4：-12分贝，F0：-16分贝，EC：-20分贝，D8：-30分贝
	NRF_RADIO->TXPOWER = (0x04<<0);//无线功率4分贝
  
	NRF_RADIO->FREQUENCY = 0UL;//无线频率24MHz+2400MHz=2424MHz

	//无线速率：00：1Mbit，01：2Mbit，02：250Kbit，03：1Mbit（蓝牙）
	NRF_RADIO->MODE = (01<<0);//速率2MHz

	// 无线地址关系设置
// * 51822[31:24] = 2401[24:31] 
// * 51822[23:16] = 2401[16:23]
// * 51822[15:8]  = 2401[8:15]
// * 51822[7:0]   = 2401[0:7]
// 通道3 到 0 的低字节
	NRF_RADIO->PREFIX0 =(0x23<<24) // 通道3的低字节地址C4 1100 0100
	                   |(0xC3<<16) // 通道2的低字节地址C3 1100 0011
	                   |(0x43<< 8) // 通道1的低字节地址C2 1100 0010  0XC2头尾调换后是0X43
	                   |(0xE7<< 0);// 通道0的低字节地址E7 1110 0111
// 通道7 到 4 的低字节地址
	NRF_RADIO->PREFIX1 =(0x13<<24) // 通道7的低字节地址C8 1100 1000
	                   |(0xE3<<16) // 通道6的低字节地址C7 1100 0111
	                   |(0x63<< 8) // 通道5的低字节地址C6 1100 0110
	                   |(0xA3<< 0);// 通道4的低字节地址C5 1100 0101
 
  NRF_RADIO->BASE0   = (0xE7<<24)  // 通道0的高字节E7
                      |(0xE7<<16)
	                    |(0xE7<<8)
	                    |(0xE7<<0);

  NRF_RADIO->BASE1   = (0x43<<24)  // 通道1-7的高字节C2 1100 0010
                      |(0x43<<16)
	                    |(0x43<<8)
	                    |(0x43<<0);

  NRF_RADIO->TXADDRESS = 0x01UL;      // TX发射使用的通道号：0通道  2401需要设置0通道为RX接收通道
  NRF_RADIO->RXADDRESSES = 0x02UL;    // RX接收的通道号：1通道      2401需要设置1通道为TX发送通道

  // 配置包0的设置
  NRF_RADIO->PCNF0 = (0<<16)| //S1领域的长度
                     (0<<8) | //S0场的长度
                     (0<<0);  //长度字段中的比特数

  // 配置包1的设置
   NRF_RADIO->PCNF1 = (0<<25)| //效验位（0关，1开）
                      (1<<24)| //数据大小端（高低字节哪个先发 0低字节，1高字节）
                      (4<<16)| //通道1~7高字节长度（ nrf24高低字节5字节：4个高+1个低）
                      (32<<8)| //数据字节长度（255~1）32字节QxC
                      (32<<0);  //硬件传输字节长度（255~1）32字节QxC

  // CRC 校验长度配置
  NRF_RADIO->CRCCNF = 2; // 校验长度 2个char
  if ((NRF_RADIO->CRCCNF & 0x03)== 2 )
  {
    NRF_RADIO->CRCINIT = 0xFFFFUL;      // 校验初始值
    NRF_RADIO->CRCPOLY = 0x11021UL;     // CRC poly: x^16+x^12^x^5+1
  }
  else if ((NRF_RADIO->CRCCNF & 0x03) == 1 )
  {
    NRF_RADIO->CRCINIT = 0xFFUL;        // 校验初始值
    NRF_RADIO->CRCPOLY = 0x107UL;       // CRC poly: x^8+x^2^x^1+1
  }
	
//接收寄存器是  NRF_RADIO->PACKETPTR	
}

/** 
 * @}
 */
