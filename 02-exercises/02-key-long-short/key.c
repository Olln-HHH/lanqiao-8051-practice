#include <REGX52.H>
#include "key.h"
sbit S4=P3^3;
void DelayK(unsigned char t)
{
	while(t--);
}
unsigned char stat_k=0;

void ScanKeys_Alone()
{
	if(S4==0)
	{
		DelayK(100);
		if(S4==0)
		{
			if(stat_k==1)
			{
				L4=0;
				while(S4==0);
				L4=1;
			}
			else if(stat_k==2)
			{
				L6=0;
				while(S4==0);
				L6=1;
			}
		}

	}
}
