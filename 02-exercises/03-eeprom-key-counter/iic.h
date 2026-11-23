#ifndef __IIC_H
#define __IIC_H

#include <REGX52.H>
static void I2C_Delay(unsigned char n);
void I2CStart(void);
void I2CStop(void);
void I2CSendByte(unsigned char byt);
unsigned char I2CReceiveByte(void);
unsigned char I2CWaitAck(void);
void I2CSendAck(unsigned char ackbit);
unsigned char Ad_Read(unsigned char addr);
void Da_Write(unsigned char dat);
void EEPROM_Write(unsigned char *str, unsigned char addr, unsigned char num);
void EEPROM_Read(unsigned char *str, unsigned char addr, unsigned char num);
#endif
