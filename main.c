#include "MDR32F9Qx_config.h"
#include "MDR32Fx.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_timer.h"


#include "init.h"


volatile char SEC=0;
volatile unsigned int temp;

volatile unsigned int mytime=0;

//Обработчик прерывания (на примере Таймера 1)
void Timer1_IRQHandler(void){

  //Исполняемый код
  mytime++;
  MDR_TIMER1->CNT = 0x0000;
  MDR_TIMER1->STATUS &= ~(1 << 1);
  NVIC_ClearPendingIRQ(Timer1_IRQn);
	
	MDR_PORTC->RXTX ^= (1<<1);
	
}

void SysTick_Handler(){
	
	//if ( SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)
			//MDR_PORTC->RXTX ^= (1<<0);
	//MDR_TIMER2->CNTRL = 1; /*счет вверх по TIM_CLK, таймер вкл.*/
  
//	MDR_TIMER2->CNT = 0x0000;
//	//NVIC_EnableIRQ(Timer2_IRQn);  
//	PORT_SetBits(MDR_PORTC, PORT_Pin_0);
//	for(cnt=0;cnt<25;cnt++);
//	PORT_ResetBits(MDR_PORTC, PORT_Pin_0);	
//	for(cnt=0;cnt<25;cnt++);	
//	PORT_SetBits(MDR_PORTC, PORT_Pin_0);
}


void Timer2_IRQHandler(void){
  //Исполняемый код
	//PORT_SetBits(MDR_PORTC, PORT_Pin_0);
	MDR_PORTC->RXTX &= ~(1<<0);
	MDR_TIMER2->CNTRL = 0;/*счет вверх по TIM_CLK, таймер вЫЫЫЫкл.*/

  MDR_TIMER2->STATUS &= ~(1 << 1);
	NVIC_ClearPendingIRQ(Timer2_IRQn);
	NVIC_DisableIRQ(Timer2_IRQn);
}


int main(void){

unsigned char cnt;

  CPU_init();
	//ITM_SendChar('c');
	Init_All_Ports();
	
	Timer_Init();
	//SysTick_Config(4000000); // 80 MHz -> 0.5 sec
	
	
  while(1){
				if(mytime>10000){
						mytime=0;
						MDR_TIMER2->CNT = 0x0020;
					  MDR_TIMER2->CNTRL = 1; /*счет вверх по TIM_CLK, таймер вкл.*/
						NVIC_EnableIRQ(Timer2_IRQn);
						MDR_PORTC->RXTX |= (1<<0);//PORT_ResetBits(MDR_PORTC, PORT_Pin_0);
//						__asm{nop};//for(cnt=0;cnt<1;cnt++);
//						MDR_PORTC->RXTX |= (1<<0);//PORT_SetBits(MDR_PORTC, PORT_Pin_0);	
//						__asm{nop};//for(cnt=0;cnt<1;cnt++);	
//						MDR_PORTC->RXTX &= ~(1<<0);//PORT_ResetBits(MDR_PORTC, PORT_Pin_0);
				}
  }

}

