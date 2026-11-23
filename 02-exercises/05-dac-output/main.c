#include <REGX52.H>
#include "iic.h"
#include "smg_swj.h"
sbit S4=P3^3;
void Init_74HC138(unsigned char n)
{
	switch(n)
	{
		case 4:P2=(P2&0x1f)|0x80;break;
		case 5:P2=(P2&0x1f)|0xa0;break;
		case 6:P2=(P2&0x1f)|0xc0;break;
		case 7:P2=(P2&0x1f)|0xe0;break;
		case 0:P2=(P2&0x1f)|0x00;break;
	}
	P2=(P2&0x1f)|0x00;
}
void Init_System()
{
	Init_74HC138(0);
	P0 = 0x00;
	Init_74HC138(5);
	P0 = 0xff;
	Init_74HC138(4);
}
void SMG_Display_Data(unsigned char n);
unsigned char mod=1;
void delay(unsigned int t)
{
	while(t--);
}
void ScanKeys_Alone()
{

	if(S4==0)
	{
		delay(100);
		while(S4==0)
		{
			SMG_Display_Data(mod);
		}
		delay(100);
		mod++;
		if(mod>3)
		{
			mod=1;
		}
	}
}
unsigned char ad_val;        // 原始AD采集值（0-255）
float voltage;               // 精准电压值（浮点型，如2.51V）
unsigned int voltage_int;    // 电压放大100倍的整数（如251代表2.51V，方便数码管显示）
void Get_Voltage(unsigned char ad_channel)//光敏电阻是0x01，可调电阻是0x03
{
	// 步骤1：读取AD原始值（0-255）
	ad_val = Ad_Read(ad_channel);

	// 方式1：精准浮点换算（适合串口打印、精准计算）
	voltage = (float)ad_val * 5.0 / 255.0;

	// 方式2：整数换算（适合数码管显示，避免浮点运算）
	// 放大100倍，四舍五入，比如2.5098→251
	voltage_int = (unsigned int)((ad_val * 500.0 + 127) / 255);
	// +127是为了四舍五入（255/2=127.5），提升显示精度
}

void SMG_Display_Data(unsigned char n)
{
	if(n==1)
	{
		SMG_DisplayBit(0,smg_data[16]);
		delay(200);
		SMG_DisplayBit(1,smg_data[n]);
		delay(200);
		SMG_DisplayBit(2,smg_data[16]);
		delay(200);
		SMG_DisplayBit(3,0xff);
		delay(200);
		SMG_DisplayBit(4,0xff);
		delay(200);
		SMG_DisplayBit(5,smg_data[2]-0x80);
		delay(200);
		SMG_DisplayBit(6,smg_data[0]);
		delay(200);
		SMG_DisplayBit(7,smg_data[0]);
		delay(200);
		SMG_All(0xff);
		delay(200);
	}
	else if(n==2)
	{
		SMG_DisplayBit(0,smg_data[16]);
		delay(200);
		SMG_DisplayBit(1,smg_data[n]);
		delay(200);
		SMG_DisplayBit(2,smg_data[16]);
		delay(200);
		SMG_DisplayBit(3,0xff);
		delay(200);
		SMG_DisplayBit(4,0xff);
		delay(200);
		SMG_DisplayBit(5,smg_data[4]-0x80);
		delay(200);
		SMG_DisplayBit(6,smg_data[0]);
		delay(200);
		SMG_DisplayBit(7,smg_data[0]);
		delay(200);
		SMG_All(0xff);
		delay(200);
	}
	else if(n==3)
	{
		SMG_DisplayBit(0,smg_data[16]);
		delay(200);
		SMG_DisplayBit(1,smg_data[n]);
		delay(200);
		SMG_DisplayBit(2,smg_data[16]);
		delay(200);
		SMG_DisplayBit(3,0xff);
		delay(200);
		SMG_DisplayBit(4,0xff);
		delay(200);
		SMG_DisplayBit(5,smg_data[voltage_int/100]-0x80);
		delay(200);
		SMG_DisplayBit(6,smg_data[(voltage_int/10)%10]);
		delay(200);
		SMG_DisplayBit(7,smg_data[voltage_int%10]);
		delay(200);
		SMG_All(0xff);
		delay(200);
	}
}
void main()
{
	Init_System();
	while(1)
	{
		ScanKeys_Alone();
		if(mod==1){Da_Write(102);}
		else if(mod==2){Da_Write(204);}
		else{Get_Voltage(0x03);Da_Write(ad_val);}
		SMG_Display_Data(mod);
	}
}
