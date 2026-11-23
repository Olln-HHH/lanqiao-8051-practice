#include "15f2k60s2.h"
#include "key.h"
#include "iic.h"
#include "onewire.h"
#include "ds1302.h"
#include "intrins.h"
sbit Tx=P1^0;
sbit Rx=P1^1;
volatile unsigned int temp=0;
volatile unsigned char smg_mod=1;
volatile unsigned int jl=0;
volatile unsigned int V_int=0;
volatile unsigned char smg_pos=0;
volatile unsigned char smg_buf[8]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
volatile unsigned char smg_data[16]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e};
volatile unsigned char key_val,key_up,key_down,key_old;
volatile unsigned char key_slow_down=0;
volatile unsigned char write_ds1302_addr[7]={0x80,0x82,0x84,0x86,0x88,0x8a,0x8c};
volatile unsigned char read_ds1302_addr[7]={0x81,0x83,0x85,0x87,0x89,0x8b,0x8d};
volatile unsigned char timer[7]={0x00,0x00,0x00,0x00,0x00,0x00,0x00};
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
void delay(unsigned int t)
{
	while(t--)

	{
		unsigned char data i, j;

		_nop_();
		_nop_();
		_nop_();
		i = 11;
		j = 190;
		do
		{
			while (--j);
		} while (--i);

	}
}
void init_system()
{

	P0=0xff;
	HC138(4);

	P0=0x00;
	HC138(5);

}
void smg_proc()
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
		smg_buf[0]=smg_data[temp/100];
		smg_buf[1]=smg_data[temp/10%10];
		smg_buf[2]=~0x40;
		smg_buf[3]=smg_data[timer[1]/16];
		smg_buf[4]=smg_data[timer[1]%16];
		smg_buf[5]=~0x40;
		smg_buf[6]=smg_data[timer[0]/16];
		smg_buf[7]=smg_data[timer[0]%16];
	}
	else if(smg_mod==2)
	{
		smg_buf[0]=smg_data[V_int/100]-0x80;
		smg_buf[1]=smg_data[V_int/10%10];
		smg_buf[2]=smg_data[V_int%10];
		smg_buf[3]=0xff;
		smg_buf[4]=0xff;
		smg_buf[5]=smg_data[jl/100];
		smg_buf[6]=smg_data[jl/10%10];
		smg_buf[7]=smg_data[jl%10];
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
		case 12:
		smg_mod++;
		if(smg_mod==3)
		{
			smg_mod=1;
		}
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
void read_ds1302()
{
	char i;
	for(i=0;i<7;i++)
	{
		timer[i]=Read_Ds1302_Byte(read_ds1302_addr[i]);
	}
}
void read_tmep()
{
	int raw;
	bit ea_old;
	if(OneWire_ReadRaw(&raw))
	{
		/* 当前数码管界面显示0~99度，先限制范围再取段码。 */
		if(raw<0){raw=0;}
		if(raw>1584){raw=1584;}
		ea_old=EA;EA=0;
		temp=(int)((long)raw*10/16);
		EA=ea_old;
	}
}
volatile unsigned char ad_val;
void get_V(unsigned char channel)
{
	ad_val=AD_read(channel);
	V_int=(unsigned int)((ad_val*500.0+127)/255);
}
sbit L1=P0^0;
sbit L2=P0^1;
sbit L3=P0^2;
sbit L4=P0^3;
sbit L5=P0^4;
sbit L8=P0^7;
volatile unsigned char L8_falg=0;
void LED()
{
	P0=0xff;
	if((V_int>400)&&(V_int<=500)){}
	else if((V_int>300)&&(V_int<=400)){}
	else if((V_int>200)&&(V_int<=300)){L1=0;}
	else if((V_int>100)&&(V_int<=200)){L1=0;L2=0;}
	else if((V_int>=0)&&(V_int<=100)){L1=0;L2=0;L3=0;}
	//L8=L8_falg;
	HC138(4);
}
void Delay12us(void)	//@12.000MHz
{
	unsigned char data i;

	_nop_();
	_nop_();
	i = 33;
	while (--i);
}

void Ut_wave_init()
{
	bit ea_old;
	unsigned char i=0;
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
unsigned int Ut_dis_data()
{
	unsigned int dis=0;
	CR=0;CF=0;
	CMOD=0;
	CH=CL=0;
	Ut_wave_init();
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
		CF=0;
		dis=999;
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
volatile unsigned int ds_count=0;
volatile unsigned char ds_flag=0;
volatile unsigned int v_count=0;
volatile unsigned char v_flag=0;
void Timer0_Isr(void) interrupt 1
{
	smg_proc();
	if(++key_slow_down==10){key_slow_down=0;}
	if(++ds_count==500){ds_flag=1;ds_count=0;}
	if(++v_count==100){v_flag=1;v_count=0;LED();L8_falg=~L8_falg;}

}
void main()
{
	init_system();
	Timer0_Init();
	ds1302_config();
	while(1)
	{
		displaySMG();
		key_porc();
		if(ds_flag==1)
		{
			read_ds1302();
			read_tmep();
			ds_flag=0;
		}
		if(v_flag==1)
		{
			v_flag=0;
			get_V(0x01);
			jl=Ut_dis_data();
		}
	}
}
