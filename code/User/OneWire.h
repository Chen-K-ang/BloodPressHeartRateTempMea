#ifndef BUS_ONEWIRE_H
#define BUS_ONEWIRE_H

#include "delay.h"

sbit OneWire_port = P3^6;

bit OneWire_reset(void);
void OneWire_write(unsigned char dat);
unsigned char OneWire_read(void);

#endif
