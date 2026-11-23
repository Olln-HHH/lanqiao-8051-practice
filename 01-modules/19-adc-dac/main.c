#include <REGX52.H>
#include "iic.h"
#include "smg_swj.h"
unsigned char ad_val;       // 原始AD采集值（0-255）
float voltage;              // 精准电压值（浮点型，如2.51V）
unsigned int voltage_int;   // 电压放大100倍的整数（如251代表2.51V，方便数码管显示）
void delay(unsigned int t)
{
	while(t--);
}
void Init_74HC138(unsigned char channel)
{
	switch(channel)
	{
		case 4:P2 = (P2 & 0x1f) | 0x80;break;
		case 5:P2 = (P2 & 0x1f) | 0xa0;break;
		case 6:P2 = (P2 & 0x1f) | 0xc0;break;
		case 7:P2 = (P2 & 0x1f) | 0xe0;break;
		case 0:P2 = (P2 & 0x1f) | 0x00;break;
	}
	P2 = (P2 & 0x1f) | 0x00;
}

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
void SMG_Display_Data()
{
	SMG_DisplayBit(5,smg_data[voltage_int/100]);
	delay(200);
	SMG_DisplayBit(5,0xff);
	delay(200);
	SMG_DisplayBit(6,smg_data[(voltage_int/10)%10]);
	delay(200);
	SMG_DisplayBit(6,0xff);
	delay(200);
	SMG_DisplayBit(7,smg_data[voltage_int%10]);
	delay(200);
	SMG_DisplayBit(7,0xff);
	SMG_All(0xff);
	delay(200);
}
void Init_System()
{
	P0 = 0xff;
	Init_74HC138(4);
	P0 = 0x00;
	Init_74HC138(5);
	SMG_All(0xff);
}
void Delay200ms()	//@11.0592MHz
{
	unsigned char data i, j, k;

	i = 9;
	j = 104;
	k = 139;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

unsigned int cc;
void main(void)
{
	Init_System();

	while(1)
	{
		if(cc>300)
		{
			// 读取AD通道0x01/0x03的电压值（光敏电阻是0x01，可调电阻是0x03）
			Get_Voltage(0x01);
			cc=0;
		}
		cc++;
		// 显示电压值（数码管）
		SMG_Display_Data();

		// 【可选】串口打印精准电压值（如需串口功能，需添加串口初始化）
		// printf("AD值：%d，电压：%.2fV\r\n", ad_val, voltage);

	}
}
////1. 输出 0V
//Da_Write(0);
//// 2. 输出约 2.5V（128/255*5≈2.5V）
//Da_Write(128);
//// 3. 输出 5V
//Da_Write(255);
//// 4. 输出 1V（计算：dat = 1*255/5 = 51）
//Da_Write(51);
//// 5. 输出 3.3V（计算：dat = 3.3*255/5 ≈ 168）
//Da_Write(168);
//五、关键注意事项
//控制字固定为 0x41
//PCF8591 的 DA 模式需要特定的控制字节，不能修改，否则会进入 AD 模式或其他错误模式。
//必须补充停止信号
//没有 I2CStop() 会导致 I2C 总线一直处于忙状态，后续的 AD 采集或 EEPROM 操作都会失败。
//电压输出有延迟
//DA 输出电压不会立即变化，芯片需要约几微秒的转换时间，无需额外延时，函数执行完成后电压即稳定。
//负载能力有限
//PCF8591 的 DA 输出是电压源，负载电流不能太大（建议 ≤1mA），如果需要驱动大负载，需外接运放进行功率放大。
//六、验证方法
//将 PCF8591 的 AOUT 引脚接电压表；
//调用 Da_Write(128)，电压表应显示 2.4~2.6V（芯片精度误差）；
//调用 Da_Write(51)，电压表应显示 0.9~1.1V，验证输出正确。
