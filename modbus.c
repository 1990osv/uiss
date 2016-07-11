#include <string.h>

#include "global.h"
#include "modbus.h"
#include "crc16.h"


unsigned char RXbuf[RX_BUFFER_SIZE];
unsigned char TXbuf[TX_BUFFER_SIZE];
unsigned char RXn;
unsigned char TXn,TXi;

static void parsing(unsigned char *data, unsigned char n);
static void parsingFunction0x01(unsigned char *data, unsigned char n);
static void parsingFunction0x03(unsigned char *data, unsigned char n);
static void parsingFunction0x05(unsigned char *data, unsigned char n);
static void parsingFunction0x10(unsigned char *data, unsigned char n);
static void mb_exception_rsp(unsigned char func, unsigned char code);
static void send_msg(unsigned char n);

void parsing(unsigned char *data, unsigned char n)
{
	if(data[0] != Par.adress){
		return;
	}
	TXbuf[0] = data[0]; //adress
	TXbuf[1] = data[1]; //function code 	
	switch (data[1]){
		case 0x01:
			parsingFunction0x01(data,n);
			break;
		case 0x03:
			parsingFunction0x03(data,n);
			break;
		case 0x05:
			parsingFunction0x05(data,n);
			break;
		case 0x10:
			parsingFunction0x10(data,n);
			break;
		default:
			mb_exception_rsp(data[1],0x01); //function not supported
			break;
	}
}

void parsingFunction0x01(unsigned char *data, unsigned char n)
{
	uint16_t addr=0;
	
	TXbuf[1]=data[1];
	TXbuf[2]=1;	//byte count

	addr = data[3]+(data[2]<<8);

	switch (addr)
	{
	case 0:{
		//TXbuf[3] = Par.boolean>>8;
		//TXbuf[4] = timeCode & 0x00FF;
	} break;
	
	default:
		mb_exception_rsp(data[1],0x02);
		return;
	}

	send_msg(4);
}

void parsingFunction0x03(unsigned char *data, unsigned char n)
{
	unsigned int addr = 0;
	unsigned int count = 0;
	unsigned int i = 0;
	unsigned int *needData;
        unsigned char *byteNeedData;
	count = data[5] * 2;
	addr = (data[3] + (data[2] << 8) ) * 4;

	TXbuf[2] = count;
	
	if (addr < 200){
		byteNeedData = &Par.bbuf[0];
		for(i = 0; i < count; i += 2) {
			TXbuf[3 + i] = byteNeedData[addr + i + 1];
			TXbuf[3 + i + 1] = byteNeedData[addr + i];
		}
		send_msg(3 + count);
		return;		
	}
	else if (addr >= 600) {
		mb_exception_rsp(data[1], 0x02);
		return;
	}	
	else if (addr >= 500) {
		addr -= 500;
		needData = &rawStoreTimeCode[0];
	}	
	else if (addr >= 400) {
		addr -= 400;
		needData = &ADCConvertedValue[0];
	}	
	else if (addr >= 300) {
		addr -= 300;
		needData = &im[0];
	}
	else if (addr >= 200) {
		addr -= 200;
		needData = &m[0];		
	}
	else 
		return;
	for(i = 0; i < count; ++i){
		TXbuf[3+i] = needData[addr + i];
	}
	send_msg(3 + count);
}

/**
* @brief  Формирует ответ на функцию 0x05 MODBUS RTU
* @detailed 	Бит 0 - увеличить содержание; 
*		Бит 1 - уменьшить содержание; 
*		Бит 2 - начальная установка; 
*		Бит 3 - неотсортированный массив;
*		Бит 4 - оцифровка;
* @param  data: Принятое сообщение
* @param  n: Размер в байтах принятого сообщения
*/
void parsingFunction0x05(unsigned char *data, unsigned char n)
{
	uint16_t addr=0;
	uint8_t i;
	
	for(i=1;i<=5;i++){
		TXbuf[i] = data[i];
	}

	addr = data[3] + (data[2] << 8);
		
	if(TXbuf[4] == 0xFF) {	// записывают 1
	switch (addr){
		case 0: 
			if(Par.baseResinContent[0] < 50) 
				++Par.baseResinContent[0];
		break;
		
		case 1:
			if(Par.baseResinContent[0] > 1)
				--Par.baseResinContent[0];
		break;

		case 2:
			contentInitialisation(); 
		break;

		case 3:
			sod_refreshRawData(); 
		break;
		
		case 4:
			start_digitize();
		break;
		
		default:
			mb_exception_rsp(data[1],0x02);
			return;
		
	}
	}
	else {
		mb_exception_rsp(data[1],0x02);
		return;		
	}
	send_msg(6);
}


void parsingFunction0x10(unsigned char *data, unsigned char n)
{
	uint16_t addr=0;
	uint16_t quantity=0;
	uint8_t count=0;
	uint8_t i=0;
	addr = (data[3] + (data[2] << 8)) * 4;
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
	save_parametrs(PARAMETRS_ADDR,Par.BUF);  	//Сохранение параметров
	update_state_time();				//Обновление времени срабатывания таймеров
	send_msg(6);
	
	if(addr == 0x04){	  //Записали базовое содержание
		contentInitialisation();
	}

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
		parsing(RXbuf, RXn);
		RXn = 0;
		GTimer_Stop(MB_GTIMER);
	}
}

void modbus_TxInterrupt(void)
{
	if(TXn > TXi){
		UART_SendData (MDR_UART1, (uint16_t)(TXbuf[TXi++]));
	}
	else if(TXn == TXi){
		SWITCH_READ_MODE;
	}
}

void modbus_RxInterrupt(void)
{
	RXbuf[RXn] = UART_ReceiveData (MDR_UART1);
	RXn++;
	GTimer_Reset(MB_GTIMER);
}

