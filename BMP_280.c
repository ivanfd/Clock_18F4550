#include "BMP_280.h"
#include <stdlib.h>

uint16_t dig_T1;
int16_t dig_T2;
int16_t dig_T3;
uint16_t dig_P1;
int16_t dig_P2;
int16_t dig_P3;
int16_t dig_P4;
int16_t dig_P5;
int16_t dig_P6;
int16_t dig_P7;
int16_t dig_P8;
int16_t dig_P9;
//int16_t dig_H1;

BMP280_S32_t t_fine;

//  ≥н≥ц≥ал≥зац≥€ датчика

void bmp280_Init(void) {


    dig_T1 = bmp280ReadInt16(0x88);
    dig_T2 = (int16_t) bmp280ReadInt16(0x8A);
    dig_T3 = (int16_t) bmp280ReadInt16(0x8C);
    dig_P1 = bmp280ReadInt16(0x8E); //dig_P1
    dig_P2 = (int16_t) bmp280ReadInt16(0x90); //dig_P2
    dig_P3 = (int16_t) bmp280ReadInt16(0x92); //dig_P3
    dig_P4 = (int16_t) bmp280ReadInt16(0x94); //dig_P4
    dig_P5 = (int16_t) bmp280ReadInt16(0x96); //dig_P5
    dig_P6 = (int16_t) bmp280ReadInt16(0x98); //dig_P6
    dig_P7 = (int16_t) bmp280ReadInt16(0x9A); //dig_P7
    dig_P8 = (int16_t) bmp280ReadInt16(0x9C); //dig_P8
    dig_P9 = (int16_t) bmp280ReadInt16(0x9E); //dig_P9  
    // dig_H1 = bmp280ReadInt16(0xA1);

    i2cBuffer[0] = BMP280_REG_CONFIG;
    i2cBuffer[1] = BMP280_TSB_2000 | BMP280_FILTER_COEFFICIENT8 | BMP280_SPI_OFF;
    I2C_Write(i2cBuffer, BMP280_ADDRESS, 2);

    i2cBuffer[0] = BMP280_REG_CONTROL;
    i2cBuffer[1] = BMP280_OVERSAMPLING_T16 | BMP280_OVERSAMPLING_P16 | BMP280_MODE_NORMAL;
    I2C_Write(i2cBuffer, BMP280_ADDRESS, 2);
    //    bmp280WriteByte(BMP280_REG_CONFIG, (BMP280_TSB_2000 | BMP280_FILTER_COEFFICIENT8 | BMP280_SPI_OFF));
    //    bmp280WriteByte(BMP280_REG_CONTROL, (BMP280_OVERSAMPLING_T16 | BMP280_OVERSAMPLING_P16 | BMP280_MODE_NORMAL));

#ifdef DEBUG
    dig_T1 = 27504;
    dig_T2 = 26435;
    dig_T3 = -1000;
    dig_P1 = 36477;
    dig_P2 = -10685;
    dig_P3 = 3024;
    dig_P4 = 2855;
    dig_P5 = 140;
    dig_P6 = -7;
    dig_P7 = 15500;
    dig_P8 = -14600;
    dig_P9 = 6000;
#endif

#ifdef DEBUG
    __delay_ms(200);
    printf("dig_T1: %u\r\n", dig_T1);
    printf("dig_T2: %d\r\n", dig_T2);
    printf("dig_T3: %d\r\n", dig_T3);
    printf("dig_P1: %u\r\n", dig_P1);
    printf("dig_P2: %d\r\n", dig_P2);
    printf("dig_P3: %d\r\n", dig_P3);
    printf("dig_P4: %d\r\n", dig_P4);
    printf("dig_P5: %d\r\n", dig_P5);
    printf("dig_P6: %d\r\n", dig_P6);
    printf("dig_P7: %d\r\n", dig_P7);
    printf("dig_P8: %d\r\n", dig_P8);
    printf("dig_P9: %d\r\n", dig_P9);
#endif

}

//=====================================================
//      „итаЇмо з датчика 16 б≥т - по двох адресах
//              
//=====================================================

uint16_t bmp280ReadInt16(uint8_t address) {
    //  uint16_t msb = 0;
    //  uint16_t lsb = 0;

    i2cBuffer[0] = address;
    I2C_Read(i2cBuffer, BMP280_ADDRESS, 2, 1); // читаЇмо 2 байти

    return (uint16_t) (i2cBuffer[1] << 8) | i2cBuffer[0];


}

//=====================================================
//      „итаЇмо з датчика 24 б≥ти - по трьох адресах
//              
//=====================================================

uint32_t bmp280ReadInt32(uint8_t address) {

    i2cBuffer[0] = address;
    I2C_Read(i2cBuffer, BMP280_ADDRESS, 3, 1); // читаЇмо 3 байти

    return (((uint32_t) i2cBuffer[0] << 16) + ((uint32_t) i2cBuffer[1] << 8) + i2cBuffer[2]) >> 4;
}

void bmp280WriteByte(uint8_t address, uint8_t data)
//---------------------------------------------- 
{
    I2C_Master_Start();
    I2C_Master_Write(BMP280_ADDRESS);
    I2C_Master_Write(address);
    I2C_Master_Write(data);
    I2C_Master_Stop();
}

//********************************************* 
//  ѕроцедура читанн€ 1-го байту з датчика
//  address: I2C ареса датчика  
//  повертаЇ 1 байт прочитаних даних  
//---------------------------------------------- 

//uint8_t bmp280ReadByte(uint8_t address)
////---------------------------------------------- 
//{
//    uint8_t data;
//
//    I2C_Master_Start();
//    I2C_Master_Write(BMP280_ADDRESS);
//    I2C_Master_Write(address);
//    I2C_Master_RepeatedStart();
//    I2C_Master_Write(BMP280_ADDRESS | 0x01);
//    data = I2C_Master_Read(0); // читаЇмо без п≥дтвердженн€
//    I2C_Master_Stop();
//    return (data);
//}

//===========================================
//   „итаЇмо температуру
//===========================================

int32_t bmp280ReadTemp() {
    int32_t temperature = 0;

    temperature = (int32_t) bmp280ReadInt32(BMP280_REG_RESULT_TEMPRERATURE);
    //temperature = 519888;
#ifdef DEBUG
    __delay_ms(200);
    // temperature = 519888;
    printf("UncTemp: %ld\r\n", temperature);
#endif
    //		temperature = bmp280ReadLong(BMP280_REG_RESULT_TEMPRERATURE);
    return temperature;
}


//===========================================
//   „итаЇмо тиск
//===========================================

int32_t bmp280ReadPressure(void) {
    int32_t pressure = 0;


    pressure = (int32_t) bmp280ReadInt32(BMP280_REG_RESULT_PRESSURE);
    // pressure = 415148;
#ifdef DEBUG
    __delay_ms(200);
    // pressure = 415148;
    printf("UncPress: %ld\r\n", pressure);
#endif
    return pressure;
}


// Returns temperature in DegC, resolution is 0.01 DegC. Output value of У5123Ф equals 51.23 DegC.
// t_fine carries fine temperature as global value

BMP280_S32_t bmp280_compensate_T_int32(void) {
    BMP280_S32_t var1, var2, T;
    BMP280_S32_t adc_T;

    adc_T = bmp280ReadTemp();

    var1 = ((((adc_T >> 3) - ((BMP280_S32_t) dig_T1 << 1))) * ((BMP280_S32_t) dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((BMP280_S32_t) dig_T1)) * ((adc_T >> 4) - ((BMP280_S32_t) dig_T1))) >> 12) * ((BMP280_S32_t) dig_T3)) >> 14;
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return T;
}

// Returns pressure in Pa as unsigned 32 bit integer. Output value of У96386Ф equals 96386 Pa = 963.86 hPa

BMP280_U32_t bmp280_compensate_P_int32(void) {
    BMP280_S32_t var1, var2;
    BMP280_U32_t p;
    BMP280_S32_t adc_P;

    adc_P = bmp280ReadPressure();

    var1 = (((BMP280_S32_t) t_fine) >> 1) - (BMP280_S32_t) 64000;
    var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * ((BMP280_S32_t) dig_P6);
    var2 = var2 + ((var1 * ((BMP280_S32_t) dig_P5)) << 1);
    var2 = (var2 >> 2)+(((BMP280_S32_t) dig_P4) << 16);
    var1 = (((dig_P3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3) + ((((BMP280_S32_t) dig_P2) * var1) >> 1)) >> 18;
    var1 = ((((32768 + var1))*((BMP280_S32_t) dig_P1)) >> 15);
    if (var1 == 0) {
        return 0; // avoid exception caused by division by zero
    }
    p = (BMP280_U32_t) (((((BMP280_S32_t) 1048576) - adc_P)-(var2 >> 12)) * (BMP280_S32_t) 3125);
    if (p < 0x80000000) {
        p = (p << 1) / ((BMP280_U32_t) var1);
    } else {
        p = (p / (BMP280_U32_t) var1) * 2;
    }
    var1 = (((BMP280_S32_t) dig_P9) * ((BMP280_S32_t) (((p >> 3) * (p >> 3)) >> 13))) >> 12;
    var2 = (((BMP280_S32_t) (p >> 2)) * ((BMP280_S32_t) dig_P8)) >> 13;
    p = (BMP280_U32_t) ((BMP280_S32_t) p + ((var1 + var2 + dig_P7) >> 4));

    p = (p * 1000 / 13332);
    return p;
}

uint8_t bmp280Convert(uint16_t *press, uint16_t *temper) {
    int32_t adc_T;
    int32_t adc_P;
    //  int32_t temperature1;
    //  int32_t pressure1;

    // double var1, var2, T, P, tfine;
    int32_t var1, var2, t_fine;
    uint32_t T, P;



    adc_T = bmp280ReadTemp();
    adc_P = bmp280ReadPressure();

    var1 = ((((adc_T >> 3) - ((int32_t) dig_T1 << 1))) * ((int32_t) dig_T2)) >> 11;
#ifdef DEBUG
    __delay_ms(10);
    printf("var1 =  %lu\r\n", var1);
#endif
    var2 = (((((adc_T >> 4) - ((int32_t) dig_T1)) * ((adc_T >> 4) - ((int32_t) dig_T1))) >> 12) * ((int32_t) dig_T3)) >> 14;
#ifdef DEBUG
    __delay_ms(10);
    printf("var2 =  %ld\r\n", var2);
#endif
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
#ifdef DEBUG
    __delay_ms(10);
    printf("T1 =  %ld\r\n", T);
#endif    
    *temper = (uint16_t) T;

    var1 = (((int32_t) t_fine) >> 1) - (int32_t) 64000;
#ifdef DEBUG
    __delay_ms(10);
    printf("var1 =  %ld\r\n", var1);
#endif  
    var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * ((int32_t) dig_P6);
#ifdef DEBUG
    __delay_ms(10);
    printf("var2 =  %ld\r\n", var2);
#endif  
    var2 = var2 + ((var1 * ((int32_t) dig_P5)) << 1);
#ifdef DEBUG
    __delay_ms(10);
    printf("var2 =  %ld\r\n", var2);
#endif  
    var2 = (var2 >> 2)+(((int32_t) dig_P4) << 16);
#ifdef DEBUG
    __delay_ms(10);
    printf("var2 =  %ld\r\n", var2);
#endif  
    var1 = (((dig_P3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3) + ((((int32_t) dig_P2) * var1) >> 1)) >> 18;
#ifdef DEBUG
    __delay_ms(10);
    printf("var1 =  %ld\r\n", var1);
#endif  
    var1 = ((((32768 + var1))*((int32_t) dig_P1)) >> 15);
#ifdef DEBUG
    __delay_ms(10);
    printf("var1 =  %ld\r\n", var1);
#endif  
    if (var1 == 0) {
        return 0; // avoid exception caused by division by zero
    }
    P = (((uint32_t) (((int32_t) 1048576) - adc_P) - (var2 >> 12)))*3125;
    if (P < 0x80000000) {
        P = (P << 1) / ((uint32_t) var1);
    } else {
        P = (P / (uint32_t) var1) * 2;
    }
#ifdef DEBUG
    __delay_ms(10);
    printf("P =  %ld\r\n", P);
#endif  

    var1 = (((int32_t) dig_P9) * ((int32_t) (((P >> 3) * (P >> 3)) >> 13))) >> 12;
#ifdef DEBUG
    __delay_ms(10);
    printf("var1 =  %ld\r\n", var1);
#endif  
    var2 = (((int32_t) (P >> 2)) * ((int32_t) dig_P8)) >> 13;
#ifdef DEBUG
    __delay_ms(10);
    printf("var2 =  %ld\r\n", var2);
#endif  
    P = (uint32_t) ((int32_t) P + ((var1 + var2 + dig_P7) >> 4));
#ifdef DEBUG
    __delay_ms(10);
    printf("P =  %lu\r\n", P);
#endif  

    //*press = P;
    *press = (uint16_t) (P * 1000 / 13332);
    //*press = P * 0.7501;
    //    var1 = (((double)adc_T)/16384.0 - ((double)dig_T1)/1024.0) * ((double)dig_T2);
    //    //var9=(double)45678.125;
    //    #ifdef DEBUG
    //        printf("var1 =  %f\r", var1);
    //    #endif
    //	var2 = ((((double)adc_T)/131072.0 - ((double)dig_T1)/8192.0) * (((double)adc_T)/131072.0 - ((double) dig_T1)/8192.0)) * ((double)dig_T3);
    //    #ifdef DEBUG
    //        printf("var2 =  %f\r", var2);
    //    #endif
    //
    //	tfine = (var1 + var2);
    //    #ifdef DEBUG
    //        printf("tfine =  %f\r", tfine);
    //    #endif
    //    
    //	*temper = (int32_t) (tfine*10 / 5120.0);
    //    #ifdef DEBUG
    //        printf("temper =  %u\r", *temper);
    //    #endif
    //    
    //    var1 = ((double)tfine/2.0) - 64000.0;
    //    #ifdef DEBUG
    //        printf("var1 =  %f\r", var1);
    //    #endif
    //	var2 = var1 * var1 * ((double)dig_P6) / 32768.0;
    //    #ifdef DEBUG
    //        printf("var2 =  %f\r", var2);
    //    #endif
    //	var2 = var2 + var1 * ((double)dig_P5) * 2.0;
    //    #ifdef DEBUG
    //        printf("var2 =  %f\r", var2);
    //    #endif
    //	var2 = (var2/4.0)+(((double)dig_P4) * 65536.0);
    //    #ifdef DEBUG
    //        printf("var2 =  %f\r", var2);
    //    #endif
    //	var1 = (((double)dig_P3) * var1 * var1 / 524288.0 + ((double)dig_P2) * var1) / 524288.0;
    //    #ifdef DEBUG
    //        printf("var1 =  %f\r", var1);
    //    #endif
    //	var1 = (1.0 + var1 / 32768.0)*((double)dig_P1);
    //    #ifdef DEBUG
    //        printf("var1 =  %f\r", var1);
    //    #endif
    //
    //	P = 1048576.0 - (double)adc_P;
    //    #ifdef DEBUG
    //        printf("P =  %f\r", P);
    //    #endif
    //	P = (P - (var2 / 4096.0)) * 6250.0 / var1;
    //    #ifdef DEBUG
    //        printf("P =  %f\r", P);
    //    #endif
    //	var1 = ((double)dig_P9) * P * P / 2147483648.0;
    //    #ifdef DEBUG
    //        printf("var1 =  %f\r", var1);
    //    #endif
    //	var2 = P * ((double)dig_P8) / 32768.0;
    //    #ifdef DEBUG
    //        printf("var2 =  %f\r", var2);
    //    #endif
    //	P = (P + (var1 + var2 + ((double)dig_P7)) / 16.0);
    //    #ifdef DEBUG
    //        printf("P =  %f\r", P);
    //    #endif
    //
    //	*press = ((uint32_t)P)*100/13332;
    //    //*press = *press  * 100/13332;    
#ifdef DEBUG
    __delay_ms(200);

    printf("Temp280: %d\r\n", *temper);
    printf("Press280: %lu\n", *press);
#endif
    return 1;

}
