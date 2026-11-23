#include <REGX52.H>

void selecthc573(unsigned char channel);
void delaysmg(unsigned int t)
{
	while(t--);
}
void displaysmg_bit(unsigned char pos,unsigned char dat)
{
	selecthc573(7);
	P0=0xff;
	selecthc573(6);
	P0=0x01<<pos;
	selecthc573(7);
	P0=dat;
	selecthc573(0);
}
void display_all(unsigned char dat)
{
	selecthc573(6);
	P0=0xff;
	selecthc573(7);
	P0=dat;
	selecthc573(0);
}
