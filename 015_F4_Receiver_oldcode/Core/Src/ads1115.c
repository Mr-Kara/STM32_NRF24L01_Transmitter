
#include "ads1115.h"

float Read_Ads1115(uint8_t channel)
{
	
	unsigned char buffer2[3];
	unsigned char i2c_addr2 = 0x90; //Default address of ads1115 when addr pin connected to ground
	unsigned short data2=0;
  
	//select config register with 0x01 address according to datasheet
	buffer2[0] = 0x01; 
	
	
	//first 8 bit for config register configuration, select channel which will use
	if(channel == 1)      buffer2[1] = 0xC2; //channel 0
	else if(channel == 2) buffer2[1] = 0xD2; // channel 1
	
	//second 8 bit for config register configuration
	buffer2[2] = 0x93;
	
	//send all 3 member of buffer to set configurations
	HAL_I2C_Master_Transmit(&hi2c1,i2c_addr2+0,(uint8_t*)buffer2,3,100); 
	
	
	buffer2[0] = 0x00;
	// send 0x00 to set ads1115 as receive mod
	HAL_I2C_Master_Transmit(&hi2c1,i2c_addr2+0,(uint8_t*)buffer2,1,100); 
	
	// read ads1115 and write to buffer 0 and 1 
	HAL_I2C_Master_Receive(&hi2c1,i2c_addr2+1,buffer2,2,100); 
	
	// xxxx0000 + 0000xxxx 
	data2 = (buffer2[0]<<8) + buffer2[1]; 
	
	if(data2 == 0xFFFF) data2 =0;
	
	// FSR(full scale range) is +-4.096V as default and it means 2*4096=8192 resolution is 8192/2^16=0.125
	return (float)(0.125 * data2);
	
}




