#include <REGX52.H>
unsigned char code SMG_duanma[18]=
{0xc0,0xf9,0xa4,0xb0,0x99,0x92,
	0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e,0xbf,0x7f};
sbit A0=P2^5;
sbit A1=P2^6;
sbit A2=P2^7;
unsigned char yu=1;
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
void InitSystem()
{
	SelectHC573(5);
	P0=0x00;
	P2&=0x1f;

}
void DisplaySMG_Bit(unsigned char value,unsigned char pos)
{
	SelectHC573(6);//位置
	P0=0x01<<pos;
	SelectHC573(7);//内容
	P0=value;
}

void DelaySMG(unsigned int t)
{
	while(t--);
}
void Display_Dynamic()
{
	DisplaySMG_Bit(SMG_duanma[2],0);
	DelaySMG(200);
	DisplaySMG_Bit(SMG_duanma[0],1);
	DelaySMG(200);
	DisplaySMG_Bit(SMG_duanma[2],2);
	DelaySMG(200);
	DisplaySMG_Bit(SMG_duanma[5],3);
	DelaySMG(200);

	DisplaySMG_Bit(SMG_duanma[16],4);
	DelaySMG(200);
	DisplaySMG_Bit(SMG_duanma[16],5);
	DelaySMG(200);

	DisplaySMG_Bit(SMG_duanma[yu/10],6);
	DelaySMG(200);
	DisplaySMG_Bit(SMG_duanma[yu%10],7);
	DelaySMG(200);
}
void Delay(unsigned int m)
{
	while(m--)
	{
		Display_Dynamic();
	}
}
void main()
{
	InitSystem();
	SelectHC573(4);
	P0=0xff;
	while(1)
	{

		Display_Dynamic();
		yu++;
		if(yu>12)
		{
			yu=1;
		}
		Delay(600);
	}

}
