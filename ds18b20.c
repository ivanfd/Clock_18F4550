#include "ds18b20.h"


uint8_t scratch[9];
//bit minus = 0;      // знак температури
const char fractS[] = {0, 1, 1, 2, 2, 3, 4, 4, 5, 6, 6, 7, 7, 8, 9, 9}; // дискретність, знак після коми
//  0/16..., 1/16..., 2/16......


//===================================================
//      ініціалізація
//===================================================

void init_ds18b20(void) {

    if (!ow_reset()) // якщо датчик присутній
    {

        write_byte(0xCC);
        write_byte(0x4E);
        write_byte(0x00);
        write_byte(0x00);
        write_byte(0x7F); //12 -біт режим  
    }
}


//====================================================
//  Читаємо температуру з датчика
//  один датчик на шині!
//====================================================

uint8_t readTemp_Single(uint16_t *buf, uint8_t *minus, uint8_t *time_flag, uint8_t *timer_val) {

    const char fract[] = {0, 1, 1, 2, 2, 3, 4, 4, 5, 6, 6, 7, 7, 8, 9, 9}; // дискретність, знак після коми
    //  0/16..., 1/16..., 2/16......

    uint16_t temp = 0; // температура
    uint8_t i, tmp, fptmp;

    *minus = '+';

    switch (*time_flag) {
        case 0:
            if (!ow_reset()) // якщо датчик присутній
            {
                //             write_byte(0xCC); //  команда пропуск ROM
                //             write_byte(0x4E); //  конвертувати температуру
                //             write_byte(0x00);    
                //             write_byte(0x00);    
                //             write_byte(0x3F); //10 -біт режим      
                //             ow_reset();
                write_byte(0xCC); //  команда пропуск ROM
                write_byte(0x44); //  конвертувати температуру
                *timer_val = 0;
                *time_flag = 2; // сюди більше не заходимо до наступного вимірювання
                T1CONbits.TMR1ON = 1; // пуск таймер 1 відраховуємо час конвертування

                return 0;
            }
            break;
        case 1:
            //     if (!Tmp_Del)
            //        {
            //         Tmp_Del = 1;
            //         return false;
            //        }
            //     do
            //      {
            //         __delay_ms(25);
            //         ret = read_bit();
            //
            //      } while (!ret);


            ow_reset();
            write_byte(0xCC); //  команда пропуск ROM
            write_byte(0xBE); // Read scratch pad command
            for (i = 0; i < 2; i++)//  читаємо два байти температури
            {
                scratch[i] = read_byte();
            }

            temp = (((uint16_t) scratch[1]) << 8) | ((uint16_t) scratch[0]);
            // temp |= scratch[0];

            if (temp & 0x8000) {
                temp = -temp; // якщо від`ємна 
                *minus = '-';
            }

            tmp = temp & 0x0f; // десяті вибираємо
            tmp = fract[tmp]; //  цифра після коми
            temp = temp >> 4; //  формуємо кінцеву температуру
            temp = (temp & 0x00ff) * 10;
            temp = temp + tmp;

#ifdef DEBUG
            printf("Temperature: %d\n\r ", temp);
#endif

            //  fptmp = temp % 10;// остача
            //  temp = temp / 10; // ціла частина

            //   if (fptmp >=6) temp += 1;
            //  temp = floor(temp);


            *buf = temp;
            *time_flag = 0;

            return TRUE;
            break;
        case 2:
            return FALSE;
            ;
            break;
    }
    return FALSE;
}

void ds18b20_start_conversion(void) {
    if (!ow_reset()) // якщо датчик присутній
    {
        write_byte(0xCC); //  команда пропуск ROM
        write_byte(0x44); //  конвертувати температуру
    }
}

uint16_t ds18b20_read_temperature(uint8_t *minus) {
    uint16_t temp = 0; // температура
    uint8_t i, tmp, fptmp;

    ow_reset();
    write_byte(0xCC); //  команда пропуск ROM
    write_byte(0xBE); // Read scratch pad command
    for (i = 0; i < 2; i++)//  читаємо два байти температури
    {
        scratch[i] = read_byte();
    }

    temp = (((uint16_t) scratch[1]) << 8) | ((uint16_t) scratch[0]);

    if (temp & 0x8000) {
        temp = -temp; // якщо від`ємна 
        *minus = '-';
    }

    tmp = temp & 0x0f; // десяті вибираємо
    tmp = fractS[tmp]; //  цифра після коми
    temp = temp >> 4; //  формуємо кінцеву температуру
    temp = (temp & 0x00ff) * 10;
    temp = temp + tmp;

    return temp;
}




