#ifndef GLOBAL_H
#define GLOBAL_H

#include "MDR32F9Qx_config.h"
#include "MDR32Fx.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_uart.h"
#include "MDR32F9Qx_timer.h"
#include "MDR32F9Qx_eeprom.h"

#include "init.h"
#include "timers.h"
#include "main_algorithm.h"
#include "modbus.h"
#include "ports.h"
#include <stdio.h>

#define GLOBAL_CPU_CLOCK 80000000

#define PARAMETRS_CNT		20
#define PARAMETRS_ADDR		0x08010000

#define ADDR_SODER		1

#define ADDR_MYFLOAT		4

#define ADDR_TIME1		5
#define ADDR_TIME2		6
#define ADDR_TIME3		7
#define ADDR_TIME4		8
#define ADDR_TIME5		9
#define ADDR_TIME6		10

extern uint8_t RXbuf[16];
extern uint8_t RXn;

extern uint8_t TXbuf[16];
extern uint8_t TXn,TXi;

extern volatile unsigned int main_time;

extern uint16_t Soder;

union __all {
	struct {
		unsigned char Sod;
		unsigned char aa;
		unsigned char bb;
		unsigned char cc;
		float myFloat;
		unsigned int Time1;
		unsigned int Time2;
		unsigned int Time3;
		unsigned int Time4;
		unsigned int Time5;
		unsigned int Time6;
		
	};
	uint32_t BUF[PARAMETRS_CNT];
};

extern union __all Par;


void readParamToRAM(uint32_t Address, uint32_t *ptr);
void writeDefaultParamToROM(uint32_t Address, uint32_t *ptr);
void writeParamToROM(uint32_t Address, uint32_t *ptr);
	
//void dbg_print( unsigned char *ptr)
//{
//#ifdef MY_DEBUG_PRINTF_USE
//	unsigned char c;
//	do
//	{
//		c=*ptr++;
//		ITM_SendChar(c);
//	} while(c!='\0');
//#endif
//}

#endif
