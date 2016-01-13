#include "MDR32F9Qx_config.h"
#include "MDR32Fx.h"
#include "MDR32F9Qx_timer.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_port.h"


TIMER_CntInitTypeDef sTIM_CntInit;
TIMER_ChnInitTypeDef sTIM_ChnInit;
TIMER_ChnOutInitTypeDef sTIM_ChnOutInit;

//PORT_InitTypeDef PORT_InitStructure;

uint16_t CCR1_Val = 0x1FF;
uint16_t CCR2_Val = 0x3FF;
uint16_t CCR3_Val = 0x7FF;


#ifdef __CC_ARM
int timer_init(void)
#else
void timer_init(void)
#endif
{
  RST_CLK_DeInit();
  RST_CLK_CPU_PLLconfig (RST_CLK_CPU_PLLsrcHSIdiv2,0);
  /* Enable peripheral clocks --------------------------------------------------*/
  RST_CLK_PCLKcmd((RST_CLK_PCLK_RST_CLK | RST_CLK_PCLK_TIMER1),ENABLE);
  
	//RST_CLK_PCLKcmd((RST_CLK_PCLK_PORTA), ENABLE);

  /* Init NVIC */
  SCB->AIRCR = 0x05FA0000 | ((uint32_t)0x500);
  SCB->VTOR = 0x08000000;
  /* Disable all interrupt */
  NVIC->ICPR[0] = 0xFFFFFFFF;
  NVIC->ICER[0] = 0xFFFFFFFF;

//  /* Reset PORTB settings */
//  PORT_DeInit(MDR_PORTA);

//  /* Configure TIMER1 pins: CH1, CH1N, CH2, CH2N, CH3 */
//  /* Configure PORTA pins 1, 2, 3, 4, 5 */
//  PORT_InitStructure.PORT_Pin   = (PORT_Pin_1 | PORT_Pin_2 | PORT_Pin_3 | PORT_Pin_4 | PORT_Pin_5);
//  PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
//  PORT_InitStructure.PORT_FUNC  = PORT_FUNC_ALTER;
//  PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
//  PORT_InitStructure.PORT_SPEED = PORT_SPEED_FAST;
//  PORT_Init(MDR_PORTA, &PORT_InitStructure);

  /* Reset all TIMER1 settings */
  TIMER_DeInit(MDR_TIMER1);

  /* TIM1 Configuration ---------------------------------------------------
   Generates 5 PWM signals with 4 different duty cycles:
   TIM1CLK = 8 MHz, Prescaler = 0, TIM1 counter clock = 8 MHz
   TIM1 frequency = TIM1CLK/(TIM1_Period + 1) = 1.95 KHz
  - TIM1 Channel1 & Channel1N duty cycle = TIM1->CCR1 / (TIM1_Period + 1) = 50%
  - TIM1 Channel2 & Channel2N duty cycle = TIM1->CCR2 / (TIM1_Period + 1) = 25%
  - TIM1 Channel3 duty cycle = TIM1->CCR3 / (TIM1_Period + 1) = 12.5%
  ----------------------------------------------------------------------- */

  /* Initializes the TIMERx Counter ------------------------------------*/
  sTIM_CntInit.TIMER_Prescaler                = 0x0;
  sTIM_CntInit.TIMER_Period                   = 0xFFF;
  sTIM_CntInit.TIMER_CounterMode              = TIMER_CntMode_ClkFixedDir;
  sTIM_CntInit.TIMER_CounterDirection         = TIMER_CntDir_Up;
  sTIM_CntInit.TIMER_EventSource              = TIMER_EvSrc_None;
  sTIM_CntInit.TIMER_FilterSampling           = TIMER_FDTS_TIMER_CLK_div_1;
  sTIM_CntInit.TIMER_ARR_UpdateMode           = TIMER_ARR_Update_Immediately;
  sTIM_CntInit.TIMER_ETR_FilterConf           = TIMER_Filter_1FF_at_TIMER_CLK;
  sTIM_CntInit.TIMER_ETR_Prescaler            = TIMER_ETR_Prescaler_None;
  sTIM_CntInit.TIMER_ETR_Polarity             = TIMER_ETRPolarity_NonInverted;
  sTIM_CntInit.TIMER_BRK_Polarity             = TIMER_BRKPolarity_NonInverted;
  TIMER_CntInit (MDR_TIMER1,&sTIM_CntInit);

  /* Initializes the TIMER1 Channel 1,1N,2,2N,3 -------------------------------------*/
  TIMER_ChnStructInit(&sTIM_ChnInit);

  sTIM_ChnInit.TIMER_CH_Mode                = TIMER_CH_MODE_PWM;
  sTIM_ChnInit.TIMER_CH_REF_Format          = TIMER_CH_REF_Format6;
  sTIM_ChnInit.TIMER_CH_Number              = TIMER_CHANNEL1;
  TIMER_ChnInit(MDR_TIMER1, &sTIM_ChnInit);

  sTIM_ChnInit.TIMER_CH_Number              = TIMER_CHANNEL2;
  TIMER_ChnInit(MDR_TIMER1, &sTIM_ChnInit);

  sTIM_ChnInit.TIMER_CH_Number              = TIMER_CHANNEL3;
  TIMER_ChnInit(MDR_TIMER1, &sTIM_ChnInit);

  TIMER_SetChnCompare(MDR_TIMER1, TIMER_CHANNEL1, CCR1_Val);
  TIMER_SetChnCompare(MDR_TIMER1, TIMER_CHANNEL2, CCR2_Val);
  TIMER_SetChnCompare(MDR_TIMER1, TIMER_CHANNEL3, CCR3_Val);

  /* Initializes the TIMER1 Channel 1,1N,2,2N,3 Output -------------------------------*/

  TIMER_ChnOutStructInit(&sTIM_ChnOutInit);

  sTIM_ChnOutInit.TIMER_CH_DirOut_Polarity          = TIMER_CHOPolarity_NonInverted;
  sTIM_ChnOutInit.TIMER_CH_DirOut_Source            = TIMER_CH_OutSrc_REF;
  sTIM_ChnOutInit.TIMER_CH_DirOut_Mode              = TIMER_CH_OutMode_Output;
  sTIM_ChnOutInit.TIMER_CH_NegOut_Polarity          = TIMER_CHOPolarity_NonInverted;
  sTIM_ChnOutInit.TIMER_CH_NegOut_Source            = TIMER_CH_OutSrc_REF;
  sTIM_ChnOutInit.TIMER_CH_NegOut_Mode              = TIMER_CH_OutMode_Output;
  sTIM_ChnOutInit.TIMER_CH_Number                   = TIMER_CHANNEL1;
  TIMER_ChnOutInit(MDR_TIMER1, &sTIM_ChnOutInit);

  sTIM_ChnOutInit.TIMER_CH_Number                   = TIMER_CHANNEL2;
  TIMER_ChnOutInit(MDR_TIMER1, &sTIM_ChnOutInit);

  sTIM_ChnOutInit.TIMER_CH_Number                   = TIMER_CHANNEL3;
  TIMER_ChnOutInit(MDR_TIMER1, &sTIM_ChnOutInit);

  /* Enable TIMER1 clock */
  TIMER_BRGInit(MDR_TIMER1,TIMER_HCLKdiv1);

  /* Enable TIMER1 */
  TIMER_Cmd(MDR_TIMER1,ENABLE);

  while(1)
  {
  }
}

