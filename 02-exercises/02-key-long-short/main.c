#include <REGX52.H>

sbit S4 = P3^3;
volatile unsigned char count = 0,press = 0,num =28;	//计时，按键长按标志，数值
unsigned char code smg_data[18]={0xc0,0xf9,
	0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
	0x88,0x83,0xc6,0xa1,0x86,0x8e,0xbf,0x7f};	//段码

void delay(unsigned int t)
{
	while(t--);
}

void Init_74HC138(unsigned char channel)
{
	switch(channel)
	{
		case 4:P2 = (P2 & 0x1f) | 0x80;break;
		case 5:P2 = (P2 & 0x1f) | 0xa0;break;
		case 6:P2 = (P2 & 0x1f) | 0xc0;break;
		case 7:P2 = (P2 & 0x1f) | 0xe0;break;
		case 0:P2 = (P2 & 0x1f) | 0x00;break;
	}
	P2 = (P2 & 0x1f) | 0x00;
}

void SMG_All(unsigned char dat)
{
	P0 = 0xff;
	Init_74HC138(6);
	P0 = dat;
	Init_74HC138(7);
}

void SMG_DisplayBit(unsigned char pos,unsigned char dat)
{
	P0 = (0x01 << pos);
	Init_74HC138(6);
	P0 = dat;
	Init_74HC138(7);
}

void SMG_Display_Data()
{
	SMG_DisplayBit(6,smg_data[num/10]);
	delay(200);
	SMG_DisplayBit(6,0xff);
	delay(200);
	SMG_DisplayBit(7,smg_data[num%10]);
	delay(200);
	SMG_DisplayBit(7,0xff);
	SMG_All(0xff);
	delay(200);
}

void Init_System(void)
{
	P0 = 0xff;
	Init_74HC138(4);
	P0 = 0x00;
	Init_74HC138(5);
	SMG_All(0xff);
}

void Scan_Keys()
{
	if(S4 == 0)
	{
		delay(200);      //去抖动处理
		if(S4 == 0)          //确认为按下信号
		{
			count = 0;      //时间计数变量清0
			press = 1;        //标志按键按下状态
			while(S4 == 0)    //等待按键松开
			{
				SMG_Display_Data();  //在按下期间保持数码管正常显示
			}
			press = 0;        //标志按键松开状态

			if(count > 100)  //按下时间大于1秒，长按
			{
				num = 0;        //数码管计数清除为00
			}
			else              //按下时间小于1秒，短按
			{
				num++;          //数码管计数加1
				if(num == 100)
				{
					num = 0;      //计数超出最大值99后恢复00
				}
			}
		}
	}
}

void Init_Timer()
{
	TMOD = 0x01;
	TH0  =(65536UL-10000)/256;	//10ms
	TL0  =(65536UL-10000)%256;
	ET0  = 1;
	TR0  = 1;
	EA   = 1;
}

void main(void)
{
	Init_System();
	Init_Timer();
	while(1)
	{
		Scan_Keys();
		SMG_Display_Data();
	}
}

void Server_Timer0() interrupt 1
{
	TH0 = (65536UL-10000)/256;
	TL0 = (65536UL-10000)%256;
	if(press==1)
	{
		if(count<101){count++;}
	}
}
