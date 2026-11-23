#include <REGX52.H>

sbit L1=P0^0;
sbit L2=P0^1;
sbit L3=P0^2;
sbit L4=P0^3;
sbit L5=P0^4;
sbit L6=P0^5;
sbit L7=P0^6;
sbit L8=P0^7;
sbit beep=P0^6;
int i;
void Delay(unsigned int t)
{
	while(t--);
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
void led()
{
	L1=0;
	L2=1;
	L3=0;
	L4=1;
	L5=0;
	L6=1;
	L7=0;
	L8=1;

}
void ledn()
{
	L8=~L8;
	Delay(60000);
}
//void LEDRunning()
//{
//	L3=1;
//	L2=0;
//	L1=0;
//	P0=0x00;
//	Delay(60000);
//	Delay(60000);
//	P0=0xFF;
//	Delay(60000);
//	Delay(60000);
//	for(i=1;i<=8;i++)
//	{
//		P0=(0xFF<<i);
//		Delay(60000);
//	Delay(60000);
//	}
//	for(i=1;i<=8;i++)
//	{
//		P0=(0xFF>>i);
//		Delay(60000);
//	Delay(60000);
//	}
//}
void Init_System()
{
	P0 = 0xff;
	Init_74HC138(4);
	P0 = 0x00;
	Init_74HC138(5);
	//SMG_All(0xff);
}

void main()
{
	Init_System();
	P0 = 0xff;
	Init_74HC138(4);
	while(1)
	{
		ledn();
		Init_74HC138(4);
	}
}
