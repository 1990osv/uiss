#include "global.h"

unsigned int dac_out;

union __all Par;

//static uint32_t dac_out=0;
unsigned char adcConvertationEnable;
uint16_t ADCConvertedValue[ADC_DATA_SIZE];

DMA_ChannelInitTypeDef DMA_InitStr;
DMA_CtrlDataInitTypeDef DMA_PriCtrlStr;
DMA_CtrlDataInitTypeDef DMA_AltCtrlStr;

void adc_initialisation(void)
{
	ADC_InitTypeDef sADC;
	ADCx_InitTypeDef sADCx;

	RST_CLK_PCLKcmd((RST_CLK_PCLK_RST_CLK | RST_CLK_PCLK_DMA | RST_CLK_PCLK_ADC),ENABLE);
	RST_CLK_PCLKcmd((RST_CLK_PCLK_SSP1 | RST_CLK_PCLK_SSP2),ENABLE);

	
	/* DMA Configuration */
	/* Reset all settings */
	DMA_DeInit();
	DMA_StructInit(&DMA_InitStr);
	/* Set Primary Control Data */
	DMA_PriCtrlStr.DMA_SourceBaseAddr = (uint32_t)(&(MDR_ADC->ADC1_RESULT));
	DMA_PriCtrlStr.DMA_DestBaseAddr = (uint32_t)ADCConvertedValue;
	DMA_PriCtrlStr.DMA_SourceIncSize = DMA_SourceIncNo;
	DMA_PriCtrlStr.DMA_DestIncSize = DMA_DestIncHalfword;
	DMA_PriCtrlStr.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_PriCtrlStr.DMA_Mode = DMA_Mode_PingPong;
	DMA_PriCtrlStr.DMA_CycleSize = ADC_DATA_SIZE;
	DMA_PriCtrlStr.DMA_NumContinuous = DMA_Transfers_1;
	DMA_PriCtrlStr.DMA_SourceProtCtrl = DMA_SourcePrivileged;
	DMA_PriCtrlStr.DMA_DestProtCtrl = DMA_DestPrivileged;

	/* Set Alternate Control Data */
	DMA_AltCtrlStr.DMA_SourceBaseAddr = (uint32_t)(&(MDR_ADC->ADC1_RESULT));
	DMA_AltCtrlStr.DMA_DestBaseAddr   = (uint32_t)ADCConvertedValue;
	DMA_AltCtrlStr.DMA_SourceIncSize = DMA_SourceIncNo;
	DMA_AltCtrlStr.DMA_DestIncSize = DMA_DestIncHalfword;
	DMA_AltCtrlStr.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_AltCtrlStr.DMA_Mode = DMA_Mode_PingPong;
	DMA_AltCtrlStr.DMA_CycleSize = ADC_DATA_SIZE;
	DMA_AltCtrlStr.DMA_NumContinuous = DMA_Transfers_1;
	DMA_AltCtrlStr.DMA_SourceProtCtrl = DMA_SourcePrivileged;
	DMA_AltCtrlStr.DMA_DestProtCtrl = DMA_DestPrivileged;

	/* Set Channel Structure */
	DMA_InitStr.DMA_PriCtrlData = &DMA_PriCtrlStr;
	DMA_InitStr.DMA_AltCtrlData = &DMA_AltCtrlStr;
	DMA_InitStr.DMA_Priority = DMA_Priority_Default;
	DMA_InitStr.DMA_UseBurst = DMA_BurstClear;
	DMA_InitStr.DMA_SelectDataStructure = DMA_CTRL_DATA_PRIMARY;

	/* Init DMA channel ADC1 */
	DMA_Init(DMA_Channel_ADC1, &DMA_InitStr);

	/* Enable dma_req or dma_sreq to generate DMA request */
	MDR_DMA->CHNL_REQ_MASK_CLR = (1<<DMA_Channel_ADC1);
	MDR_DMA->CHNL_USEBURST_CLR = (1<<DMA_Channel_ADC1);

	/* Enable DMA channel ADC1 */
	DMA_Cmd(DMA_Channel_ADC1, ENABLE);

	/* ADC Configuration */
	/* Reset all ADC settings */
	ADC_DeInit();
	ADC_StructInit(&sADC);

	sADC.ADC_SynchronousMode      = ADC_SyncMode_Independent;
	sADC.ADC_StartDelay           = 0;
	sADC.ADC_TempSensor           = ADC_TEMP_SENSOR_Enable;
	sADC.ADC_TempSensorAmplifier  = ADC_TEMP_SENSOR_AMPLIFIER_Enable;
	sADC.ADC_TempSensorConversion = ADC_TEMP_SENSOR_CONVERSION_Enable;
	sADC.ADC_IntVRefConversion    = ADC_VREF_CONVERSION_Disable;
	sADC.ADC_IntVRefTrimming      = 1;
	ADC_Init (&sADC);

	/* ADC1 Configuration */
	ADCx_StructInit (&sADCx);
	sADCx.ADC_ClockSource      = ADC_CLOCK_SOURCE_CPU;
	sADCx.ADC_SamplingMode     = ADC_SAMPLING_MODE_CICLIC_CONV;
	sADCx.ADC_ChannelSwitching = ADC_CH_SWITCHING_Disable;
	sADCx.ADC_ChannelNumber    = ADC_CH_ADC7; //ADC_CH_TEMP_SENSOR
	sADCx.ADC_Channels         = 0;
	sADCx.ADC_LevelControl     = ADC_LEVEL_CONTROL_Disable;
	sADCx.ADC_LowLevel         = 0;
	sADCx.ADC_HighLevel        = 0;
	sADCx.ADC_VRefSource       = ADC_VREF_SOURCE_INTERNAL;
	sADCx.ADC_IntVRefSource    = ADC_INT_VREF_SOURCE_INEXACT;
	sADCx.ADC_Prescaler        = ADC_CLK_div_2;
	sADCx.ADC_DelayGo          = 7;
	ADC1_Init (&sADCx);

	/* Enable ADC1 EOCIF and AWOIFEN interupts */
	ADC1_ITConfig((ADCx_IT_END_OF_CONVERSION  | ADCx_IT_OUT_OF_RANGE), DISABLE);

}

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
	if((Par.Time2<1)||(Par.Time2>20000)){
		Par.Time2	= 18; 			// мертвое время 1,8 us 	
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
	if((Par.Time5<10)||(Par.Time5>1000)){
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
	DAC2_SetData(0);
	
	adcConvertationEnable = 0;	
	adc_initialisation();
	
	while(true){
		modbus_process();
		sod_raschet();
		DAC2_SetData(dac_out);
//		if (GTimer_Get(DAC_GTIMER) >= 10000){  //1000ms
//			DAC2_SetData(dac_out);
//			GTimer_Reset(DAC_GTIMER);
//		}
	}
}

