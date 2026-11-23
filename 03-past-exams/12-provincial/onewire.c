bit OneWire_ReadRaw(int *raw);
/*	# 	单总线代码片段说明
1. 	本文件夹中提供的驱动代码供参赛选手完成程序设计参考。
2. 	参赛选手可以自行编写相关代码或以该代码为基础，根据所选单片机类型、运行速度和试题
中对单片机时钟频率的要求，进行代码调试和修改。
*/
#include "15f2k60s2.h"
sbit DQ=P1^4;
int temp=0;
//
void Delay_OneWire(unsigned int t)
{
	unsigned char i;
	while(t--){
		for(i=0;i<12;i++);
	}
}

//
void Write_DS18B20(unsigned char dat)
{
	bit ea_old;
	unsigned char i;

	ea_old=EA;
	EA=0;
	for(i=0;i<8;i++)
	{
		DQ = 0;
		DQ = dat&0x01;
		Delay_OneWire(5);
		DQ = 1;
		dat >>= 1;
	}
	Delay_OneWire(5);

	EA=ea_old;
}

//
unsigned char Read_DS18B20(void)
{
	bit ea_old;
	unsigned char i;
	unsigned char dat=0;

	ea_old=EA;
	EA=0;
	for(i=0;i<8;i++)
	{
		DQ = 0;
		dat >>= 1;
		DQ = 1;
		if(DQ)
		{
			dat |= 0x80;
		}
		Delay_OneWire(5);
	}
	EA=ea_old;
	return dat;
}

//
bit init_ds18b20(void)
{
	bit ea_old;
	bit initflag = 0;

	ea_old=EA;
	EA=0;
	DQ = 1;
	Delay_OneWire(12);
	DQ = 0;
	Delay_OneWire(80);
	DQ = 1;
	Delay_OneWire(10);
	initflag = DQ;
	Delay_OneWire(5);

	EA=ea_old;
	return initflag;
}

/* 外部供电DS18B20：转换未完成时保留上一次温度，不阻塞显示和按键。 */
bit OneWire_ReadRaw(int *raw)
{
	static bit started=0;
	unsigned char buf[9],i,j,crc=0;
	bit ready=0;
	if(started)
	{
		if(Read_DS18B20()!=0xff){return 0;}
		if(init_ds18b20()){started=0;return 0;}
		Write_DS18B20(0xcc);
		Write_DS18B20(0xbe);
		for(i=0;i<9;i++){buf[i]=Read_DS18B20();}
		for(i=0;i<9;i++)
		{
			crc^=buf[i];
			for(j=0;j<8;j++)
			{
				if(crc&1){crc=(crc>>1)^0x8c;}
				else{crc>>=1;}
			}
		}
		if(crc==0)
		{
			*raw=(int)((unsigned int)buf[1]<<8|buf[0]);
			ready=1;
		}
	}
	if(init_ds18b20()){started=0;return ready;}
	Write_DS18B20(0xcc);
	Write_DS18B20(0x44);
	started=1;
	return ready;
}
