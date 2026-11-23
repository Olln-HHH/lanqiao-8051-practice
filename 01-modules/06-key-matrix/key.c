#include <REGX52.H>
#include "key.h"
sfr P4 = 0xc0;

sbit R1=P3^0;
sbit R2=P3^1;
sbit R3=P3^2;
sbit R4=P3^3;

sbit C1=P4^4;
sbit C2=P4^2;
sbit C3=P3^5;
sbit C4=P3^4;
unsigned int   Key_num;
char ScanKeysMulti()
{
	R1=0;
	R2=R3=R4=1;
	C1=C2=C3=C4=1;
	if(C1==0)
	{
		while(C1==0);
		Key_num=0;

	}
	else if(C2==0)
	{
		while(C2==0);
		Key_num=1;

	}
	else if(C3==0)
	{
		while(C3==0);
		Key_num=2;

	}
	else if(C4==0)
	{
		while(C4==0);
		Key_num=3;

	}
	R2=0;
	R1=R3=R4=1;
	C1=C2=C3=C4=1;
	if(C1==0)
	{
		while(C1==0);
		Key_num=4;

	}
	else if(C2==0)
	{
		while(C2==0);
		Key_num=5;

	}
	else if(C3==0)
	{
		while(C3==0);
		Key_num=6;

	}
	else if(C4==0)
	{
		while(C4==0);
		Key_num=7;

	}
	R3=0;
	R1=R2=R4=1;
	C1=C2=C3=C4=1;
	if(C1==0)
	{
		while(C1==0);
		Key_num=8;

	}
	else if(C2==0)
	{
		while(C2==0);
		Key_num=9;

	}
	else if(C3==0)
	{
		while(C3==0);
		Key_num=10;

	}
	else if(C4==0)
	{
		while(C4==0);
		Key_num=11;

	}
	R4=0;
	R1=R2=R3=1;
	C1=C2=C3=C4=1;
	if(C1==0)
	{
		while(C1==0);
		Key_num=12;

	}
	else if(C2==0)
	{
		while(C2==0);
		Key_num=13;

	}
	else if(C3==0)
	{
		while(C3==0);
		Key_num=14;

	}
	else if(C4==0)
	{
		while(C4==0);
		Key_num=15;

	}
	return Key_num;
}
