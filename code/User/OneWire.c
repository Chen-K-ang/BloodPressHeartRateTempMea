#include "OneWire.h"

bit OneWire_reset(void)
{
	bit ack;
	EA = 0;
	OneWire_port = 0;
	delay_600us();
	OneWire_port = 1;
	delay_60us();
	ack = OneWire_port;
	while (!OneWire_port);
	EA = 1;
	return (ack);
}

void OneWire_write(unsigned char dat)
{
	unsigned char temp;
	EA = 0;
	for (temp = 0x01; temp != 0; temp <<= 1) {
		OneWire_port = 0;
		_nop_();
		_nop_();
		if ((dat & temp) == 0)
			OneWire_port = 0;
		else
			OneWire_port = 1;
		delay_60us();
		OneWire_port = 1;
	}
	EA = 1;
}

unsigned char OneWire_read(void)
{
	unsigned char dat;
	unsigned char temp;
	EA = 0;
	for (temp = 0x01; temp != 0; temp <<= 1) {
		OneWire_port = 0;
		_nop_();
		_nop_();
		OneWire_port = 1;
		_nop_();
		_nop_();
		if (!OneWire_port)
			dat &= ~temp;
		else
			dat |= temp;
		delay_60us();
	}
	EA = 1;
	
	return (dat);
}
