#include "global.h"

#define SWAP(A, B) {unsigned int t = A; A = B; B = t; } //меняет местами
#define FALSE_DATA	(DATA15_PORT->RXTX & DATA15_PIN)

#define STORE_SIZE	50			//величина буфера для обработки
#define IMPULSE_ZAP	25E-9			//величина импульса заполнения ПЛИС =25E-9 секунд , 40 MHz
						
unsigned int im[SIZE_D];			// массив с подсчетами
unsigned int m[SIZE_D];				// массив с диапазонами (хранит только начало диапазона)

volatile unsigned int 	time_code=0; 		//*25 ns - время от ПЛИС
volatile unsigned int 	btime_code=0; 		//*25 ns - время от ПЛИС в сухом материале	
volatile bool 		new_time_code=false; 	// флаг получения новыго кода
volatile unsigned int 	count_time_code=0; 	// счетчик полученных кодов

unsigned int store[STORE_SIZE]; //массив кодов
	
volatile double speed,speed_begin,speed_smol;		//скорости текущая, начальная, в смоле
volatile double deadtime;				// мертвое время вычитаемое из time_code
volatile double time;					// время прохождения сигнала в секундах
volatile int sod;					// расчитанное содержание связующего

volatile bool sod_begin_init=false;			// флаг начальной установки по команде
volatile bool send_raw_data=false;			// флаг отправки сырых данных

volatile unsigned char status=0; 			// автомат отсчета времени
volatile unsigned int state_time[SIZE_TIME]; 		// временные задержки автомата
volatile unsigned int start_time[SIZE_START_TIME];	// временные задержки автомата только стартового импульса
volatile unsigned char start_i=0, start_j=0;

struct time{
//	unsigned int startWidth0;
//	unsigned int startWidth1;
	unsigned int deadTime;
	unsigned int strob;
	unsigned int mainTime;
} Time;



static void qs(unsigned int* s_arr, int first, int last);
//static int average(unsigned int* s_arr, unsigned char n);
static unsigned int my_filter(unsigned int * arr, unsigned int data_size,unsigned int step);
			      

void sod_init(void)
{
	count_time_code=0;
	sod_begin_init=true;
}
void sod_send_raw_data(void)
{
	count_time_code=0;
	send_raw_data=true;	
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
	SysTick_Config(GLOBAL_CPU_CLOCK / Par.Time5);

	for(i = 0; i < SIZE_START_TIME; ++i) {
		start_time[i] = (Par.startTime[i] - Par.Time6)*8/10;
	}

//	Time.startWidth0 = (Par.Time1 - Par.Time6)*8/10;			// длительность стартовых импульсов 7.5 us
//	Time.startWidth1 = (Par.PWMperiod - Par.Time1 -Par.Time6)*8/10;	// время между стартовыми импульсами
	Time.deadTime = (Par.Time2 - Par.Time6)*8/10;				// мертвое время 24 us 
	Time.strob = (Par.Time4-Par.Time6)*8/10;				// строб 5 us
	Time.mainTime =  (Par.Time3 - Par.Time2  -Par.Time6)*8/10;		// общее время 120 us
	__enable_irq();
}


void loop_start(void)
{
	status = 1;	
	MDR_PORTC->RXTX &= ~(1<<0); // 0
	MDR_TIMER2->PSG = 0x0;
	MDR_TIMER2->ARR = 1; // просто чтобы в прерывание зашел
	MDR_TIMER2->IE = (1 << 1); //разрешение прерывания по совпадению
	MDR_TIMER2->CNTRL = 1; /*счет вверх по TIM_CLK, таймер вкл.*/
}

void loop_stop(void)
{
	status = 0;
	MDR_TIMER2->IE = (0 << 1); //запрет прерывания по совпадению
	MDR_TIMER2->CNTRL = 0; /*счет вверх по TIM_CLK, таймер выкл.*/
}

void query_code(void)
{ 
	switch (status){
	case 1 :{
		WR_PORT->RXTX &= ~(1<<WR_PIN);
		MDR_TIMER2->ARR = Time.strob;
		status++;
	} break;
	case 2 :{
		WR_PORT->RXTX |= (1<<WR_PIN);
		
		time_code = ((DATAH_PORT->RXTX & DATAH_MASK) << 8) | (DATAL_PORT->RXTX & DATAL_MASK);
		new_time_code = true & !FALSE_DATA;
		RESDAT_PORT->RXTX |= (1<<RESDAT_PIN);	
		DT_PORT->RXTX |= (1<<DT_PIN);
		MDR_TIMER2->ARR = 1;
		start_i = 0;
		start_j = 0;
		status++;
	} break;
	case 3 :{						//START PULSE
		
		if(start_i == 0){
			START_PORT->RXTX |= (1<<START_PIN);
			MDR_TIMER2->ARR = start_time[start_j]; //Time.startWidth0; 
			start_i = 1;
			start_j ++;
		}
		else{
			START_PORT->RXTX &= ~(1<<START_PIN);
			start_i = 0;
			MDR_TIMER2->ARR = start_time[start_j]; //Time.startWidth1; 
			start_j ++;
			if(start_j >= (Par.PWMcnt * 2))
				status++;
		}
	} break;	
	case 4 :{
		RZ_PORT->RXTX &= ~(1<<RZ_PIN);			
		MDR_TIMER2->ARR = Time.deadTime; 		
		status++;	
	} break;	
	case 5 :{
		DT_PORT->RXTX &= ~(1<<DT_PIN);	
		DOT5_PORT->RXTX &= ~(1<<DOT5_PIN);
		RESDAT_PORT->RXTX &= ~(1<<RESDAT_PIN);
		MDR_TIMER2->ARR = Time.mainTime; 
		status++;
	} break;	
	case 6 :{
		DOT5_PORT->RXTX |= (1<<DOT5_PIN);	
		RZ_PORT->RXTX |= (1<<RZ_PIN);
		loop_stop();
	} break;
	default:
	{
		status = 0;
	} break;
	}
}


void debug_send_data(unsigned int * arr, unsigned int size)
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

char sod_raschet(void)
{
	if(new_time_code){
		store[count_time_code++] = time_code;
		new_time_code=0;
	}	
	if(count_time_code >= STORE_SIZE){
		if(send_raw_data == false){
			qs(&store[0],0,STORE_SIZE-1);
		}
		else{
			debug_send_data(store,STORE_SIZE);
			send_raw_data=0;
		}
		
		/**/Par.timeCodA=my_filter(store,STORE_SIZE,10);
	
		deadtime=(double) Par.Time2 /100000; //0.01 uS *1000 cause BASE mm
		
		time = (double)Par.timeCodA*0.000025; //*1000  cause BASE mm
		
		speed=(Par.AcBase ) / (time);// - deadtime);
		
		/**/Par.cSpeed = speed;

		speed_smol = (double)Par.SmSpeed;
		if (speed < speed_begin){
			sod = calc_sod(speed,speed_begin,speed_smol);
			/**/Par.Sod = Par.bSod + sod;
		}
		else{
			Par.Sod = 0;
		}
		if(sod_begin_init == true){
			speed_begin = speed;
			/**/Par.timeCod = Par.timeCodA;
			/**/Par.bSpeed = speed_begin;
			validation_param();
			writeParamToROM(PARAMETRS_ADDR,Par.BUF);  	//Сохранение параметров
			sod_begin_init=0;
		}
		count_time_code=0;
	}
		
	
	return sod;
}



void qs(unsigned int * s_arr, int first, int last) //быстрая сортировка n - количество элементов
{
int i = first, j = last, x = s_arr[(first + last) / 2];

	do{
		while (s_arr[i] < x)
			i++;
		while (s_arr[j] > x)
			j--;

		if(i <= j) {
			if (s_arr[i] > s_arr[j])
				SWAP(s_arr[i], s_arr[j]);
			i++;
			j--;
		}
	} while (i <= j);
	if (i < last)
		qs(s_arr, i, last);
	if (first < j)
		qs(s_arr, first, j);
}

/*
int average(unsigned int* s_arr, unsigned char n) //n - количество элементов
{
	unsigned char i;
	long summ=0;
	
	for (i = 0; i < n; i++){
		summ += s_arr[i];	
	}
	return summ/n;
}
*/


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
