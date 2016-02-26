#ifndef GLOBAL_H
#define GLOBAL_H

#include "MDR32F9Qx_config.h"
#include "MDR32Fx.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_uart.h"
#include "MDR32F9Qx_timer.h"
#include "MDR32F9Qx_eeprom.h"
#include "MDR32F9Qx_dac.h"
#include "MDR32F9Qx_adc.h"
#include "MDR32F9Qx_dma.h"

#include "init.h"
#include "timers.h"
#include "main_algorithm.h"
#include "modbus.h"
#include "ports.h"

#include <stdbool.h>
#include <stdio.h>

#define GLOBAL_CPU_CLOCK 	80000000

#define PARAMETRS_CNT		40
#define PARAMETRS_ADDR		0x08019000

#define ADC_DATA_SIZE		100

union __all {
	struct {
					//address
			 int Sod;	//0
		unsigned int Time1;	//4
		unsigned int Time2;	//8	
		unsigned int Time3;	//12
		unsigned int Time4;	//16
		unsigned int Time5;	//20
		unsigned int Time6;	//24
		unsigned int timeCod;	//28
		unsigned int timeCodA;	//32
			 int bSpeed;	//36
		unsigned int AcBase;	//40
		unsigned int SmSpeed;	//44
		unsigned int cSpeed;	//48
			 int bSod;	//52
		unsigned int PWMperiod;	//56
		unsigned int PWMcnt;	//60
		unsigned int startTime[SIZE_START_TIME];	//64..144
					//146
	};
	uint32_t BUF[PARAMETRS_CNT];
	uint8_t	bbuf[PARAMETRS_CNT * 4];
};

extern union __all Par;


extern uint16_t ADCConvertedValue[ADC_DATA_SIZE];

extern DMA_ChannelInitTypeDef DMA_InitStr;
extern DMA_CtrlDataInitTypeDef DMA_PriCtrlStr;
extern DMA_CtrlDataInitTypeDef DMA_AltCtrlStr;

void readParamToRAM(uint32_t Address, uint32_t *ptr);
void writeDefaultParamToROM(uint32_t Address, uint32_t *ptr);
void writeParamToROM(uint32_t Address, uint32_t *ptr);
void validation_param(void);


#endif
