
#include <string.h>

#include "global.h"
#include "modbus.h"
#include "crc16.h"
#define false 	0x00
#define	true	~false

static volatile unsigned char status=MB_COMPLETE;

static void MB_F03(unsigned char *data, unsigned char n);
static void MB_F06(unsigned char *data, unsigned char n);
static void MB_F10(unsigned char *data, unsigned char n);
static void mb_exception_rsp(unsigned char func, unsigned char code);

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
	
	TXbuf[1] = data[1]; //function code   
	
	
	if(data[1]==0x03){
		MB_F03(data,n);
	}
	else if(data[1] == 0x06){
		MB_F06(data,n);
	}
	else if (data[1] == 0x10){
		MB_F10(data,n);
	}	
	else{
		mb_exception_rsp(data[1],0x01); //function not supported
		return MB_FUNCTION_ERR;
	}
	return MB_COMPLETE;
}


void MB_F03(unsigned char *data, unsigned char n)
{
	uint16_t addr=0;
	
	TXbuf[2] = data[5]*2; //registrs count
	
	
	addr = data[3]+(data[2]<<8);

	switch (addr)
	{
	case ADDR_SODER:{
		TXbuf[3] = 0x00;	
		TXbuf[4] = Soder;
		addCRC16(TXbuf,5); //ToDo
		TXn=7;
	} break;
	case ADDR_DEAD_TIME:{
		TXbuf[3] = (Par.DeadTime>>8) & 0x00FF;
		TXbuf[4] = Par.DeadTime & 0x00FF;
		addCRC16(TXbuf,5); //ToDo
		TXn=7;
	}break;
	case 3:{
		TXbuf[3] = 0xFF;
		TXbuf[4] = 0xFF;
		addCRC16(TXbuf,5); //ToDo
		TXn=7;
	}break;
	case ADDR_MYFLOAT:{
//		fpar.ffloat=Par.myFloat;
//		TXbuf[3]=fpar.dd;
//		TXbuf[4]=fpar.cc;
//		TXbuf[5]=fpar.bb;
//		TXbuf[6]=fpar.aa;
		memcpy(&TXbuf[3],&Par.myFloat,4);
		addCRC16(TXbuf,7); //ToDo
		TXn=9;
	}break;
	default:
		mb_exception_rsp(data[1],0x02);
		return;
	}

	
	
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

	case ADDR_DEAD_TIME:{
		Par.DeadTime = data[5] + (data[4] << 8);
		TXbuf[4] = data[4];//(Par.DeadTime>>8) & 0x00FF;
		TXbuf[5] = data[5];//Par.DeadTime & 0x00FF;
		writeParamToROM(PARAMETRS_ADDR,Par.BUF);  //WRITE TO ROM
	}break;
	
	default:
		mb_exception_rsp(0x06,0x02);
		return;
	}

	addCRC16(TXbuf,6); //ToDo
	TXn=8;
	TXi=0;

	UART_SendData (MDR_UART1, (uint16_t)(TXbuf[TXi++]));
}

void MB_F10(unsigned char *data, unsigned char n)
{
	uint16_t addr=0;
	uint16_t quantity=0;
	uint8_t count=0;
	
	TXbuf[2] = data[2]; 	//starting address
	TXbuf[3] = data[3];	//starting address
	addr = data[3] + (data[2] << 8);
	
	TXbuf[4] = data[4]; 	//quantity of registers
	TXbuf[5] = data[5];	//quantity of registers	
	quantity = data[5] + (data[4] << 8);
	
	count = data[6];
	
	if((quantity==0)||(quantity>0x007B)||(count>2))  // больше 4 байт не работает
		mb_exception_rsp(data[1],0x03); //many registrs
	
	switch (addr)
	{

	case ADDR_DEAD_TIME:{
		Par.DeadTime = data[8] + (data[7] << 8);
		writeParamToROM(PARAMETRS_ADDR,Par.BUF);  //WRITE TO ROM
	}break;		
		
	case ADDR_MYFLOAT:{
		memcpy(&Par.myFloat,&data[7],4);
		//Par.myFloat = data[8] + (data[7] << 8);
		writeParamToROM(PARAMETRS_ADDR,Par.BUF);  //WRITE TO ROM
	}break;
	
	default:
		mb_exception_rsp(0x10,0x02);
		return;
	}

	addCRC16(TXbuf,6); //ToDo
	TXn=8;
	TXi=0;

	UART_SendData (MDR_UART1, (uint16_t)(TXbuf[TXi++]));
}

void mb_exception_rsp(unsigned char func, unsigned char code)
{
	TXbuf[0] = MY_MODBUS_ADR;
	TXbuf[1] = func|0x80;   
	TXbuf[2] = code;   //error code (illegal data address)
	addCRC16(TXbuf,3); //ToDo
	TXn=5;
	TXi=0;	
	UART_SendData (MDR_UART1, (uint16_t)(TXbuf[TXi++]));
	
}

