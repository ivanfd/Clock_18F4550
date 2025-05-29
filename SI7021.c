
/*
 Драйвер для роботи з датчиком Si7021
 Драйвер не повний, тільки температура і вологість.
 (Датчик вологості і темрператури)
 Автор: Ivan_fd
 20.01.2025р.  
 */

#include "SI7021.h"

uint8_t si7021_init(void) {

    si7021_reset();

    i2cBuffer[0] = SI7021_READRHT_REG_CMD;
    I2C_Read(i2cBuffer, SI7021_ADDRESS, 1, 1);

    if (i2cBuffer[0] != 0x3A)
        return FALSE;

    return TRUE;
}

void si7021_reset(void) {

    i2cBuffer[0] = SI7021_RESET_CMD;
    I2C_Write(i2cBuffer, SI7021_ADDRESS, 1);
    __delay_ms(50);

}


// вимірювання вологості

uint16_t si7021_Hum(void) {
    uint16_t tmp1;
    uint32_t tmp2;
    uint8_t tmpHi, tmpLo;

    i2cBuffer[0] = SI7021_MEASRH_NOHOLD_CMD;
    I2C_Write(i2cBuffer, SI7021_ADDRESS, 1); // Передаємо команду для читання вологості

    __delay_ms(30); // пауза для перетворення

    I2C_Read(i2cBuffer, SI7021_ADDRESS, 3, 0); // читаємо 3 байти вологості
    tmp1 = (uint16_t)(i2cBuffer[0] << 8) | i2cBuffer[1]; // перетворення в волгість
    if (tmp1 == 65535)
        return ERROR_SI7021;
    tmp2 = (uint32_t) tmp1 * 12500; // множимо додатково на 100, щоб уникнути float
    tmp2 = tmp2 / 65536;
    tmp2 = tmp2 - 600;
    tmpHi = (uint8_t)(tmp2 / 100); // сотні вологості
    tmpLo = tmp2 % 100; // перетворюємо десяті

    tmpLo = ((tmpLo + 5) / 10)*10; // округлюємо до десятків, так звану, цифру після коми

    //tmp1 = (tmpHi * 100 + tmpLo) / 10;  // результат


    return (tmpHi * 100 + tmpLo) / 10; // результат
}

// вимірювання температури

uint16_t si7021_Temp(void) {
    uint16_t tmp1;
    uint32_t tmp2;
    uint8_t tmpHi, tmpLo;

    i2cBuffer[0] = SI7021_MEASTEMP_NOHOLD_CMD;
    I2C_Write(i2cBuffer, SI7021_ADDRESS, 1); // Передаємо команду для читання

    __delay_ms(30); // пауза для перетворення

    I2C_Read(i2cBuffer, SI7021_ADDRESS, 3, 0); // читаємо 3 байти температури
    tmp1 = (uint16_t)(i2cBuffer[0] << 8) | i2cBuffer[1]; // перетворення температуру
    if (tmp1 == 65535)
        return ERROR_SI7021;
    tmp2 = (uint32_t) tmp1 * 17572; // множимо додатково на 100, щоб уникнути float
    tmp2 = tmp2 / 65536;
    tmp2 = tmp2 - 4685;
    tmpHi = (uint8_t)(tmp2 / 100); // сотні температури
    tmpLo = tmp2 % 100; // перетворюємо десяті

    tmpLo = ((tmpLo + 5) / 10)*10; // округлюємо до десятків, так звану, цифру після коми

    //tmp1 = (tmpHi * 100 + tmpLo) / 10;  // результат


    return (tmpHi * 100 + tmpLo) / 10; // результат
}