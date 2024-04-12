#ifndef PUBLIC_DELAY_H
#define PUBLIC_DELAY_H

#include <reg52.h>
#include <intrins.h>

void delay_ms(unsigned int times);
void delay_600us(void);	//@12.000MHz
void delay_60us(void);	//@12.000MHz


#endif


