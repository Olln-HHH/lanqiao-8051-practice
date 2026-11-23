#ifndef __ONEWIRE_H
#define __ONEWIRE_H

#include <REGX52.H>
float rd_temperature();
void Write_DS18B20(unsigned char dat);
unsigned char Read_DS18B20(void);
bit init_ds18b20(void);

bit OneWire_ReadRaw(int *raw);
#endif
