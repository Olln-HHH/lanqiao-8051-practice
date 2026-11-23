#include "15f2k60s2.h"
#include "key.h"
#include "onewire.h"
#include "ds1302.h"
volatile unsigned char smg_pos=0;
volatile unsigned char smg_buf[8]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
volatile unsigned char smg_data[16]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e};
volatile unsigned char key_val,key_up,key_down,key_old;
volatile unsigned char key_slow_down=0;
volatile unsigned char write_ds1302_addr[7]={0x80,0x82,0x84,0x86,0x88,0x8a,0x8c};
volatile unsigned char read_ds1302_addr[7]={0x81,0x83,0x85,0x87,0x89,0x8b,0x8d};
volatile unsigned char timer[7]={0x55,0x59,0x20,0x01,0x04,0x03,0x26};
volatile int temp;
volatile unsigned char smg_mod=1;
volatile unsigned char jdq_mod=1;
volatile unsigned char sj_mod=0;
volatile int wd=23;
volatile unsigned int read_temp_count=0;
volatile unsigned char read_temp_flag=0;
volatile unsigned int read_ds_count=0;
volatile unsigned char read_ds_flag=0;
volatile unsigned int count_jdq=0;
volatile unsigned char L3_num=0;
volatile unsigned char count_L3=0;
volatile unsigned char jdq_flag=1;
sbit jdq=P0^4;
sbit fmq=P0^6;
sbit L2=P0^1;
sbit L3=P0^2;
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
void init_system()
{
	P0=0xff;
	HC138(4);

	P0=0x00;
	HC138(5);
}
void smg_disp()
{
	if(smg_mod==1)
	{
		smg_buf[0]=~0x3e;
		smg_buf[1]=smg_data[1];
		smg_buf[2]=0xff;
		smg_buf[3]=0xff;
		smg_buf[4]=0xff;
		smg_buf[5]=smg_data[temp/100];
		smg_buf[6]=smg_data[temp/10%10]-0x80;
		smg_buf[7]=smg_data[temp%10];

	}
	else if((smg_mod==2)&&(sj_mod==0))
	{
		smg_buf[0]=~0x3e;
		smg_buf[1]=smg_data[2];
		smg_buf[2]=0xff;
		smg_buf[3]=smg_data[timer[2]/16];
		smg_buf[4]=smg_data[timer[2]%16];
		smg_buf[5]=~0x40;
		smg_buf[6]=smg_data[timer[1]/16];
		smg_buf[7]=smg_data[timer[1]%16];
	}
	else if((smg_mod==2)&&(sj_mod==1))
	{
		smg_buf[0]=~0x3e;
		smg_buf[1]=smg_data[2];
		smg_buf[2]=0xff;
		smg_buf[3]=smg_data[timer[1]/16];
		smg_buf[4]=smg_data[timer[1]%16];
		smg_buf[5]=~0x40;
		smg_buf[6]=smg_data[timer[0]/16];
		smg_buf[7]=smg_data[timer[0]%16];
	}
	else if(smg_mod==3)
	{
		smg_buf[0]=~0x3e;
		smg_buf[1]=smg_data[3];
		smg_buf[2]=0xff;
		smg_buf[3]=0xff;
		smg_buf[4]=0xff;
		smg_buf[5]=0xff;
		smg_buf[6]=smg_data[wd/10];
		smg_buf[7]=smg_data[wd%10];
	}
}
void key_proc()
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
		if(smg_mod>3){smg_mod=1;}
		break;
		case 13:
		jdq_mod=~jdq_mod;
		break;
		case 16:
		if(smg_mod==3){if(wd<99){wd++;}}

		break;
		case 17:
		if(smg_mod==3){if(wd>0){wd--;}}
		if(smg_mod==2){sj_mod=~sj_mod;}
		break;
	}
	if((key_old==17)&&(smg_mod==2)){sj_mod=1;}
	else {sj_mod=0;}
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
void read_ds1302_timer()
{
	char i;
	for(i=0;i<7;i++)
	{
		timer[i]=Read_Ds1302_Byte(read_ds1302_addr[i]);
	}
}
void read_temp()
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
void jdq_proc()
{
	if(jdq_mod==1)
	{
		if(temp/10>wd){jdq=1;jdq_flag=1;}
		else
		{jdq=0;jdq_flag=0;}
	}
	else
	{
		if(((timer[1])==0x00)&&((timer[0])==0x00))
		{
			jdq=1;jdq_flag=1;count_jdq=0;
		}
		else
		{
			jdq=0;jdq_flag=0;
		}
		if(jdq_flag == 1)
		{
			count_jdq++;
		}
		if(count_jdq>=5000)
		{
			jdq=0;jdq_flag=0;
			count_jdq=0;
		}

	}
	fmq=0;
	HC138(5);
}
void led_proc()
{
	P0=0xff;
	if(jdq_mod==1){L2=0;}
	else {L2=1;}
	if(jdq_flag==1){L3=L3_num;}
	if(jdq_flag==0)
	{L3=1;}
	HC138(4);
}

void Timer0_Init(void)		//1����@12.000MHz
{
	AUXR |= 0x80;			//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;			//
	TL0 = 0x20;				//���ö�ʱ��ʼֵ
	TH0 = 0xD1;				//���ö�ʱ��ʼֵ
	TF0 = 0;				//���TF0��־
	TR0 = 1;				//��ʱ��0��ʼ��ʱ
	EA=1;
	ET0=1;
}

void timer1() interrupt 1
{
	TL0 = 0x20;				//���ö�ʱ��ʼֵ
	TH0 = 0xD1;				//���ö�ʱ��ʼֵ
	smg_proc();
	if(read_ds_flag==1){jdq_proc();}
	if(++count_L3==100){L3_num=~L3_num;count_L3=0;led_proc();}
	if(++key_slow_down==50){key_slow_down=0;}
	if(++read_temp_count==500){read_temp_flag=1;read_temp_count=0;}
	if(++read_ds_count==1000){read_ds_flag=1;read_ds_count=0;}
}
void main()
{
	init_system();
	Timer0_Init();
	ds1302_config();
	read_temp();
	while(1)
	{
		key_proc();
		smg_disp();
		if(read_temp_flag==1)
		{
			read_temp_flag=0;
			read_temp();
		}
		if(read_ds_flag==1)
		{
			read_ds_flag=0;
			read_ds1302_timer();
		}
	}
}
