#ifndef HOOP_H
#define HOOP_H
#include <stdint.h>
namespace HOOP 
{
	#define HOOP_LENGTH 1000
	#define PAYLOAD_LENGTH 25
	enum HOOP_STATUS
	{
		HOOPING =0,
		LOSEONE =1,
		LOSETWO =2,
		LOSETHREE =3,
		LOSEFOUR =4,
		LOSEFIVE =5,
		NEEDSYNC=6,
	};
	enum RFMODE
	{
		TXMODE=0,
		RXMODE=1,
	};
	enum CHANNEL_STATUS
	{
		HOOP_CHANNEL=0,
		SYNC_CHANNEL=1,
	};
	class NRF_HOOP
	{
	private:
		int next_hoop;
		int hoop_channel;
		int sync_channel;
		int lose_count;
		int interval_2ms;
		int interval_4ms;
		int interval_1s;
		HOOP_STATUS status;
		unsigned char * hoop_sequence;
		unsigned char * sync_sequence;
		unsigned char * package_buf;
	
	
		//update channel:
		 void update_channel(CHANNEL_STATUS sta);
		//reset time interval counter
		 void reset_interval_2ms();
		 void reset_interval_4ms();
	
		int store_channel [1000];
		int store_hoop[1000];
		int latest_hoop[2];
	
		
		int valid_data_cnt,valid_loseone_cnt,valid_losetwo_cnt,valid_losethree_cnt,valid_losefour_cnt,valid_losefive_cnt,valid_sync_cnt,unuse_wait,total_cnt,total_valid;
		float miss_rate;
		

		
		/* 32B 
	datalength：
			|| 随机种子HSB || 随机种子LSB || 当前频点HSB || 当前频点LSB || 下一个频点HSB || 下一个频点LSB || 载荷 0 || ... || 载荷 26 || 校验字节 ||
	sync package:
			|| 随机种子HSB || 随机种子LSB ||     0xAA    ||     0xAA    || 下一个频点HSB || 下一个频点LSB || 载荷 0 || ... || 载荷 26 || 校验字节 ||
	*/
	public:
		int seed;
	//for debug:
		int delt_time;
	
		void init_hoop(int seed,unsigned char * hoop_sequence,int hoop_length,unsigned char * sync_sequence,int sync_length,unsigned char * package_buf,RFMODE  mode);

		void rx_loop(void) ;

	/** should be private:  **/
		//change nrf channel
		void set_channel(int channel);
		//function pointer:
		//tx send function:
		unsigned char (*rf_send_function)(unsigned char * txbuf);
		unsigned char (*rf_receive_function)(unsigned char * rxbuf);
		void (*reset_timer)(void);
	
		/*for rx mode */
		void set_rx_callback(unsigned char (*rf_receive_function)(unsigned char * rxbuf));
		bool receive_nrf_package(unsigned char * return_data_package = 0 );
		bool check_sum(unsigned char * package_buf);
		HOOP_STATUS get_status(void) ;		

	/** end  **/

		/* for tx mode */
		void set_tx_callback(unsigned char (*rf_send_function)(unsigned char * txbuf));
		void tx_loop(unsigned char * tx_data);

		//set time for timer
		void set_interval2ms(bool value);
		void set_interval4ms(bool value);
		void set_interval1s(bool value);
		
		void set_Reset_Callback(void (*reset_timer)(void));
		
		void test_rx_loop();
		void test_tx_loop();
	};
}

#endif
