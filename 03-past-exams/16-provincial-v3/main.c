#include "15f2k60s2.h"
#include "onewire.h"
#include "iic.h"
#include "key.h"
#include "intrins.h"
sbit Tx=P1^0;
sbit Rx=P1^1;
unsigned char check_v();
volatile unsigned char smg_pos=0;
volatile unsigned char smg_buf[8]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
unsigned char smg_data[16]={0xc0,0xf9,
	0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
	0x88,0x83,0xc6,0xa1,0x86,0x8e};
volatile unsigned char key_val,key_down,key_up,key_old;
volatile unsigned char key_slow_down;
volatile unsigned char V_val;
volatile unsigned int V_int;
volatile unsigned char smg_mod=1;
volatile unsigned char cs_mod=0;
volatile unsigned char smg_sun=0;
volatile int cs_wd=30;
volatile int temp=0;
volatile unsigned int cs_jl=30;
volatile unsigned int jl=0;
volatile unsigned char jdq_count=0;
volatile unsigned char jdq_flag=0;
sbit L1=P0^0;
sbit L2=P0^1;
sbit L3=P0^2;
sbit L4=P0^3;
sbit L8=P0^7;
sbit jdq=P0^4;
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
		smg_buf[0]=smg_data[12];
		smg_buf[1]=smg_data[temp/10];
		smg_buf[2]=smg_data[temp%10];
		smg_buf[3]=0xff;
		smg_buf[4]=0xff;
		smg_buf[5]=0xff;
		smg_buf[6]=~0x37;
		smg_buf[7]=smg_data[smg_sun];
	}
	else if(smg_mod==2)
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
	else if((smg_mod==3)&&(cs_mod==0))
	{
		smg_buf[0]=~0x73;
		smg_buf[1]=smg_data[12];
		smg_buf[2]=0xff;
		smg_buf[3]=0xff;
		smg_buf[4]=0xff;
		smg_buf[5]=0xff;
		smg_buf[6]=smg_data[cs_wd/10];
		smg_buf[7]=smg_data[cs_wd%10];
	}
	else if((smg_mod==3)&&(cs_mod==1))
	{
		smg_buf[0]=~0x73;
		smg_buf[1]=~0x38;
		smg_buf[2]=0xff;
		smg_buf[3]=0xff;
		smg_buf[4]=0xff;
		smg_buf[5]=0xff;
		smg_buf[6]=smg_data[cs_jl/10];
		smg_buf[7]=smg_data[cs_jl%10];
	}
	else if(smg_mod==4)
	{
		smg_buf[0]=~0x37;
		smg_buf[1]=smg_data[12];
		smg_buf[2]=0xff;
		smg_buf[3]=0xff;
		smg_buf[4]=0xff;
		smg_buf[5]=0xff;
		smg_buf[6]=smg_data[jdq_count/10];
		smg_buf[7]=smg_data[jdq_count%10];
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

		if(smg_mod>4){smg_mod=1;cs_mod=0;}
		break;
		case 5:
		if(smg_mod==3)
		{
			cs_mod=!cs_mod;
		}
		break;
		case 8:
		if(smg_mod==3&&cs_mod==0){cs_wd++;}
		if(smg_mod==3&&cs_mod==1){cs_jl+=5;}
		if(cs_wd>80){cs_wd=80;}
		if(cs_jl>80){cs_jl=80;}
		break;
		case 9:
		if(smg_mod==3&&cs_mod==0){cs_wd--;}
		if(smg_mod==3&&cs_mod==1){cs_jl-=5;}
		if(cs_wd<20){cs_wd=20;}
		if(cs_jl<20){cs_jl=20;}
		break;
	}
}
void get_V(unsigned char channel)
{
	V_val=AD_read(channel);
	V_int=(unsigned int)(((unsigned long)V_val*500+127)/255);
}

unsigned char check_v()
{
	unsigned char level=0;
	if(V_int>=300){level=1;}
	if((V_int>=200)&&(V_int<300)){level=2;}
	if((V_int>=50)&&(V_int<200)){level=3;}
	if(V_int<50){level=4;}
	smg_sun=level;
	return level;

}
void turn_led()
{
	unsigned char level=0;
	level=check_v();
	P0=0xff;
	if(jl<cs_jl)
	{
		if(level==1){L1=0;L2=1;L3=1;L4=1;}
		if(level==2){L1=0;L2=0;L3=1;L4=1;}
		if(level==3){L1=0;L2=0;L3=0;L4=1;}
		if(level==4){L1=0;L2=0;L3=0;L4=0;}
	}
	else
	{
		L1=1;L2=1;L3=1;L4=1;
	}
	HC138(4);
}
volatile unsigned int count_2s=0;
void jdq_porc()
{
	P0=0x00;
	if((jl<cs_jl)&&(temp>cs_wd))
	{
		jdq=1;
		if(jdq_flag==0)
		{
			if(jdq_count<99){jdq_count++;}
			jdq_flag=1;
		}
	}
	else
	{
		jdq=0;
		jdq_flag=0;
	}
	HC138(5);
	if(key_old==99)
	{
		count_2s++;
		if(count_2s==2000)
		{
			jdq_count=0;
			count_2s=0;
		}
	}
	else
	{
		count_2s=0;
	}

}

void Delay12us(void)	//@12.000MHz
{
	unsigned char data i;
	_nop_();
	_nop_();
	i = 33;
	while (--i);
}

void ut_wave_init()
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
unsigned int ut_wave_dis()
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
		CF=0;
		dis=999;
	}
	return dis;
}
void Timer0_Init(void)		//1����@12.000MHz
{
	AUXR |= 0x80;			//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;			//���ö�ʱ��ģʽ
	TL0 = 0x20;				//���ö�ʱ��ʼֵ
	TH0 = 0xD1;				//���ö�ʱ��ʼֵ
	TF0 = 0;				//���TF0��־
	TR0 = 1;				//��ʱ��0��ʼ��ʱ
	ET0 = 1;				//ʹ�ܶ�ʱ��0�ж�
	EA=1;
}
volatile unsigned int ms100=0;
volatile unsigned int ms500=0;

volatile bit ms500_flag=0;
volatile bit ms100_flag=0;
void Timer0_Isr(void) interrupt 1
{
	if(++key_slow_down==50)
	{
		key_slow_down=0;
	}
	if(++ms100==100)
	{
		ms100=0;
		turn_led();
		ms100_flag=1;

	}
	if(++ms500==500)
	{
		ms500=0;
		ms500_flag=1;

	}
	smg_porc();
	jdq_porc();
}
void main()
{
	init_system();
	Timer0_Init();
	ut_wave_init();

	while(1)
	{
		key_porc();
		displaySMG();
		if(ms100_flag==1)
		{
			get_V(0x01);
			ms100_flag=0;
		}
		if(ms500_flag==1)
		{
			jl=ut_wave_dis();
			temp=read_temp();
			ms500_flag=0;
		}
	}
}
