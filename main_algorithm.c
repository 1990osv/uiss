#include "global.h"
#include "main_algorithm.h"

volatile unsigned char soderjanie=0;
volatile unsigned char status=0;
volatile unsigned int time_code=0; //*25 ns - время от ПЛИС
volatile unsigned char new_time_code=0; // получен новый код
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
		//PORT_SetBits(START_PORT,START_PIN_E);
		//PORT_SetBits(RZ_PORT,RZ_PIN_F);
		status = 2;
		MDR_TIMER2->ARR = (Par.Time1-Par.Time6)*8/10;// 7.5 us
	} break;
		
	case 2 :{
		//PORT_ResetBits(START_PORT,START_PIN_E);
		status = 3;
		MDR_TIMER2->ARR = (Par.Time2-Par.Time6)*8/10;// 24 us
	} break;
	
	case 3 :{
		//PORT_SetBits(DT_PORT,DT_PIN_F);
		status = 4;
		MDR_TIMER2->ARR = (Par.Time3-Par.Time6)*8/10;// 36 us
	} break;	

	case 4 :{
		//PORT_SetBits(WR_PORT,WR_PIN_F);
		status = 5;
		MDR_TIMER2->ARR = (Par.Time4-Par.Time6)*8/10;// 5 us
	} break;	

	case 5 :{
		//PORT_ResetBits(WR_PORT,WR_PIN_F);
		time_code = 	((DATAH_PORT->RXTX & DATAH_MASK) << 8) | 
				(DATAL_PORT->RXTX & DATAL_MASK);
		new_time_code = 1;
		MDR_TIMER2->ARR = (Par.Time4-Par.Time6)*8/10;// 5 us
		status = 6;
	} break;	
	
	case 6 :{
		//PORT_SetBits(RESDAT_PORT,RESDAT_PIN_F);
		MDR_TIMER2->ARR = (Par.Time4-Par.Time6)*8/10;// 5 us
		status = 7;
	} break;
	
	case 7 :{
		//PORT_ResetBits(RESDAT_PORT,RESDAT_PIN_F);
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

//unsigned char raschet(void)
//{
//	
//}
