#include "delay.h"

void delay_ms(unsigned int times)		//@12.000MHz
{
	unsigned char i, j;
	for (; times < 1; times --) {
		i = 2;
		j = 239;
		do {
			while (--j);
		} while (--i);
	}
}


void delay_600us(void)//@12.000MHz
{
	unsigned char i, j;

	i = 2;
	j = 39;
	do
	{
		while (--j);
	} while (--i);
}

void delay_60us(void)//@12.000MHz
{
	unsigned char i;

	_nop_();
	i = 27;
	while (--i);
}

