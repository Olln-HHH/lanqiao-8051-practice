#include <REGX52.H>
sfr AUXR=0x8e;
void selecthc573(unsigned char channel)
{
	switch(channel)
	{
		case 4:
		P2=(P2&0x1f)|0x80;
		break;
		case 5:
		P2=(P2&0x1f)|0xa0;
		break;
		case 6:
		P2=(P2&0x1f)|0xc0;
		break;
		case 7:
		P2=(P2&0x1f)|0xe0;
		break;
	}
}
void initsystem()
{
	selecthc573(5);//关蜂鸣器和继电器
	P0=0x00;
	selecthc573(4);//关LED
	P0=0xff;
}
//================================
void inituart()
{
	TMOD=0x20;
	TH1=0xfd;
	TL1=0xfd;
	TR1=1;

	SCON=0x50;
	AUXR=0x00;

	ES=1;
	EA=1;
}
volatile unsigned char command=0x00;

void serviceuart() interrupt 4
{
	if(RI==1)
	{
		command=SBUF;
		RI=0;
	}

}
void sendbyte(unsigned char dat)
{
	bit es_old;
	es_old=ES;ES=0;
	TI=0;
	SBUF=dat;
	while(TI==0);
	TI=0;
	ES=es_old;
}
void sendstring(unsigned char *str)
{
	while(*str!='\0')
	{
		sendbyte(*str++);
	}
}

//================================
void working()
{
	if(command!=0)
	{
		switch(command & 0xf0)
		{
			case 0xa0:
			P0=(P0|0x0f) & (~command|0xf0);
			command=0;
			break;
			case 0xb0:
			P0=(P0|0x0f) & ((~command<<4)|0x0f);
			command=0;
			break;
			case 0xc0:
			sendstring("The System is Running...\n");
			command=0x00;
			break;

		}
	}
}

void main()
{
	initsystem();
	inituart();
	sendstring("hello AH\n");
	while(1)
	{
		working();
	}
}
