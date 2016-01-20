#include "global.h"
#include "init.h"
#include "timers.h"
#include "main_algorithm.h"

volatile char SEC=0;
volatile unsigned int temp;

volatile unsigned int mytime=0;


static PORT_InitTypeDef PortInit;
static UART_InitTypeDef UART_InitStructure;

uint8_t DataByte=0x01;
//static uint8_t ReciveByte[16];
uint32_t i;

void Uart1_Init(void){
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

  /* Enables UART1 peripheral */
  UART_Cmd(MDR_UART1,ENABLE);

}



void SysTick_Handler(){
		loopStart();

}
void Timer1_IRQHandler(void){

  //Исполняемый код
	GTimers_Process();
  MDR_TIMER1->CNT = 0x0000;
  MDR_TIMER1->STATUS &= ~(1 << 1);
  NVIC_ClearPendingIRQ(Timer1_IRQn);
	
}

void Timer2_IRQHandler(void){

	query();
  MDR_TIMER2->CNT = 0x0000;
  MDR_TIMER2->STATUS &= ~(1 << 1);
	NVIC_ClearPendingIRQ(Timer2_IRQn);
}

int main(void){

//unsigned char cnt;

  CPU_init();
	
	GTimers_Init();
	GTimer_Run(REG_GTIMER);

	//ITM_SendChar('c');
	Init_All_Ports();
	
	Timer_Init();
	SysTick_Config(GLOBAL_CPU_CLOCK/10); // 80 MHz -> 0.1 sec

	Uart1_Init();
	
  while(1){
		
		if (GTimer_Get(REG_GTIMER)>=10000)
		{
			GTimer_Reset(REG_GTIMER);
		
			MDR_PORTC->RXTX ^= (1<<1);
			/* Check TXFE flag */
			while (UART_GetFlagStatus (MDR_UART1, UART_FLAG_TXFE)!= SET)
			{
			}

			UART_SendData (MDR_UART1, (uint16_t)(DataByte));
			/* Increment Data */
			DataByte++;
		}
		if (UART_GetFlagStatus (MDR_UART1, UART_FLAG_RXFF) == SET)
    /* Recive data */
    DataByte = UART_ReceiveData (MDR_UART1);
		
	}
  

}

