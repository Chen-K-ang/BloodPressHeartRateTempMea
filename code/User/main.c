#include <reg52.h>
#include "DS18B20.h"
#include "ADC0832.h"
#include "lcd1602.h"
#include <math.h>
#include <stdio.h>

//指标限制值
#define T_LIMIT_H     37.2 //人体温度36.5~37.2
#define T_LIMIT_L     36.5
#define DP_LIMIT_H    90   //舒张压 60~90
#define DP_LIMIT_L    60
#define SP_LIMIT_H    140  //收缩压 90~140
#define SP_LIMIT_L    90
#define PLUSE_LIMIT_H 100  //脉搏60~100
#define PLUSE_LIMIT_L 60

sbit PLUSE  = P3^2; //脉搏传感器
sbit BUZZER = P2^0; //蜂鸣器

unsigned char DS18B20_flag = 0; //温度测量标志位
unsigned char pluse_flag   = 0; //脉搏显示标志位
unsigned char pluse_val    = 0; //脉搏值
unsigned char pluse_cnt    = 0; //中断检测脉搏次数

void Int0Init(void);
void Timer0Init(void); //10微秒@12.000MHz
void Timer1Init(void); //5毫秒@12.000MHz

void main(void)
{
	//温度参数
	bit ack;
	int temp = 0, intTemp = 0, decTemp = 0; //温度采样值，温度整数值，温度小数值
	unsigned char temperature[12] = {0};
	unsigned char temp_len = 0;
	float compare_temp = 0;

	//血压参数
	int ad_show_SP = 0, ad_show_DP = 0;
	unsigned char ad_result_SP = 0, ad_result_DP = 0, ad_sampling_cnt = 0;
	int ad_filiter_SP = 0, ad_filiter_DP = 0, ad_SP = 0, ad_DP = 0;
	unsigned char SP_str[5] = {0}, DP_str[5] = {0};
	
	//脉搏保存数组
	unsigned char PLUSE_str[4] = {0};
	
	//蜂鸣器报警变量
	unsigned char bueezr_flag = 0, count = 0, buzzer_time = 9;
	
	//共用体
//	union float_data {
//		float f_data;
//		u8 byte[F_DATALEN];
//	};

	LCD1602_init();
	Timer0Init();
	Int0Init();
	Timer1Init();
	DS18B20_start();
	ADC0832_init();
	delay_ms(10);
	LCD1602_show_str(0,  0, "T:");
	LCD1602_show_str(10, 0, "P:");
	LCD1602_show_str(8,  1, "SP:");
	LCD1602_show_str(0,  1, "DP:");
	
	while (1) {
		//检测人体温度
		if (DS18B20_flag) {
			DS18B20_flag = 0;

			ack = DS18B20_read_T(&temp);
			if (ack) {
				intTemp = temp >> 4;
				decTemp = temp & 0x0F;
				temp_len = LCD1602_integer_to_str(intTemp, temperature);
				temperature[temp_len++] = '.';

				decTemp = (decTemp * 10) * 0.625; //保留一位小数
				temperature[temp_len++] = decTemp / 10 + '0';
				
				compare_temp = intTemp * 1.0f + decTemp / 10 * 0.1;
				
				while (temp_len < 6) {
					temperature[temp_len++] = ' ';
				}
				temperature[temp_len] = '\0';

				LCD1602_show_str(2, 0, temperature);
				delay_ms(15);
				
				if ((compare_temp > T_LIMIT_H) || (compare_temp < T_LIMIT_L))
					bueezr_flag = 1;
				else
					bueezr_flag = 0;
			}
			DS18B20_start();
		}
		
		//检测舒张压和收缩压
		ad_result_DP = ADC0832_conv(0x00);
		ad_result_SP = ADC0832_conv(0x01);
		ad_DP = ad_result_DP * 1.0 * 500 / 255;
		ad_SP = ad_result_SP * 1.0 * 500 / 255;
		ad_filiter_DP = ad_filiter_DP + ad_DP;
		ad_filiter_SP = ad_filiter_SP + ad_SP;
		ad_sampling_cnt++;
		if (ad_sampling_cnt >= 8) {
			ad_show_DP = ad_filiter_DP >> 3;
			ad_show_SP = ad_filiter_SP >> 3;
			ad_filiter_DP = 0;
			ad_filiter_SP = 0;
			ad_sampling_cnt = 0;
			LCD1602_integer_to_str(ad_show_DP, DP_str);
			LCD1602_integer_to_str(ad_show_SP, SP_str);
			LCD1602_show_str(4, 1, "   ");
			LCD1602_show_str(11, 1, "   ");
			LCD1602_show_str(4, 1, DP_str);
			LCD1602_show_str(11, 1, SP_str);
			
			if ((ad_show_DP > DP_LIMIT_H) || (ad_show_DP < DP_LIMIT_L)
				|| (ad_show_SP > SP_LIMIT_H) || (ad_show_SP < SP_LIMIT_L))
				bueezr_flag = 1;
			else
				bueezr_flag = 0;
		}

		//显示脉搏数据
		if (pluse_flag) {
			pluse_flag = 0;
			LCD1602_integer_to_str(pluse_val, PLUSE_str);
			LCD1602_show_str(13, 0, "   ");
			LCD1602_show_str(13, 0, PLUSE_str);
			
			if ((pluse_val > PLUSE_LIMIT_H) || (pluse_val < PLUSE_LIMIT_L))
				bueezr_flag = 1;
			else
				bueezr_flag = 0;
		}
		
		//蜂鸣器报警
		count++;
		if (count > buzzer_time * 10)
			count = buzzer_time + 1;
		if (count % buzzer_time == 0 && bueezr_flag)
			BUZZER = ~BUZZER;//蜂鸣器取反  发出声音提示
	}
}

void Timer0Init(void)		//10微秒@12.000MHz
{
//	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TMOD |= 0x01;		//设置定时器模式
	TL0 = 0xF6;		//设置定时初值
	TH0 = 0xFF;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	ET0 = 1;
	EA = 1;
	TR0 = 1;		//定时器0开始计时
}

void Timer1Init(void)		//10毫秒@12.000MHz
{
//	AUXR &= 0xBF;		//定时器时钟12T模式
	TMOD &= 0x0F;		//设置定时器模式
	TMOD |= 0x10;		//设置定时器模式
	TL1 = 0xF0;		//设置定时初值
	TH1 = 0xD8;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	ET1 = 1;
	EA = 1;
	TR1 = 1;		//定时器1开始计时
}

void Int0Init(void)
{
	//设置INT0
	IT0 = 1;      //跳变沿出发方式（下降沿）
	EX0 = 1;      //打开INT0的中断允许。	
	EA = 1;       //打开总中断
}

void Int0(void)	interrupt 0
{
	pluse_cnt++;
}

void Timer0_ISR(void) interrupt 1
{
	static unsigned char timer0_cnt = 0;
	TR0 = 0;
	TL0 = 0xF6;
	TH0 = 0xFF;
	
	timer0_cnt++;
	if (timer0_cnt >= 100) {
		timer0_cnt = 0;
		DS18B20_flag = 1;
	}
	TR0 = 1;
}

void Timer1_ISR(void) interrupt 3
{
	static unsigned char timer1_cnt = 0;
	static unsigned char min_cnt = 0;
	TR1 = 0;
	TL1 = 0xF0;
	TH1 = 0xD8;
	timer1_cnt++;
	if (timer1_cnt >= 100) { //time : 1s
		timer1_cnt = 0;
		min_cnt++;
	}
	if (min_cnt >= 5) { //time : 5s
		min_cnt = 0;
		pluse_val = pluse_cnt * 12;
		pluse_cnt = 0;
		pluse_flag = 1;
	}
	TR1 = 1;
}
