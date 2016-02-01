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

#define GLOBAL_CPU_CLOCK 	80000000

#define PARAMETRS_CNT		20
#define PARAMETRS_ADDR		0x08010000

#define ADDR_SODER		1

#define ADDR_MYFLOAT		4

#define ADDR_TIME1		8
#define ADDR_TIME2		10
#define ADDR_TIME3		12
#define ADDR_TIME4		14
#define ADDR_TIME5		16
#define ADDR_TIME6		18


#define ADDR_TIME_CODE		20

extern uint8_t RXbuf[16];
extern uint8_t RXn;

extern uint8_t TXbuf[16];
extern uint8_t TXn,TXi;

extern volatile unsigned int main_time;

extern uint16_t Soder;

union __all {
	struct {
					//address
		unsigned int Sod;	//0
		unsigned int Time1;	//4
		unsigned int Time2;	//8	
		unsigned int Time3;	//12
		unsigned int Time4;	//16
		unsigned int Time5;	//20
		unsigned int Time6;	//24
		unsigned int timeCod;	//28
	};
	uint32_t BUF[PARAMETRS_CNT];
	uint8_t	bbuf[PARAMETRS_CNT * 4];
};

extern union __all Par;

void readParamToRAM(uint32_t Address, uint32_t *ptr);
void writeDefaultParamToROM(uint32_t Address, uint32_t *ptr);
void writeParamToROM(uint32_t Address, uint32_t *ptr);
	
#endif
