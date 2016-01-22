#include "hoop.h"
#include "SyncGen.h"
#include "24l01.h"
#include "string.h"
//#include "log.h"
#include "stdio.h"
namespace HOOP
{

	void NRF_HOOP::init_hoop(int seed,unsigned char * hoop_sequence,int hoop_length,unsigned char * sync_sequence,int sync_length,unsigned char * package_buf,RFMODE  mode)
	{
		this->package_buf = package_buf;
		this->hoop_sequence = hoop_sequence;
		this->sync_sequence = sync_sequence;
		this->seed = seed;
		hoop_sequence_gen(seed,hoop_sequence,hoop_length);
		sync_sequence_gen(seed,sync_sequence,sync_length);
		if(RXMODE == mode)
		{
			NRF24L01_RX_Mode();
			set_channel(hoop_sequence[hoop_channel]);
		}
		if(TXMODE == mode)
		{
			NRF24L01_TX_Mode();
			set_channel(hoop_sequence[hoop_channel]);
		}
	}
	
	void NRF_HOOP::set_tx_callback(unsigned char (*rf_send_function)(unsigned char * txbuf))
	{
		this->rf_send_function = rf_send_function;
	}
	void NRF_HOOP::set_rx_callback(unsigned char (*rf_receive_function)(unsigned char * rxbuf))
	{
		this->rf_receive_function = rf_receive_function;
	}
	void NRF_HOOP::set_Reset_Callback(void (*reset_timer)(void))
	{
		this->reset_timer=reset_timer;
		
	}
	void NRF_HOOP::update_channel(CHANNEL_STATUS sta)
	{
		if(sta==HOOP_CHANNEL)
		{
			if(hoop_channel<HOOP_LENGTH-1)
			{
				hoop_channel++;
			}
			else 
			{
				hoop_channel=0;
			}
			
		}
		if(sta==SYNC_CHANNEL)
		{
			if(sync_channel<2)sync_channel++; 
			else sync_channel=0;
		}
	}
	void NRF_HOOP::set_channel(int channel)
	{
		NRF24L01_ChangeChannel(channel);
	}
	
	bool NRF_HOOP::check_sum(unsigned char * package_buf)
	{
		u8 sum;
		if(seed == (package_buf[0]<<8) + package_buf[1])//check seed 
		{
			if((hoop_channel == (package_buf[2]<<8) + package_buf[3] ) || ( package_buf[3] == 0xAA))//check hoop channel is right
			{
				for(int i=0;i<31;i++)
					sum += (*package_buf+i);
				if(package_buf[31] == sum)return true;
			}
		}
		return false;
	}
	bool NRF_HOOP::receive_nrf_package(unsigned char * return_data_package)
	{
		bool get_data;
		for(int i=0;i<32;i++)
			*(package_buf+i)=0;
		get_data=!(rf_receive_function(package_buf)) ;//&& check_sum(package_buf)==true);
		if(return_data_package)return_data_package = package_buf;
		return get_data;
	}
	
	void NRF_HOOP::set_interval2ms(bool value)
	{
		interval_2ms = value;
	}
	void NRF_HOOP::reset_interval_2ms()
	{
		//to do 
	}
	
	void NRF_HOOP::set_interval4ms(bool value)
	{
		interval_4ms = value;
	}
	void NRF_HOOP::set_interval1s(bool value)
	{
		interval_1s = value;
	}
	void NRF_HOOP::reset_interval_4ms()
	{
		reset_timer();
	}
	void NRF_HOOP::tx_loop(unsigned char * tx_data)
	{
		u8 sum;
		static bool send_switch=true;
		if(interval_2ms == true)
		{
			interval_2ms =false;
			send_switch = !send_switch;
			if(send_switch)
			{
				GPIO_ResetBits(GPIOC,GPIO_Pin_4);
				set_channel(hoop_sequence[hoop_channel]);
				
				package_buf[0]=(unsigned char)(seed>>8);
				package_buf[1]=(unsigned char)(seed);
				package_buf[2]=(unsigned char)(hoop_channel>>8);
				package_buf[3]=(unsigned char) hoop_channel;
				if(hoop_channel+1<HOOP_LENGTH)
				{
					package_buf[4]=(unsigned char)((hoop_channel+1)>>8);
					package_buf[5]=(unsigned char)((hoop_channel+1));
				}
				else 
				{
					package_buf[4]=(unsigned char)(0);
					package_buf[5]=(unsigned char)(0);
				}
				//carry payload data 
				memcpy(package_buf+6,tx_data,PAYLOAD_LENGTH);
				//caculate check sum
				for(int i=0;i<30;i++)
					sum += package_buf[i];
				package_buf[31]=sum;
				rf_send_function(package_buf);
				//To-Do:
				//load message data
				update_channel(HOOP_CHANNEL);
			}
			else 
			{
				GPIO_SetBits(GPIOC,GPIO_Pin_4);
				set_channel(sync_sequence[sync_channel]);
				
				package_buf[0]=(unsigned char)(seed>>8);
				package_buf[1]=(unsigned char)(seed);
				package_buf[2]=0xAA;//means sync package
				package_buf[3]=0xAA;//means sync package
				package_buf[4]=(unsigned char)(hoop_channel>>8);
				package_buf[5]=(unsigned char)(hoop_channel);
				
				rf_send_function(package_buf);
				//To-Do:
				//load message data
				update_channel(SYNC_CHANNEL);
		
			}
		}
	}
	void NRF_HOOP::rx_loop()
	{
		char log_buffer[100];
		if(receive_nrf_package(package_buf) == true)//time is not overflow and get some data ,normal hoop
		{	
			//Calibrate timer 
			reset_interval_4ms();
			//count valid data per second
			if(status == HOOPING)valid_data_cnt ++;
			if(status == LOSEONE)valid_loseone_cnt++;
			if(status == LOSETWO)valid_losetwo_cnt++;
			if(status == LOSETHREE)valid_losethree_cnt++;
			if(status == LOSEFOUR)valid_losefour_cnt++;
			if(status == LOSEFIVE)valid_losefive_cnt++;
			if(status == NEEDSYNC)valid_sync_cnt++;
			//Get data update status 
			status = HOOPING;
			//Set payload to hoop_channel
			hoop_channel = (package_buf[4] <<8) + package_buf[5];
			
			
			for(int i=0;i<999;i++)
			{
				store_channel[i]=store_channel[i+1];
			}
			store_channel[999]=hoop_channel;
			
			set_channel(hoop_sequence[hoop_channel]);	
		}			
		else if(interval_4ms == true) //time is overflow ,miss data ,update status
		{	
			interval_4ms = false;//clear flag
			if(status == HOOPING) 
			{
				status = LOSEONE;
				//set channel
				update_channel(HOOP_CHANNEL);
				set_channel(hoop_sequence[hoop_channel]);	
			}
			else if(status == LOSEONE)
			{	
				status = LOSETWO;
				//set channel 
				update_channel(HOOP_CHANNEL);
				set_channel(hoop_sequence[hoop_channel]);	
			}
			else if(status == LOSETWO)
			{
				status =  LOSETHREE;
				//set channel 
				update_channel(HOOP_CHANNEL);
				set_channel(hoop_sequence[hoop_channel]);
			}
			else if(status == LOSETHREE)
			{
				status =  LOSEFOUR;
				//set channel 
				update_channel(HOOP_CHANNEL);
				set_channel(hoop_sequence[hoop_channel]);
			}
			else if(status == LOSEFOUR)
			{
				status =  LOSEFIVE;
				//set channel 
				update_channel(HOOP_CHANNEL);
				set_channel(hoop_sequence[hoop_channel]);
			}
			else if(status == LOSEFIVE)
			{
				status =  NEEDSYNC;
				//set channel 
				update_channel(HOOP_CHANNEL);
				set_channel(hoop_sequence[hoop_channel]);
			}
			else if(status == NEEDSYNC)
			{
				unuse_wait++;
				//change channel 
				update_channel(SYNC_CHANNEL);
				set_channel(sync_sequence[sync_channel]);
				//count valid data per second
			}
		}
		//this loop is for caculate miss data percentage etc.  cycletime:1s
		if(interval_1s == true)
		{
			interval_1s=false;
			total_valid = valid_data_cnt+valid_loseone_cnt+valid_losetwo_cnt+valid_losethree_cnt+valid_losefour_cnt+valid_losefive_cnt+valid_sync_cnt;
			total_cnt = total_valid + unuse_wait;
			miss_rate = 1.0-(total_valid/1.0)/total_cnt;
			
			//for log data in sd card:
			sprintf(log_buffer, "rx_count:%3d,sync_count:%3d,lose_count:%3d,miss_rate:%4f\n,delt_time:%d",total_valid,valid_sync_cnt,unuse_wait,miss_rate,delt_time);
//			log_write(log_buffer,strlen(log_buffer));
			//clear counter
			valid_loseone_cnt=0;
			valid_losetwo_cnt=0;
			valid_losethree_cnt=0;
			valid_losefour_cnt=0;
			valid_losefive_cnt=0;
			valid_data_cnt=0;
			valid_sync_cnt=0;
			unuse_wait=0;
			GPIO_ToggleBits(GPIOC,GPIO_Pin_4);
		}
	}
	void NRF_HOOP::test_rx_loop()
	{
		this->seed=0;
		if(receive_nrf_package(package_buf) == true)
		{
			valid_data_cnt++;
			for(int i=0;i<999;i++)
			{
				store_channel[i]=store_channel[i+1];
				store_hoop[i]=store_hoop[i+1];
			}
			store_hoop[999]=hoop_sequence[hoop_channel];
			store_channel[999] = (package_buf[2]<<8) + package_buf[3];
			update_channel(HOOP_CHANNEL);
			set_channel(hoop_sequence[hoop_channel]);	
			latest_hoop[0]=hoop_channel;
			latest_hoop[1]=hoop_sequence[hoop_channel];
		}	
	}
	HOOP_STATUS NRF_HOOP::get_status() 
	{
		return status;	
	}
	void NRF_HOOP::test_tx_loop()
	{
		static int counter_1000;
		if(interval_2ms == true)
		{
			interval_2ms=false;
			if(counter_1000<999)
			{
				counter_1000++;
				rf_send_function(package_buf);
				set_channel(hoop_sequence[hoop_channel]);
				update_channel(HOOP_CHANNEL);
				GPIO_ResetBits(GPIOC,GPIO_Pin_5);
			}
			else
			{
				GPIO_SetBits(GPIOC,GPIO_Pin_5);
			}
		}
	}
}
