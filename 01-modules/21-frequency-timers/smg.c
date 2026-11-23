#include "absacc.h"
void delaysmg(unsigned int t)
{
	while(t--);
}
void displaysmg_bit(unsigned char pos,unsigned char dat)
{
	XBYTE[0xE000]=0xff;
	XBYTE[0xC000]=0x01<<pos;
	XBYTE[0xE000]=dat;
}
void display_all(unsigned char dat)
{
	XBYTE[0xC000]=0xff;
	XBYTE[0xE000]=dat;
}
