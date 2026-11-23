#include <REGX52.H>
#include "onewire.h"
#include "smg_swj.h"
float temp;
void Init_74HC138(unsigned char n)
{
	switch(n)
	{
		case 4:P2=(P2&0x1f)|0x80;break;
		case 5:P2=(P2&0x1f)|0xa0;break;
		case 6:P2=(P2&0x1f)|0xc0;break;
		case 7:P2=(P2&0x1f)|0xe0;break;
		case 0:P2=(P2&0x1f)|0x00;break;
	}
	P2=(P2&0x1f)|0x00;
}
void Init_System()
{
	Init_74HC138(0);
	P0 = 0x00;
	Init_74HC138(5);
	P0 = 0xff;
	Init_74HC138(4);
}
void delay(unsigned int t)
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
void SMG_Display_Data()
{

	SMG_DisplayBit(0,0xff);
	//delay(200);
	SMG_DisplayBit(1,0xff);
	//delay(200);
	SMG_DisplayBit(2,0xff);
	//delay(200);
	SMG_DisplayBit(3,0xff);
	//delay(200);
	SMG_DisplayBit(4,0xff);
	//delay(200);
	SMG_DisplayBit(5,smg_data[(int)temp/10]);
	delay(200);
	SMG_DisplayBit(6,smg_data[(int)temp%10]-0x80);
	delay(200);
	SMG_DisplayBit(7,smg_data[(int)(temp*10)%10]);
	delay(200);
	SMG_All(0xff);
	delay(200);

}
void main()
{
	Init_System();
	while(1)
	{
		static unsigned int cnt = 0;
		if(cnt >= 500)  // 约1秒读取一次温度
		{
			read_ds18b20_temp();
			cnt = 0;
		}
		cnt++;
		SMG_Display_Data();
	}
}
