#include "reg52.h"

sbit L1 = P0^0;
sbit L2 = P0^1;
sbit L8 = P0^7;
unsigned char led_stat = 0xff;
unsigned char num1 = 0,num2 = 0;
unsigned char code smg_data[] = {0xc0,0xf9,0xA4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};

void SMG_Display_Data(void);//1.简单延时

void delay(unsigned int t)
{
	while(t--);
}

void Delay_s(unsigned int t)
{
	while(t--)
	{
		SMG_Display_Data();                //在延时期间保持数码管刷新
	}
}

void Init_74HC138(unsigned char channel)//主要是在使用之后确定把锁存器关掉了
{
	switch(channel)
	{
		case 4:P2 = (P2 & 0x1f) | 0x80;break;//LED
		case 5:P2 = (P2 & 0x1f) | 0xa0;break;//蜂鸣器和继电器
		case 6:P2 = (P2 & 0x1f) | 0xc0;break; //数码管位选
		case 7:P2 = (P2 & 0x1f) | 0xe0;break;//数码管段码
		case 0:P2 = (P2 & 0x1f) | 0x00;break;//所有锁存器不选择
	}
	P2 = (P2 & 0x1f) | 0x00;//所有锁存器不选择
}

void SMG_DisplayBit(unsigned char pos,unsigned char dat)//先输送内容，再打开响应的端口
{
	P0 = (0x01 << pos);
	Init_74HC138(6);
	P0 = dat;
	Init_74HC138(7);
}

void SMG_All(unsigned char dat)
{
	P0 = 0xff;
	Init_74HC138(6);
	P0 = dat;
	Init_74HC138(7);
}

void Init_System()
{
	P0 = 0xff;
	Init_74HC138(4);
	P0 = 0x00;
	Init_74HC138(5);
	SMG_All(0xff);
}

void SMG_Display_Data(void)
{
	SMG_DisplayBit(0,smg_data[num1]);
	delay(200);
	SMG_DisplayBit(1,0xff);
	delay(200);
	SMG_DisplayBit(2,0xff);
	delay(200);
	SMG_DisplayBit(3,0xff);
	delay(200);
	SMG_DisplayBit(4,0xff);
	delay(200);
	SMG_DisplayBit(5,0xff);
	delay(200);
	SMG_DisplayBit(6,smg_data[num2/10]);
	delay(200);
	SMG_DisplayBit(7,smg_data[num2%10]);
	delay(200);
	SMG_All(0xff);
	delay(200);
}

void Led()
{
	led_stat &= ~0x80;  //L8电亮
	P0 = led_stat;
	Init_74HC138(4);
	Delay_s(200);   //在进行LED操作的时候也对数码管进行刷新

	led_stat |= 0x80;   //熄灭
	P0 = led_stat;
	Init_74HC138(4);
	Delay_s(200);

	num2++;
	if(num2 == 100)
	{
		num2 = 0;
	}

	//L1 和L2 灯同时翻转
	if((led_stat & 0x03) == 0x03)
	{
		led_stat &= ~0x03;
	}
	else
	{
		led_stat |= 0x03;
	}
	P0 = led_stat;
	Init_74HC138(4);
	num1++;
	if(num1 > 9)
	{
		num1 = 0;
	}
}

void main(void)
{
	Init_System();
	while(1)
	{
		Led();
		SMG_Display_Data();
	}
}
