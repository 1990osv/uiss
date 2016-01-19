#include "global.h"
#include "main_algorithm.h"

extern volatile unsigned int mytime;

volatile unsigned char status=0;


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
	MDR_TIMER2->IE = (0 << 1); //разрешение прерывания по совпадению
	MDR_TIMER2->CNTRL = 1; /*счет вверх по TIM_CLK, таймер вкл.*/
}

/*
 * tick - 80 Mhz   1 us == 80
*/

void query(void){ 
	MDR_PORTC->RXTX ^= (1<<0);
	switch (status){
	case 1 :{
			status = 2;
			MDR_TIMER2->ARR = 300;//7.5 us
	} break;
		
	case 2 :{
			status = 3;
			MDR_TIMER2->ARR = 1920;//24 us
	} break;
	
	case 3 :{
			status = 4;
			MDR_TIMER2->ARR = 2880;//36 us
	} break;	

	case 4 :{
			status = 5;
			MDR_TIMER2->ARR = 400;//5 us
	} break;	

	case 5 :{
			status = 0;								//TODO 
			loopStop();
	} break;	
	
	default:
	{
		status = 0;
	} break;
}
	
	
	
}

