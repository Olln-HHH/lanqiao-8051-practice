#include "smg_swj.h"
#include "key.h"
#include "15f2k60s2.h"
#include "iic.h"
#include "absacc.h"
#include "intrins.h"
#include "onewire.h"

sbit Tx=P1^0;
sbit Rx=P1^1;

sbit L1=P0^0;
sbit L2=P0^1;
sbit L3=P0^2;
sbit L4=P0^3;
sbit bzz=P0^6;
sbit L8=P0^7;
sbit jdq=P0^4;
volatile unsigned char ad_val;        // 原始AD采集值（0-255）
volatile unsigned int voltage_int;    // 电压放大100倍的整数（如251代表2.51V，方便数码管显示）
volatile float temp;
volatile bit jdq_flag=0;
volatile unsigned int ms=0;
volatile unsigned int mss=0;
volatile unsigned int mt=0;
volatile unsigned int wd=30;
volatile unsigned int jl=30;
volatile unsigned int count=0;
volatile unsigned int jdqnum=3;
volatile unsigned int distance=0;
volatile unsigned int p1,p2,dert;
volatile unsigned int mod=1;
volatile unsigned int l8_flash_cnt = 0;  // L8 闪烁计数器
volatile bit l8_flash_flag = 0;          // L8 亮灭标志
volatile unsigned char Key_Val,Key_Down,Key_Old,Key_Up;//按键专用变量
volatile unsigned char Key_Slow_Down;//按键减速专用变量
//数码管变量
volatile unsigned char  smg_buf[8]={0xff};//段选，存放要显示的数据
volatile unsigned char  smg_pos=0; //位选
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
void Init_System(void)
{
	P0=0xff;
	Init_74HC138(4);
	Init_74HC138(0);
	P0=0x00;
	Init_74HC138(5);
	Init_74HC138(0);

}
void Get_Voltage(unsigned char ad_channel)//光敏电阻是0x01，可调电阻是0x03
{
	ad_val = Ad_Read(ad_channel);
	voltage_int = (unsigned int)((ad_val * 500.0 + 127) / 255);
}
void delayt(unsigned int t)
{

	while(t--);
}
void read_ds18b20_temp()
{
	int raw;
	bit ea_old;
	if(OneWire_ReadRaw(&raw))
	{
		/* 当前数码管界面显示0~99度，先限制范围再取段码。 */
		if(raw<0){raw=0;}
		if(raw>1584){raw=1584;}
		ea_old=EA;EA=0;
		temp=raw/16.0f;
		EA=ea_old;
	}
}
void check()
{
	if((distance<jl)&&(temp>wd))
	{

		jdq=1;
		bzz=0;
		Init_74HC138(5);
		Init_74HC138(0);
		if(jdq_flag==0)
		{
			if(jdqnum<99){jdqnum++;}
			jdq_flag=1;
		}

	}
	else
	{

		jdq=0;
		bzz=0;
		Init_74HC138(5);
		Init_74HC138(0);
		jdq_flag=0;

	}
}
void LED(unsigned char LEDmod)
{

	switch(LEDmod)
	{
		case 1: L1=0;L2=1;L3=1;L4=1; break;
		case 2: L1=0;L2=0;L3=1;L4=1; break;
		case 3: L1=0;L2=0;L3=0;L4=1; break;
		case 4: L1=0;L2=0;L3=0;L4=0; break;
		default :P0=0xff; break;
	}
	Init_74HC138(4);

}
unsigned char slevel()
{
	if(voltage_int>=300) return 1;
	else if((voltage_int>=200)&&(voltage_int<300)) return 2;
	else if((voltage_int>=50)&&(voltage_int<200)) return 3;
	else if(voltage_int<50) return 4;
	else return 0;
}
unsigned char checkmove()
{
	if(dert<5){return 1;}
	if(dert<10){return 2;}
	return 3;
}
volatile unsigned int lb=0;
void turn_led()
{

	P0=0xff;
	Init_74HC138(4);
	Init_74HC138(0);
	if(distance<jl)
	{
		LED(slevel());
	}
	else
	{
		LED(0);
	}
	if((checkmove())==1)
	{
		L8=0;

	}
	else if((checkmove())==2)
	{
		L8=1;

	}
	else if((checkmove())==3)
	{
		L8 = l8_flash_flag;
	}
	Init_74HC138(4);
	Init_74HC138(0);
}
volatile unsigned char i;

void displaySMG(void)
{

	if (mod==1)
	{
		smg_buf[0]=0xc6;
		smg_buf[1]=smg_data[(int)temp/10];
		smg_buf[2]=smg_data[(int)temp%10];
		smg_buf[3]=0xff;
		smg_buf[4]=0xff;
		smg_buf[5]=0xff;
		smg_buf[6]=0xc8;
		smg_buf[7]=smg_data[slevel()];
	}
	else if(mod==2)
	{
		smg_buf[0]=0xc7;
		smg_buf[1]=smg_data[checkmove()];
		smg_buf[2]=0xff;
		smg_buf[3]=0xff;
		smg_buf[4]=0xff;
		smg_buf[5]=smg_data[distance / 100];
		smg_buf[6]=smg_data[(distance % 100) / 10];
		smg_buf[7]=smg_data[distance % 10];

	}
	else if(mod==3)
	{
		smg_buf[0]=0x8c;
		smg_buf[1]=0xc6;
		smg_buf[2]=0xff;
		smg_buf[3]=0xff;
		smg_buf[4]=0xff;
		smg_buf[5]=0xff;
		smg_buf[6]=smg_data[wd/10];
		smg_buf[7]=smg_data[wd%10];

	}
	else if(mod==4)
	{
		smg_buf[0]=0xc8;
		smg_buf[1]=0xc6;
		smg_buf[2]=0xff;
		smg_buf[3]=0xff;
		smg_buf[4]=0xff;
		smg_buf[5]=0xff;
		smg_buf[6]=smg_data[jdqnum/10];
		smg_buf[7]=smg_data[jdqnum%10];

	}
	else if(mod==9)
	{
		smg_buf[0]=0x8c;
		smg_buf[1]=0xc7;
		smg_buf[2]=0xff;
		smg_buf[3]=0xff;
		smg_buf[4]=0xff;
		smg_buf[5]=0xff;
		smg_buf[6]=smg_data[jl/10];
		smg_buf[7]=smg_data[jl%10];

	}

}

void delayk(unsigned int t)
{
	while(t--);
}
void Key_Proc()
{
	if(Key_Slow_Down) return;
	Key_Slow_Down = 1;//键盘减速程序

	Key_Val = Key_Read();//实时读取键码值
	Key_Down = Key_Val & (Key_Old ^ Key_Val);//捕捉按键下降沿
	Key_Up = ~Key_Val & (Key_Old ^ Key_Val);//捕捉按键上降沿
	Key_Old = Key_Val;//辅助扫描变量

	switch(Key_Down)
	{
		case 4:
		mod++;
		if(mod>4){mod=1;}
		break;
		case 5:
		if(mod==3)
		{
			mod=9;
		}
		else if(mod==9)
		{
			mod=3;
		}
		break;
		case 8:
		if(mod==3&&wd<80) {wd++;}
		if(mod==9&&jl<80) {jl+=5;}
		break;
		case 9:
		if(mod==3&&wd>20) {wd--;}
		if(mod==9&&jl>20) {jl-=5;}
		break;

	}
}
void jdqinit()
{
	if((Key_Old==99)&&(mod==4))
	{
		count++;
	}
	else
	{
		count=0;
	}
	if(count==2000)
	{
		jdqnum=0;
	}
}
volatile unsigned int n=0;
volatile unsigned int t=0;

unsigned int Ut_Wave_Data();
//=============定时器0==========================================================================================

void Timer0_Init(void)		//1毫秒@12.000MHz
{
	AUXR |= 0x80;			//定时器时钟1T模式
	TMOD &= 0xF0;			//设置定时器模式
	TL0 = 0x20;				//设置定时初始值
	TH0 = 0xD1;				//设置定时初始值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时
	ET0 = 1;				//使能定时器0中断
	EA=1;
}

//=======================================================================================================
void Delay12us()	//@12.000MHz
{
	unsigned char data i;

	_nop_();
	_nop_();
	i = 33;
	while (--i);
}
void Ut_Wave_Init()
{
	bit ea_old;
	unsigned char i;
	ea_old=EA;EA=0;
	for(i=0;i<8;i++)
	{
		Tx=0;
		Delay12us();
		Tx=1;
		Delay12us();
	}
	EA=ea_old;
}
unsigned int Ut_Wave_Data()
{
	unsigned int dis;
	CR=0;CF=0;
	CMOD=0;
	CH=CL=0;
	Ut_Wave_Init();
	CR=1;
	while((Rx==1)&&(CF==0));
	CR=0;
	if(CF==0)
	{
		dis=(CH<<8)|CL;
		dis=(unsigned int)((unsigned long)dis*17/1000);
		if(dis>999){dis=999;}
	}
	else
	{
		CF=0;
		dis=999;
	}
	return dis;
}
volatile unsigned char m=0;
void time0() interrupt 1
{
	TL0 = 0x20;				//设置定时初始值
	TH0 = 0xD1;				//设置定时初始值
	jdqinit();
	if(n<100){n++;}
	if(++l8_flash_cnt>=200){l8_flash_cnt=0;l8_flash_flag=!l8_flash_flag;}
	if(++Key_Slow_Down==50)  Key_Slow_Down=0;
	//--------数码管---------
	P0=0xff;
	Init_74HC138(7);

	P0=1<<smg_pos;
	Init_74HC138(6);

	P0=smg_buf[smg_pos];
	Init_74HC138(7);

	smg_pos++;
	if(smg_pos>=8)
	{
		smg_pos=0;
	}
	//===============================================
	turn_led();
	if(++m>=10)
	{
		m=0;check();
	}
}
void main()
{
	unsigned int dis;
	bit ea_old;
	Init_System();
	Timer0_Init();
	while(1)
	{
		Key_Proc();
		if(n>=100)
		{
			ea_old=EA;EA=0;n=0;EA=ea_old;
			read_ds18b20_temp();
			Get_Voltage(0x01);
			dis=Ut_Wave_Data();
			ea_old=EA;EA=0;
			if(dis==999||distance==999){dert=0;}
			else if(dis>distance){dert=dis-distance;}
			else{dert=distance-dis;}
			distance=dis;
			EA=ea_old;
		}
		displaySMG();
	}
}
