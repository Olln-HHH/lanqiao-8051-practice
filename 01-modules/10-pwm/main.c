#include <REGX52.H>
sbit L1=P0^0;
sbit S7=P3^0;

void selecthc573(unsigned char channel)
{
	switch(channel)
	{
		case 4:
		P2=(P2&0x1f)|0x80;
		break;
		case 5:
		P2=(P2&0x1f)|0xa0;
		break;
		case 6:
		P2=(P2&0x1f)|0xc0;
		break;
		case 7:
		P2=(P2&0x1f)|0xe0;
		break;
	}
}
void delay(unsigned int t)
{
	while(t--);
}
//==================定时器=============
volatile unsigned char count;
volatile unsigned char pwm_duty;
void inittimer0()
{
	TMOD=0x01;
	TH0=(65536UL-100)/256;
	TL0=(65536UL-100)%256;

	ET0=1;
	EA=1;
}
void servicetimer0() interrupt 1
{
	TH0=(65536UL-100)/256;
	TL0=(65536UL-100)%256;
	if(++count>=100){count=0;}
	L1=(count>=pwm_duty);
}
//=====================================
volatile unsigned char stat=0;
void scank()
{
	if(S7==0)
	{
		delay(1000);

		switch(stat)
		{
			case 0:
			L1=0;//开灯
			TR0=1;//打开定时器
			pwm_duty=10;
			stat=1;
			while(S7==0);
			break;
			case 1:
			pwm_duty=50;
			stat=2;
			while(S7==0);
			break;
			case 2:
			pwm_duty=90;
			stat=3;
			while(S7==0);
			break;
			case 3:
			L1=1;//关灯
			TR0=0;//定时器关
			stat=0;
			while(S7==0);
			break;
		}

	}
}
void main()
{
	selecthc573(4);
	inittimer0();
	while(1)
	{
		scank();
	}
}
