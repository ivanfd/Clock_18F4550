
#ifndef BMP180_H
#define	BMP180_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include "i2c.h"
#include "newmain.h"

#define BMP180_ADDRESS 0xEE          // I2C адреса BMP180

const uint8_t OSS =  3; // роздільна здатність 0,1,2,3 


uint8_t BMP085ReadByte(uint8_t address);
int16_t bmp085ReadInt(uint8_t address);
void BMP085WriteByte(uint8_t address, int8_t data);
void bmp085Calibration();
int32_t BMP085ReadUT();
int32_t bmp085ReadUP();
int32_t BMP085GetTemp(int32_t _tu);
uint16_t BMP085GetPressure(int32_t _pu);
uint16_t BMP085Pressure(uint8_t getTemp);
uint32_t BMP085Temperature(void);



#endif	/* XC_HEADER_TEMPLATE_H */

