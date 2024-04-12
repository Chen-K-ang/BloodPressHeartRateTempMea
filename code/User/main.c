#include <reg52.h>
#include "DS18B20.h"
#include "ADC0832.h"
#include "lcd1602.h"
#include <math.h>
#include <stdio.h>

//ָ������ֵ
#define T_LIMIT_H     37.2 //�����¶�36.5~37.2
#define T_LIMIT_L     36.5
#define DP_LIMIT_H    90   //����ѹ 60~90
#define DP_LIMIT_L    60
#define SP_LIMIT_H    140  //����ѹ 90~140
#define SP_LIMIT_L    90
#define PLUSE_LIMIT_H 100  //����60~100
#define PLUSE_LIMIT_L 60

sbit PLUSE  = P3^2; //����������
sbit BUZZER = P2^0; //������

unsigned char DS18B20_flag = 0; //�¶Ȳ�����־λ
unsigned char pluse_flag   = 0; //������ʾ��־λ
unsigned char pluse_val    = 0; //����ֵ
unsigned char pluse_cnt    = 0; //�жϼ����������

void Int0Init(void);
void Timer0Init(void); //10΢��@12.000MHz
void Timer1Init(void); //5����@12.000MHz

void main(void)
{
	//�¶Ȳ���
	bit ack;
	int temp = 0, intTemp = 0, decTemp = 0; //�¶Ȳ���ֵ���¶�����ֵ���¶�С��ֵ
	unsigned char temperature[12] = {0};
	unsigned char temp_len = 0;
	float compare_temp = 0;

	//Ѫѹ����
	int ad_show_SP = 0, ad_show_DP = 0;
	unsigned char ad_result_SP = 0, ad_result_DP = 0, ad_sampling_cnt = 0;
	int ad_filiter_SP = 0, ad_filiter_DP = 0, ad_SP = 0, ad_DP = 0;
	unsigned char SP_str[5] = {0}, DP_str[5] = {0};
	
	//������������
	unsigned char PLUSE_str[4] = {0};
	
	//��������������
	unsigned char bueezr_flag = 0, count = 0, buzzer_time = 9;
	
	//������
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
		//��������¶�
		if (DS18B20_flag) {
			DS18B20_flag = 0;

			ack = DS18B20_read_T(&temp);
			if (ack) {
				intTemp = temp >> 4;
				decTemp = temp & 0x0F;
				temp_len = LCD1602_integer_to_str(intTemp, temperature);
				temperature[temp_len++] = '.';

				decTemp = (decTemp * 10) * 0.625; //����һλС��
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
		
		//�������ѹ������ѹ
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

		//��ʾ��������
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
		
		//����������
		count++;
		if (count > buzzer_time * 10)
			count = buzzer_time + 1;
		if (count % buzzer_time == 0 && bueezr_flag)
			BUZZER = ~BUZZER;//������ȡ��  ����������ʾ
	}
}

void Timer0Init(void)		//10΢��@12.000MHz
{
//	AUXR &= 0x7F;		//��ʱ��ʱ��12Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TMOD |= 0x01;		//���ö�ʱ��ģʽ
	TL0 = 0xF6;		//���ö�ʱ��ֵ
	TH0 = 0xFF;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	ET0 = 1;
	EA = 1;
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
}

void Timer1Init(void)		//10����@12.000MHz
{
//	AUXR &= 0xBF;		//��ʱ��ʱ��12Tģʽ
	TMOD &= 0x0F;		//���ö�ʱ��ģʽ
	TMOD |= 0x10;		//���ö�ʱ��ģʽ
	TL1 = 0xF0;		//���ö�ʱ��ֵ
	TH1 = 0xD8;		//���ö�ʱ��ֵ
	TF1 = 0;		//���TF1��־
	ET1 = 1;
	EA = 1;
	TR1 = 1;		//��ʱ��1��ʼ��ʱ
}

void Int0Init(void)
{
	//����INT0
	IT0 = 1;      //�����س�����ʽ���½��أ�
	EX0 = 1;      //��INT0���ж�����	
	EA = 1;       //�����ж�
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
