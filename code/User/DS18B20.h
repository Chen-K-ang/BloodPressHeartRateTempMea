#ifndef HARDWARE_DS18B20_H
#define HARDWARE_DS18B20_H

#include "OneWire.h"

//ROM指令
#define SEARCH_ROM        0xF0 //搜索
#define READ_ROM          0x33 //读取
#define MATH_ROM          0x55 //匹配
#define SKIP_ROM          0xCC //跳过
#define ALARM_SEARCH      0xEC //报警搜索

//暂存器指令
#define CONVERT_T         0x44 //温度转换
#define WRITE_REGISTER    0x4E //写暂存器
#define READ_REGISTER     0xBE //读暂存器
#define COPY_REGISTER     0x48 //拷贝暂存器
#define RECALL_E2         0xB8 //召回EEPROM
#define READ_POWER_SUPPLY 0xB4 //读取电源模式

bit DS18B20_start(void);
bit DS18B20_read_T(int *temp);
	
#endif
