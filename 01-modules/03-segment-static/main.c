#include <REGX52.H>
unsigned char code SMG_duanma[18]=
{0xc0,0xf9,0xa4,0xb0,0x99,0x92,
	0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e,0xbf,0x7f};

void Delay(unsigned int t)
{
	while(t--);
}
sbit A0=P2^5;
sbit A1=P2^6;
sbit A2=P2^7;
void InitHC138(unsigned char n)
{
	switch(n)
	{
		case 4:
		A2=1;
		A1=0;
		A0=0;
		break;
		case 5:
		A2=1;
		A1=0;
		A0=1;
		break;
		case 6:
		A2=1;
		A1=1;
		A0=0;
		break;
		case 7:
		A2=1;
		A1=1;
		A0=1;
		break;

	}
}
void InitSystem()
{
	InitHC138(5);
	P0=0x00;
	P2&=0x1f;

}
void ShowSMG_Bit(unsigned char dat, unsigned char pos)
{

	Delay(6000);
	InitHC138(6);//数码管的位置
	P0=0x01<< pos;

	InitHC138(7);//数码管的内容
	P0=dat;
	//	InitHC138(4);
	//	P0=0xff;
	//	Delay(6000);
	//	InitHC138(5);
	//	P0=0xff;
}
void SMG_Static()
{
	unsigned char i,j;
	for(i=0;i<8;i++)
	{
		for(j=0;j<10;j++)
		{
			ShowSMG_Bit(SMG_duanma[j],i);
			Delay(60000);
		}
	}
	for(j=0;j<16;j++)
	{

		InitHC138(6);//数码管的位置
		P0=0xff;

		InitHC138(7);//数码管的内容
		P0=SMG_duanma[j];
		Delay(60000);
		Delay(60000);
		Delay(60000);
	}
}
void main()
{
	InitSystem();
	InitHC138(4);
	P0=0xff;
	while(1)
	{

		SMG_Static();
	}
}
