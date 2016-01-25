#ifndef MODBUS_H
#define MODBUS_H

#define MY_MODBUS_ADR	0x10

enum mbStatus
{
	MB_COMPLETE=0,
	MB_PROCESSING,
	MB_ADDRESS_ERROR,
	MB_FUNCTION_ERR
};

unsigned char GetStatus(void);
unsigned char razbor(unsigned char *data, unsigned char n);

#endif
