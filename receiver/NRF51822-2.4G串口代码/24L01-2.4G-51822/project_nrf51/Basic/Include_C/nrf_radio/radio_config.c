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
#include <stdint.h>//������֪��С����������ʾ����������
#include "nrf51.h"//����51822�����Ĵ����ṹ��ַ
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

// ���ߵ�ַ����
//��ַһ��40��add_bit��add0~add39
//��2401��ַadd0��д(add0,add1,add2......add39)

//��2401��51822�ĵ�ַ��ϵ����
// 2401[add0 :add7] =PREFIX0[bit7:bit0](add7,add6,add5,add4,add3,add2,add1,add0)
// 2401[add8 :add15]=BASE0[bit31:bit24](add15,add14,add13,add12,add11,add10,add9,add8)
// 2401[add16:add23]=BASE0[bit23:bit16](add23,add22,add21,add20,add19,add18,add17,add16)
// 2401[add24:add31]=BASE0[bit15:bit8] (add31,add30,add29,add28,add27,add26,add25,add24)
// 2401[add32:add39]=BASE0[bit7 :bit0] (add39,add38,add37,add36,add35,add34,add33,add32)

//���Ͻ��۵ó���2401��ַ��ÿ8��bit��β�Ե��������ǹٷ��õĵ�ַ
//const u8  ip[6]={0xE7,0xE7,0xE7,0xE7,0xE7,0x00};//�����ַ  ��[bit0~7] [bit8~15] [bit16~23] [bit24~31] [bit32~39]
//const u8 ip0[6]={0xE7,0xE7,0xE7,0xE7,0xE7,0x00};//���յ�ַ0 ��51822ͨѶ�ҵ���(bit0��д)

//const u8 ip1[6]={0xC2,0xC2,0xC2,0xC2,0xC2,0x00};//���յ�ַ1 bit39~bit8Ϊ"0x43"(bit0��д)

//const u8 ip2[6]={0xC3,0x00};//���յ�ַ2 bit39~8��ͨ��1����,ֻдbit0~bit7
//const u8 ip3[6]={0xC4,0x00};//���յ�ַ3	ͨ��2345ֻ������char
//const u8 ip4[6]={0xC5,0x00};//���յ�ַ4	д��ʱ��˳����bit0~bit39
//const u8 ip5[6]={0xC6,0x00};//���յ�ַ5		ֻ��bit0~7���á����յ�ַ6��0xC7 ���յ�ַ7��0xC8



//���߹���4�ֱ���2424MHz��ͨ��0��ַFUCK0��ͨ��1~7��ַFUCK 1~7
//����2MHz�������ֽ�3�ֽ�QXC(Q��ʼ��C������x������char)
void radio_configure()//�������ã�׼����nrf24L01ͨѶ
{
  //���߹���04��+4�ֱ���0��0�ֱ���FC��-4�ֱ���F8��-8�ֱ�
	//    F4��-12�ֱ���F0��-16�ֱ���EC��-20�ֱ���D8��-30�ֱ�
	NRF_RADIO->TXPOWER = (0x04<<0);//���߹���4�ֱ�
  
	NRF_RADIO->FREQUENCY = 0UL;//����Ƶ��24MHz+2400MHz=2424MHz

	//�������ʣ�00��1Mbit��01��2Mbit��02��250Kbit��03��1Mbit��������
	NRF_RADIO->MODE = (01<<0);//����2MHz

	// ���ߵ�ַ��ϵ����
// * 51822[31:24] = 2401[24:31] 
// * 51822[23:16] = 2401[16:23]
// * 51822[15:8]  = 2401[8:15]
// * 51822[7:0]   = 2401[0:7]
// ͨ��3 �� 0 �ĵ��ֽ�
	NRF_RADIO->PREFIX0 =(0x23<<24) // ͨ��3�ĵ��ֽڵ�ַC4 1100 0100
	                   |(0xC3<<16) // ͨ��2�ĵ��ֽڵ�ַC3 1100 0011
	                   |(0x43<< 8) // ͨ��1�ĵ��ֽڵ�ַC2 1100 0010  0XC2ͷβ��������0X43
	                   |(0xE7<< 0);// ͨ��0�ĵ��ֽڵ�ַE7 1110 0111
// ͨ��7 �� 4 �ĵ��ֽڵ�ַ
	NRF_RADIO->PREFIX1 =(0x13<<24) // ͨ��7�ĵ��ֽڵ�ַC8 1100 1000
	                   |(0xE3<<16) // ͨ��6�ĵ��ֽڵ�ַC7 1100 0111
	                   |(0x63<< 8) // ͨ��5�ĵ��ֽڵ�ַC6 1100 0110
	                   |(0xA3<< 0);// ͨ��4�ĵ��ֽڵ�ַC5 1100 0101
 
  NRF_RADIO->BASE0   = (0xE7<<24)  // ͨ��0�ĸ��ֽ�E7
                      |(0xE7<<16)
	                    |(0xE7<<8)
	                    |(0xE7<<0);

  NRF_RADIO->BASE1   = (0x43<<24)  // ͨ��1-7�ĸ��ֽ�C2 1100 0010
                      |(0x43<<16)
	                    |(0x43<<8)
	                    |(0x43<<0);

  NRF_RADIO->TXADDRESS = 0x01UL;      // TX����ʹ�õ�ͨ���ţ�0ͨ��  2401��Ҫ����0ͨ��ΪRX����ͨ��
  NRF_RADIO->RXADDRESSES = 0x02UL;    // RX���յ�ͨ���ţ�1ͨ��      2401��Ҫ����1ͨ��ΪTX����ͨ��

  // ���ð�0������
  NRF_RADIO->PCNF0 = (0<<16)| //S1����ĳ���
                     (0<<8) | //S0���ĳ���
                     (0<<0);  //�����ֶ��еı�����

  // ���ð�1������
   NRF_RADIO->PCNF1 = (0<<25)| //Ч��λ��0�أ�1����
                      (1<<24)| //���ݴ�С�ˣ��ߵ��ֽ��ĸ��ȷ� 0���ֽڣ�1���ֽڣ�
                      (4<<16)| //ͨ��1~7���ֽڳ��ȣ� nrf24�ߵ��ֽ�5�ֽڣ�4����+1���ͣ�
                      (32<<8)| //�����ֽڳ��ȣ�255~1��32�ֽ�QxC
                      (32<<0);  //Ӳ�������ֽڳ��ȣ�255~1��32�ֽ�QxC

  // CRC У�鳤������
  NRF_RADIO->CRCCNF = 2; // У�鳤�� 2��char
  if ((NRF_RADIO->CRCCNF & 0x03)== 2 )
  {
    NRF_RADIO->CRCINIT = 0xFFFFUL;      // У���ʼֵ
    NRF_RADIO->CRCPOLY = 0x11021UL;     // CRC poly: x^16+x^12^x^5+1
  }
  else if ((NRF_RADIO->CRCCNF & 0x03) == 1 )
  {
    NRF_RADIO->CRCINIT = 0xFFUL;        // У���ʼֵ
    NRF_RADIO->CRCPOLY = 0x107UL;       // CRC poly: x^8+x^2^x^1+1
  }
	
//���ռĴ�����  NRF_RADIO->PACKETPTR	
}

/** 
 * @}
 */
