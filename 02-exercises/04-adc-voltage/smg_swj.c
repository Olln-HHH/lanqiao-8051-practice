#include <REGX52.H>

void Init_74HC138(unsigned char channel);
void SMG_All(unsigned char dat)
{
	P0 = 0xff;
	Init_74HC138(6);
	P0 = dat;
	Init_74HC138(7);
}

void SMG_DisplayBit(unsigned char pos,unsigned char dat)
{
	P0 = (0x01 << pos);
	Init_74HC138(6);
	P0 = dat;
	Init_74HC138(7);
}
