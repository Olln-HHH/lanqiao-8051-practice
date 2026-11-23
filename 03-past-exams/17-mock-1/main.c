#include "15f2k60s2.h"
#include "ds1302.h"
#include "key.h"
#include "intrins.h"
volatile unsigned char smg_pos=0;
volatile unsigned char smg_data[16]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e};
volatile unsigned char smg_buf[8]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
volatile unsigned char key_val,key_up,key_down,key_old;
volatile unsigned char key_slow_down;
volatile unsigned char write_ds1302_addr[7]={0x80,0x82,0x84,0x86,0x88,0x8a,0x8c};
volatile unsigned char read_ds1302_addr[7]={0x81,0x83,0x85,0x87,0x89,0x8b,0x8d};
volatile unsigned char timer[7]={0x50,0x59,0x23,0x02,0x02,0x02,0x26};
volatile unsigned char timer1[3]={0x00,0x00,0x00};
volatile unsigned char timer2[3]={0x00,0x00,0x00};
volatile unsigned char timer3[3]={0x00,0x00,0x00};
volatile unsigned int jl=0;
volatile unsigned char smg_mod=1;
volatile unsigned char record_mod=1;
sbit Tx=P1^0;
sbit Rx=P1^1;
sbit L1=P0^0;
sbit L2=P0^1;
sbit L3=P0^2;
sbit fmq=P0^6;
volatile unsigned char fmq_flag=0;
void HC138(unsigned char channel)
{
	switch(channel)
	{
		case 4:P2=(P2&0x1f)|0x80;break;
		case 5:P2=(P2&0x1f)|0xa0;break;
		case 6:P2=(P2&0x1f)|0xc0;break;
		case 7:P2=(P2&0x1f)|0xe0;break;
	}
	P2=(P2&0x1f)|0x00;
}
void init_system()
{
	P0=0xff;
	HC138(4);

	P0=0x00;
	HC138(5);
}
void smg_porc()
{
	P0=0xff;
	HC138(7);

	P0=1<<smg_pos;
	HC138(6);

	P0=smg_buf[smg_pos];
	HC138(7);

	smg_pos++;
	if(smg_pos>=8){smg_pos=0;}
}
void displaySMG()
{
	if(smg_mod==1)
	{
		smg_buf[0]=~0x38;
		smg_buf[1]=0xff;
		smg_buf[2]=0xff;
		smg_buf[3]=0xff;
		smg_buf[4]=0xff;
		smg_buf[5]=smg_data[jl/100];
		smg_buf[6]=smg_data[jl/10%10];
		smg_buf[7]=smg_data[jl%10];
	}
	else if(smg_mod==2)
	{
		smg_buf[0]=smg_data[timer[2]/16];
		smg_buf[1]=smg_data[timer[2]%16];
		smg_buf[2]=~0x40;
		smg_buf[3]=smg_data[timer[1]/16];
		smg_buf[4]=smg_data[timer[1]%16];
		smg_buf[5]=~0x40;
		smg_buf[6]=smg_data[timer[0]/16];
		smg_buf[7]=smg_data[timer[0]%16];
	}
	else if((smg_mod==3)&&(record_mod==1))
	{
		smg_buf[0]=smg_data[10];
		smg_buf[1]=smg_data[1];
		smg_buf[2]=smg_data[timer1[2]/16];
		smg_buf[3]=smg_data[timer1[2]%16];
		smg_buf[4]=smg_data[timer1[1]/16];
		smg_buf[5]=smg_data[timer1[1]%16];
		smg_buf[6]=smg_data[timer1[0]/16];
		smg_buf[7]=smg_data[timer1[0]%16];
	}
	else if((smg_mod==3)&&(record_mod==2))
	{
		smg_buf[0]=smg_data[10];
		smg_buf[1]=smg_data[2];
		smg_buf[2]=smg_data[timer2[2]/16];
		smg_buf[3]=smg_data[timer2[2]%16];
		smg_buf[4]=smg_data[timer2[1]/16];
		smg_buf[5]=smg_data[timer2[1]%16];
		smg_buf[6]=smg_data[timer2[0]/16];
		smg_buf[7]=smg_data[timer2[0]%16];
	}
	else if((smg_mod==3)&&(record_mod==3))
	{
		smg_buf[0]=smg_data[10];
		smg_buf[1]=smg_data[3];
		smg_buf[2]=smg_data[timer3[2]/16];
		smg_buf[3]=smg_data[timer3[2]%16];
		smg_buf[4]=smg_data[timer3[1]/16];
		smg_buf[5]=smg_data[timer3[1]%16];
		smg_buf[6]=smg_data[timer3[0]/16];
		smg_buf[7]=smg_data[timer3[0]%16];
	}
}
void key_porc()
{
	if(key_slow_down)return;
	key_slow_down=1;
	key_val=key_read();
	key_down=key_val&(key_old^key_val);
	key_up=~key_val&(key_old^key_val);
	key_old=key_val;
	switch(key_down)
	{
		case 4:
		smg_mod++;
		if(smg_mod>3){smg_mod=1;record_mod=1;}
		break;
		case 5:
		record_mod++;
		if(record_mod>3){record_mod=1;}
		break;
		case 8:
		timer3[0]=0x00;
		timer3[1]=0x00;
		timer3[2]=0x00;

		timer2[0]=0x00;
		timer2[1]=0x00;
		timer2[2]=0x00;

		timer1[0]=0x00;
		timer1[1]=0x00;
		timer1[2]=0x00;
		break;
		case 9:
		fmq_flag=0;

		break;
	}
}
void ds1302_config()
{
	char i;
	Write_Ds1302_Byte(0x8e,0x00);
	for(i=0;i<7;i++)
	{
		Write_Ds1302_Byte(write_ds1302_addr[i],timer[i]);
	}
	Write_Ds1302_Byte(0x8e,0x80);
}
void ds1302_read()
{
	char i;
	for(i=0;i<7;i++)
	{
		timer[i]=Read_Ds1302_Byte(read_ds1302_addr[i]);
	}
}
void ds1302_jilu()
{
	if(jl<30)
	{
		timer3[0]=timer2[0];
		timer3[1]=timer2[1];
		timer3[2]=timer2[2];

		timer2[0]=timer1[0];
		timer2[1]=timer1[1];
		timer2[2]=timer1[2];

		timer1[0]=timer[0];
		timer1[1]=timer[1];
		timer1[2]=timer[2];
	}
}
void Delay12us(void)	//@11.0592MHz
{
	unsigned char data i;

	_nop_();
	_nop_();
	_nop_();
	i = 30;
	while (--i);
}

void ut_wave_init()
{
	bit ea_old;
	char i;
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
unsigned int ut_wave_data()
{
	unsigned int dis=0;
	CR=0;CF=0;
	CMOD=0;
	CH=CL=0;
	ut_wave_init();
	CR=1;
	while((Rx==1)&&(CF==0));
	CR=0;
	if(CF==0)
	{
		dis=CH<<8|CL;
		dis=(unsigned int)((unsigned long)dis*17/1000);
		if(dis>999){dis=999;}
	}
	else
	{
		dis=999;
		CF=0;
	}
	return dis;
}

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
void fmq_bj()
{
	P0=0x00;
	if(jl>=30){fmq_flag=1;}
	if((jl<30)&&(fmq_flag==1))
	{
		fmq=1;
	}
	else
	{
		fmq=0;
	}
	HC138(5);
}
void LED()
{
	P0=0xff;
	if(smg_mod==1){L1=0;}
	else {L1=1;}

	if(smg_mod==2){L2=0;}
	else {L2=1;}

	if(smg_mod==3){L3=0;}
	else {L3=1;}
	HC138(4);
}
volatile unsigned int ms500=0;
volatile unsigned char ms500_flag=0;
void Timer0_Isr(void) interrupt 1
{
	TL0 = 0x20;				//设置定时初始值
	TH0 = 0xD1;				//设置定时初始值
	if(++key_slow_down==50){key_slow_down=0;}
	smg_porc();
	ms500++;
	if(ms500==500){ms500_flag=1;ms500=0;fmq_bj();LED();}
}

void main()
{
	init_system();
	Timer0_Init();
	ds1302_config();

	while(1)
	{
		key_porc();
		displaySMG();
		if(ms500_flag==1)
		{
			ds1302_read();
			jl=ut_wave_data();
			ds1302_jilu();
			ms500_flag=0;
		}
	}
}
