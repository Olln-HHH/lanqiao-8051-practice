/* 头文件声明区域 */
#include "15f2k60s2.h"
#include <Init.h>
#include <Led.h>
#include <Key.h>
#include <Seg.h>
#include <ds1302.h>
#include <iic.h>
/* 变量声明区域 */
volatile unsigned char Key_val,Key_old,Key_down,Key_up;
volatile unsigned char Seg_buf[8] = {10,10,10,10,10,10,10,10};
volatile unsigned char ucLed[8] = {0,0,0,0,0,0,0,0};
volatile unsigned char Seg_point[8] = {0,0,0,0,0,0,0,0};
volatile unsigned char ucRtc[3] = {13,3,5};
volatile unsigned char ucRtc_freq_max[3] = {13,3,5};
volatile unsigned int Slow_down;
volatile bit Seg_slow_flag,Key_slow_flag;

long Freq;
volatile unsigned int Freq_count;
volatile bit Freq_ready; // 频率
volatile int Freq_add_disp; // 校准
volatile int Freq_add_ctrl;
volatile unsigned int Freq_max; // 回显最大频率
volatile int Freq_limit_disp = 2000; //超限
volatile int Freq_limit_ctrl = 2000;

volatile unsigned int Time_1000ms;
volatile unsigned int Time_200ms;
volatile bit enable;
volatile float Y; // 函数相关参数
volatile float K;
volatile float B_Y;

volatile unsigned char Seg_mode; // 0 - 频率 1 - 参数 2 - 时间 3 - 回显
volatile bit P_flag;  // 参数设置界面子界面 0 - 超限 1 - 校准
volatile bit H_flag;  // 频率回显界面子界面 0 - 频率 1 - 时间
/* 函数声明区域 */

void Key_proc()
{
	if(Key_slow_flag) return;
	Key_slow_flag = 1;

	Key_val = Key_read();
	Key_down = Key_val & (Key_val ^ Key_old);
	Key_up = ~Key_val & (Key_val ^ Key_old);
	Key_old = Key_val;

	switch(Key_down)
	{
		case 4:
		if(++Seg_mode == 4) Seg_mode = 0;
		if(Seg_mode == 2)
		{
			Freq_add_ctrl = Freq_add_disp;
			Freq_limit_ctrl = Freq_limit_disp;
		}
		if(Seg_mode == 1)
		{
			P_flag = 0;
			Freq_add_disp = Freq_add_ctrl;
			Freq_limit_disp = Freq_limit_ctrl;
		}
		if(Seg_mode == 3) H_flag = 0;
		break;
		case 5:
		if(Seg_mode == 1)
		{
			P_flag = ~P_flag;
		}
		else if(Seg_mode == 3)
		{
			H_flag = ~H_flag;
		}
		break;
		case 8: // +
		if(Seg_mode == 1 && P_flag == 0)
		{
			Freq_limit_disp += 1000;
			if(Freq_limit_disp > 9000) Freq_limit_disp = 9000;
		}
		else if(Seg_mode == 1 && P_flag == 1)
		{
			Freq_add_disp += 100;
			if(Freq_add_disp > 900) Freq_add_disp = 900;
		}
		break;
		case 9: // -
		if(Seg_mode == 1 && P_flag == 0)
		{
			Freq_limit_disp -= 1000;
			if(Freq_limit_disp < 1000) Freq_limit_disp = 1000;
		}
		else if(Seg_mode == 1 && P_flag == 1)
		{
			Freq_add_disp -= 100;
			if(Freq_add_disp < -900) Freq_add_disp = -900;
		}
		break;
	}
}

void Seg_proc()
{
	unsigned char i;
	if(Seg_slow_flag) return;
	Seg_slow_flag = 1;
	/* 信息获取区域 */
	Ds1302_read(ucRtc);
	/* 信息处理区域 */
	switch(Seg_mode)
	{
		case 0:
		for(i=0;i<8;i++) Seg_buf[i] = 10;
		Seg_buf[0] = 12;
		if(Freq + Freq_add_disp >= 0)
		{
			Seg_buf[3] = (Freq + Freq_add_disp) / 10000 % 10;
			Seg_buf[4] = (Freq + Freq_add_disp) / 1000 % 10;
			Seg_buf[5] = (Freq + Freq_add_disp) / 100 % 10;
			Seg_buf[6] = (Freq + Freq_add_disp) / 10 % 10;
			Seg_buf[7] = (Freq + Freq_add_disp) % 10;

			for(i=3;i<7;i++)
			{
				if(Seg_buf[i] == 0) Seg_buf[i] = 10;
				else break;
			}
		}
		else
		{
			Seg_buf[6] = 14;
			Seg_buf[7] = 14;
		}
		break;
		case 1:
		for(i=0;i<8;i++) Seg_buf[i] = 10;
		Seg_buf[0] = 13;
		if(P_flag == 0)
		{
			Seg_buf[1] = 1;
			Seg_buf[4] = Freq_limit_disp / 1000 % 10;
			for(i=5;i<8;i++) Seg_buf[i] = 0;
		}
		else
		{
			Seg_buf[1] = 2;
			if(Freq_add_disp < 0)
			{
				Seg_buf[4] = 11;
				Seg_buf[5] = (Freq_add_disp * -1) / 100 % 10;
			}
			else
			{
				Seg_buf[4] = 10;
				Seg_buf[5] = Freq_add_disp / 100 % 10;
			}

			for(i=6;i<8;i++) Seg_buf[i] = 0;
			for(i=5;i<7;i++)
			{
				if(Seg_buf[i] == 0) Seg_buf[i] = 10;
				else break;
			}
		}
		break;
		case 2:
		for(i=0;i<8;i++) Seg_buf[i] = 10;
		Seg_buf[0] = ucRtc[0] / 10 % 10;
		Seg_buf[1] = ucRtc[0] % 10;
		Seg_buf[2] = 11;
		Seg_buf[3] = ucRtc[1] / 10 % 10;
		Seg_buf[4] = ucRtc[1] % 10;
		Seg_buf[5] = 11;
		Seg_buf[6] = ucRtc[2] / 10 % 10;
		Seg_buf[7] = ucRtc[2] % 10;
		break;
		case 3:
		for(i=0;i<8;i++) Seg_buf[i] = 10;
		Seg_buf[0] = 15;
		if(H_flag == 0)
		{
			Seg_buf[1] = 12;
			Seg_buf[3] = Freq_max / 10000 % 10;
			Seg_buf[4] = Freq_max / 1000 % 10;
			Seg_buf[5] = Freq_max / 100 % 10;
			Seg_buf[6] = Freq_max / 10 % 10;
			Seg_buf[7] = Freq_max % 10;
			for(i=3;i<7;i++)
			{
				if(Seg_buf[i] == 0) Seg_buf[i] = 10;
				else break;
			}
		}
		else
		{
			Seg_buf[1] = 16;
			Seg_buf[2] = ucRtc_freq_max[0] / 10 % 10;
			Seg_buf[3] = ucRtc_freq_max[0] % 10;
			Seg_buf[4] = ucRtc_freq_max[1] / 10 % 10;
			Seg_buf[5] = ucRtc_freq_max[1] % 10;
			Seg_buf[6] = ucRtc_freq_max[2] / 10 % 10;
			Seg_buf[7] = ucRtc_freq_max[2] % 10;
		}
		break;
	}
}

void Timer1_Isr(void) interrupt 3
{
	if(++Slow_down == 200)
	{
		Slow_down = Seg_slow_flag = 0;
	}
	if(Slow_down % 10 == 0)
	{
		Key_slow_flag = 0;
	}
	if(++Time_1000ms == 1000)
	{
		Time_1000ms = 0;
		TR0=0;
		Freq_count=(unsigned int)TH0<<8|TL0;
		TL0=TH0=0;
		TR0=1;
		Freq_ready=1;
	}
	if(++Time_200ms == 200)
	{
		Time_200ms = 0;
		enable = ~enable;
	}
	Seg_disp(Slow_down % 8,Seg_buf[Slow_down % 8],Seg_point[Slow_down % 8]);
	Led_disp(Slow_down % 8,ucLed[Slow_down % 8]);
}

void Timer1_Init(void)		//1毫秒@12.000MHz
{
	AUXR &= 0xBF;			//定时器时钟12T模式
	TMOD &= 0x0F;			//设置定时器模式
	TL1 = 0x18;				//设置定时初始值
	TH1 = 0xFC;				//设置定时初始值
	TF1 = 0;				//清除TF1标志
	TR1 = 1;				//定时器1开始计时
	ET1 = 1;				//使能定时器1中断
	EA = 1;
}

void Timer0_Init(void)		//1毫秒@12.000MHz
{
	AUXR &= 0x7F;			//定时器时钟12T模式
	TMOD &= 0xF0;			//设置定时器模式
	TMOD |= 0x05;
	TL0 = 0x00;				//设置定时初始值
	TH0 = 0x00;				//设置定时初始值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时
}

void Led_proc()
{
	long Freq_pro = (long)Freq + Freq_add_disp; // 校准后

	/* 回显界面最大参数记录 */
	if(Freq_pro > (long)Freq_max && Freq_pro >= 0)
	{
		Freq_max = Freq_pro;
		Ds1302_read(ucRtc_freq_max);
	}
	/* ADC输出相关 */
	if(Freq_pro > Freq_limit_ctrl) AD_write(255);
	else if(Freq_pro < 500 && Freq_pro >= 0) AD_write(51);
	else if(Freq_pro < 0) AD_write(0);
	else
	{
		K = 4.0f / (Freq_limit_ctrl - 500);
		B_Y = 1.0f - K * 500;
		Y = K * (Freq_pro) + B_Y;
		AD_write((unsigned char)(Y * 51));
	}
	/* Led相关 */
	if(Seg_mode == 0)
	{
		ucLed[0] = enable;
	}
	else ucLed[0] = 0;
	if(Freq + Freq_add_ctrl > Freq_limit_ctrl && Freq + Freq_add_ctrl >= 1000)
	{
		ucLed[1] = enable;
	}
	else if(Freq + Freq_add_ctrl < 0) ucLed[1] = 1;
	else ucLed[1] = 0;
}

/* 主函数 */
void main()
{
	bit ea_old;
	Init();
	Timer1_Init();
	Timer0_Init();
	Ds1302_set(ucRtc);
	while(1)
	{
		if(Freq_ready)
		{
			ea_old=EA;EA=0;
			Freq=Freq_count;Freq_ready=0;
			EA=ea_old;
		}
		Key_proc();
		Seg_proc();
		Led_proc();
	}
}
