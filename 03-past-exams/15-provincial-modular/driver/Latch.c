#include <Latch.h>

void Latch(unsigned char num)
{
	switch(num)
	{
		case 1: P2 = P2 & 0x1f | 0x80;break;
		case 2: P2 = P2 & 0x1f | 0xa0;break;
		case 3: P2 = P2 & 0x1f | 0xc0;break;
		case 4: P2 = P2 & 0x1f | 0xe0;break;
	}
	P2 &= 0x1f;
}
