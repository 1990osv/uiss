#include <string.h>

#include "global.h"
#include "modbus.h"
#include "crc16.h"


unsigned char RXbuf[RX_BUFFER_SIZE];
unsigned char TXbuf[TX_BUFFER_SIZE];
unsigned char RXn;
unsigned char TXn,TXi;

static void razbor(unsigned char *data, unsigned char n);
static void MB_F01(unsigned char *data, unsigned char n);
static void MB_F03(unsigned char *data, unsigned char n);
static void MB_F05(unsigned char *data, unsigned char n);
static void MB_F10(unsigned char *data, unsigned char n);
static void mb_exception_rsp(unsigned char func, unsigned char code);
static void send_msg(unsigned char n);

void razbor(unsigned char *data, unsigned char n)
{
	TXbuf[0]=MY_MODBUS_ADR;

	if(data[0] != MY_MODBUS_ADR){
		return;
	}
	
	TXbuf[1] = data[1]; //function code   
	
	if(data[1]==0x01){
		MB_F01(data,n);
	}	
	else 
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
	}
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
		//TXbuf[3] = Par.boolean>>8;
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
	
	count = data[5]*2; //registers count
	addr = data[3]+(data[2]<<8);
	
	TXbuf[2] = count;

	if (addr>=200){	
		addr-=200;
		TXbuf[3] = im[addr]>>8;
		TXbuf[3+1] = im[addr]&0xff;	
	}
	else if(addr>=100){
		addr-=100;
		TXbuf[3] = m[addr]>>8;
		TXbuf[3+1] = m[addr]&0xff;	
	}
	else{
		if(addr+count>PARAMETRS_CNT*4){
			mb_exception_rsp(data[1],0x02);	
		}
		for(i=0;i<count;i+=2){
			TXbuf[3+i] = Par.bbuf[addr+i+1];
			TXbuf[3+i+1] = Par.bbuf[addr+i];
		}
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
		if(TXbuf[4] == 0xFF){
			Par.bSod++;
		}
	} break;
	
	case 1:{
		if(TXbuf[4] == 0xFF){
			Par.bSod--;
		}
	} break;

	case 2:{
		if(TXbuf[4] == 0xFF){
			sod_init(); 
		}
	} break;
	
	case 3:{
		if(TXbuf[4] == 0xFF){	
			sod_send_raw_data(); 
		}
	} break;
	
	default:{
		mb_exception_rsp(data[1],0x02);
		return;
	}
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
	validation_param();
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
	addCRC16(TXbuf,n);
	TXi=0;
	TXn=n+3;
	SWITCH_SEND_MODE;
	UART_SendData (MDR_UART1, (uint16_t)(TXbuf[TXi++]));
}

void modbus_process(void)
{
	if (GTimer_Get(MB_GTIMER) >= 40){  //4ms 9600
		razbor(RXbuf, RXn);
		RXn = 0;
		GTimer_Stop(MB_GTIMER);
	}
}



void modbus_TxInterrupt(void)
{
	if(TXn>TXi){
		UART_SendData (MDR_UART1, (uint16_t)(TXbuf[TXi++]));
	}
	else if(TXn==TXi){
		SWITCH_READ_MODE;
	}
}

void modbus_RxInterrupt(void)
{
	RXbuf[RXn] = UART_ReceiveData (MDR_UART1);
	RXn++;
	GTimer_Reset(MB_GTIMER);
}

