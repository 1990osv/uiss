#include "global.h"
#include "sort.h"

#define STORE_SIZE	50
#define IMPULSE_ZAP	25E-9	//величина такта счетчика 
				//ПЛИС =25E-9

volatile unsigned int time_code=0; //*25 ns - время от ПЛИС
volatile unsigned int btime_code=0; //*25 ns - время от ПЛИС
volatile unsigned char new_time_code=0; // получен новый код
volatile unsigned char count_time_code=0; // счетчик

unsigned int store[STORE_SIZE]; //массив кодов
	
volatile double speed,speed_begin,speed_smol;
volatile double deadtime;
volatile double time;
volatile int sod;

volatile unsigned char sod_begin_init=0;
volatile unsigned char sod_first_start=0;

volatile unsigned char status=0; // автомат отсчета времени
volatile unsigned int state_time[5]; // временные задержки автомата


void sod_init(void)
{
	count_time_code=0;
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
	SysTick_Config(GLOBAL_CPU_CLOCK / Par.Time5); // 80 MHz -> 0.1 sec
	
	state_time[1] = (Par.Time1 - Par.Time6)*8/10;	// стартовый импульс 7.5 us
	state_time[2] = (Par.Time2 - Par.Time6)*8/10;	// мертвое время 24 us
	state_time[3] = (( 	Par.Time3 - 		// общее время 120 us
				Par.Time2  )-Par.Time6)*8/10;
	state_time[4] = (Par.Time4-Par.Time6)*8/10;	// строб 5 us
}


void loopStart(void)
{
	status = 1;	
	MDR_PORTC->RXTX &= ~(1<<0); // 0
	MDR_TIMER2->PSG = 0x0;
	MDR_TIMER2->ARR = 1; // просто чтобы в прерывание зашел
	MDR_TIMER2->IE = (1 << 1); //разрешение прерывания по совпадению
	MDR_TIMER2->CNTRL = 1; /*счет вверх по TIM_CLK, таймер вкл.*/
}

void loopStop(void)
{
	status = 0;
	MDR_TIMER2->IE = (0 << 1); //запрет прерывания по совпадению
	MDR_TIMER2->CNTRL = 0; /*счет вверх по TIM_CLK, таймер выкл.*/
}

void query(void){ 
	MDR_PORTC->RXTX ^= (1<<0);
	switch (status){
	case 1 :{
		WR_PORT->RXTX &= ~(1<<WR_PIN);
		MDR_TIMER2->ARR = state_time[4];
		status++;
	} break;
	case 2 :{
		WR_PORT->RXTX |= (1<<WR_PIN);
		
		time_code = 	((DATAH_PORT->RXTX & DATAH_MASK) << 8) | 
				(DATAL_PORT->RXTX & DATAL_MASK);
		new_time_code = 1;
		RESDAT_PORT->RXTX |= (1<<RESDAT_PIN);	
		DT_PORT->RXTX |= (1<<DT_PIN);
		MDR_TIMER2->ARR = 1;//state_time[4];		
		status++;
	} break;
	case 3 :{
		RESDAT_PORT->RXTX &= ~(1<<RESDAT_PIN);
		START_PORT->RXTX &= ~(1<<START_PIN);
		MDR_TIMER2->ARR = state_time[1];		
		status++;		
	} break;	
	case 4 :{
		START_PORT->RXTX |= (1<<START_PIN);
		RZ_PORT->RXTX &= ~(1<<RZ_PIN);			
		MDR_TIMER2->ARR = state_time[2];		
		status++;	
	} break;	
	case 5 :{
		DT_PORT->RXTX &= ~(1<<DT_PIN);	
		
		MDR_TIMER2->ARR = state_time[3];
		status++;
	} break;	
	case 6 :{
		RZ_PORT->RXTX |= (1<<RZ_PIN);
		loopStop();
	} break;
	default:
	{
		status = 0;
	} break;
	}
}

unsigned int mabs(int a)
{
	if (a > 0){
		return a;
	}
	else{
		return a*(-1);
	}
		
	
}

char sod_raschet(void)
{
uint8_t i;
	if(new_time_code && !(DATA15_PORT->RXTX & DATA15_PIN)){
		store[count_time_code++] = time_code;
		
		if(count_time_code >= STORE_SIZE){
			qs(&store[0],0,STORE_SIZE-1);
			/**/Par.timeCodA=average(&store[5],STORE_SIZE-10);
			time = (double)Par.timeCodA*0.000025; //*1000  BASE mm
			deadtime=(double) Par.Time2 /100000; //0.01 uS *1000  BASE mm
			
			speed=(Par.AcBase ) / (time - deadtime);
			
			/**/Par.cSpeed = speed;

			speed_smol = (double)Par.SmSpeed;
			if (speed < speed_begin){
				sod = calc_sod(speed,speed_begin,speed_smol);
				/**/Par.Sod = Par.bSod + sod;
			}
			else
			{
				Par.Sod = 100;
			}
			if(sod_begin_init){
				speed_begin = speed;
				/**/Par.timeCod = Par.timeCodA;
				/**/Par.bSpeed = speed_begin;
				
				for(i=0;i<STORE_SIZE;i++){
					ITM_SendChar((store[i]>>8 )& 0xFF);
					ITM_SendChar(store[i] & 0xFF);	
					ITM_SendChar(0xFF);

				}
				sod_begin_init=0;
			}
			count_time_code=0;
		}
		new_time_code=0;
	}
	return sod;
}
