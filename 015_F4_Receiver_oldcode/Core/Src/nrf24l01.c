
#include "nrf24l01.h"


void CS_Select(void)  //Select the slave device
{
	HAL_GPIO_WritePin(NRF_CS_GPIO_Port,NRF_CS_Pin,GPIO_PIN_RESET);
}


void CS_UnSelect(void) //Unselect the slave device
{	
	HAL_GPIO_WritePin(NRF_CS_GPIO_Port,NRF_CS_Pin,GPIO_PIN_SET);

}


void CE_Enable(void) //Enable NRF |Activates RX or TX mode
{
	HAL_GPIO_WritePin(NRF_CE_GPIO_Port,NRF_CE_Pin,GPIO_PIN_SET);
}


void CE_Disable(void) //Disable NRF | Activates RX or TX mode
{
	HAL_GPIO_WritePin(NRF_CE_GPIO_Port,NRF_CE_Pin,GPIO_PIN_RESET);	
}


//Write a single byte to the particular register
void nrf24_WriteReg(uint8_t Reg, uint8_t Data)
{
	
	uint8_t buf[2];    
	buf[0]=Reg | 1<<5;  // this one is register address
	buf[1]=Data;
	
	//Pull the CS pin low to select the device
	CS_Select();
	
	HAL_SPI_Transmit(NRF24_SPI, buf, 2, 1000);
	
	//Pull CS pin high to relase the device
	CS_UnSelect();
	
}


// write multiple bytes starting from a particular register
void nrf24_WriteRegMulti(uint8_t Reg, uint8_t *data, int size)
{
	uint8_t buf[2];
	buf[0]=Reg|1<<5; // this one is register address
	
	//Pull the CS pin low to select the device
	CS_Select();
	
	HAL_SPI_Transmit(NRF24_SPI,buf,1,100); // first we send the register address
	HAL_SPI_Transmit(NRF24_SPI,data,size,1000); // then we send the data at once
	
	//Pull CS pin high to relase the device
	CS_UnSelect();
	
}


//Read one byte from register
uint8_t nrf24_ReadReg(uint8_t Reg)
{
	uint8_t data=0; // store the data here
	
	//Pull the CS pin low to select the device
	CS_Select();
	
	//according to datasheet we just have to send 5 b
	HAL_SPI_Transmit(NRF24_SPI,&Reg,1,100); //send the register address where we want the read data from  
	HAL_SPI_Receive(NRF24_SPI,&data,1,100); //Read one byte from &Reg and store to &data
	
	//Pull CS pin high to relase the device
	CS_UnSelect();
	
	return data;
}


//Read multiple bytes from register
void nrf24_ReadRegMulti(uint8_t Reg, uint8_t *data, int size)
{
	//Pull the CS pin low to select the device
	CS_Select();
	
	//according to datasheet we just have to send 5 b
	HAL_SPI_Transmit(NRF24_SPI,&Reg,1,100); //send the register address where we want the read data from 
	HAL_SPI_Receive(NRF24_SPI,data,size,1000); // Read amount size of data and store it at &data
	
	//Pull CS pin high to relase the device
	CS_UnSelect();
}


//send the command to NRF
void nrf24_SendCommand(uint8_t cmd)
{
	//Pull the CS pin low to select the device
	CS_Select();
	
	HAL_SPI_Transmit(NRF24_SPI, &cmd,1,100);
	
	//Pull CS pin high to relase the device
	CS_UnSelect();
	
}

//Reset the nrf according to datasheet
void nrf24_Reset(uint8_t REG)
{
	if (REG == STATUS)
	{
		nrf24_WriteReg(STATUS, 0x00);
	}

	else if (REG == FIFO_STATUS)
	{
		nrf24_WriteReg(FIFO_STATUS, 0x11);
	}

	else {
	nrf24_WriteReg(CONFIG, 0x08);
	nrf24_WriteReg(EN_AA, 0x3F);
	nrf24_WriteReg(EN_RXADDR, 0x03);
	nrf24_WriteReg(SETUP_AW, 0x03);
	nrf24_WriteReg(SETUP_RETR, 0x03);
	nrf24_WriteReg(RF_CH, 0x02);
	nrf24_WriteReg(RF_SETUP, 0x0E);
	nrf24_WriteReg(STATUS, 0x00);
	nrf24_WriteReg(OBSERVE_TX, 0x00);
	nrf24_WriteReg(CD, 0x00);
	uint8_t rx_addr_p0_def[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
	nrf24_WriteRegMulti(RX_ADDR_P0, rx_addr_p0_def, 5);
	uint8_t rx_addr_p1_def[5] = {0xC2, 0xC2, 0xC2, 0xC2, 0xC2};
	nrf24_WriteRegMulti(RX_ADDR_P1, rx_addr_p1_def, 5);
	nrf24_WriteReg(RX_ADDR_P2, 0xC3);
	nrf24_WriteReg(RX_ADDR_P3, 0xC4);
	nrf24_WriteReg(RX_ADDR_P4, 0xC5);
	nrf24_WriteReg(RX_ADDR_P5, 0xC6);
	uint8_t tx_addr_def[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
	nrf24_WriteRegMulti(TX_ADDR, tx_addr_def, 5);
	nrf24_WriteReg(RX_PW_P0, 0);
	nrf24_WriteReg(RX_PW_P1, 0);
	nrf24_WriteReg(RX_PW_P2, 0);
	nrf24_WriteReg(RX_PW_P3, 0);
	nrf24_WriteReg(RX_PW_P4, 0);
	nrf24_WriteReg(RX_PW_P5, 0);
	nrf24_WriteReg(FIFO_STATUS, 0x11);
	nrf24_WriteReg(DYNPD, 0);
	nrf24_WriteReg(FEATURE, 0);
	}
}



void nrf24_Init(void)
{

	//Disable the nrf chip before configuring the device 
	CE_Disable();
	
	nrf24_Reset(0); // Reset all nrf registers

	nrf24_WriteReg(CONFIG,0x0C);
	
	nrf24_WriteReg(EN_AA,0);
	
	nrf24_WriteReg(EN_RXADDR,0);
	
	nrf24_WriteReg(SETUP_AW,0x03); //5 bytes for the TX/RX address
	
	nrf24_WriteReg(SETUP_RETR,0); //no retransmission
	
	nrf24_WriteReg(RF_CH,0);
	
	nrf24_WriteReg(RF_SETUP,0x0E); // power=0 dbm, data rate=2Mbps
	
	//Enable the nrf after configured
	CE_Enable();
	
	
}

//Setup the TX Mode
void nrf24_TxMode(uint8_t *Address, uint8_t channel)
{
	//disable the chip before configuring
	CE_Disable();
	
	nrf24_WriteReg(RF_CH,channel); // select the channel
	
	nrf24_WriteRegMulti(TX_ADDR,Address,5); //write the TX address. We decided in nrf24_init that address is 5 bit.
//	nrf24_WriteRegMulti(RX_ADDR_P0,Address,5); // Tx add and Rx_p0 has to be same
	
	
	// power up device
	uint8_t config =nrf24_ReadReg(CONFIG); // Read config register
	config = config | (1<<1); // set 1st bit as high to power up according to datasheet
	nrf24_WriteReg(CONFIG,config); // write local config to CONFIG register
	
	//Enable the chip after configuring the device
	CE_Enable();
}

//Transmit the data
uint8_t nrf24_Transmit (uint8_t *data)
{
	uint8_t cmdtosend =0; //buffer of payload command
	
	//select the device 
	CS_Select();
	
	//payload command
	cmdtosend = W_TX_PAYLOAD;
	HAL_SPI_Transmit(NRF24_SPI,&cmdtosend,1,100);
	
	//send the payload
	HAL_SPI_Transmit(NRF24_SPI,data,2,1000);       //Data packet size 2 bytes
	
	//Unselect the device
	CS_UnSelect();
	
	HAL_Delay(1);
	
	uint8_t fifostatus = nrf24_ReadReg(FIFO_STATUS); //We are checking if the transmission is completed or not. Check FIFO_STATUS at datasheet 
	// if fifo is empty that means transmission is completed succesfully 
	
	//When device is disconnected the 4th(tx_empty) bit gonna be 1. To make sure the problem is just transmission is ok or not, there is 
	//reserved bits 3:2 should be 0 (if device is disconnect these bits will be 0. 
	
	if((fifostatus&(1<<4)) && (!(fifostatus&(1<<3))))
	{
		cmdtosend = FLUSH_TX; // if the data is transferred we will send the
		//command flush the fifo
		nrf24_SendCommand(cmdtosend);
		
		//CE_Disable();  // To transmit only once 
		
		return 1;
		
	}
	
	//CE_Disable(); // To transmit only once 
	
	return 0; // if its not succesful
	
	
}



void nrf24_RxMode(uint8_t *Address, uint8_t channel)
{
	//disable the chip before configuring the device
	CE_Disable();

	nrf24_WriteReg(RF_CH,channel); // select the channel
	
	//select daara pipe 1
	uint8_t en_rxaddr = nrf24_ReadReg(EN_RXADDR); //taking EN_RXADDR reg to temp value 
	en_rxaddr = en_rxaddr | (1<<1); // make sure just change the data pipe 1
	nrf24_WriteReg(EN_RXADDR,en_rxaddr); // select pipe 1
		
	nrf24_WriteRegMulti(RX_ADDR_P1,Address,5); // Write 5 byte address for the data pipe 1
	
	nrf24_WriteReg(RX_PW_P1,2); //Data packet size 2 bytes
	
	//power up device in RX mode
	uint8_t config = nrf24_ReadReg(CONFIG);
	config = config | (1<<1) | (1<<0);
	nrf24_WriteReg(CONFIG,config);
	
	//Enable the chip after configuring the device 
	CE_Enable();
	
}

uint8_t isDataAvailable(int pipenum)
{
	uint8_t status = nrf24_ReadReg(STATUS);
	
	//check RX_DR register (when new data arrive in rx fifo this register sets)
	//check pipenum if its the selected one 
	if((status&(1<<6)) && (status&(pipenum<<1)))
	{
		nrf24_WriteReg(STATUS,(1<<6)); // clear RX_DR (RX FIFO)
		
		return 1;
		
	}
		return 0;
	
}


void nrf24_Receive(uint8_t *data)
{
	uint8_t cmdtosend=0;
	
	//select the device
	CS_Select();
	
	//payload command
	cmdtosend = R_RX_PAYLOAD;
	HAL_SPI_Transmit(NRF24_SPI,&cmdtosend,1,100);
	
	//receive the payload
	HAL_SPI_Receive(NRF24_SPI,data,2,1000);  //Data packet size 2 bytes
	
	//Unselect the device
	CS_UnSelect();
	
	HAL_Delay(1);
	
	cmdtosend = FLUSH_RX; //once the data has been received flush the rx fifo
	nrf24_SendCommand(cmdtosend); 
	
}


//	uint8_t nrf_CONFIG, nrf_EN_AA, nrf_EN_RXADDR, nrf_SETUP_AW, nrf_SETUP_RETR, nrf_RF_CH, nrf_RF_SETUP, nrf_STATUS, nrf_OBSERVE_TX, nrf_CD, nrf_RX_ADDR_P0,
//	nrf_RX_ADDR_P1, nrf_RX_ADDR_P2, nrf_RX_ADDR_P3, nrf_RX_ADDR_P4, nrf_RX_ADDR_P5, nrf_TX_ADDR, nrf_RX_PW_P0, nrf_RX_PW_P1, nrf_RX_PW_P2, nrf_RX_PW_P3, nrf_RX_PW_P4, nrf_RX_PW_P5,
//	nrf_FIFO_STATUS, nrf_DYNPD, nrf_FEATURE;


//	void NRF_READ_ALL_REG(void)
//{
//	nrf_CONFIG = nrf24_ReadReg(CONFIG);
//	nrf_EN_AA =  nrf24_ReadReg(EN_AA);
//	nrf_EN_RXADDR = nrf24_ReadReg(EN_RXADDR);
//	nrf_SETUP_AW = nrf24_ReadReg(SETUP_AW);
//	nrf_SETUP_RETR = nrf24_ReadReg(SETUP_RETR);
//	nrf_RF_CH = nrf24_ReadReg(RF_CH);
//	nrf_RF_SETUP = nrf24_ReadReg(RF_SETUP);
//	nrf_STATUS = nrf24_ReadReg(STATUS);
//	nrf_OBSERVE_TX = nrf24_ReadReg(OBSERVE_TX);
//	nrf_RX_ADDR_P0 = nrf24_ReadReg(RX_ADDR_P0);
//	nrf_RX_ADDR_P1 = nrf24_ReadReg(RX_ADDR_P1);
//	nrf_RX_ADDR_P2 = nrf24_ReadReg(RX_ADDR_P2);
//	nrf_RX_ADDR_P3 = nrf24_ReadReg(RX_ADDR_P3);
//	nrf_RX_ADDR_P4 = nrf24_ReadReg(RX_ADDR_P4);
//	nrf_RX_ADDR_P5 = nrf24_ReadReg(RX_ADDR_P5);
//	nrf_TX_ADDR = nrf24_ReadReg(TX_ADDR);
//	nrf_RX_PW_P0 = nrf24_ReadReg(RX_PW_P0);
//	nrf_RX_PW_P1 = nrf24_ReadReg(RX_PW_P1);
//	nrf_RX_PW_P2 = nrf24_ReadReg(RX_PW_P2);
//	nrf_RX_PW_P3 = nrf24_ReadReg(RX_PW_P3);
//	nrf_RX_PW_P4 = nrf24_ReadReg(RX_PW_P4);
//	nrf_RX_PW_P5 = nrf24_ReadReg(RX_PW_P5);
//	nrf_FIFO_STATUS = nrf24_ReadReg(FIFO_STATUS);
//	nrf_DYNPD = nrf24_ReadReg(DYNPD);
//	nrf_FEATURE = nrf24_ReadReg(FEATURE);
//}


