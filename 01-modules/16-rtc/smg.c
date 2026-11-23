#include <REGX52.H>
void selecthc573(unsigned char channel)
{
	switch(channel)
	{
		case 0:
		break;
		case 4:
		P2=P2&(0x1f)|0x80;
		break;
		case 5:
		P2=P2&(0x1f)|0xa0;
		break;
		case 6:
		P2=P2&(0x1f)|0xc0;
		break;
		case 7:
		P2=P2&(0x1f)|0xe0;
		break;
	}
}

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
