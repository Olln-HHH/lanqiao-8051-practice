#include <REGX52.H>
sfr AUXR=0x8e;
sbit S5 = P3^2;								//定义按键S5引脚
sbit S4 = P3^3;								//定义按键S4引脚
unsigned char code SMG_duanma[18]=
{0xc0,0xf9,0xa4,0xb0,0x99,0x92,
	0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e,0xbf,0x7f};
volatile unsigned char stat_led = 0xff;
void delay(unsigned int t)
{
	while(t--);
}
void selechc573(unsigned char channel)
{
	switch(channel)
	{
		case 4:
		P2=(P2 & 0x1f)|0x80;
		break;
		case 5:
		P2=(P2 & 0x1f)|0xa0;
		break;
		case 6:
		P2=(P2 & 0x1f)|0xc0;
		break;
		case 7:
		P2=(P2 & 0x1f)|0xe0;
		break;
	}
}
void init()
{
	selechc573(4);
	P0=0xff;
	selechc573(5);
	P0=0x00;
}
volatile unsigned char i;
volatile unsigned char led_on = 0x00;
void checkled()
{
	selechc573(4);
	led_on=0;
	for(i=0;i<8;i++)
	{
		led_on |= (0x01 << i);
		P0 = ~led_on;
		delay(50000);

	}
	led_on=0;

	for(i=0;i<8;i++)
	{
		led_on |= (0x01 << i);
		P0 = led_on;
		delay(50000);

	}
	P0=0xff;

}
void DisplaySMG_Bit(unsigned char value,unsigned char pos)
{
	selechc573(6);//位置
	P0=0x01<<pos;
	selechc573(7);//内容
	P0=value;
}
void DelaySMG(unsigned int t)
{
	while(t--);
}
volatile unsigned int shi,fen,miao;
void Display_Dynamic()
{
	DisplaySMG_Bit(SMG_duanma[shi/10],0);
	DelaySMG(200);
	DisplaySMG_Bit(SMG_duanma[shi%10],1);
	DelaySMG(200);
	DisplaySMG_Bit(SMG_duanma[16],2);
	DelaySMG(200);
	DisplaySMG_Bit(SMG_duanma[fen/10],3);
	DelaySMG(200);

	DisplaySMG_Bit(SMG_duanma[fen%10],4);
	DelaySMG(200);
	DisplaySMG_Bit(SMG_duanma[16],5);
	DelaySMG(200);

	DisplaySMG_Bit(SMG_duanma[miao/10],6);
	DelaySMG(200);
	DisplaySMG_Bit(SMG_duanma[miao%10],7);
	DelaySMG(200);
}
void checksmg()
{
	unsigned char i;
	unsigned char smg_on=0x00;

	for(i=0; i<8; i++) {

		selechc573(6);
		smg_on|=(0x01<<i);
		P0 = smg_on;

		selechc573(7);
		P0 = 0xc0;
		delay(50000);
	}
	smg_on=0;
	for(i=0; i<8; i++) {

		selechc573(6);
		smg_on|=(0x01<<i);
		P0 = ~smg_on;

		selechc573(7);
		P0 = 0xc0;
		delay(50000);
	}
	selechc573(6);
	P0=0x00;
}
//==========定时器相关函数=====
void inittimer0()
{
	TMOD=0x21;
	TH0=(65536UL-46080)/256;
	TL0=(65536UL-46080)%256;

	ET0=1;//打开定时器中断
	EA=1;//总中断打开
	TR0=1;//定时器工作
}
volatile unsigned int ms;
void servicetimer0() interrupt 1
{
	TH0=(65536UL-46080)/256;
	TL0=(65536UL-46080)%256;

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
		if(fen>=60)
		{
			shi++;
			fen=0;
		}

	}
}
//=============================
void inituart()
{
	TMOD=0x21;
	TH1=0xfd;
	TL1=0xfd;
	TR1=1;

	SCON=0x50;
	AUXR=0x00;

	ES=1;
	EA=1;
}
volatile unsigned char command=0x00;
// 时间变量（示例：时、分、秒）

// 串口发送一个字节函数（用于响应指令）
void sendbyte(unsigned char dat)
{
	bit es_old;
	es_old=ES;ES=0;
	TI=0;
	SBUF=dat;
	while(TI==0);
	TI=0;
	ES=es_old;
}

// 串口中断服务函数：解析指令并处理
void serviceuart() interrupt 4
{
	if(RI==1)  // 确认是接收中断
	{
		command=SBUF;  // 读取接收的指令
		RI=0;          // 清空接收标志

	}
}
/*===============上位机命令解析执行函数==================
功能：接收上位机的数据并保持在command变量中。
参数：无。
返回：空。
设计：欧浩源（2018年3月11日）
=======================================================*/
void ExecuteCommand()
{
	if(command != 0x00)							//接收到一个上位机命令
	{
		switch(command & 0xf0)				//将命令类型取出来
		{
			case 0xa0:									//远程灯光控制命令
			selechc573(4);
			stat_led = (stat_led | 0x0f) & (~command | 0xf0);
			P0 = stat_led;
			selechc573(0);
			command = 0x00;
			break;

			case 0xb0:									//读取现场系统运行时间命令
			sendbyte((shi / 10 << 4) | (shi % 10));
			sendbyte((fen / 10 << 4) | (fen % 10));
			sendbyte((miao / 10 << 4) | (miao % 10));
			command = 0x00;
			break;
		}
	}
}
/*=================独立按键扫描函数====================
功能：扫描S5和S4按键并执行现场灯光控制。
参数：无。
返回：空。
设计：欧浩源（2018年3月11日）
=======================================================*/
void ScanKeys()
{
	if(S5 == 0)							//发现按键S5信号
	{
		Display_Dynamic();				//去抖动处理
		if(S5 == 0)						//确认按键S5信号
		{
			while(S5 == 0)			//等待按键S5松开
			{
				Display_Dynamic();
			}
			selechc573(4);
			stat_led = (stat_led | 0x40) & (~stat_led | 0xbf);
			P0 = stat_led;			//执行现场灯光控制
			selechc573(0);
		}
	}

	if(S4 == 0)							//发现按键S4信号
	{
		Display_Dynamic();				//去抖动处理
		if(S4 == 0)						//确认按键S4信号
		{
			while(S4 == 0)			//等待按键S4松开
			{
				Display_Dynamic();
			}
			selechc573(4);
			stat_led = (stat_led | 0x80) & (~stat_led | 0x7f);
			P0 = stat_led;			//执行现场灯光控制
			selechc573(0);
		}
	}
}
void main()
{
	init();
	checkled();
	checksmg();
	inittimer0();
	inituart();
	while(1)
	{
		Display_Dynamic();
		ScanKeys();
		ExecuteCommand();
	}
}
