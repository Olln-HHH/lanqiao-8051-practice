#include <REGX52.H>
#include "absacc.h"
#include "intrins.h"
#include "smg_swj.h"

sbit TX=P1^0;
sbit RX=P1^1;
void DelaySMG(unsigned int t)
{
	while(t--);
}
void Init_74HC138(unsigned char channel)
{
	switch(channel)
	{
		case 4:P2 = (P2 & 0x1f) | 0x80;break;
		case 5:P2 = (P2 & 0x1f) | 0xa0;break;
		case 6:P2 = (P2 & 0x1f) | 0xc0;break;
		case 7:P2 = (P2 & 0x1f) | 0xe0;break;
		case 0:P2 = (P2 & 0x1f) | 0x00;break;
	}
	P2 = (P2 & 0x1f) | 0x00;
}
void Init_System()
{
	P0 = 0xff;
	Init_74HC138(4);
	P0 = 0x00;
	Init_74HC138(5);
	SMG_All(0xff);
}
unsigned int distance = 0;
void Display_Distance()
{
	if(distance == 999)
	{
		SMG_DisplayBit(0, smg_data[15]);
		DelaySMG(500);
	}
	else
	{
		SMG_DisplayBit(5, smg_data[distance / 100]);
		DelaySMG(500);
		SMG_DisplayBit(6, smg_data[(distance % 100) / 10]);
		DelaySMG(500);
		SMG_DisplayBit(7, smg_data[distance % 10]);
		DelaySMG(500);
	}
}

void Delay12us()                  //@12.000MHz 延时12us
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 33;
	while (--i);
}

void Send_Wave()                 //产生8个40KHx超声波信号
{
	unsigned char i;
	for(i = 0; i < 8; i++)
	{
		TX = 1;
		Delay12us();
		TX = 0;
		Delay12us();
	}
}

void Measure_Distance()	        //超声波测距
{
	unsigned int time = 0;

	TMOD &= 0x0f;
	TL1 = 0x00;
	TH1 = 0x00;

	Send_Wave();
	TR1 = 1;
	while((RX == 1) && (TF1 == 0));
	TR1 = 0;

	if(TF1 == 0)
	{
		time = TH1;
		time = (time << 8) | TL1;
		distance = ((time / 10) * 17) / 100 ;
	}
	else
	{
		TF1 = 0;
		distance = 999;
	}
}

void Delay(unsigned char n)
{
	while(n--)
	{
		Display_Distance();
	}
}

void main()
{
	Init_System();
	while(1)
	{
		Measure_Distance();
		Delay(10);
	}
}
