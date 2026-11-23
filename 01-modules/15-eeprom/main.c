#include <REGX52.H>
#include "iic.h"
#include "smg_swj.h"

unsigned char dat1=0,dat2=0,dat3=0;
void delay(unsigned int t)
{
	while(t--);
}
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

void SMG_Display_Data()
{
	SMG_DisplayBit(0,smg_data[dat1/10]);
	delay(200);
	SMG_DisplayBit(1,smg_data[dat1%10]);
	delay(200);
	SMG_DisplayBit(2,0xbf);
	delay(200);
	SMG_DisplayBit(3,smg_data[dat2/10]);
	delay(200);
	SMG_DisplayBit(4,smg_data[dat2%10]);
	delay(200);
	SMG_DisplayBit(5,0xbf);
	delay(200);
	SMG_DisplayBit(6,smg_data[dat3/10]);
	delay(200);
	SMG_DisplayBit(7,smg_data[dat3%10]);
	delay(200);
	SMG_All(0xff);
	delay(200);
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
//ATC02读取与写入
void Read_Write_Data(void)
{
	//先读取数据
	dat1 = 	AT24C02_Read(0x01);
	dat2 =  AT24C02_Read(0x03);
	dat3 = 	AT24C02_Read(0x05);

	dat1  = dat1 +1;
	dat2  = dat2 +2;
	dat3  = dat3 + 3 ;
	if( dat1 > 10)
	{
		dat1 = 0;
	}
	if(dat2  > 20)
	{
		dat2 = 0;
	}
	if(dat3  > 30)
	{
		dat3 = 0;
	}

	AT24C02_Write(0x01,dat1);
	delay(1000);
	AT24C02_Write(0x03,dat2);
	delay(1000);
	AT24C02_Write(0x05,dat3);
	delay(1000);

}
void Init_System()
{
	Init_74HC138(0);
	P0 = 0x00;
	Init_74HC138(5);
	P0 = 0xff;
	Init_74HC138(4);
}

void main(void)
{
	Init_System();
	Read_Write_Data();
	while(1)
	{
		SMG_Display_Data();
	}
}
