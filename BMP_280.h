
#ifndef BMP_280_H
#define	BMP_280_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>
#include "i2c.h"

typedef uint32_t BMP280_U32_t;
typedef int32_t BMP280_S32_t;

#define BMP280_ADDRESS 0xEC          // I2C адреса BMP280

#define	BMP280_REG_CONTROL 0xF4
#define	BMP280_REG_CONFIG 0xF5

#define	BMP280_REG_RESULT_PRESSURE 0xF7			// 0xF7(msb) , 0xF8(lsb) , 0xF9(xlsb)
#define BMP280_REG_RESULT_TEMPRERATURE 0xFA		// 0xFA(msb) , 0xFB(lsb) , 0xFC(xlsb)

#define	BMP280_OVERSAMPLING_T1		0x20 // роздільна здатність по температурі
#define	BMP280_OVERSAMPLING_T2		0x40
#define	BMP280_OVERSAMPLING_T4		0x60
#define	BMP280_OVERSAMPLING_T8		0x80
#define	BMP280_OVERSAMPLING_T16		0xA0

#define	BMP280_OVERSAMPLING_P1		0x04 // роздільна здатність по тиску
#define	BMP280_OVERSAMPLING_P2		0x08
#define	BMP280_OVERSAMPLING_P4		0x0C
#define	BMP280_OVERSAMPLING_P8		0x10
#define	BMP280_OVERSAMPLING_P16		0x14

#define	BMP280_TSB_0_5				0x00    //  в режимі NORMAL, час через який датчик вимірює
#define	BMP280_TSB_62_5				0x20
#define	BMP280_TSB_125				0x40
#define	BMP280_TSB_250				0x60
#define	BMP280_TSB_500				0x80
#define	BMP280_TSB_1000				0xA0
#define	BMP280_TSB_2000				0xC0
#define	BMP280_TSB_4000				0xE0

#define	BMP280_FILTER_OFF			0x00
#define	BMP280_FILTER_COEFFICIENT2	0x04
#define	BMP280_FILTER_COEFFICIENT4	0x08
#define	BMP280_FILTER_COEFFICIENT8	0x0C
#define	BMP280_FILTER_COEFFICIENT16	0x10

#define	BMP280_MODE_SLEEP			0x00
#define	BMP280_MODE_FORCED			0x01
#define	BMP280_MODE_NORMAL			0x03

#define	BMP280_SPI_OFF	0x00
#define	BMP280_SPI_ON	0x01


void bmp280_Init(void);
uint16_t bmp280ReadInt16(uint8_t address);
void bmp280WriteByte(uint8_t address, uint8_t data);
//uint8_t bmp280ReadByte(uint8_t address) ;
int32_t bmp280ReadTemp();
uint32_t bmp280ReadInt32(uint8_t address);
int32_t bmp280ReadPressure(void);
uint8_t bmp280Convert(uint16_t *press, uint16_t *temper);

BMP280_S32_t bmp280_compensate_T_int32(void);
BMP280_U32_t bmp280_compensate_P_int32(void);


#endif	/* XC_HEADER_TEMPLATE_H */

