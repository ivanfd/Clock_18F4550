
#ifndef I2C_H
#define	I2C_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>
#include "common.h"

extern uint8_t i2cBuffer[];


void I2C_Master_Wait();
void I2C_Master_Start();
void I2C_Master_RepeatedStart();
void I2C_Master_Stop();
void I2C_Master_Write(uint8_t d);
uint8_t I2C_Master_Read(uint8_t a);
void I2C_Init(void);
void I2C_Write(uint8_t *byteAr, uint8_t i2cAdress, uint8_t n);
void I2C_Read(uint8_t *byteAr, uint8_t i2cAdress, uint8_t n, uint8_t enRegByte);






//void I2C_Init(void);                    // ініціалізація I2C
//uint8_t I2C_start (uint8_t adress, uint8_t _R_W); //старт
//void I2C_idle (void);                   // чекати закінчення активності
//uint8_t I2C_write (uint8_t data);       // запис байту
//uint8_t I2C_read_noack (void);          //читати байт
//uint8_t I2C_restart (uint8_t adres, uint8_t _R_W);
//void I2C_stop (void);                   // формувати стоп біт
//uint8_t i2c_read_ack (void);

#endif	/* XC_HEADER_TEMPLATE_H */

