#include "15f2k60s2.h"
#include "key.h"
#include "ds1302.h"
#include "iic.h"
volatile unsigned char write_ds1302_adrr[7]={0x80,0x82,0x84,0x86,0x88,0x8a,0x8c};
volatile unsigned char read_ds1302_adrr[7]={0x81,0x83,0x85,0x87,0x89,0x8b,0x8d};
//2025.12.10 周三 20:51:20
//从左到右依次是  秒 分 时 日 月 周几  年
volatile unsigned char timer[7]={0x20,0x51,0x20,0x10,0x12,0x03,0x25};
volatile unsigned char max_timer[3]={0x00,0x00,0x00};
unsigned char code smg_data[18]={0xc0,0xf9,
	0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
	0x88,0x83,0xc6,0xa1,0x86,0x8e,0xbf,0x7f};	//段码
volatile unsigned char key_val,key_down,key_up,key_old;
volatile unsigned char key_slow_down;
volatile unsigned char smg_buf[8]={0xff};
volatile unsigned char smg_pos=0;
volatile unsigned int mod=1;
volatile long fre=0;
volatile unsigned int fre_num=2000;
volatile unsigned int fre_max=0;
volatile unsigned int V_num=0;
volatile unsigned int DAC_num=0;
volatile unsigned int LED_num=0;
volatile unsigned char Lnum;
volatile unsigned char DAC_flag=0;
volatile int xzz=0;
volatile unsigned char read_ne555_flag=0;
volatile unsigned int n=0;
sbit L1=P0^0;
sbit L2=P0^1;
sbit L3=P0^2;
sbit L4=P0^3;
sbit L5=P0^4;
sbit L6=P0^5;
sbit L7=P0^6;
sbit L8=P0^7;
void LED();
void Init_74HC138(unsigned char channel)
{
	switch(channel)
	{
		case 4:P2=(P2&0x1f)|0x80; break;
		case 5:P2=(P2&0x1f)|0xa0; break;
		case 6:P2=(P2&0x1f)|0xc0; break;
		case 7:P2=(P2&0x1f)|0xe0; break;
		case 0:P2=(P2&0x1f)|0x00; break;
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
void key_Proc()
{
	if(key_slow_down) return;
	key_slow_down=1;
	key_val=key_read();
	key_down=key_val&(key_old^key_val);
	key_up=~key_val&(key_old^key_val);
	key_old=key_val;
	switch(key_down)
	{
		case 4:
		mod++;
		if(mod>4){mod=1;}
		break;
		case 5:
		if(mod==2)      mod=12;
		else if(mod==12)mod=2;
		else if(mod==4) mod=14;
		else if(mod==14)mod=4;
		break;
		case 8:
		if(mod==2&&fre_num<9000){fre_num+=1000;}
		else if(mod==12&&xzz<900){xzz+=100;}
		break;
		case 9:
		if(mod==2&&fre_num>1000){fre_num-=1000;}
		else if(mod==12&&xzz> -900){xzz-=100;}
		break;

	}
}

void SMG_Proc()
{
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
	if(smg_pos>=8){smg_pos=0;}
}

void displaySMG()
{
	switch(mod)
	{
		case 1:
		smg_buf[0]=smg_data[15];
		smg_buf[1]=0xff;
		smg_buf[2]=0xff;
		if(fre>=0&&fre<34000)
		{
			if(fre>=10000){smg_buf[3]=smg_data[fre/10000%10];}else {smg_buf[3]=0xff;}
			if(fre>=1000){smg_buf[4]=smg_data[fre/1000%10];}else {smg_buf[4]=0xff;}
			if(fre>=100){smg_buf[5]=smg_data[fre/100%10];}else {smg_buf[5]=0xff;}
			if(fre>=10){smg_buf[6]=smg_data[fre/10%10];}else {smg_buf[6]=0xff;}
			smg_buf[7]=smg_data[fre%10];
		}
		else
		{
			smg_buf[3]=0xff;
			smg_buf[4]=0xff;
			smg_buf[5]=0xff;
			smg_buf[6]=~0x38;
			smg_buf[7]=~0x38;
		}
		break;
		case 2:
		smg_buf[0]=~0x73;
		smg_buf[1]=smg_data[1];
		smg_buf[2]=0xff;
		smg_buf[3]=0xff;
		smg_buf[4]=smg_data[fre_num/1000%10];
		smg_buf[5]=smg_data[fre_num/100%10];
		smg_buf[6]=smg_data[fre_num/10%10];
		smg_buf[7]=smg_data[fre_num/1%10];
		break;
		case 12:
		smg_buf[0]=smg_data[15];
		smg_buf[1]=smg_data[2];
		smg_buf[2]=0xff;
		smg_buf[3]=0xff;
		if(xzz>0)
		{
			smg_buf[4]=0xff;
			smg_buf[5]=smg_data[xzz/100%10];
			smg_buf[6]=smg_data[xzz/10%10];
			smg_buf[7]=smg_data[xzz%10];
		}
		else if(xzz<0)
		{
			smg_buf[4]=~0x40;
			smg_buf[5]=smg_data[(-xzz)/100%10];
			smg_buf[6]=smg_data[(-xzz)/10%10];
			smg_buf[7]=smg_data[(-xzz)%10];
		}
		else if(xzz==0)
		{
			smg_buf[4]=0xff;
			smg_buf[5]=0xff;
			smg_buf[6]=0xff;
			smg_buf[7]=smg_data[(-xzz)%10];
		}
		break;
		case 3:
		smg_buf[0]=smg_data[timer[2]/16];
		smg_buf[1]=smg_data[timer[2]%16];
		smg_buf[2]=~0x40;
		smg_buf[3]=smg_data[timer[1]/16];
		smg_buf[4]=smg_data[timer[1]%16];
		smg_buf[5]=~0x40;
		smg_buf[6]=smg_data[timer[0]/16];
		smg_buf[7]=smg_data[timer[0]%16];
		break;
		case 4:
		smg_buf[0]=~0x76;
		smg_buf[1]=smg_data[15];
		smg_buf[2]=0xff;
		if(fre_max>=10000){smg_buf[3]=smg_data[fre_max/10000%10];}else {smg_buf[3]=0xff;}
		if(fre_max>=1000){smg_buf[4]=smg_data[fre_max/1000%10];}else {smg_buf[4]=0xff;}
		if(fre_max>=100){smg_buf[5]=smg_data[fre_max/100%10];}else {smg_buf[5]=0xff;}
		if(fre_max>=10){smg_buf[6]=smg_data[fre_max/10%10];}else {smg_buf[6]=0xff;}
		smg_buf[7]=smg_data[fre_max%10];

		break;
		case 14:
		smg_buf[0]=~0x76;
		smg_buf[1]=~0x77;
		smg_buf[2]=smg_data[max_timer[2]/16];
		smg_buf[3]=smg_data[max_timer[2]%16];
		smg_buf[4]=smg_data[max_timer[1]/16];
		smg_buf[5]=smg_data[max_timer[1]%16];
		smg_buf[6]=smg_data[max_timer[0]/16];
		smg_buf[7]=smg_data[max_timer[0]%16];
		break;
	}
}

void Timer0_Init(void)		//1毫秒@11.0592MHz
{
	//	AUXR |= 0x80;			//定时器时钟1T模式 NE555必须删除这行！！！！！
	TMOD &= 0xF0;			//设置定时器模式
	TMOD |= 0x05;   //NE555必加！！！！！！！！！！！！
	TL0 = 0x00;				//设置定时初始值
	TH0 = 0x00;				//设置定时初始值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时
}

void read_ne555()
{
	if(read_ne555_flag==1)
	{

		TR0=0;
		fre=(unsigned long)TH0;
		fre<<=8;
		fre|=TL0;
		fre=fre+xzz;
		TH0=0;
		TL0=0;
		TR0=1;
		read_ne555_flag=0;
	}

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
char i;
void timer1() interrupt 3
{
	TL1 = 0xCD;				//设置定时初始值
	TH1 = 0xD4;				//设置定时初始值
	SMG_Proc();
	LED();
	n++;
	DAC_num++;
	LED_num++;
	if(n>=1000){read_ne555_flag=1;n=0;}
	if(++key_slow_down==10)  {key_slow_down=0;}
	if(fre>fre_max)
	{
		fre_max=fre;
		for(i=0;i<3;i++)
		{
			max_timer[i]=timer[i];
		}

	}
	if(DAC_num>=200){DAC_flag=1;DAC_num=0;}
	if(LED_num>=200){Lnum=~Lnum;LED_num=0;}

}
void ds1302_config()
{
	char i;
	Write_Ds1302_Byte(0x8e, 0x00);//打开
	for(i=0;i<7;i++)
	{
		Write_Ds1302_Byte(write_ds1302_adrr[i], timer[i]);
	}
	Write_Ds1302_Byte(0x8e, 0x80);//关闭
}
void read_ds1302_timer()
{
	char i;
	for(i=0;i<7;i++)
	{
		timer[i]=Read_Ds1302_Byte(read_ds1302_adrr[i]);
	}
}
void PFCwrite()
{
	if(DAC_flag==1)
	{
		if(fre<500){DA_Write(51);}
		else if(fre>fre_num){DA_Write(255);}
		else
		{
			V_num = 51 + (fre - 500) * 204.0f / (fre_num - 500);
			DA_Write(V_num);
		}
		DAC_flag=0;
	}

}
void LED()
{
	P0=0xff;
	if(mod==1){L1=Lnum;}
	else {L1=1;}
	if(fre>fre_num){L2=Lnum;}
	else {L2=1;}
	Init_74HC138(4);
}
void main()
{
	Init_system();
	Timer0_Init();
	Timer1_Init();
	ds1302_config();
	while(1)
	{
		key_Proc();
		displaySMG();
		read_ne555();
		read_ds1302_timer();
		PFCwrite();

	}
}
