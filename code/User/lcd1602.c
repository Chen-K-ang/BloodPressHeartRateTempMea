#include "lcd1602.h"

static void LCD1602_read_busy_flag(void)
{
	unsigned char state;
	unsigned char i;
	LCD1602_DB = 0xFF;
	LCD1602_RS = 0;
	LCD1602_RW = 1;
	do {
		LCD1602_EN = 1;
		state = LCD1602_DB;
		LCD1602_EN = 0;
		i++;
		if (i > 50)
			break;
	} while (state & 0x80);
}

void LCD1602_write_cmd(unsigned char cmd)
{
	LCD1602_read_busy_flag();
	LCD1602_RS = 0;
	LCD1602_RW = 0;
	LCD1602_DB = cmd;
	LCD1602_EN = 1;
	LCD1602_EN = 0;
}

void LCD1602_write_data(unsigned char dat)
{
	LCD1602_read_busy_flag();
	LCD1602_RS = 1;
	LCD1602_RW = 0;
	LCD1602_DB = dat;
	LCD1602_EN = 1;
	LCD1602_EN = 0;
}

void LCD1602_init(void)
{
	LCD1602_write_cmd(LCD_MODE_PIN8);
	LCD1602_write_cmd(LCD_DIS_ON);
	LCD1602_write_cmd(LCD_CURSOR_RIGHT);
	LCD1602_write_cmd(LCD_SCREEN_CLR);
}

static void LCD1602_set_pos(unsigned char x, unsigned char y)
{
	if (y) {
		LCD1602_write_cmd(LCD_MODE_FIR | (LCD_MODE_SEC + x));
	} else {
		LCD1602_write_cmd(LCD_MODE_FIR | x);
	}
}

void LCD1602_show_str(unsigned char x, unsigned char y, unsigned char *str)
{
	LCD1602_set_pos(x, y);
	while (*str != '\0') {
		LCD1602_write_data(*str++);
	}
}

void LCD1602_set_char(unsigned char x, unsigned char y, unsigned char pos, unsigned char *str)
{
	unsigned char i;
	for (i = 0; i < 8; i++) {
		LCD1602_write_cmd(LCD_SET_CGRAM_ADDR + pos >> 3 + i);
		LCD1602_write_data(*str++);
	}
	LCD1602_set_pos(x, y);
	LCD1602_write_data(0x00 + pos);
}

unsigned char LCD1602_integer_to_str(int dat, unsigned char *str)
{
	unsigned char i = 0;
	unsigned char len = 0;
	unsigned char buf[6];
	if (dat < 0) {
		dat = -dat;
		*str++ = '-';
		len++;
	}
	do {
		buf[i++] = dat % 10 + '0';
		dat /= 10;
	} while (dat > 0);

	len += i;
	while (i-- > 0) {
		*str++ = buf[i];
	}
	*str = '\0';
	
	return (len);
}
