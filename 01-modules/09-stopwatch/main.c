#include <REGX52.H>
unsigned char code SMG_duanma[18]=
{0xc0,0xf9,0xa4,0xb0,0x99,0x92,
	0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e,0xbf,0x7f};
sbit S4 = P3^3;
sbit S5 = P3^2;
volatile unsigned char fen=0;
volatile unsigned char miao=0;
volatile unsigned char ms=0;

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
void InitSystem()
{
	selecthc573(5);
	P0=0x00;
	P2&=0x1f;

}
void displaysmg_bit(unsigned char value,unsigned char pos)
{
	selecthc573(6);
	P0=0x01<<pos;
	selecthc573(7);
	P0=value;
}
void delaysmg(unsigned int t)
{
	while(t--);
}
void displaytime()
{
	displaysmg_bit(SMG_duanma[ms%10],7);
	delaysmg(200);
	displaysmg_bit(SMG_duanma[ms/10],6);
	delaysmg(200);
	displaysmg_bit(SMG_duanma[16],5);
	delaysmg(200);

	displaysmg_bit(SMG_duanma[miao%10],4);
	delaysmg(200);
	displaysmg_bit(SMG_duanma[miao/10],3);
	delaysmg(200);
	displaysmg_bit(SMG_duanma[16],2);
	delaysmg(200);

	displaysmg_bit(SMG_duanma[fen%10],1);
	delaysmg(200);
	displaysmg_bit(SMG_duanma[fen/10],0);
	delaysmg(200);

}
//==========定时器相关函数=====
void inittimer0()
{
	TMOD=0x01;
	TH0=(65536UL-50000)/256;
	TL0=(65536UL-50000)%256;

	ET0=1;//打开定时器中断
	EA=1;//总中断打开
	TR0=1;//定时器工作
}
void servicetimer0() interrupt 1
{
	TH0=(65536UL-50000)/256;
	TL0=(65536UL-50000)%256;

	ms++;
	if(ms>=20)
	{
		miao++;
		ms=0;
		if(miao>=60)
		{
			fen++;
			miao=0;
		}
		if(fen==99)
		{
			fen=0;
		}
	}
}
//=============================
void delayk(unsigned char t)
{
	while(t--);
}
void scankeys()
{
	if(S4==0)							//秒表启动与暂停
	{
		delayk(100);
		if(S4==0)
		{
			TR0=~TR0;
			while(S4==0)
			{
				displaytime();
			}
		}
	}
	if(S5==0)							//秒表清零
	{
		delayk(100);
		if(S5==0)
		{
			while(S5==0)
			{
				displaytime();
			}
			fen=0;
			miao=0;
			ms=0;
		}
	}
}
void main()
{
	InitSystem();
	selecthc573(4);
	P0=0xff;
	inittimer0();
	while(1)
	{
		displaytime();
		scankeys();
	}
}
