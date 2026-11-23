#include <REGX52.H>
#include "iic.h"
#include "smg_swj.h"
sbit S4=P3^3;
sbit S5=P3^2;
sbit L1=P0^0;
sbit L2=P0^1;
sbit L3=P0^2;
sbit L4=P0^3;
sbit L5=P0^4;
sbit L6=P0^5;
sbit L7=P0^6;
sbit L8=P0^7;
unsigned int mmm,mm,m;
unsigned char mod=1;
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
void delay_ms(unsigned int ms)
{
	unsigned int i,j;
	for(i=ms;i>0;i--)
	for(j=110;j>0;j--);
}
unsigned char ad_val;        // 原始AD采集值（0-255）
float voltage;               // 精准电压值（浮点型，如2.51V）
unsigned int voltage_int;    // 电压放大100倍的整数（如251代表2.51V，方便数码管显示）
void Get_Voltage(unsigned char ad_channel)//光敏电阻是0x01，可调电阻是0x03
{
	// 步骤1：读取AD原始值（0-255）
	ad_val = Ad_Read(ad_channel);

	// 方式1：精准浮点换算（适合串口打印、精准计算）
	voltage = (float)ad_val * 5.0 / 255.0;

	// 方式2：整数换算（适合数码管显示，避免浮点运算）
	// 放大100倍，四舍五入，比如2.5098→251
	voltage_int = (unsigned int)((ad_val * 500.0 + 127) / 255);
	// +127是为了四舍五入（255/2=127.5），提升显示精度
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
	delay_ms(5);
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
void delay(unsigned int t)
{
	while(t--);
}
void SMG_Display_Data(unsigned char n)
{
	if(n==1)
	{
		SMG_DisplayBit(0,smg_data[16]);
		delay(200);
		SMG_DisplayBit(1,smg_data[n]);
		delay(200);
		SMG_DisplayBit(2,smg_data[16]);
		delay(200);
		SMG_DisplayBit(3,0xff);
		delay(200);
		SMG_DisplayBit(4,0xff);
		delay(200);
		SMG_DisplayBit(5,smg_data[2]-0x80);
		delay(200);
		SMG_DisplayBit(6,smg_data[0]);
		delay(200);
		SMG_DisplayBit(7,smg_data[0]);
		delay(200);
		SMG_All(0xff);
		delay(200);
	}
	else if(n==2)
	{
		SMG_DisplayBit(0,smg_data[16]);
		delay(200);
		SMG_DisplayBit(1,smg_data[n]);
		delay(200);
		SMG_DisplayBit(2,smg_data[16]);
		delay(200);
		SMG_DisplayBit(3,0xff);
		delay(200);
		SMG_DisplayBit(4,0xff);
		delay(200);
		SMG_DisplayBit(5,smg_data[4]-0x80);
		delay(200);
		SMG_DisplayBit(6,smg_data[0]);
		delay(200);
		SMG_DisplayBit(7,smg_data[0]);
		delay(200);
		SMG_All(0xff);
		delay(200);
	}
	else if(n==3)
	{
		SMG_DisplayBit(0,smg_data[16]);
		delay(200);
		SMG_DisplayBit(1,smg_data[n]);
		delay(200);
		SMG_DisplayBit(2,smg_data[16]);
		delay(200);
		SMG_DisplayBit(3,0xff);
		delay(200);
		SMG_DisplayBit(4,0xff);
		delay(200);
		SMG_DisplayBit(5,smg_data[voltage_int/100]-0x80);
		delay(200);
		SMG_DisplayBit(6,smg_data[(voltage_int/10)%10]);
		delay(200);
		SMG_DisplayBit(7,smg_data[voltage_int%10]);
		delay(200);
		SMG_All(0xff);
		delay(200);
	}
	else if(n==4)
	{
		SMG_DisplayBit(0,smg_data[16]);
		delay(200);
		SMG_DisplayBit(1,smg_data[n]);
		delay(200);
		SMG_DisplayBit(2,smg_data[16]);
		delay(200);
		SMG_DisplayBit(3,0xff);
		delay(200);
		SMG_DisplayBit(4,0xff);
		delay(200);
		SMG_DisplayBit(5,smg_data[voltage_int/100]-0x80);
		delay(200);
		SMG_DisplayBit(6,smg_data[(voltage_int/10)%10]);
		delay(200);
		SMG_DisplayBit(7,smg_data[voltage_int%10]);
		delay(200);
		SMG_All(0xff);
		delay(200);
	}
	else if(n==5)
	{
		SMG_DisplayBit(0,smg_data[16]);
		delay(200);
		SMG_DisplayBit(1,smg_data[n]);
		delay(200);
		SMG_DisplayBit(2,smg_data[16]);
		delay(200);
		SMG_DisplayBit(3,0xff);
		delay(200);
		SMG_DisplayBit(4,0xff);
		delay(200);
		SMG_DisplayBit(5,smg_data[voltage_int/100]-0x80);
		delay(200);
		SMG_DisplayBit(6,smg_data[(voltage_int/10)%10]);
		delay(200);
		SMG_DisplayBit(7,smg_data[voltage_int%10]);
		delay(200);
		SMG_All(0xff);
		delay(200);
	}
	else if(n==6)
	{
		SMG_DisplayBit(0,smg_data[16]);
		delay(200);
		SMG_DisplayBit(1,smg_data[m]);
		delay(200);
		SMG_DisplayBit(2,smg_data[16]);
		delay(200);
		SMG_DisplayBit(3,0xff);
		delay(200);
		SMG_DisplayBit(4,0xff);
		delay(200);
		SMG_DisplayBit(5,smg_data[mm]-0x80);
		delay(200);
		SMG_DisplayBit(6,smg_data[mmm/10]);
		delay(200);
		SMG_DisplayBit(7,smg_data[mmm%10]);
		delay(200);
		SMG_All(0xff);
		delay(200);
	}
}

void ScanKeys_Alone()
{
	if(S5==0)
	{
		delay(100);
		while(S5==0)
		{
			m=AT24C02_Read(0x01);
			if(m<1||m>5){m=0;}
			mm=AT24C02_Read(0x02);
			if(mm>5){mm=0;}
			mmm=AT24C02_Read(0x03);
			if(mmm>99){mmm=0;}
			SMG_Display_Data(6);
			Init_74HC138(0);
			L1=1;L2=1;L3=1;L4=1;L5=1;L6=1;L7=1;L8=1;
			Init_74HC138(4);
		}
		delay(100);

	}
	if(S4==0)
	{
		delay(100);
		while(S4==0)
		{
			SMG_Display_Data(mod);
		}
		delay(100);
		AT24C02_Write(0x01,mod);
		AT24C02_Write(0x02,voltage_int/100);
		AT24C02_Write(0x03,voltage_int%100);
	}
}
void main()
{
	Init_System();
	while(1)
	{
		Get_Voltage(0x01);
		if(voltage_int<=100)
		{
			mod=1;
			Init_74HC138(0);
			L1=0;L2=0;L3=0;L4=0;L5=0;L6=0;L7=0;L8=0;
			Init_74HC138(4);
		}
		else if((voltage_int>100)&&(voltage_int<=200))
		{
			mod=2;
			Init_74HC138(0);
			L1=0;L2=0;L3=0;L4=0;L5=0;L6=0;L7=1;L8=1;
			Init_74HC138(4);
		}
		else if((voltage_int>200)&&(voltage_int<=300))
		{
			mod=3;
			Init_74HC138(0);
			L1=0;L2=0;L3=0;L4=0;L5=1;L6=1;L7=1;L8=1;
			Init_74HC138(4);
		}
		else if((voltage_int>300)&&(voltage_int<=400))
		{
			mod=4;
			Init_74HC138(0);
			L1=0;L2=0;L3=1;L4=1;L5=1;L6=1;L7=1;L8=1;
			Init_74HC138(4);
		}
		else if((voltage_int>400)&&(voltage_int<=500))
		{
			mod=5;
			Init_74HC138(0);
			L1=0;L2=1;L3=1;L4=1;L5=1;L6=1;L7=1;L8=1;
			Init_74HC138(4);
		}
		SMG_Display_Data(mod);
		ScanKeys_Alone();
	}
}
