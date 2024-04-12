#ifndef HARDWARE_LCD1602_H
#define HARDWARE_LCD1602_H

#include <reg52.h>
#include <stdio.h>
#include <math.h>

#define LCD1602_DB  P0  //数据端口定义
sbit LCD1602_RS = P1^2; //RS
sbit LCD1602_RW = P1^3; //RW
sbit LCD1602_EN = P1^4; //E

//LCD1602指令
//显示模式设置指令
// DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
//  0   0   1   DL  N   F   *   *
// DL = 1，8位数据接口； DL = 0，4位数据接口。
// N  = 1，两行显示；    N  = 0，单行显示。
// F  = 1，5*10点阵字符；F  = 0, 5*8点阵字符
#define LCD_MODE_PIN8      0x38 //8位数据接口，两行，5*8点阵
#define LCD_MODE_PIN4      0x28 //4位数据接口，两行，5*8点阵

#define LCD_SCREEN_CLR     0x01 //清屏
#define LCD_CURSOR_RST     0x02 //光标复位

//显示开关控制指令
#define LCD_DIS_CUR_BLK_ON 0x0F //显示开，光标开，光标闪烁
#define LCD_DIS_CUR_ON     0x0E //显示开，光标开，光标不闪烁
#define LCD_DIS_ON         0x0C //显示开，光标关，光标不闪烁
#define LCD_DIS_OFF        0x08 //显示关，光标关，光标不闪烁

//显示模式控制
#define LCD_CURSOR_RIGHT   0x06 //光标右移，显示不移动(proteus仿真可用)
#define LCD_CURSOR_LEFT    0x04 //光标左移，显示不移动
#define LCD_DIS_MODE_RIGHT 0x05 //操作后，AC自减，画面平移
#define LCD_DIS_MODE_LEFT  0x07 //操作后，AC自增，画面平移

//光标、显示移动指令
#define LCD_CUR_MOVE_LEFT  0x10 //光标左移
#define LCD_CUR_MOVE_RIGHT 0x14 //光标右移
#define LCD_DIS_MOVE_LEFT  0x18 //显示左移
#define LCD_DIS_MOVE_RIGHT 0x1C //显示右移

//其他
#define LCD_SET_CGRAM_ADDR 0x40
#define LCD_MODE_FIR       0x80 //第一行显示
#define LCD_MODE_SEC       0xC0 //第二行显示

void LCD1602_write_cmd(unsigned char cmd);
void LCD1602_write_data(unsigned char dat);
void LCD1602_init(void);
void LCD1602_show_str(unsigned char x, unsigned char y, unsigned char *str);
void LCD1602_set_char(unsigned char x, unsigned char y, unsigned char pos, unsigned char *str);
unsigned char LCD1602_integer_to_str(int dat, unsigned char *str);

// CGRAM 地址设置，character generator RAM (0~3FH)
// DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
//  0   1   A5  A4  A3  A2  A1  A0

// DDRAM 地址设置
// DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
//  1   A6   A5  A4  A3  A2  A1  A0
// N  = 0，单行显示 A6~A0 = 00H~4FH
// N  = 1，首行显示 A6~A0 = 00H~27H
//         次行显示 A6~A0 = 40H~67H

// 读BF、AC值(BF:Busy Flag)
// RS RW DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
//  0  1  BF  A6  A5  A4  A3  A2  A1  A0

//写数据
#endif
