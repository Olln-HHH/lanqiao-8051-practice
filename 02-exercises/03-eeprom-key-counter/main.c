#include <REGX52.H>
#include "iic.h"
#include "smg_swj.h"
sbit S6=P3^1;
sbit S5=P3^2;
sbit S4=P3^3;
unsigned int num6=0;
unsigned int num5=0;
unsigned int num4=0;
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
//AT24c02单字节写
void AT24C02_WaitReady(void)
{
	unsigned int n;
	unsigned char ack;
	for(n=0;n<1000;n++)
	{
		I2CStart();
		I2CSendByte(0xa0);
		ack=I2CWaitAck();
		I2CStop();
		if(ack==0){break;}
	}
}
void AT24C02_Write(unsigned char addr,unsigned char dat)
{
	I2CStart();         //起始信号
	I2CSendByte(0xa0);  //EEPROM的写设备地址
	I2CWaitAck();       //等待从机应答
	I2CSendByte(addr);  //内存单元地址
	I2CWaitAck();      //等待从机应答
	I2CSendByte(dat);   //内存写入数据
	I2CWaitAck();      //等待从机应答
	I2CStop();       //停止信号
	AT24C02_WaitReady();
}
//AT24C02数据读
unsigned char AT24C02_Read(unsigned char addr)
{
	unsigned char tmp = 0;
	//首先，进行一个伪写操作
	I2CStart();          //起始信号
	I2CSendByte(0xa0);    //EEPROM的写设备地址
	I2CWaitAck();        //等待从机应答
	I2CSendByte(addr);    //内存单元地址
	I2CWaitAck();        //等待从机应答
	//然后，开始字节读操作
	I2CStart();          //起始信号
	I2CSendByte(0xa1);    //EEPROM的读设备地址
	I2CWaitAck();        //等待从机应答
	tmp = I2CReceiveByte();  //读取内存中的数据
	I2CSendAck(1);        //产生非应答信号
	I2CStop();            //停止信号
	return tmp;
}
void Init_System()
{
	Init_74HC138(0);
	P0 = 0x00;
	Init_74HC138(5);
	P0 = 0xff;
	Init_74HC138(4);
}
void DelayK(unsigned char t)
{
	while(t--);
}
void delay(unsigned char t)
{
	while(t--);
}
void SMG_Display_Data()
{
	SMG_DisplayBit(0,smg_data[num4/10]);
	delay(200);
	SMG_DisplayBit(1,smg_data[num4%10]);
	delay(200);
	SMG_DisplayBit(2,0xbf);
	delay(200);
	SMG_DisplayBit(3,smg_data[num5/10]);
	delay(200);
	SMG_DisplayBit(4,smg_data[num5%10]);
	delay(200);
	SMG_DisplayBit(5,0xbf);
	delay(200);
	SMG_DisplayBit(6,smg_data[num6/10]);
	delay(200);
	SMG_DisplayBit(7,smg_data[num6%10]);
	delay(200);
	SMG_All(0xff);
	delay(200);
}
void ScanKeys_Alone()
{

	if(S6==0)
	{
		DelayK(100);
		while(S6==0)
		{
			SMG_Display_Data();
		}
		DelayK(100);
		num6++;
		if(num6>13){num6=0;}
		AT24C02_Write(0x02,num6);
	}

	if(S5==0)
	{
		DelayK(100);
		while(S5==0)
		{
			SMG_Display_Data();
		}
		DelayK(100);
		num5++;
		if(num5>13){num5=0;}
		AT24C02_Write(0x01,num5);
	}
	if(S4==0)
	{
		DelayK(100);
		while(S4==0)
		{
			SMG_Display_Data();
		}
		DelayK(100);
		num4++;
		if(num4>13){num4=0;}
		AT24C02_Write(0x00,num4);
	}
}

void main()
{
	Init_System();
	num4=AT24C02_Read(0x00);
	if(num4>13){num4=0;}
	num5=AT24C02_Read(0x01);
	if(num5>13){num5=0;}
	num6=AT24C02_Read(0x02);
	if(num6>13){num6=0;}
	while(1)
	{
		ScanKeys_Alone();
		SMG_Display_Data();
	}
}
