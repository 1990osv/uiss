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

#define PARAMETRS_CNT		  40
#define PARAMETRS_ADDR		0x08019000

#define ADC_DATA_SIZE		100

union __all {
	struct {					                            //address
							
		volatile 	        int resinContent;	        //0     0	    содержание связующего
		volatile unsigned int baseResinContent[3];	//4..12 1..3	начальная установка	
		volatile unsigned int acusticBase;	        //16    4	    акустическая база, мм
		volatile unsigned int speedResin;	          //20    5     скорость звука в смоле

		volatile unsigned int deadTime;		          //24    6	    мертвое время, 0.01 us
		volatile unsigned int mainTime;		          //28	  7     общее время, 0.01 us
		volatile unsigned int strobTime;	          //32	  8     стробирующий импульс для ПЛИС, 0.01 us
		volatile unsigned int adjustTime;	          //36	  9     время коррекции(выполнения прерывания), 0.01 us

		volatile unsigned int measureFrequence;	    //40	  10    частота измерения, Гц
		
		volatile unsigned int timeCod;		          //44	  11    код времени
		volatile unsigned int timeCodA;		          //48    12  
		volatile unsigned int speedDry;		          //52    13
		volatile unsigned int speedCurrent;		      //56    14
		
		volatile unsigned int adress;		            //60    15
		volatile unsigned int baudRate;	            //64    16  
		volatile unsigned int wordLength;	          //68    17
		volatile unsigned int stopBits;	            //72    18
		volatile unsigned int parity;		            //76    19
		
		volatile unsigned int startImpTime[SIZE_START_TIME];	//80..160 20..40

	};
	uint32_t BUF[PARAMETRS_CNT];
	uint8_t	bbuf[PARAMETRS_CNT * 4];				      //40 * 4 = 160 bite
};

extern unsigned int dac_out;

extern union __all Par;

extern unsigned char adcConvertationEnable;
extern unsigned int ADCConvertedValue[ADC_DATA_SIZE];

extern DMA_ChannelInitTypeDef DMA_InitStr;
extern DMA_CtrlDataInitTypeDef DMA_PriCtrlStr;
extern DMA_CtrlDataInitTypeDef DMA_AltCtrlStr;

void readParamToRAM(uint32_t Address, uint32_t *ptr);
void writeDefaultParamToROM(uint32_t Address, uint32_t *ptr);
void save_parametrs(uint32_t Address, uint32_t *ptr);
void validation_param(void);


#endif
