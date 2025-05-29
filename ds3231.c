#include "ds3231.h"                                                       

uint8_t bcd_to_decimal(uint8_t d) {
    return ((d & 0x0F) + (((d & 0xF0) >> 4) * 10));
}

uint8_t decimal_to_bcd(uint8_t d) {
    return (((d / 10) << 4) & 0xF0) | ((d % 10) & 0x0F);
}

void DS3231_init(void) {
    i2cBuffer[0] = controlREG;
    i2cBuffer[1] = 0x00;
    I2C_Write(i2cBuffer, DS3231_Address, 2);

    i2cBuffer[0] = statusREG;
    i2cBuffer[1] = 0x08;
    I2C_Write(i2cBuffer, DS3231_Address, 2);
}
//****************************************************
// прочитати дан≥ з годинника
// p3 - години, p2 - хвилини, p1 - секунди
//****************************************************

void getTime(uint8_t *p3, uint8_t *p2, uint8_t *p1) {

    i2cBuffer[0] = secondREG;
    I2C_Read(i2cBuffer, DS3231_Address, 1, 1);
    *p1 = bcd_to_decimal(i2cBuffer[0]);

    i2cBuffer[0] = minuteREG;
    I2C_Read(i2cBuffer, DS3231_Address, 1, 1);
    *p2 = bcd_to_decimal(i2cBuffer[0]);

    i2cBuffer[0] = hourREG;
    I2C_Read(i2cBuffer, DS3231_Address, 1, 1);
    *p3 = bcd_to_decimal(0x3F & i2cBuffer[0]);
}


// прочитати дан≥ з годинника
// p4 - день, p3 - число, p2 - м≥с€ць, p1 - р≥к

void getDate(uint8_t *p4, uint8_t *p3, uint8_t *p2, uint8_t *p1) {

    i2cBuffer[0] = yearREG;
    I2C_Read(i2cBuffer, DS3231_Address, 1, 1);
    *p1 = bcd_to_decimal(i2cBuffer[0]);

    i2cBuffer[0] = monthREG;
    I2C_Read(i2cBuffer, DS3231_Address, 1, 1);
    *p2 = bcd_to_decimal(0x1F & i2cBuffer[0]);

    i2cBuffer[0] = dateREG;
    I2C_Read(i2cBuffer, DS3231_Address, 1, 1);
    *p3 = bcd_to_decimal(0x3F & i2cBuffer[0]);

    i2cBuffer[0] = dayREG;
    I2C_Read(i2cBuffer, DS3231_Address, 1, 1);
    *p4 = bcd_to_decimal(0x07 & i2cBuffer[0]);
}

void setTime(uint8_t hSet, uint8_t mSet, uint8_t sSet) {
    i2cBuffer[0] = secondREG;
    i2cBuffer[1] = decimal_to_bcd(sSet);
    I2C_Write(i2cBuffer, DS3231_Address, 2);

    i2cBuffer[0] = minuteREG;
    i2cBuffer[1] = decimal_to_bcd(mSet);
    I2C_Write(i2cBuffer, DS3231_Address, 2);

    i2cBuffer[0] = hourREG;
    i2cBuffer[1] = 0x3F & (decimal_to_bcd(hSet));
    I2C_Write(i2cBuffer, DS3231_Address, 2);
}

void setDate(uint8_t daySet, uint8_t dateSet, uint8_t monthSet, uint8_t yearSet) {

    i2cBuffer[0] = dayREG;
    i2cBuffer[1] = decimal_to_bcd(daySet);
    I2C_Write(i2cBuffer, DS3231_Address, 2);

    i2cBuffer[0] = dateREG;
    i2cBuffer[1] = decimal_to_bcd(dateSet);
    I2C_Write(i2cBuffer, DS3231_Address, 2);

    i2cBuffer[0] = monthREG;
    i2cBuffer[1] = 0x3F & (decimal_to_bcd(monthSet));
    I2C_Write(i2cBuffer, DS3231_Address, 2);

    i2cBuffer[0] = yearREG;
    i2cBuffer[1] = 0x3F & (decimal_to_bcd(yearSet));
    I2C_Write(i2cBuffer, DS3231_Address, 2);
}


