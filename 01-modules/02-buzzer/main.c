#include <REGX52.H>

sbit A0=P2^5;
sbit A1=P2^6;
sbit A2=P2^7;

void Delay(unsigned int t)
{
	while(t--);

}
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
void OutOutP0(unsigned char channel,unsigned char dat)
{
	InitHC138(channel);
	P0=dat;
}
void LEDRunning()
{
	unsigned char i;
	OutOutP0(4,0x00);
	P0=0x00;
	Delay(60000);

	OutOutP0(4,0xff);
	Delay(60000);
	Delay(60000);
	for(i=1;i<=8;i++)
	{
		OutOutP0(4,(0xFF<<i));
		Delay(60000);

	}
	OutOutP0(5,0x10);
	Delay(60000);
	Delay(60000);
	OutOutP0(5,0x00);
	InitHC138(4);
	for(i=1;i<=8;i++)
	{
		OutOutP0(4,~(0xFF<<i));
		Delay(60000);

	}
	OutOutP0(5,0x40);

	Delay(60000);
	Delay(60000);
	OutOutP0(5,0x00);

}
void InitSystem()
{
	OutOutP0(5,0x00);
}
void main()
{
	InitSystem();

	while(1)
	{
		LEDRunning();
	}
}
