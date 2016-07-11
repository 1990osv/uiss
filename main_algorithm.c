#include "global.h"

#define SWAP(A, B) {unsigned int t = A; A = B; B = t; }
#define RIGHT_DATA	(!(DATA15_PORT -> RXTX & DATA15_PIN))


#define IMPULSE_ZAP	25E-9				// величина импульса заполнения ПЛИС =25E-9 секунд , 40 MHz
						
unsigned int im[SIZE_D];				// массив с колличеством элементов в диапазоне
unsigned int m[SIZE_D];					// массив с диапазонами (хранит только начало диапазона)

volatile unsigned int 	timeCode = 0; 			// *25 ns - время от ПЛИС
volatile unsigned int 	cntTimeCode = 0; 		// счетчик полученных кодов

unsigned int storeTimeCode[STORE_SIZE]; 		// массив кодов
unsigned int rawStoreTimeCode[STORE_SIZE]; 		// массив не отсортированных кодов 
unsigned int i;

volatile double speedCurrent,speedBase,speedResin;	// скорости текущая, начальная, в смоле
volatile double deadTime;				// мертвое время вычитаемое из timeCode
volatile double timePropagation;			// время прохождения сигнала в секундах
volatile int 	resinContent;				// расчитанное содержание связующего

volatile bool beginInitialisation = false;			// флаг начальной установки по команде
volatile bool refreshRawData = false;			// флаг отправки сырых данных

volatile unsigned char status=0; 			// автомат отсчета времени
//volatile unsigned int stateTime[SIZE_TIME]; 		// временные задержки автомата

volatile unsigned int startTime[SIZE_START_TIME];	// временные задержки автомата только стартового импульса
volatile unsigned char start_i, start_j;		// индексы для формирования стартовой последовательности

struct time{						// посчитанные интервалы в тактах с учетом времени выполнения прерывания
	unsigned int deadTime;
	unsigned int strobTime;
	unsigned int mainTime;
} Time;

static void quickSort(unsigned int* s_arr, int first, int last);
static int findAverage(unsigned int* s_arr, unsigned char n);
static unsigned int my_filter(unsigned int * arr, unsigned int data_size,unsigned int step);
			      

void contentInitialisation(void)
{
	cntTimeCode=0;
	beginInitialisation=true;
}

void sod_refreshRawData(void)
{
	cntTimeCode=0;
	refreshRawData=true;	
}

void start_digitize(void)
{
	ADC1_Cmd (ENABLE);
	NVIC_SetPriority(DMA_IRQn,7);
	NVIC_EnableIRQ(DMA_IRQn);	
	adcConvertationEnable = 1;
}


double calc_sod(double s, double sb, double ss)
{
	double s2,sb2,ss2,r1,r2;
	s2 = s * s;
	sb2 = sb *sb;
	ss2 = ss *ss;
	r1 = (s2 - sb2);
	r2 = (ss2 - sb2);
	return 	( r1 / r2 ) * 100;
}

void update_state_time(void)
{
unsigned char i;
	__disable_irq();
	SysTick_Config(GLOBAL_CPU_CLOCK / Par.measureFrequence);

	for(i = 0; i < SIZE_START_TIME; ++i) {
		if(Par.startImpTime[i] != 0)
			startTime[i] = (Par.startImpTime[i] - Par.adjustTime) * 8/10;
		else
			startTime[i] = 0;
	}

	Time.deadTime  = (Par.deadTime  - Par.adjustTime) * 8/10;			// мертвое время 24 us 
	Time.strobTime = (Par.strobTime - Par.adjustTime) * 8/10;			// строб 5 us
	Time.mainTime  = (Par.mainTime - Par.deadTime  - Par.adjustTime) * 8/10;	// общее время 120 us
	__enable_irq();
}


void startGeneralLoop(void)
{
	status = 1;	
	MDR_PORTC->RXTX &= ~(1<<0); // 0
	MDR_TIMER2->PSG = 0x0;
	MDR_TIMER2->ARR = 1; // просто чтобы в прерывание зашел
	MDR_TIMER2->IE = (1 << 1); //разрешение прерывания по совпадению
	MDR_TIMER2->CNTRL = 1; /*счет вверх по TIM_CLK, таймер вкл.*/
}

void stopGeneralLoop(void)
{
	status = 0;
	MDR_TIMER2->IE = (0 << 1); //запрет прерывания по совпадению
	MDR_TIMER2->CNTRL = 0; /*счет вверх по TIM_CLK, таймер выкл.*/
}

void generalLoop(void)
{ 
	switch (status){
	case 1 :{	//
		WR_PORT->RXTX &= ~(1 << WR_PIN);
		MDR_TIMER2->ARR = Time.strobTime;
		status++;
	} break;
	case 2 :{	//READ TIME CODE
		WR_PORT->RXTX |= (1 << WR_PIN);
		if((RIGHT_DATA) && (cntTimeCode < (STORE_SIZE - 1))){
			storeTimeCode[ cntTimeCode++ ] = ((DATAH_PORT->RXTX & DATAH_MASK) << 8) |
							     (DATAL_PORT->RXTX & DATAL_MASK);
		}
		RESDAT_PORT->RXTX |= (1 << RESDAT_PIN);	
		DT_PORT->RXTX |= (1 << DT_PIN);
		MDR_TIMER2->ARR = 1;
		start_i = 0;
		start_j = 0;
		status++;
	} break;
	case 3 :{	//START IMPULSE
		if(start_i == 0){
			START_PORT->RXTX &= ~(1 << START_PIN);
			MDR_TIMER2->ARR = startTime[start_j];
			start_i = 1;
		}
		else{
			START_PORT->RXTX |= (1 << START_PIN);
			start_i = 0;
			MDR_TIMER2->ARR = startTime[start_j]; 
		}
		if(Par.startImpTime[start_j++] == 0){
			RESDAT_PORT->RXTX &= ~(1<<RESDAT_PIN);	//!!!!!!!		
			status++;
		}
	} break;	
	case 4 :{	//
		RZ_PORT->RXTX &= ~(1<<RZ_PIN);		
		MDR_TIMER2->ARR = Time.deadTime; 		
		status++;	
	} break;	
	case 5 :{	//
		DT_PORT->RXTX &= ~(1<<DT_PIN);	
		DOT5_PORT->RXTX &= ~(1<<DOT5_PIN);
		//RESDAT_PORT->RXTX &= ~(1<<RESDAT_PIN);	!!!!
		MDR_TIMER2->ARR = Time.mainTime; 
		status++;
	} break;	
	case 6 :{	//
		DOT5_PORT->RXTX |= (1<<DOT5_PIN);	
		RZ_PORT->RXTX |= (1<<RZ_PIN);
		adcConvertationEnable = 0;
		stopGeneralLoop();
	} break;
	default:
	{
		status = 0;
	} break;
	}
}

void sendDebugData(unsigned int * arr, unsigned int size)
{
static unsigned int i;
	for(i=0;i<size;i++){
		ITM_SendChar((arr[i]>>8 )& 0xFF);
		ITM_SendChar(arr[i] & 0xFF);	
		ITM_SendChar(0xFF);
		GTimer_Reset(DBG_GTIMER);
		while(GTimer_Get(DBG_GTIMER<100)){
		}
	}
}

void computeContent(void)
{
	if(cntTimeCode == (STORE_SIZE - 1)){
		cntTimeCode = 0;
		
		if(refreshRawData){
			for(i = 0; i < STORE_SIZE; ++i){
				rawStoreTimeCode[i] = storeTimeCode[i];
			}
			refreshRawData=0;
		}
		
		/*Par.timeCodA = my_filter(storeTimeCode, STORE_SIZE, 10);*/
		
		timeCode = my_filter(storeTimeCode, STORE_SIZE, 10);
		
		deadTime = (double) Par.deadTime / 100000000; //0.01 uS   2400 = 24 us
		
		timePropagation = (double)timeCode * IMPULSE_ZAP; 
		
		speedCurrent = Par.acusticBase / ((timePropagation - deadTime) * 1000) ;	// *1000 because BASE mm
		
		/*Par.speedCurrent = speedCurrent;*/
		speedResin = (double)Par.speedResin;
		if (speedCurrent < speedBase){
			resinContent = calc_sod(speedCurrent,speedBase,speedResin);
		}
		else{
			resinContent = 0;
		}
		if(beginInitialisation == true){
			speedBase = speedCurrent;
			/*Par.timeCod = Par.timeCodA;*/
			/*Par.speedDry = speedBase;*/
			save_parametrs(PARAMETRS_ADDR,Par.BUF);  	//Сохранение параметров
			beginInitialisation = 0;
		}

    if (PORT_ReadInputDataBit(MD2_PORT, MD2_PIN_F) == Bit_SET){
      if( resinContent != 0)
        Par.resinContent = resinContent;
    }
    else{
      Par.resinContent = resinContent;
    }
    dac_out = Par.resinContent * 37.52; //0x0EA8 = 100 % (3752)		
  }
}

void quickSort(unsigned int * s_arr, int first, int last) //быстрая сортировка n - количество элементов
{
	int i = first, j = last, x = s_arr[(first + last) / 2];

	do {
		while (s_arr[i] < x)
			i++;
		while (s_arr[j] > x)
			j--;

		if (i <= j) {
			if (s_arr[i] > s_arr[j])
				SWAP(s_arr[i], s_arr[j]);
			i++;
			j--;
		}
	} while (i <= j);
	if (i < last)
		quickSort(s_arr, i, last);
	if (first < j)
		quickSort(s_arr, first, j);
}



int findAverage(unsigned int* s_arr, unsigned char n) //n - количество элементов
{
	unsigned char i;
	long summ = 0;

	for (i = 0; i < n; i++) {
		summ += s_arr[i];
	}
	return summ / n;
}

/**
* @brief  Находит наиболее вероятное значение.
* @detailed Находит распределение диапазонов величины в массиве с заданным шагом.
* Находит какой диапазон самый распространенный. Определяет среднее значение в этом диапазоне.
* @param  arr: Массив с данными.
* @param  data_size: Размер массива с данными.
* @param  step: Шаг диапазонов.
* @retval result: Наиболее вероятное значение. Среднее в наибольшем интервале.
*/
unsigned int my_filter(unsigned int * arr, unsigned int data_size, unsigned int step)
{
	//unsigned int im[SIZE_D];		// массив с подсчетами
	//unsigned int m[SIZE_D];		  // массив с диапазонами (хранит только начало диапазона)
	unsigned int iarr[SIZE_D];		// индекс начала диапазона в исходном массиве 
	unsigned char i, j;
	unsigned int iMaxEntryBegin;	// начало большего диапазона в исходном массиве
	unsigned int maxEntry; 			  // для поиска диапазона с большим количеством попаданий	
	unsigned int result;
	
	quickSort(&arr[0],0,STORE_SIZE-1);
	
	for (i = 0; i < SIZE_D; i++) {
		im[i] = 0;
		m[i] = 0;
	}
	j = 0;
	m[j] = arr[0];
	iarr[0] = 0;
	for (i = 0; i < data_size; i++) {
		if (arr[i] <= (m[j] + step)) {
			im[j]++;
		}
		else {
			if (j < (SIZE_D - 1)) {
				j++;
			}
			else {
				return 0;
			}
			m[j] = arr[i];
			iarr[j] = i;
			im[j]++;
		}
	}
	maxEntry = 0;
	for (i = 0; i < SIZE_D; i++) {
		if (maxEntry < im[i]) {
			maxEntry = im[i];
			iMaxEntryBegin = iarr[i];
		}
	}
	result = findAverage(&arr[iMaxEntryBegin], maxEntry);
	return result;
}
