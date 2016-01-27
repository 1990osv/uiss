#include "global.h"
volatile char SEC=0;
volatile unsigned int temp;

volatile unsigned int mytime=0;


uint8_t RXbuf[16];
uint8_t TXbuf[16];
uint8_t RXn;
uint8_t TXn,TXi;

uint32_t i;

uint32_t RAMParam[PARAMETRS_CNT];

void readParamToRAM(uint32_t Address);
void writeDefaultParamToROM(uint32_t Address);
void writeParamToROM(uint32_t Address);
	

void readParamToRAM(uint32_t Address)
{
static uint8_t i = 0;
	__disable_irq();
	for(i = 0; i < PARAMETRS_CNT; i++){
		RAMParam[i]=EEPROM_ReadWord(Address + i * 4, EEPROM_Main_Bank_Select);
		if(RAMParam[i] == 0xFFFFFFFF)
			writeDefaultParamToROM(Address);
	}
	__enable_irq();
}

void writeDefaultParamToROM(uint32_t Address)
{
static uint8_t i = 0;
	__disable_irq();
	EEPROM_ErasePage(Address, EEPROM_Main_Bank_Select);
	for(i = 0; i < PARAMETRS_CNT; i++){
		EEPROM_ProgramWord(Address + i * 4, EEPROM_Main_Bank_Select, 0x00000000);
		RAMParam[i] = 0x00000000;
	}
	__enable_irq();
}


void writeParamToROM(uint32_t Address)
{
static uint8_t i = 0;
	__disable_irq();
	EEPROM_ErasePage(Address, EEPROM_Main_Bank_Select);
	for(i = 0; i < PARAMETRS_CNT; i++){
		EEPROM_ProgramWord(Address + i * 4, EEPROM_Main_Bank_Select, RAMParam[i]);
	}
	__enable_irq();
}

int main(void)
{
	CPU_init();
	//dbg_print("Hello world!");	
	
	readParamToRAM(PARAMETRS_ADDR);
	
	RAMParam[0]=0xAAAAAAAA;
	RAMParam[1]=0xBBBBBBBB;
	RAMParam[2]=0xCCCCCCCC;
	RAMParam[3]=0xDDDDDDDD;
	RAMParam[4]=0xEEEEEEEE;
	RAMParam[PARAMETRS_CNT-1]=0x11111111;
	
	writeParamToROM(PARAMETRS_ADDR);
	
	GTimers_Init();
	GTimer_Run(REG_GTIMER);

	// Вывод в отладочный порт строки

	Init_All_Ports();
	
	Timer_Init();
	SysTick_Config(GLOBAL_CPU_CLOCK / 10); // 80 MHz -> 0.1 sec

	Uart1_Init();
	RXn = 0;

	while(1){
//		if (GTimer_Get(REG_GTIMER)>=1000){ //10 == 1 ms
//			GTimer_Reset(REG_GTIMER);
//			if(DataByte <= 0x39){
//				MDR_PORTC->RXTX ^= (1<<1);
//				/* Check TXFE flag */
//				while (UART_GetFlagStatus (MDR_UART1, UART_FLAG_TXFE)!= SET){
//				}
//				UART_SendData (MDR_UART1, (uint16_t)(TXbuf[]));
//				TXn++;
//			}
//		}
		
		if (GTimer_Get(MB_GTIMER) >= 100){  //10ms
			//if(GetStatus()!=MB_COMPLETE)
			razbor(RXbuf, RXn);
			RXn = 0;
			GTimer_Stop(MB_GTIMER);
		}
		///!!!!!!!!!!!!!!FLASH_PROG_FREQ_MHZ ->>>> 8.0 

	}
}

