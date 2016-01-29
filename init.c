#include "global.h"
#include "init.h"

#define ALL_PORTS_CLK (RST_CLK_PCLK_PORTA | RST_CLK_PCLK_PORTB | \
                       RST_CLK_PCLK_PORTC | RST_CLK_PCLK_PORTD | \
                       RST_CLK_PCLK_PORTE | RST_CLK_PCLK_PORTF)


void CPU_init (void)
{ // 80 MHz
  //MDR_RST_CLK->HS_CONTROL = 0x01; /* вкл. HSE осцилятора */
  //while ((MDR_RST_CLK->CLOCK_STATUS & (1 << 2)) == 0x00); 

	RST_CLK_HSEconfig(RST_CLK_HSE_ON);
	while(RST_CLK_HSEstatus()!=SUCCESS){
	}/* ждем пока HSE выйдет в рабочий режим */
	
	RST_CLK_PCLKcmd(RST_CLK_PCLK_EEPROM, ENABLE);	
	EEPROM_SetLatency(EEPROM_Latency_3);
	
  MDR_RST_CLK->PLL_CONTROL = ((1 << 2) | (9 << 8)); //вкл. PLL | коэф. умножения = 10
  while((MDR_RST_CLK->CLOCK_STATUS & 0x02) != 0x02); //ждем когда PLL выйдет в раб. режим

  MDR_RST_CLK->CPU_CLOCK = (2 /*источник для CPU_C1*/
  | (1 << 2) /*источник для CPU_C2*/
  | (0 << 4) /*предделитель для CPU_C3*/
  | (1 << 8));/*источник для HCLK*/
	

//}
//	//8 MHz
//  MDR_RST_CLK->HS_CONTROL = 0x01; /* вкл. HSE осцилятора */
//  while ((MDR_RST_CLK->CLOCK_STATUS & (1 << 2)) == 0x00); /* ждем пока HSE выйдет в рабочий режим */

//  MDR_RST_CLK->CPU_CLOCK = (2 /*источник для CPU_C1*/
//  | (0 << 2) /*источник для CPU_C2*/
//  | (0 << 4) /*предделитель для CPU_C3*/
//  | (1 << 8));/*источник для HCLK*/
}
void Init_All_Ports(void)
{
	PORT_InitTypeDef PORT_InitStructure;
	//PORT_InitTypeDef PORTA_InitStructure;
	//PORT_InitTypeDef PORTB_InitStructure;
	PORT_InitTypeDef PORTC_InitStructure;
	//PORT_InitTypeDef PORTD_InitStructure;
	PORT_InitTypeDef PORTE_InitStructure;
	//PORT_InitTypeDef PORTF_InitStructure;

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
	
	
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB |RST_CLK_PCLK_PORTC | RST_CLK_PCLK_PORTE, ENABLE);

	/* Configure PORTC */
	PORTC_InitStructure.PORT_Pin	= (DOT8_PIN_C | DOT9_PIN_C);
	PORTC_InitStructure.PORT_FUNC  	= PORT_FUNC_PORT;	//порт
	PORTC_InitStructure.PORT_GFEN	= PORT_GFEN_OFF;	//входной фильтр выкл
	PORTC_InitStructure.PORT_MODE	= PORT_MODE_DIGITAL;	//цифровой режим работы
	PORTC_InitStructure.PORT_OE	= PORT_OE_OUT;		//направление - выход
	PORTC_InitStructure.PORT_PD	= PORT_PD_DRIVER;	//управляемый драйвер
	PORTC_InitStructure.PORT_PD_SHM = PORT_PD_SHM_OFF;	//триггер Шмитта выключен гистерезис 200 мВ;
	PORTC_InitStructure.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
	PORTC_InitStructure.PORT_PULL_UP = PORT_PULL_UP_OFF;
	PORTC_InitStructure.PORT_SPEED = PORT_SPEED_MAXFAST;
	PORT_Init(MDR_PORTC, &PORTC_InitStructure);
	
	/* Configure PORTE */
	PORTE_InitStructure.PORT_Pin	= (DOT4_PIN_E | DOT5_PIN_E);
	PORTE_InitStructure.PORT_FUNC  	= PORT_FUNC_PORT;	//порт
	PORTE_InitStructure.PORT_GFEN	= PORT_GFEN_OFF;	//входной фильтр выкл
	PORTE_InitStructure.PORT_MODE	= PORT_MODE_DIGITAL;	//цифровой режим работы
	PORTE_InitStructure.PORT_OE	= PORT_OE_OUT;		//направление - выход
	PORTE_InitStructure.PORT_PD	= PORT_PD_DRIVER;	//управляемый драйвер
	PORTE_InitStructure.PORT_PD_SHM = PORT_PD_SHM_OFF;	//триггер Шмитта выключен гистерезис 200 мВ;
	PORTE_InitStructure.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
	PORTE_InitStructure.PORT_PULL_UP = PORT_PULL_UP_OFF;
	PORTE_InitStructure.PORT_SPEED = PORT_SPEED_MAXFAST;
	PORT_Init(MDR_PORTE, &PORTE_InitStructure);
	
//	/* Configure PORTB */
//	PORTB_InitStructure.PORT_Pin	= (DE_PIN_B);
//	PORTB_InitStructure.PORT_FUNC  	= PORT_FUNC_PORT;	//порт
//	PORTB_InitStructure.PORT_GFEN	= PORT_GFEN_OFF;	//входной фильтр выкл
//	PORTB_InitStructure.PORT_MODE	= PORT_MODE_DIGITAL;	//цифровой режим работы
//	PORTB_InitStructure.PORT_OE	= PORT_OE_OUT;		//направление - выход
//	PORTB_InitStructure.PORT_PD	= PORT_PD_DRIVER;	//управляемый драйвер
//	PORTB_InitStructure.PORT_PD_SHM = PORT_PD_SHM_OFF;	//триггер Шмитта выключен гистерезис 200 мВ;
//	PORTB_InitStructure.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
//	PORTB_InitStructure.PORT_PULL_UP = PORT_PULL_UP_OFF;
//	PORTB_InitStructure.PORT_SPEED = PORT_SPEED_MAXFAST;
//	PORT_Init(MDR_PORTB, &PORTB_InitStructure);	
	

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


void Uart1_Init(void)
{
	static PORT_InitTypeDef PortInit;
	static UART_InitTypeDef UART_InitStructure;

	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB,ENABLE);

	/* Fill PortInit structure */
	PortInit.PORT_PULL_UP = PORT_PULL_UP_OFF;
	PortInit.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
	PortInit.PORT_PD_SHM = PORT_PD_SHM_OFF;
	PortInit.PORT_PD = PORT_PD_DRIVER;
	PortInit.PORT_GFEN = PORT_GFEN_OFF;
	PortInit.PORT_FUNC = PORT_FUNC_ALTER;
	PortInit.PORT_SPEED = PORT_SPEED_MAXFAST;
	PortInit.PORT_MODE = PORT_MODE_DIGITAL;

	/* Configure PORTB pins 5 (UART1_TX) as output */
	PortInit.PORT_OE = PORT_OE_OUT;
	PortInit.PORT_Pin = PORT_Pin_5;
	PORT_Init(MDR_PORTB, &PortInit);

	/* Configure PORTB pins 6 (UART1_RX) as input */
	PortInit.PORT_OE = PORT_OE_IN;
	PortInit.PORT_Pin = PORT_Pin_6;
	PORT_Init(MDR_PORTB, &PortInit);

	/* Enables the CPU_CLK clock on UART1 */
	RST_CLK_PCLKcmd(RST_CLK_PCLK_UART1, ENABLE);

	/* Set the HCLK division factor = 16 for UART1*/
	UART_BRGInit(MDR_UART1, UART_HCLKdiv16);

	NVIC_SetPriority(UART1_IRQn,2);	
	NVIC_EnableIRQ(UART1_IRQn);

	UART_InitStructure.UART_BaudRate                = 9600;
	UART_InitStructure.UART_WordLength              = UART_WordLength8b;
	UART_InitStructure.UART_StopBits                = UART_StopBits1;
	UART_InitStructure.UART_Parity                  = UART_Parity_No;
	UART_InitStructure.UART_FIFOMode                = UART_FIFO_OFF;
	UART_InitStructure.UART_HardwareFlowControl     = UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE;
	/* Configure UART1 parameters */
	UART_Init (MDR_UART1,&UART_InitStructure);

	/* Configure DMA for UART1 */
	UART_DMAConfig (MDR_UART1, UART_IT_FIFO_LVL_12words, UART_IT_FIFO_LVL_12words);
	UART_DMACmd(MDR_UART1, UART_DMA_TXE | UART_DMA_RXE | UART_DMA_ONERR, ENABLE);


	/* Enable transmitter interrupt (UARTTXINTR) */
	UART_ITConfig (MDR_UART1, UART_IT_TX, ENABLE);

	/* Enable Receiver interrupt */
	UART_ITConfig (MDR_UART1, UART_IT_RX, ENABLE);

	/* Enables UART1 peripheral */
	UART_Cmd(MDR_UART1,ENABLE);

}
