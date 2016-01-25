#ifndef GLOBAL_H
#define GLOBAL_H

#include "MDR32F9Qx_config.h"
#include "MDR32Fx.h"

#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_uart.h"
#include "MDR32F9Qx_timer.h"


#define GLOBAL_CPU_CLOCK 80000000

#define ADC_PIN					PORT_Pin_0
#define ADC_PORT				MDR_PORTC

#define DEF_PIN					PORT_Pin_0
#define START_PIN				PORT_Pin_0
#define WR_PIN					PORT_Pin_0
#define RESDAT_PIN			PORT_Pin_0
#define DT_PIN					PORT_Pin_0
#define RZ_PIN					PORT_Pin_0
#define DV_PIN					PORT_Pin_0



#endif
