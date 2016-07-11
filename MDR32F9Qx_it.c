#include "global.h"
#include "MDR32F9Qx_it.h"

void SysTick_Handler(void)
{
	startGeneralLoop();
}

void DMA_IRQHandler(void)
{
	if( adcConvertationEnable == 1) {
		/* Reconfigure the inactive DMA data structure*/
		if((MDR_DMA->CHNL_PRI_ALT_SET & (1<<DMA_Channel_ADC1)) == (0<<DMA_Channel_ADC1)){
			DMA_AltCtrlStr.DMA_CycleSize = ADC_DATA_SIZE;
			DMA_Init(DMA_Channel_ADC1, &DMA_InitStr);
		}
		else if((MDR_DMA->CHNL_PRI_ALT_SET & (1<<DMA_Channel_ADC1)) == (1<<DMA_Channel_ADC1)){
			DMA_PriCtrlStr.DMA_CycleSize = ADC_DATA_SIZE;
			DMA_Init(DMA_Channel_ADC1, &DMA_InitStr);
		}
	}
	else {
		ADC1_Cmd (DISABLE);
		NVIC_SetPriority(DMA_IRQn,7);
		NVIC_DisableIRQ(DMA_IRQn);	
	}
		
//	for(i = 0; i < ADC_DATA_SIZE; ++i) {
//		ITM_SendChar(ADCConvertedValue[i]>>8);
//		ITM_SendChar(ADCConvertedValue[i] & 0x00FF);
//		ITM_SendChar(0xAA);
//		ITM_SendChar(0xAA);
//	}
	
	//NVIC_ClearPendingIRQ(DMA_IRQn);
}
void UART1_IRQHandler(void)
{
	if (UART_GetITStatusMasked(MDR_UART1, UART_IT_RX) == SET){
		UART_ClearITPendingBit(MDR_UART1, UART_IT_TX);
		modbus_RxInterrupt();
	}
	if (UART_GetITStatusMasked(MDR_UART1, UART_IT_TX) == SET){
		UART_ClearITPendingBit(MDR_UART1, UART_IT_TX);
		modbus_TxInterrupt();
	}

}

void Timer1_IRQHandler(void)
{
	GTimers_Process();
	MDR_TIMER1->CNT = 0x0000;
	MDR_TIMER1->STATUS &= ~(1 << 1);
	NVIC_ClearPendingIRQ(Timer1_IRQn);
}

//#define DELAY_LOOP_CYCLES               (8UL)
//#define GET_US_LOOPS(N)                 ((uint32_t)((float)(N) * (FLASH_PROG_FREQ_MHZ/10) / DELAY_LOOP_CYCLES))

void Timer2_IRQHandler(void)
{
	generalLoop();
	MDR_TIMER2->CNT = 0x0000;
	MDR_TIMER2->STATUS &= ~(1 << 1);
	NVIC_ClearPendingIRQ(Timer2_IRQn);
}

void NMI_Handler(void)
{
}
void HardFault_Handler(void)
{
  while (1)
  {
  }
}
void MemManage_Handler(void)
{
  while (1)
  {
  }
}
void BusFault_Handler(void)
{
  while (1)
  {
  }
}
void UsageFault_Handler(void)
{
  while (1)
  {
  }
}
void SVC_Handler(void)
{
}
void DebugMon_Handler(void)
{
}
void PendSV_Handler(void)
{
}
void CAN1_IRQHandler(void)
{
}
void CAN2_IRQHandler(void)
{
}
void USB_IRQHandler(void)
{
}

void UART2_IRQHandler(void)
{
}
void SSP1_IRQHandler(void)
{
}
void I2C_IRQHandler(void)
{
}
void POWER_IRQHandler(void)
{
}
void WWDG_IRQHandler(void)
{
}
void Timer3_IRQHandler(void)
{
}
void ADC_IRQHandler(void)
{
}
void COMPARATOR_IRQHandler(void)
{
}
void SSP2_IRQHandler(void)
{
}
void BACKUP_IRQHandler(void)
{
}
void EXT_INT1_IRQHandler(void)
{
}
void EXT_INT2_IRQHandler(void)
{
}
void EXT_INT3_IRQHandler(void)
{
}
void EXT_INT4_IRQHandler(void)
{
}

