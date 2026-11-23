#include <REGX52.H>
sfr AUXR=0x8e;
volatile unsigned char urdat;
volatile bit urflag=0;
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
void serviceuart () interrupt 4
{
	if(RI==1)
	{
		urdat=SBUF;
		RI=0;
		urflag=1;
	}
}

void main()
{
	unsigned char dat;
	selecthc573(4);
	P0=0xff;
	inituart();
	sendbyte(0x5a);
	sendbyte(0xa5);
	while(1)
	{
		if(urflag)
		{
			ES=0;dat=urdat;urflag=0;ES=1;
			sendbyte(dat+1);
		}
	}
}
