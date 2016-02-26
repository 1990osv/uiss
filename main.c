#include "global.h"

union __all Par;

uint32_t dac_temp=0;

void readParamIntoRAM(uint32_t Address, uint32_t *ptr)
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
	Par.Time1	= 1000;  	// стартовый импульс 10.0 us
	Par.Time2	= 2400; 	// мертвое время 24.0 us 	
	Par.Time3	= 12000; 	// общее время 120.0 us
	Par.Time4	= 500;		// строб 5.0 us
	Par.Time5	= 50;		// частота опроса
	Par.Time6	= 212;		// смещение 2.12 us 
					// (среднее время выполнения прерывания от таймера)
	Par.timeCod	= 10;
	Par.AcBase	= 200; 		//акустическая база (мм)
	Par.SmSpeed	= 1500;		//скорость звука в смоле
	Par.bSod	= 0;		//содержание связующего коррекция
	
	Par.PWMperiod	= 1500;
	Par.PWMcnt	= 1;
	
	for(i = 0; i < SIZE_START_TIME; ++i) {
		Par.startTime[i] = 1000;
	}
	
	
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

void validation_param(void)
{
	if((Par.Time1<500)||(Par.Time1>2000)){
		Par.Time1	= 750;		// стартовый импульс 7.5 us
		writeParamToROM(PARAMETRS_ADDR,Par.BUF);  	//Сохранение параметров
	}
	if((Par.Time2<500)||(Par.Time2>20000)){
		Par.Time2	= 2400; 	// мертвое время 24 us 	
		writeParamToROM(PARAMETRS_ADDR,Par.BUF);  	//Сохранение параметров
	}
	if((Par.Time3<500)||(Par.Time3>20000)){
		Par.Time3	= 12000; 	// общее время 120 us
		writeParamToROM(PARAMETRS_ADDR,Par.BUF);  	//Сохранение параметров
	}
	if((Par.Time4<500)||(Par.Time4>20000)){
		Par.Time4	= 500;		// строб 5 us
		writeParamToROM(PARAMETRS_ADDR,Par.BUF);  	//Сохранение параметров
	}
	if((Par.Time5<10)||(Par.Time5>200)){
		Par.Time5	= 50;		// частота опроса
		writeParamToROM(PARAMETRS_ADDR,Par.BUF);  	//Сохранение параметров
	}
	if((Par.Time6<100)||(Par.Time6>1000)){
		Par.Time6	= 212;		// смещение 2.12 us 
						// (среднее время выполнения прерывания от таймера)
		writeParamToROM(PARAMETRS_ADDR,Par.BUF);  	//Сохранение параметров
	}
	if((Par.AcBase<100)||(Par.AcBase>1000)){
		Par.AcBase	= 200; 		//акустическая база (мм)
		writeParamToROM(PARAMETRS_ADDR,Par.BUF);  	//Сохранение параметров
	}
	if((Par.SmSpeed<500)||(Par.SmSpeed>2000)){
		Par.SmSpeed	= 1500;		//скорость звука в смоле
		writeParamToROM(PARAMETRS_ADDR,Par.BUF);  	//Сохранение параметров
	}
	if(Par.bSod<-100){
		Par.bSod	= -100;		//содержание связующего коррекция
		writeParamToROM(PARAMETRS_ADDR,Par.BUF);  	//Сохранение параметров
	}
	if(Par.bSod>100){
		Par.bSod	= 100;		//содержание связующего коррекция
		writeParamToROM(PARAMETRS_ADDR,Par.BUF);  	//Сохранение параметров
	}

}


void uncrash_delay(void)
{
	while(GTimer_Get(DAC_GTIMER) <= 2000){ //на частоте 8 MHz секундная задержка
	}
	GTimer_Reset(DAC_GTIMER);
	MDR_PORTC->RXTX ^= (1<<1);
	while(GTimer_Get(DAC_GTIMER) <= 2000){ //на частоте 8 MHz секундная задержка
	}
	GTimer_Reset(DAC_GTIMER);
	MDR_PORTC->RXTX ^= (1<<1);	
}

int main(void)
{
	Init_All_Ports();
	Tim1_Tim2_Init();
	GTimers_Init();
	GTimer_Run(DAC_GTIMER);
	
	uncrash_delay();

	HSE_80MHz_init();
	
	readParamIntoRAM(PARAMETRS_ADDR,Par.BUF);
	validation_param();
	update_state_time();
	sod_init();
	
	Uart1_Init();
	mDAC_Init();

	while(true){
		modbus_process();
		sod_raschet();
		if (GTimer_Get(DAC_GTIMER) >= 1000){  //100ms
			DAC2_SetData(dac_temp++);
			if(dac_temp>0x0FFF)
				dac_temp=0;
			GTimer_Reset(DAC_GTIMER);
		}
	}
}

