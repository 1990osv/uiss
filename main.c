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
	Par.Time3	= 3600; 	// ожидание 36 us
	Par.Time4	= 500;		// строб 5 us
	Par.Time5	= 11500;	// общее время ? реально ~120 us
	Par.Time6	= 175;		// смещение 1.75 us
	Par.myFloat	= 10;
	__disable_irq();
	EEPROM_ErasePage(Address, EEPROM_Main_Bank_Select);
	for(i = 0; i < PARAMETRS_CNT; i++){
		if(ptr[i] != 0xFFFFFFFF){
			EEPROM_ProgramWord(Address + i * 4, EEPROM_Main_Bank_Select, ptr[i]);
		}
		else{
			EEPROM_ProgramWord(Address + i * 4, EEPROM_Main_Bank_Select, 0x00000000);		
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
	//dbg_print("Hello world!");	
	
	readParamToRAM(PARAMETRS_ADDR,Par.BUF);
	
	GTimers_Init();
	GTimer_Run(REG_GTIMER);

	Init_All_Ports();
	
	Timer_Init();
	SysTick_Config(GLOBAL_CPU_CLOCK / 10); // 80 MHz -> 0.1 sec

	Uart1_Init();
	RXn = 0;

	while(1){
		
		if (GTimer_Get(MB_GTIMER) >= 100){  //10ms
			//if(GetStatus()!=MB_COMPLETE)
			razbor(RXbuf, RXn);
			RXn = 0;
			GTimer_Stop(MB_GTIMER);
		}
		///!!!!!!!!!!!!!!FLASH_PROG_FREQ_MHZ ->>>> 8.0 

	}
}

