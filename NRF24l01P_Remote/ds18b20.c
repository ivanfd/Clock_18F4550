#include "ds18b20.h"


uint8_t scratch[9];



//===================================================
//      ініціалізація
//===================================================
void init_ds18b20(void)
{

    if (!ow_reset())  // якщо датчик присутній
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

uint8_t readTemp_Single(uint16_t *buf, uint8_t *minus) {

    uint16_t temp = 0; // температура
    uint8_t i, tmp, fptmp;

    *minus = '+';


    if (!ow_reset()) // якщо датчик присутній
    {
        write_byte(0xCC); //  команда пропуск ROM
        write_byte(0x44); //  конвертувати температуру

    } else
        return FALSE;


    __delay_ms(750);
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
    printf("Temperature: %d\r\n ", temp);
#endif

    //fptmp = temp % 10; // остача
    //temp = temp / 10; // ціла частина

    //if (fptmp >= 6) temp += 1;
    //  temp = floor(temp);

    *buf = temp;
    return TRUE;


}




