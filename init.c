#include "global.h"
#include "init.h"

#define ALL_PORTS_CLK (RST_CLK_PCLK_PORTA | RST_CLK_PCLK_PORTB | \
                       RST_CLK_PCLK_PORTC | RST_CLK_PCLK_PORTD | \
                       RST_CLK_PCLK_PORTE | RST_CLK_PCLK_PORTF)


void CPU_init (void)
{ // 80 MHz
  MDR_RST_CLK->HS_CONTROL = 0x01; /* вкл. HSE осцилятора */
  while ((MDR_RST_CLK->CLOCK_STATUS & (1 << 2)) == 0x00); /* ждем пока HSE выйдет в рабочий режим */

  MDR_RST_CLK->PLL_CONTROL = ((1 << 2) | (9 << 8)); //вкл. PLL | коэф. умножения = 10
  while((MDR_RST_CLK->CLOCK_STATUS & 0x02) != 0x02); //ждем когда PLL выйдет в раб. режим

  MDR_RST_CLK->CPU_CLOCK = (2 /*источник для CPU_C1*/
  | (1 << 2) /*источник для CPU_C2*/
  | (0 << 4) /*предделитель для CPU_C3*/
  | (1 << 8));/*источник для HCLK*/
}

void Init_All_Ports(void)
{
PORT_InitTypeDef PORT_InitStructure;

  /* Enable the RTCHSE clock on all ports */
  RST_CLK_PCLKcmd(ALL_PORTS_CLK, ENABLE);

  /* Configure all ports to the state as after reset, i.e. low power consumption */
  PORT_StructInit(&PORT_InitStructure);
  PORT_Init(MDR_PORTA, &PORT_InitStructure);
  PORT_Init(MDR_PORTB, &PORT_InitStructure);
  PORT_Init(MDR_PORTC, &PORT_InitStructure);
  PORT_Init(MDR_PORTD, &PORT_InitStructure);
  PORT_Init(MDR_PORTE, &PORT_InitStructure);
  PORT_Init(MDR_PORTF, &PORT_InitStructure);

  /* Disable the RTCHSE clock on all ports */
  RST_CLK_PCLKcmd(ALL_PORTS_CLK, DISABLE);
	
  /* Enables the RTCHSE clock on PORTB, PORTC and PORTE */
  RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB | RST_CLK_PCLK_PORTC | RST_CLK_PCLK_PORTE, ENABLE);

  /* Configure PORTC pins 0,1 for output to switch LEDs on/off */

  PORT_InitStructure.PORT_Pin   = (PORT_Pin_0 | PORT_Pin_1);
  PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
  PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
  PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
  PORT_InitStructure.PORT_SPEED = PORT_SPEED_MAXFAST;

  PORT_Init(MDR_PORTC, &PORT_InitStructure);

  /* Configure PORTB pin 6 for input to handle joystick events */

  PORT_InitStructure.PORT_Pin   = (PORT_Pin_6);
  PORT_InitStructure.PORT_OE    = PORT_OE_IN;
  PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
  PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
  PORT_InitStructure.PORT_SPEED = PORT_SPEED_MAXFAST;

  PORT_Init(MDR_PORTB, &PORT_InitStructure);

/* Configure PORTE pin 3 for input to handle joystick events */

  PORT_InitStructure.PORT_Pin   = (PORT_Pin_3);
  PORT_InitStructure.PORT_OE    = PORT_OE_IN;
  PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
  PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
  PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

  PORT_Init(MDR_PORTE, &PORT_InitStructure);

}


void Timer_Init(void){
  MDR_RST_CLK->PER_CLOCK |= 1 << 14; //разрешение тактирования Таймера 1
  MDR_RST_CLK->PER_CLOCK |= 1 << 15; //разрешение тактирования Таймера 2
	MDR_RST_CLK->TIM_CLOCK = (
  3 /*делитель тактовой частоты Таймера 1*/
  |(1 << 24) /*разешение тактирования Таймера 1*/
  |(0 << 8) /*делитель тактовой частоты Таймера 2*/
  |(1 << 25) /*разешение тактирования Таймера 2*/
  );

  MDR_TIMER1->PSG = 0x0;
  MDR_TIMER1->ARR = 999; //9999 -> 10 ms  999 -> ==100 us

  MDR_TIMER1->IE = (1 << 1); //разрешение прерывания по совпадению
  MDR_TIMER1->CNTRL = 1; /*счет вверх по TIM_CLK, таймер вкл.*/
	NVIC_SetPriority(Timer1_IRQn,3);
	NVIC_EnableIRQ(Timer1_IRQn);

	NVIC_SetPriority(Timer2_IRQn,1);
  NVIC_EnableIRQ(Timer2_IRQn);


}
