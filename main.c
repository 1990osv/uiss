#include "global.h"


volatile char SEC=0;
volatile unsigned int temp;

volatile unsigned int mytime=0;


uint8_t RXbuf[16];
uint8_t TXbuf[16];
uint8_t RXn;
uint8_t TXn,TXi;


uint8_t DataByte=0x30;
//static uint8_t ReciveByte[16];
uint32_t i;




int main(void)
{
	CPU_init();
	//dbg_print("Hello world!");	
	GTimers_Init();
	GTimer_Run(REG_GTIMER);

	// Вывод в отладочный порт строки

	Init_All_Ports();
	
	Timer_Init();
	SysTick_Config(GLOBAL_CPU_CLOCK/10); // 80 MHz -> 0.1 sec

	Uart1_Init();
	RXn=0;
	__disable_irq();
	EEPROM_ErasePage(Address, EEPROM_Info_Bank_Select);
	EEPROM_ProgramWord(Address, EEPROM_Info_Bank_Select, data32);
	__enable_irq();
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
		
		if (GTimer_Get(MB_GTIMER)>=100){  //10ms
			//if(GetStatus()!=MB_COMPLETE)
			razbor(RXbuf, RXn);
			RXn=0;
			GTimer_Stop(MB_GTIMER);
		}
		///!!!!!!!!!!!!!!FLASH_PROG_FREQ_MHZ ->>>> 8.0 

	}
}

