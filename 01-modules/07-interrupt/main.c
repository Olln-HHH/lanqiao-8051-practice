#include <REGX52.H>

sbit L1=P0^0;
sbit L8=P0^7;

void Delay(unsigned int n)	//@11.0592MHz
{
	unsigned char data i, j;
	while(n--)  // 重复n次基础延时
	{
		i = 11;
		j = 190;
		do
		{
			while (--j);
		} while (--i);
	}
}

void SelectHC573()
{
	P2=(P2&0x1f)|0x80;
}
void Working()
{
	SelectHC573();
	L1=0;
	Delay(200);
	L1=1;
	Delay(200);

}
//=============================
void Init_INT0()
{
	IT0=1;//外部中断触发方式
	EX0=1;//外部中断使能打开
	EA=1;//总中断打开

}
volatile unsigned char stat_int=0;
void ServiceINT0() interrupt 0
{
	Delay(2);      // 防抖延时
	if(P3_2 == 0)    // 确认按键真的按下（不是抖动）
	{
		stat_int = 1;
		while(P3_2 == 0);  // 等待按键松开，避免长按重复触发
	}
}
void LEDINT()
{
	if(stat_int==1)
	{
		SelectHC573();
		L8=0;
		Delay(3000);

		L8=1;
		stat_int=0;
	}

}

//=============================
void main()
{

	while(1)
	{
		Init_INT0();
		Working();
		LEDINT();
	}
}
