#include "global.h"
#include "init.h"

#define ALL_PORTS_CLK (RST_CLK_PCLK_PORTA | RST_CLK_PCLK_PORTB | \
                       RST_CLK_PCLK_PORTC | RST_CLK_PCLK_PORTD | \
                       RST_CLK_PCLK_PORTE | RST_CLK_PCLK_PORTF)


void HSE_80MHz_init (void)
{ // 80 MHz
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
}

void Init_All_Ports(void)
{
	PORT_InitTypeDef PORT_InitStructure;
	PORT_InitTypeDef PORTA_InitStructure;
	//PORT_InitTypeDef PORTB_InitStructure;
	PORT_InitTypeDef PORTC_InitStructure;
	PORT_InitTypeDef PORTD_InitStructure;
	PORT_InitTypeDef PORTE_InitStructure;
	PORT_InitTypeDef PORTF_InitStructure;

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
	
	
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTA | RST_CLK_PCLK_PORTB | 
			RST_CLK_PCLK_PORTC | RST_CLK_PCLK_PORTD | 
			RST_CLK_PCLK_PORTE | RST_CLK_PCLK_PORTF, ENABLE);

	/* Configure PORTA */
	PORTA_InitStructure.PORT_Pin	= (PORT_Pin_All);
	PORTA_InitStructure.PORT_FUNC  	= PORT_FUNC_PORT;	//порт
	PORTA_InitStructure.PORT_GFEN	= PORT_GFEN_OFF;	//входной фильтр выкл
	PORTA_InitStructure.PORT_MODE	= PORT_MODE_DIGITAL;	//цифровой режим работы
	PORTA_InitStructure.PORT_OE	= PORT_OE_IN;		//направление - вход
	PORTA_InitStructure.PORT_PD	= PORT_PD_DRIVER;	//управляемый драйвер
	PORTA_InitStructure.PORT_PD_SHM = PORT_PD_SHM_OFF;	//триггер Шмитта выключен гистерезис 200 мВ;
	PORTA_InitStructure.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
	PORTA_InitStructure.PORT_PULL_UP = PORT_PULL_UP_OFF;
	PORTA_InitStructure.PORT_SPEED = PORT_SPEED_MAXFAST;
	PORT_Init(MDR_PORTA, &PORTA_InitStructure);
	
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
	
	PORTC_InitStructure.PORT_Pin	= (DATA15_PIN_C);
	PORTC_InitStructure.PORT_FUNC  	= PORT_FUNC_PORT;	//порт
	PORTC_InitStructure.PORT_GFEN	= PORT_GFEN_OFF;	//входной фильтр выкл
	PORTC_InitStructure.PORT_MODE	= PORT_MODE_DIGITAL;	//цифровой режим работы
	PORTC_InitStructure.PORT_OE	= PORT_OE_IN;		//направление - вход
	PORTC_InitStructure.PORT_PD	= PORT_PD_DRIVER;	//управляемый драйвер
	PORTC_InitStructure.PORT_PD_SHM = PORT_PD_SHM_OFF;	//триггер Шмитта выключен гистерезис 200 мВ;
	PORTC_InitStructure.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
	PORTC_InitStructure.PORT_PULL_UP = PORT_PULL_UP_OFF;
	PORTC_InitStructure.PORT_SPEED = PORT_SPEED_MAXFAST;
	PORT_Init(MDR_PORTC, &PORTC_InitStructure);
	
	/* Configure PORTD */
	PORTD_InitStructure.PORT_Pin	= (PORT_Pin_All);
	PORTD_InitStructure.PORT_FUNC  	= PORT_FUNC_PORT;	//порт
	PORTD_InitStructure.PORT_GFEN	= PORT_GFEN_OFF;	//входной фильтр выкл
	PORTD_InitStructure.PORT_MODE	= PORT_MODE_DIGITAL;	//цифровой режим работы
	PORTD_InitStructure.PORT_OE	= PORT_OE_IN;		//направление - вход
	PORTD_InitStructure.PORT_PD	= PORT_PD_DRIVER;	//управляемый драйвер
	PORTD_InitStructure.PORT_PD_SHM = PORT_PD_SHM_OFF;	//триггер Шмитта выключен гистерезис 200 мВ;
	PORTD_InitStructure.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
	PORTD_InitStructure.PORT_PULL_UP = PORT_PULL_UP_OFF;
	PORTD_InitStructure.PORT_SPEED = PORT_SPEED_MAXFAST;
	PORT_Init(MDR_PORTD, &PORTD_InitStructure);
	
	PORT_InitStructure.PORT_Pin   = PORT_Pin_7;
	PORT_InitStructure.PORT_OE    = PORT_OE_IN;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_ANALOG;
	PORT_Init(MDR_PORTD, &PORT_InitStructure);
	
	/* Configure PORTE */
	PORTE_InitStructure.PORT_Pin	= (DOT4_PIN_E | DOT5_PIN_E | START_PIN_E);
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
	
	/* Configure PORTF */
	PORTF_InitStructure.PORT_Pin	= (WR_PIN_F | RESDAT_PIN_F | DT_PIN_F | RZ_PIN_F);
	PORTF_InitStructure.PORT_FUNC  	= PORT_FUNC_PORT;	//порт
	PORTF_InitStructure.PORT_GFEN	= PORT_GFEN_OFF;	//входной фильтр выкл
	PORTF_InitStructure.PORT_MODE	= PORT_MODE_DIGITAL;	//цифровой режим работы
	PORTF_InitStructure.PORT_OE	= PORT_OE_OUT;		//направление - выход
	PORTF_InitStructure.PORT_PD	= PORT_PD_DRIVER;	//управляемый драйвер
	PORTF_InitStructure.PORT_PD_SHM = PORT_PD_SHM_OFF;	//триггер Шмитта выключен гистерезис 200 мВ;
	PORTF_InitStructure.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
	PORTF_InitStructure.PORT_PULL_UP = PORT_PULL_UP_OFF;
	PORTF_InitStructure.PORT_SPEED = PORT_SPEED_MAXFAST;
	PORT_Init(MDR_PORTF, &PORTF_InitStructure);

	PORTF_InitStructure.PORT_Pin	= (MD0_PIN_F | MD1_PIN_F | MD2_PIN_F);
	PORTF_InitStructure.PORT_FUNC  	= PORT_FUNC_PORT;	//порт
	PORTF_InitStructure.PORT_GFEN	= PORT_GFEN_OFF;	//входной фильтр выкл
	PORTF_InitStructure.PORT_MODE	= PORT_MODE_DIGITAL;	//цифровой режим работы
	PORTF_InitStructure.PORT_OE	= PORT_OE_IN;		//направление - вход
	PORTF_InitStructure.PORT_PD	= PORT_PD_DRIVER;	//управляемый драйвер
	PORTF_InitStructure.PORT_PD_SHM = PORT_PD_SHM_OFF;	//триггер Шмитта выключен гистерезис 200 мВ;
	PORTF_InitStructure.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
	PORTF_InitStructure.PORT_PULL_UP = PORT_PULL_UP_OFF;
	PORTF_InitStructure.PORT_SPEED = PORT_SPEED_MAXFAST;
	PORT_Init(MDR_PORTF, &PORTF_InitStructure);
  
}

void mDAC_Init(void)
{
 	PORT_InitTypeDef PORT_InitStructure; 
	PORT_InitStructure.PORT_Pin   = DAC_PIN_E;
	PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_ANALOG;
	PORT_Init(MDR_PORTE, &PORT_InitStructure);
	
	RST_CLK_PCLKcmd(RST_CLK_PCLK_DAC,ENABLE);
	
	  /* ADC Configuration */
	/* Reset all ADC settings */
	DAC_DeInit();
	/* DAC channel2 Configuration */
	DAC2_Init(DAC2_AVCC);
	/* DAC channel2 Configuration */
	DAC2_Cmd(ENABLE);
	
}

void Tim1_Tim2_Init(void){
	MDR_RST_CLK->PER_CLOCK |= 1 << 14; //разрешение тактирования Таймера 1
	MDR_RST_CLK->PER_CLOCK |= 1 << 15; //разрешение тактирования Таймера 2
	MDR_RST_CLK->TIM_CLOCK = (
		3 /*делитель тактовой частоты Таймера 1*/
		|(1 << 24) /*разешение тактирования Таймера 1*/
		|(0 << 8) /*делитель тактовой частоты Таймера 2*/
		|(1 << 25) /*разешение тактирования Таймера 2*/
	);

	MDR_TIMER1->PSG = 0x0;
	MDR_TIMER1->ARR = 999; //999 == 100 us

	MDR_TIMER1->IE = (1 << 1); //разрешение прерывания по совпадению
	MDR_TIMER1->CNTRL = 1; /*счет вверх по TIM_CLK, таймер вкл.*/

	NVIC_SetPriority(Timer1_IRQn,3);
	NVIC_EnableIRQ(Timer1_IRQn);

	NVIC_SetPriority(Timer2_IRQn,1);
	NVIC_EnableIRQ(Timer2_IRQn);

}

void init_PORTF4_as_input(void)
{
//	PORT_InitTypeDef PORT_InitStructure;
//	
//	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTF, ENABLE);
//	PORT_InitStructure.PORT_Pin   = (PORT_Pin_4);
//	PORT_InitStructure.PORT_OE    = PORT_OE_IN;
//	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
//	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
//	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
//	PORT_InitStructure.PORT_PULL_UP = PORT_PULL_UP_ON;
//	PORT_Init(MDR_PORTF, &PORT_InitStructure);
}

void Uart1_Default_Protocol_Init(void)
{
	static UART_InitTypeDef UART_InitStructure;
	
	UART_InitStructure.UART_BaudRate                = 9600;
	UART_InitStructure.UART_WordLength              = UART_WordLength8b;
	UART_InitStructure.UART_StopBits                = UART_StopBits1;
	UART_InitStructure.UART_Parity                  = UART_Parity_No;
	UART_InitStructure.UART_FIFOMode                = UART_FIFO_OFF;
	UART_InitStructure.UART_HardwareFlowControl     = UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE;
	/* Configure UART1 parameters */
	UART_Init (MDR_UART1,&UART_InitStructure);
}

unsigned int return_baud_rate(unsigned char n)
{
	unsigned int br[9] = {2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, 115200};
	
	if(n <= 8) 
		return br[n];
	else
		return br[2]; //9600
}

unsigned int return_word_length(unsigned char n)
{
	unsigned int wl[4] = {UART_WordLength8b, UART_WordLength7b, UART_WordLength6b, UART_WordLength5b};

	if(n <= 3) 
		return wl[n];
	else
		return wl[0]; //UART_WordLength8b
}

unsigned int return_stop_bit(unsigned char n)
{
	if(n == 2) 
		return UART_StopBits2;
	else
		return UART_StopBits1;
}

unsigned int return_parity(unsigned char n)
{
	unsigned int p[5] = {UART_Parity_No, UART_Parity_Even, UART_Parity_Odd, UART_Parity_1, UART_Parity_0};

	if(n <= 4) 
		return p[n];
	else
		return p[0]; //UART_Parity_No	

}


void Uart1_Protocol_Init(void)
{
	static UART_InitTypeDef UART_InitStructure;
	
	//init_PORTF4_as_input();
	
	if (PORT_ReadInputDataBit(MD0_PORT, MD0_PIN_F) == Bit_SET) {
		UART_InitStructure.UART_BaudRate                = return_baud_rate(Par.baudRate);
		UART_InitStructure.UART_WordLength              = return_word_length(Par.wordLength);
		UART_InitStructure.UART_StopBits                = return_stop_bit(Par.stopBits);
		UART_InitStructure.UART_Parity                  = return_parity(Par.parity);
		UART_InitStructure.UART_FIFOMode                = UART_FIFO_OFF;
		UART_InitStructure.UART_HardwareFlowControl     = UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE;
		/* Configure UART1 parameters */
		UART_Init (MDR_UART1,&UART_InitStructure);
	}
	else {
		Uart1_Default_Protocol_Init();
	}
}

void Uart1_Init(void)
{
	static PORT_InitTypeDef PortInit;

	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB,ENABLE);

	/* Fill PortInit structure */
	PortInit.PORT_PULL_UP 	= PORT_PULL_UP_OFF;
	PortInit.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
	PortInit.PORT_PD_SHM 	= PORT_PD_SHM_OFF;
	PortInit.PORT_PD 	= PORT_PD_DRIVER;
	PortInit.PORT_GFEN 	= PORT_GFEN_OFF;
	PortInit.PORT_FUNC 	= PORT_FUNC_ALTER;
	PortInit.PORT_SPEED 	= PORT_SPEED_MAXFAST;
	PortInit.PORT_MODE 	= PORT_MODE_DIGITAL;

	/* Configure PORTB pins 5 (UART1_TX) as output */
	PortInit.PORT_OE 	= PORT_OE_OUT;
	PortInit.PORT_Pin 	= PORT_Pin_5;
	PORT_Init(MDR_PORTB, &PortInit);

	/* Configure PORTB pins 6 (UART1_RX) as input */
	PortInit.PORT_OE 	= PORT_OE_IN;
	PortInit.PORT_Pin 	= PORT_Pin_6;
	PORT_Init(MDR_PORTB, &PortInit);

	/* Enables the CPU_CLK clock on UART1 */
	RST_CLK_PCLKcmd(RST_CLK_PCLK_UART1, ENABLE);

	/* Set the HCLK division factor = 16 for UART1*/
	UART_BRGInit(MDR_UART1, UART_HCLKdiv16);

	NVIC_SetPriority(UART1_IRQn,5);	
	NVIC_EnableIRQ(UART1_IRQn);

	Uart1_Protocol_Init();
	
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
