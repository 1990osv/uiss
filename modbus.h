#ifndef MODBUS_H
#define MODBUS_H

#define RX_BUFFER_SIZE	16
#define TX_BUFFER_SIZE	16

#define MY_MODBUS_ADR	0x10

#define SWITCH_SEND_MODE	DOT4_PORT->RXTX |= (1<<DOT4_PIN);
#define SWITCH_READ_MODE	DOT4_PORT->RXTX &= ~(1<<DOT4_PIN);


void modbus_process(void);
void modbus_TxInterrupt(void);
void modbus_RxInterrupt(void);

#endif
