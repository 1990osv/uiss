#include "global.h"
#include "main_algorithm.h"

extern volatile unsigned int mytime;

volatile unsigned char status=0;


//#define DELAY_LOOP_CYCLES               (8UL)
//#define GET_US_LOOPS(N)                 ((uint32_t)((float)(N) * ( FLASH_PROG_FREQ_MHZ / 4) / DELAY_LOOP_CYCLES))



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

/*
 * tick - 80 Mhz   1 us == 80

1:		START 	= 	1
		RZ	=	1
2:		START	=	0
3:		DT	=	1
4:		WR	=	1
5:		WR	=	0
	READ_DATA
6:		RSDAT	=	1
7:		RSDAT	=	0
8:		DT	= 	0
		RZ	=	0
*/
void read_code(void)
{
	
}


void query(void){ 
	MDR_PORTC->RXTX ^= (1<<0);
	switch (status){
	case 1 :{
		status = 2;
		MDR_TIMER2->ARR = (Par.Time1-Par.Time6)*8/10;// 7.5 us
	} break;
		
	case 2 :{
		status = 3;
		MDR_TIMER2->ARR = (Par.Time2-Par.Time6)*8/10;// 24 us
	} break;
	
	case 3 :{
		status = 4;
		MDR_TIMER2->ARR = (Par.Time3-Par.Time6)*8/10;// 36 us
	} break;	

	case 4 :{
		status = 5;
		MDR_TIMER2->ARR = (Par.Time4-Par.Time6)*8/10;// 5 us
	} break;	

	case 5 :{
		read_code();
		MDR_TIMER2->ARR = (Par.Time4-Par.Time6)*8/10;// 5 us
		status = 6;
	} break;	
	
	case 6 :{
		MDR_TIMER2->ARR = (Par.Time4-Par.Time6)*8/10;// 5 us
		status = 7;
	} break;
	
	case 7 :{
		MDR_TIMER2->ARR = (( 	Par.Time5 - 
					Par.Time1 - 
					Par.Time2 - 
					Par.Time3 -
					Par.Time4 * 3)-Par.Time6)*8/10; //120 us -
		status = 8;
	} break;
	
	case 8 :{
		loopStop();
	} break;
	
	default:
	{
		status = 0;
	} break;
}
	
	
	
}

