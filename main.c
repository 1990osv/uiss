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
uint32_t dac_temp=0;


//const unsigned char size = 20;	// максимальное количество диапазонов
unsigned int im[SIZE_D];		// массив с подсчетами
unsigned int m[SIZE_D];			// массив с диапазонами (хранит только начало диапазона)


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
	
	Par.PWMperiod	= 1500;
	Par.PWMcnt	= 1;
	
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
	if((Par.Time1<500)||(Par.Time1>20000)){
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
	if((Par.Time5<10)||(Par.Time2>200)){
		Par.Time5	= 50;		// частота опроса
		writeParamToROM(PARAMETRS_ADDR,Par.BUF);  	//Сохранение параметров
	}
	if((Par.Time6<200)||(Par.Time6>1000)){
		Par.Time6	= 212;		// смещение 2.12 us 
						// (среднее время выполнения прерывания от таймера)
		writeParamToROM(PARAMETRS_ADDR,Par.BUF);  	//Сохранение параметров
	}
	if((Par.AcBase<100)||(Par.AcBase>1000)){
		Par.AcBase	= 200; 		//акустическая база (мм)
		writeParamToROM(PARAMETRS_ADDR,Par.BUF);  	//Сохранение параметров
	}
	if((Par.SmSpeed<1000)||(Par.SmSpeed>2000)){
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

int main(void)
{
	Init_All_Ports();
	Tim1_Tim2_Init();
	GTimers_Init();
	GTimer_Run(DAC_GTIMER);
	while(GTimer_Get(DAC_GTIMER) <= 2000){ //на частоте 8 MHz секундная задержка
	}
	GTimer_Reset(DAC_GTIMER);
	MDR_PORTC->RXTX ^= (1<<1);
	while(GTimer_Get(DAC_GTIMER) <= 2000){ //на частоте 8 MHz секундная задержка
	}
	GTimer_Reset(DAC_GTIMER);
	MDR_PORTC->RXTX ^= (1<<1);	

	HSE_80MHz_init();
	
	readParamIntoRAM(PARAMETRS_ADDR,Par.BUF);
	validation_param();
	update_state_time();
	sod_init();
	
	Uart1_Init();
	mDAC_Init();
	RXn = 0;
	while(1){
		if (GTimer_Get(MB_GTIMER) >= 30){  //3ms 
			razbor(RXbuf, RXn);
			RXn = 0;
			GTimer_Stop(MB_GTIMER);
		}
		if (GTimer_Get(DAC_GTIMER) >= 10000){  //1000ms
			DAC2_SetData(dac_temp++);
			if(dac_temp>0x0FFF)dac_temp=0;
			GTimer_Reset(DAC_GTIMER);
		}
		
		sod_raschet();
	}
	
}

/*быстрая сортировка
*/
void qs(unsigned int* s_arr, int first, int last) //n - количество элементов
{
int i = first, j = last, x = s_arr[(first + last) / 2];

	do{
		while (s_arr[i] < x) i++;
		while (s_arr[j] > x) j--;

		if(i <= j){
			if (s_arr[i] > s_arr[j]) SWAP(s_arr[i], s_arr[j]);
			i++;
			j--;
		}
	} while (i <= j);
	if (i < last)
		qs(s_arr, i, last);
	if (first < j)
		qs(s_arr, first, j);
}

int average(unsigned int* s_arr, unsigned char n) //n - количество элементов
{
	unsigned char i;
	long summ=0;
	
	for (i = 0; i < n; i++){
		summ += s_arr[i];	
	}
	return summ/n;
}


/**
  * @brief  Находит наиболее вероятное значение 
  * @detailed Находит распределение диапазонов величины в массиве с заданным шагом.
  * Находит какой диапазон самый распространенный. Определяет среднее значение в этом диапазоне.
  * @param  arr: Массив с данными
  * @param  data_size: Размер массива с данными
  * @param  step: Шаг диапазонов
  * @retval result: Наиболее вероятное значение
  */
unsigned int my_filter(unsigned int * arr, unsigned int data_size, unsigned int step)
{

	unsigned char i, j;
//	unsigned int current;		// текущая точка в окрестностях которой делаем подсчет

	unsigned int maxi,max;		
	unsigned int result;
	for (i = 0; i < 20; i++) {
		im[i] = 0;
		m[i] = 0;
	}
	j = 0;
	m[j] = arr[0];
	for (i = 0; i < data_size; i++) {
		if (arr[i] <= (m[j] + step)) {
			im[j]++;
		}
		else {
			if (j < (SIZE_D-1)) {
				j++;
			}
			else {
				return 0;
			}
			m[j] = arr[i];
			im[j]++;
		}
	}
	for (i = 0, maxi=0; i < 20; i++) {
		if (max < im[i]) {
			max = im[i];
			maxi = i;
		}
	}
	result = (m[maxi] + m[maxi] + step) / 2;
	return result;
}
