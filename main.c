#include "global.h"
volatile char SEC=0;
volatile unsigned int temp;

volatile unsigned int mytime=0;

volatile unsigned int main_time=0;


uint8_t RXbuf[16];
uint8_t TXbuf[16];
uint8_t RXn;
uint8_t TXn,TXi;

uint32_t i;

//uint32_t RAMParam[PARAMETRS_CNT];

union __all Par;

uint16_t Soder;

void readParamToRAM(uint32_t Address, uint32_t *ptr)
{
static uint8_t i = 0;
	__disable_irq();
	

	for(i = 0; i < PARAMETRS_CNT; i++){
		ptr[i]=EEPROM_ReadWord(Address + i * 4, EEPROM_Main_Bank_Select);
	}
	if(ptr[2] == 0xFFFFFFFF)
		writeDefaultParamToROM(Address,Par.BUF);
	__enable_irq();
}

void writeDefaultParamToROM(uint32_t Address, uint32_t *ptr)
{
static uint8_t i = 0;
	
	Par.Sod		= 0;		//содержание связующего
	Par.Time1	= 750;  	// стартовый импульс 7.5 us
	Par.Time2	= 2400; 	// мертвое время 24 us 	
	Par.Time3	= 12000; 	// общее время 120 us
	Par.Time4	= 500;		// строб 5 us
	Par.Time5	= 50;		// частота опроса
	Par.Time6	= 212;		// смещение 2.12 us 
					// (среднее время выполнения прерывания от таймера)
	Par.timeCod	= 10;
	Par.AcBase	= 200; 		//акустическая база (мм)
	Par.SmSpeed	= 1500;		//скорость звука в смоле
	Par.bSod	= 0;		//содержание связующего коррекция
	
	__disable_irq();
	EEPROM_ErasePage(Address, EEPROM_Main_Bank_Select);
	for(i = 0; i < PARAMETRS_CNT; i++){
		if(ptr[i] != 0xFFFFFFFF){
			EEPROM_ProgramWord(
				Address + i * 4, 
				EEPROM_Main_Bank_Select, 
				ptr[i]);
		}
		else{
			EEPROM_ProgramWord(
				Address + i * 4, 
				EEPROM_Main_Bank_Select, 
				0x00000000);		
		}
	}

	__enable_irq();
}




void writeParamToROM(uint32_t Address, uint32_t *ptr)
{
static uint8_t i = 0;

	__disable_irq();
	MDR_PORTC->RXTX ^= (1<<1);
	EEPROM_ErasePage(Address, EEPROM_Main_Bank_Select);
	for(i = 0; i < PARAMETRS_CNT; i++){
		EEPROM_ProgramWord(Address + i * 4, EEPROM_Main_Bank_Select, ptr[i]);
	}
	MDR_PORTC->RXTX ^= (1<<1);
	__enable_irq();
}

int main(void)
{
	CPU_init();
	readParamToRAM(PARAMETRS_ADDR,Par.BUF);
	update_state_time();
	sod_init();
	sod_first_start = 1;	//забить массив текущими показаниями кода времени
	GTimers_Init();
	GTimer_Run(REG_GTIMER);
	Init_All_Ports();
	Timer_Init();
	Uart1_Init();
	RXn = 0;
	ITM_SendChar('r');
	while(1){
		if (GTimer_Get(MB_GTIMER) >= 30){  //3ms
			razbor(RXbuf, RXn);
			RXn = 0;
			GTimer_Stop(MB_GTIMER);
		}
		sod_raschet();
	}
}

