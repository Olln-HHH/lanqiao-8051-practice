#include <Led.h>
#include <Latch.h>
void Led_disp(unsigned char addr,enable)
{
	static unsigned char temp = 0x00;
	static unsigned char temp_old = 0xff;
	if(enable) temp |= 0x01 << addr;
	else temp &= ~(0x01 << addr);
	if(temp != temp_old)
	{
		P0 = ~temp;
		Latch(1);
		temp_old = temp;
	}
}
