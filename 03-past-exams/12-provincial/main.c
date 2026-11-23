#include "15f2k60s2.h"
#include "onewire.h"
#include "iic.h"
#include "key.h"
volatile unsigned char smg_pos=0;
volatile unsigned char smg_buf[8]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
volatile unsigned char smg_data[16]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e};
volatile unsigned char key_val,key_old,key_up,key_down;
volatile unsigned char key_slow_down;
volatile unsigned char smg_mod=1;
volatile unsigned int V_int=0;
volatile unsigned char DAC_mod=1;
volatile int wd=25;
volatile int main_temp;
volatile bit read_flag;
volatile unsigned int read_count;
sbit L1=P0^0;
sbit L2=P0^1;
sbit L3=P0^2;
sbit L4=P0^3;
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
		if(smg_mod>3)
		{smg_mod=1;}
		break;
		case 5:
		if(DAC_mod==1){DAC_mod=0;}
		else {DAC_mod=1;}
		break;
		case 8:
		if(wd>0){wd--;}
		break;
		case 9:
		if(wd<99){wd++;}
		break;

	}
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
volatile float DA_data=0;
void DAC_js()
{
	if(DAC_mod==1)
	{
		if(main_temp<wd*100)
		{
			DA_write(0);
			V_int=000;
		}
		else
		{
			DA_write(255);
			V_int=500;
		}
	}
	else
	{
		if(main_temp>4000)
		{
			DA_write(204);
			V_int=400;
		}
		else if(main_temp<2000)
		{
			DA_write(51);
			V_int=100;
		}
		else
		{
			DA_data=(((main_temp/100.00f)*3/20)-2);
			DA_write((unsigned int)(DA_data*51));
			V_int=100*(((main_temp/100.00f)*3/20)-2);
		}
	}
}
void LED_porc()
{
	P0=0xff;
	if(DAC_mod==1){L1=0;}
	else {L1=1;}
	if(smg_mod==1){L2=0;}
	else {L2=1;}
	if(smg_mod==2){L3=0;}
	else {L3=1;}
	if(smg_mod==3){L4=0;}
	else {L4=1;}
	HC138(4);
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
		main_temp=(int)((long)raw*100/16);
		EA=ea_old;
	}
}
void displaySMG()
{
	if(smg_mod==1)
	{
		smg_buf[0]=0xc6;
		smg_buf[1]=0xff;
		smg_buf[2]=0xff;
		smg_buf[3]=0xff;
		smg_buf[4]=smg_data[main_temp/1000];
		smg_buf[5]=smg_data[main_temp/100%10]-0x80;
		smg_buf[6]=smg_data[main_temp/10%10];
		smg_buf[7]=smg_data[main_temp%10];

	}
	else if(smg_mod==2)
	{
		smg_buf[0]=~0x73;
		smg_buf[1]=0xff;
		smg_buf[2]=0xff;
		smg_buf[3]=0xff;
		smg_buf[4]=0xff;
		smg_buf[5]=0xff;
		smg_buf[6]=smg_data[wd/10];
		smg_buf[7]=smg_data[wd%10];

	}
	else if(smg_mod==3)
	{
		smg_buf[0]=0x88;
		smg_buf[1]=0xff;
		smg_buf[2]=0xff;
		smg_buf[3]=0xff;
		smg_buf[4]=0xff;
		smg_buf[5]=smg_data[V_int/100]-0x80;
		smg_buf[6]=smg_data[V_int/10%10];
		smg_buf[7]=smg_data[V_int%10];

	}
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
void Timer0_Isr(void) interrupt 1
{
	TL0 = 0x20;				//设置定时初始值
	TH0 = 0xD1;				//设置定时初始值
	if(++key_slow_down==50){key_slow_down=0;LED_porc();}
	if(++read_count==500){read_flag=1;read_count=0;}
	smg_porc();

}
void main()
{
	init_system();
	Timer0_Init();
	while(1)
	{
		if(read_flag==1)
		{
			read_temp();
			DAC_js();
			read_flag=0;
		}
		displaySMG();
		key_porc();

	}
}
