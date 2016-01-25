#include "global.h"
#include "modbus.h"
#include "crc16.h"
#define false 	0x00
#define	true	~false

static volatile unsigned char status=MB_COMPLETE;

unsigned char GetStatus(void)
{
	return status;
}

unsigned char checkMyAdress(unsigned char data)
{
	if(data==MY_MODBUS_ADR){
		return true;
	}
	else {
		return false;
	}
}


unsigned char razbor(unsigned char *data, unsigned char n)
{
	TXbuf[0]=MY_MODBUS_ADR;
	TXbuf[1]=data[1];
	
	if(!checkMyAdress(data[0])){
		return MB_ADDRESS_ERROR;
	}
	if(data[1]!=0x03){
		return MB_FUNCTION_ERR;
	}
	else{
		TXbuf[2]=data[2];		
		TXbuf[3]=data[3];		
		TXbuf[4]=0xAA;	
		TXbuf[5]=0xAA;
		addCRC16(TXbuf,6); //ToDo
		TXn=8;
		TXi=0;
		UART_SendData (MDR_UART1, (uint16_t)(TXbuf[TXi++]));
	}
	
	
	return MB_COMPLETE;
}

