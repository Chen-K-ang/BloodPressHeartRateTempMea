#ifndef HARDWARE_DS18B20_H
#define HARDWARE_DS18B20_H

#include "OneWire.h"

//ROMָ��
#define SEARCH_ROM        0xF0 //����
#define READ_ROM          0x33 //��ȡ
#define MATH_ROM          0x55 //ƥ��
#define SKIP_ROM          0xCC //����
#define ALARM_SEARCH      0xEC //��������

//�ݴ���ָ��
#define CONVERT_T         0x44 //�¶�ת��
#define WRITE_REGISTER    0x4E //д�ݴ���
#define READ_REGISTER     0xBE //���ݴ���
#define COPY_REGISTER     0x48 //�����ݴ���
#define RECALL_E2         0xB8 //�ٻ�EEPROM
#define READ_POWER_SUPPLY 0xB4 //��ȡ��Դģʽ

bit DS18B20_start(void);
bit DS18B20_read_T(int *temp);
	
#endif
