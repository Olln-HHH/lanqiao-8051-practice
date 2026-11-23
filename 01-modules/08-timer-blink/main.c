#include <REGX52.H>
sbit L1=P0^0;
sbit L8=P0^7;

void selecthc573()
{
	P2=(P2&0x1f)|0x80;
}
//===========================
void inittimer0()
{
	TMOD = 0x01;
	TH0=(65536UL-50000)/256;
	TL0=(65536UL-50000)%256;

	ET0=1;//打开定时器中断
	EA=1;//总中断打开
	TR0=1;//定时器工作
}
volatile unsigned char count =0;
void servicetimer0() interrupt 1
{
	TH0=(65536UL-50000)/256;
	TL0=(65536UL-50000)%256;
	count++;

	if(count%10==0)
	{
		L1=~L1;

	}
	if(count==100)
	{
		L8=~L8;
		count=0;
	}
}
//===========================
void main()
{
	inittimer0();
	selecthc573();
	while(1)
	{

	}
}
