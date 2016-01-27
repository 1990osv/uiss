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
#include <stdio.h>

#define GLOBAL_CPU_CLOCK 80000000
//#define MY_DEBUG_PRINTF_USE

#define PARAMETRS_CNT			20
#define PARAMETRS_ADDR		0x08002000

extern uint8_t DataByte;

extern uint8_t RXbuf[16];
extern uint8_t RXn;

extern uint8_t TXbuf[16];
extern uint8_t TXn,TXi;

#define pT1		1
#define pT2		2
#define pT3		3
#define pT4		4
#define pT5		5
#define pT6		6


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
