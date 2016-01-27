#include "global.h"
#include "modbus.h"
#include "crc16.h"
#define false 	0x00
#define	true	~false

static volatile unsigned char status=MB_COMPLETE;

static void MB_F03(unsigned char *data, unsigned char n);
static void MB_F06(unsigned char *data, unsigned char n);
static void MB_IllegalDataAddres(unsigned char *data, unsigned char n);

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

	if(!checkMyAdress(data[0])){
		return MB_ADDRESS_ERROR;
	}
	
	TXbuf[1] = data[1];   
	
	
	if(data[1]==0x03){
		MB_F03(data,n);
	}
	else if(data[1] == 0x06){
		MB_F06(data,n);
	}
	else
		return MB_FUNCTION_ERR;
	
	return MB_COMPLETE;
}


void MB_F03(unsigned char *data, unsigned char n)
{
	uint16_t addr=0;
	
	TXbuf[2] = data[2];
	TXbuf[3] = data[3];
	
	addr = data[3]+(data[2]<<8);

	switch (addr)
	{
	case 0:{
		TXbuf[4] = 0x00;	
		TXbuf[5] = Soder;
	} break;
	case 1:{
		TXbuf[4] = (Par.DeadTime>>8) & 0x00FF;
		TXbuf[5] = Par.DeadTime & 0x00FF;
	}break;
	
	default:
		MB_IllegalDataAddres(data,n);
		return;
	}

	addCRC16(TXbuf,6); //ToDo
	TXn=8;
	TXi=0;

	UART_SendData (MDR_UART1, (uint16_t)(TXbuf[TXi++]));
}


void MB_F06(unsigned char *data, unsigned char n)
{
	uint16_t addr=0;

	
	TXbuf[2] = data[2];
	TXbuf[3] = data[3];
	addr = data[3] + (data[2] << 8);

	switch (addr)
	{

	case 1:{
		Par.DeadTime = data[5] + (data[4] << 8);
		TXbuf[4] = (Par.DeadTime>>8) & 0x00FF;
		TXbuf[5] = Par.DeadTime & 0x00FF;
		writeParamToROM(PARAMETRS_ADDR,Par.BUF);  //WRITE TO ROM
	}break;
	
	default:
		MB_IllegalDataAddres(data,n);
		return;
	}

	addCRC16(TXbuf,6); //ToDo
	TXn=8;
	TXi=0;

	UART_SendData (MDR_UART1, (uint16_t)(TXbuf[TXi++]));
}

void MB_IllegalDataAddres(unsigned char *data, unsigned char n)
{
	TXbuf[0] = MY_MODBUS_ADR;
	TXbuf[1] = data[1]|0x80;   
	TXbuf[2] = 0x02;   //error code (illegal data address)
	addCRC16(TXbuf,3); //ToDo
	
	TXn=5;
	TXi=0;	
	UART_SendData (MDR_UART1, (uint16_t)(TXbuf[TXi++]));
	
}

