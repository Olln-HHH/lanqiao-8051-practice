#include <REGX52.H>
#include "onewire.h"
#include "smg_ca.h"
float temp;
void selecthc573(unsigned char channel)
{
	switch(channel)
	{
		case 0:
		P2 = P2 & 0x1f;
		break;
		case 4:
		P2=(P2&0x1f)|0x80;
		break;
		case 5:
		P2=(P2&0x1f)|0xa0;
		break;
		case 6:
		P2=(P2&0x1f)|0xc0;
		break;
		case 7:
		P2=(P2&0x1f)|0xe0;
		break;
	}
}

void displaysmg_temp()
{
	displaysmg_bit(7,SMGNoDot_CA[(int)(temp*10)%10]);
	delaysmg(100);
	displaysmg_bit(6,SMGDot_CA[(int)temp%10]);
	delaysmg(100);
	displaysmg_bit(5,SMGNoDot_CA[(int)temp/10]);
	delaysmg(100);

	displaysmg_bit(4,0xff);
	delaysmg(100);
	displaysmg_bit(3,0xff);
	delaysmg(100);
	displaysmg_bit(2,0xff);
	delaysmg(100);
	displaysmg_bit(1,0xff);
	delaysmg(100);
	displaysmg_bit(0,0xff);
	delaysmg(100);

	display_all(0xff);
}
void delay(unsigned int t)
{
	while(t--)
	{
		displaysmg_temp();
	}
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
void main()
{
	selecthc573(4);
	P0=0xff;
	selecthc573(0);
	while(1)
	{
		read_ds18b20_temp();
		displaysmg_temp();
	}
}
