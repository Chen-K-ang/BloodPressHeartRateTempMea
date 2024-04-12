#include "DS18B20.h"

bit DS18B20_start(void)
{
	bit ack;
	//init
	ack = OneWire_reset();
	if (ack == 0) {
		//ROM
		OneWire_write(SKIP_ROM);
		//app
		OneWire_write(CONVERT_T);
	}
	return (~ack);
}

bit DS18B20_read_T(int *temp)
{
	bit ack;
	unsigned char MSB, LSB; //high 8 bits, low 8 bits
	ack = OneWire_reset();
	if (ack == 0) {
		//ROM
		OneWire_write(SKIP_ROM);
		//app
		OneWire_write(READ_REGISTER);
		//read dat
		LSB = OneWire_read();
		MSB = OneWire_read();
		//deal
		*temp = ((int)MSB << 8) + LSB;
	}
	return (~ack);
}

