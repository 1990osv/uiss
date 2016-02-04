
#include <string.h>

#include "global.h"
#include "modbus.h"
#include "crc16.h"
#define false 	0x00
#define	true	~false

static volatile unsigned char status=MB_COMPLETE;

static void MB_F01(unsigned char *data, unsigned char n);
static void MB_F03(unsigned char *data, unsigned char n);
static void MB_F05(unsigned char *data, unsigned char n);
static void MB_F10(unsigned char *data, unsigned char n);
static void mb_exception_rsp(unsigned char func, unsigned char code);

static void send_msg(unsigned char n);

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
	
//	if(data[1]==0x01){
//		MB_F01(data,n);
//	}	
//	else 
	if(data[1]==0x03){
		MB_F03(data,n);
	}	
	else if (data[1] == 0x05){
		MB_F05(data,n);
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

void MB_F01(unsigned char *data, unsigned char n)
{
	uint16_t addr=0;
	
	TXbuf[1]=data[1];
	TXbuf[2]=1;	//byte count

	addr = data[3]+(data[2]<<8);

	switch (addr)
	{
	case 0:{
		TXbuf[3] = Par.boolean>>8;
		//TXbuf[4] = time_code & 0x00FF;
			
	} break;
	
	default:
		mb_exception_rsp(data[1],0x02);
		return;
	}

	send_msg(4);
}

void MB_F03(unsigned char *data, unsigned char n)
{
	uint16_t addr=0, count=0;
	uint8_t	i=0;
	
	count = data[5]*2; //registrs count
	addr = data[3]+(data[2]<<8);
	
	if(addr+count>PARAMETRS_CNT*4){
		mb_exception_rsp(data[1],0x02);	
	}
	
	TXbuf[2] = count;

	for(i=0;i<count;i+=2){
		TXbuf[3+i] = Par.bbuf[addr+i+1];
		TXbuf[3+i+1] = Par.bbuf[addr+i];
	}

	send_msg(3+count);
}


void MB_F05(unsigned char *data, unsigned char n)
{
	uint16_t addr=0;
	uint8_t i;
	
	for(i=1;i<=5;i++){
		TXbuf[i] = data[i];
	}

	addr = data[3]+(data[2]<<8);

	switch (addr)
	{
	case 0:{
		if(TXbuf[4] == 0xFF)
			Par.bSod++;
	} break;
	
	case 1:{
		if(TXbuf[4] == 0xFF)
			Par.bSod--;
	} break;

	case 2:{
		if(TXbuf[4] == 0xFF)
			sod_init(); //сброс счетчика усреднения
			sod_begin_init=1;
	} break;
	
	default:
		mb_exception_rsp(data[1],0x02);
		return;
	}

	send_msg(6);
}


void MB_F10(unsigned char *data, unsigned char n)
{
	uint16_t addr=0;
	uint16_t quantity=0;
	uint8_t count=0;
	uint8_t i=0;
	addr = data[3] + (data[2] << 8);
	quantity = data[5] + (data[4] << 8);
	count = data[6];
	
	if((quantity==0)||(quantity>0x007B)||(count>2))  // больше 4 байт не работает
		mb_exception_rsp(data[1],0x03); //more registrs
	
	for(i=2;i<=5;i++){
		TXbuf[i] = data[i];
	}
	
	for(i=0;i<count;i+=2){
		Par.bbuf[addr+i+1] = RXbuf[7+i];
		Par.bbuf[addr+i] = RXbuf[7+i+1];
	}
	
	writeParamToROM(PARAMETRS_ADDR,Par.BUF);  	//Сохранение параметров
	update_state_time();				//Обновление времени срабатывания таймеров
	send_msg(6);

}


void mb_exception_rsp(unsigned char func, unsigned char code)
{
	TXbuf[0] = MY_MODBUS_ADR;
	TXbuf[1] = func|0x80;   
	TXbuf[2] = code;   //error code (illegal data address)
	send_msg(3);
}

static void send_msg(unsigned char n)
{
	addCRC16(TXbuf,n); //ToDo
	TXi=0;
	TXn=n+3;
	DOT4_PORT->RXTX |= (1<<DOT4_PIN);//PORT_SetBits(DOT4_PORT,DOT4_PIN);//MDR_PORTE->RXTX |= (1<<6);//MDR_PORTE->RXTX &= ~(1<<6);//PORT_SetBits(MDR_PORTE,6);
	UART_SendData (MDR_UART1, (uint16_t)(TXbuf[TXi++]));
}
