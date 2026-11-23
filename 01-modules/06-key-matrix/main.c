#include <REGX52.H>
#include "key.h"
unsigned char KeyNum;
void Delay1ms(void)	//@11.0592MHz
{
	unsigned char data i, j;

	i = 11;
	j = 190;
	do
	{
		while (--j);
	} while (--i);
}

void delay(unsigned int t)
{
	while(t--)
	{
		Delay1ms();
	}
}

unsigned char code SMG_duanma[18]=
{0xc0,0xf9,0xa4,0xb0,0x99,0x92,
	0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e,0xbf,0x7f};
void SelectHC573(unsigned char channel)
{
	switch (channel)
	{
		case 4:
		P2 = (P2 & 0x1f) | 0x80;
		break;
		case 5:
		P2 = (P2 & 0x1f) | 0xa0;
		break;
		case 6:
		P2 = (P2 & 0x1f) | 0xc0;
		break;
		case 7:
		P2 = (P2 & 0x1f) | 0xe0;
		break;

	}
}
void DisplayKeyNum(unsigned char value)
{
	SelectHC573(6);
	P0=0x01;
	SelectHC573(7);
	P0=value;
}

void InitSystem()
{
	SelectHC573(5);
	P0=0x00;
	//  P2&=0x1f;

}
void main()
{
	SelectHC573(4);
	InitSystem();
	SelectHC573(4);
	P0=0xff;
	while(1)
	{

		DisplayKeyNum(SMG_duanma[ScanKeysMulti()]);
		delay(200);
	}
}
