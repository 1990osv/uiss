#include "MDR32F9Qx_config.h"
#include "MDR32Fx.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_timer.h"


#define ALL_PORTS_CLK (RST_CLK_PCLK_PORTA | RST_CLK_PCLK_PORTB | \
                       RST_CLK_PCLK_PORTC | RST_CLK_PCLK_PORTD | \
                       RST_CLK_PCLK_PORTE | RST_CLK_PCLK_PORTF)

PORT_InitTypeDef PORT_InitStructure;

void CPU_init (void)
{
	MDR_RST_CLK->HS_CONTROL = 0x01; // ???. HSE ??????????
	while ((MDR_RST_CLK->CLOCK_STATUS & (1 << 2)) == 0x00); // ???? ???? HSE ?????? ? ??????? ?????
	MDR_RST_CLK->PLL_CONTROL = ((1 << 2) | (9 << 8)); // ???. PLL | ????. ????????? = 10
	while((MDR_RST_CLK->CLOCK_STATUS & 0x02) != 0x02); // ???? ????? PLL ?????? ? ???. ?????
	MDR_RST_CLK->CPU_CLOCK = 	(2 // ???????? ??? CPU_C1
													| (1 << 2) // ???????? ??? CPU_C2
													| (0 << 4) // ???????????? .?? CPU_C3
													| (1 << 8)); // ???????? ??? HCLK
}
void Init_All_Ports(void)
{
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
  PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

  PORT_Init(MDR_PORTC, &PORT_InitStructure);

  /* Configure PORTB pin 6 for input to handle joystick events */

  PORT_InitStructure.PORT_Pin   = (PORT_Pin_6);
  PORT_InitStructure.PORT_OE    = PORT_OE_IN;
  PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
  PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
  PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

  PORT_Init(MDR_PORTB, &PORT_InitStructure);

/* Configure PORTE pin 3 for input to handle joystick events */

  PORT_InitStructure.PORT_Pin   = (PORT_Pin_3);
  PORT_InitStructure.PORT_OE    = PORT_OE_IN;
  PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
  PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
  PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

  PORT_Init(MDR_PORTE, &PORT_InitStructure);


  /* In this infinite loop, PORTE pin3 and PORTB pin6 (joystick) are iteratively scanned and
     PORTC output pins (LED indicators) are appropriately set/cleared.
     Note: in this example, the following fact about MDR32F9Q2 eval board is used: the
     joystic input pin numbers exactly match LED output pins, i.e:
         LEFT   => VD4
         RIGHT  => VD3
  */	
	
}


void Init_Timer()
{

}

#ifdef __CC_ARM
int main(void)
#else
void main(void)
#endif
{
  /*!< Usually, reset is done before the program is to be loaded into microcontroller,
       and there is no need to perform any special operations to switch the ports
       to low power consumption mode explicitly. So, the function Init_All_Ports
       is used here for demonstration purpose only.
  */
  CPU_init();
	Init_All_Ports();


  while(1)
  {
    if (PORT_ReadInputDataBit(MDR_PORTB, PORT_Pin_6) == Bit_RESET)
    {
      PORT_SetBits(MDR_PORTC, PORT_Pin_0);
    }
    else
    {
      PORT_ResetBits(MDR_PORTC, PORT_Pin_0);
    }

    if (PORT_ReadInputDataBit(MDR_PORTE, PORT_Pin_3) == Bit_RESET)
    {
      PORT_SetBits(MDR_PORTC, PORT_Pin_1);
    }
    else
    {
      PORT_ResetBits(MDR_PORTC, PORT_Pin_1);
    }
  }
}

#if (USE_ASSERT_INFO == 1)
void assert_failed(uint32_t file_id, uint32_t line)
{
  /* User can add his own implementation to report the source file ID and line number.
     Ex: printf("Wrong parameters value: file Id %d on line %d\r\n", file_id, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#elif (USE_ASSERT_INFO == 2)
void assert_failed(uint32_t file_id, uint32_t line, const uint8_t* expr);
{
  /* User can add his own implementation to report the source file ID, line number and
     expression text.
     Ex: printf("Wrong parameters value (%s): file Id %d on line %d\r\n", expr, file_id, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif /* USE_ASSERT_INFO */

