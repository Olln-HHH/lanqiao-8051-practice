#include "15f2k60s2.h"
unsigned char code smg_data[18]={0xc0,0xf9,
	0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
	0x88,0x83,0xc6,0xa1,0x86,0x8e,0xbf,0x7f};	//段码
volatile unsigned char smg_buf[8]={0xff};
volatile unsigned char smg_pos=0;
volatile unsigned int fre=0;
volatile unsigned char fre_flag=0;
volatile unsigned int fre_count=0;
void Init_74HC138(unsigned char channel)
{
	switch(channel)
	{
		case 4:P2=(P2&0x1f)|0x80;break;
		case 5:P2=(P2&0x1f)|0xa0;break;
		case 6:P2=(P2&0x1f)|0xc0;break;
		case 7:P2=(P2&0x1f)|0xe0;break;
		case 0:P2=(P2&0x1f)|0x00;break;
	}
	P2=(P2&0x1f)|0x00;
}
void Init_system()
{
	P0=0xff;
	Init_74HC138(4);
	Init_74HC138(0);
	P0=0x00;
	Init_74HC138(5);
	Init_74HC138(0);
}
void displaySMG()
{
	smg_buf[0] = 0xff;
	smg_buf[1] = 0xff;
	smg_buf[2] = 0xff;
	smg_buf[3]=smg_data[fre/10000%10];
	smg_buf[4]=smg_data[fre/1000%10];
	smg_buf[5]=smg_data[fre/100%10];
	smg_buf[6]=smg_data[fre/10%10];
	smg_buf[7]=smg_data[fre%10];
}
void Timer0_Init(void)		//1毫秒@12.000MHz
{

	TMOD |= 0x05;			//设置定时器模式
	TL0 = 0x00;				//设置定时初始值
	TH0 = 0x00;				//设置定时初始值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时
}
void Timer1_Init(void)		//1毫秒@11.0592MHz
{
	AUXR |= 0x40;			//定时器时钟1T模式
	TMOD &= 0x0F;			//设置定时器模式
	TL1 = 0xCD;				//设置定时初始值
	TH1 = 0xD4;				//设置定时初始值
	TF1 = 0;				//清除TF1标志
	TR1 = 1;				//定时器1开始计时
	ET1=1;
	EA=1;
}
void Timer1_isr(void) interrupt 3
{
	TL1 = 0xCD;				//设置定时初始值
	TH1 = 0xD4;				//设置定时初始值

	P0=0xff;
	Init_74HC138(7);
	Init_74HC138(0);

	P0=1<<smg_pos;
	Init_74HC138(6);
	Init_74HC138(0);

	P0=smg_buf[smg_pos];
	Init_74HC138(7);
	Init_74HC138(0);

	smg_pos++;
	if(smg_pos>=8)
	{
		smg_pos=0;
	}

	if(fre_flag==0)
	{
		fre_count++;
		if(fre_count>=1000)
		{
			fre_flag=1;
			fre_count=0;
		}

	}
}
void read_fre()
{
	if(fre_flag==1)
	{
		TR0=0;
		fre=TH0;
		fre<<=8;
		fre|=TL0;
		TH0=0;
		TL0=0;
		TR0=1;
		fre_flag=0;
	}
}
void main()
{
	Init_system();
	Timer0_Init();
	Timer1_Init();
	while(1)
	{
		read_fre();
		displaySMG();
	}
}
