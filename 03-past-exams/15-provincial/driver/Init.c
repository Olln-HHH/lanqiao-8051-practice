#include <Init.h>
#include <Latch.h>

void Init()
{
	P0 = 0xff;
	Latch(1);
	P0 = 0x00;
	Latch(2);
}
