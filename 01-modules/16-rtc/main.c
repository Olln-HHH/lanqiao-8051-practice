#include <REGX52.H>
#include "ds1302.h"
#include "smg.h"
void selecthc573(unsigned char channel);
unsigned char write_ds1302_adrr[7]={0x80,0x82,0x84,0x86,0x88,0x8a,0x8c};
unsigned char read_ds1302_adrr[7]={0x81,0x83,0x85,0x87,0x89,0x8b,0x8d};
//2025.12.10 周三 20:51:20
//从左到右依次是  秒 分 时 日 月 周几  年
unsigned char timer[7]={0x20,0x51,0x20,0x10,0x12,0x03,0x25};

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
void display_ds1302()
{
	displaysmg_bit(0,SMGNoDot_CA[timer[2]/16]);
	delaysmg(100);
	displaysmg_bit(1,SMGNoDot_CA[timer[2]%16]);
	delaysmg(100);

	displaysmg_bit(2,0xbf);
	delaysmg(100);

	displaysmg_bit(3,SMGNoDot_CA[timer[1]/16]);
	delaysmg(100);
	displaysmg_bit(4,SMGNoDot_CA[timer[1]%16]);
	delaysmg(100);

	displaysmg_bit(5,0xbf);
	delaysmg(100);

	displaysmg_bit(6,SMGNoDot_CA[timer[0]/16]);
	delaysmg(100);
	displaysmg_bit(7,SMGNoDot_CA[timer[0]%16]);
	delaysmg(100);
}
void main()
{

	ds1302_config();
	selecthc573(4);
	P0=0xff;
	while(1)
	{

		read_ds1302_timer();
		display_ds1302();
		selecthc573(6);
		P0=0x00;
	}

}
