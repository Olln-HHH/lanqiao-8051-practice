#include <REGX52.H>
#include "absacc.h"
#include "smg.h"
volatile unsigned int count_f=0;
volatile unsigned int dat_f=0;
volatile unsigned char count_t=0;
void init_timer()
{
	//定时器0用作计数，8位重装方式
	TH0=0xff;
	TL0=0xff;

	//定时器1用作定时，定时时间是50ms
	TH1=(65535-50000+1)/256;
	TL1=(65535-50000+1)%256;

	TMOD=0x16;//0001 0110

	ET0=1;
	ET1=1;
	EA=1;

	TR0=1;
	TR1=1;
}
void service_T0()interrupt 1
{
	count_f++;
}
void service_T1()interrupt 3
{
	TH1=(65535-50000+1)/256;
	TL1=(65535-50000+1)%256;
	count_t++;
	if(count_t==20)
	{
		dat_f=count_f;
		count_f=0;
		count_t=0;
	}
}
void displaysmg()
{
	displaysmg_bit(0,SMGNoDot_CA[15]);
	delaysmg(100);
	displaysmg_bit(1,0xff);
	delaysmg(100);
	displaysmg_bit(2,0xff);
	delaysmg(100);

	if(dat_f>9999)
	{
		displaysmg_bit(3,SMGNoDot_CA[dat_f/10000]);
		delaysmg(100);
	}
	if(dat_f>999)
	{
		displaysmg_bit(4,SMGNoDot_CA[(dat_f/1000)%10]);
		delaysmg(100);
	}
	if(dat_f>99)
	{
		displaysmg_bit(5,SMGNoDot_CA[(dat_f/100)%10]);
		delaysmg(100);
	}
	if(dat_f>9)
	{
		displaysmg_bit(6,SMGNoDot_CA[(dat_f/10)%10]);
		delaysmg(100);
	}

	displaysmg_bit(7,SMGNoDot_CA[(dat_f)%10]);
	delaysmg(100);

	display_all(0xff);
}
void main()
{
	XBYTE[0x8000]=0xff;
	XBYTE[0xa000]=0x00;
	init_timer();
	while(1)
	{
		displaysmg();
	}
}
