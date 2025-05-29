#include "common.h"

uint8_t events = MAIN_EVENT; // подія, яка відбувається
__bit show_digit; // чи показувати цифри, в нал. мигання
__bit en_put; // чи можна писати у буфер символи
//uint16_t temperature; // температура з кімнатного датчика
//uint16_t temperature_radio = 0; // температура з радіо датчика
uint8_t err_ds18 = 0; // помилка датчика радіо
uint16_t err_ds18_count = 0; // для тестування помилок датчика
uint16_t err_ds_count = 0;
uint8_t timer_val = 0, time_flag = 0; // для конвертування температури
//extern uint8_t(*pFont)[][5];
uint8_t setting_Val[NUM_VALUES] = {FONT_1, TYPE_CLK_1, TYPE_BRG_AUTO, VAL_BRG_NUM_3, ENABLE_SND_HR_ON, DS_ON, DS_ON, BMP_ON, DST_ON, ESP_ON, DHT_ON}; // масив з значеннями налаштувань
uint16_t data_Val[NUM_VALUES_DAT] = {0, 0, 0, 0, 0, 0, 0, 0}; // значення датчиків 
uint8_t minus; // знак температури
uint8_t minus_radio = '+'; // знак температури
//uint8_t type_font = 0; // шрифт годин
//uint8_t type_temp = 2;
//uint16_t press, temperbmp280; // атмосферний тиск
//extern uint8_t play_sound; //  чи можна програвати
//uint8_t type_clk = TYPE_CLK_1; // вигляд годинника
uint8_t idx_pnt = 0; // індекс, для вигляду мигаючих крапок
volatile uint8_t x1 = 0, x2 = 0, x3 = 0, x4 = 0, y1, y2, y3, y4; //Для зсуву стовбця вниз
uint16_t adc_res = 500; // результат вимірювання АЦП
uint8_t oldsec; // секунди попередні
__bit oldsec_flag, mess_show; //oldmin_flag, bmp_show,
//int8_t brg_type; // яскравість по датчику, чи постійна
//uint8_t brig; // значення яскравості
uint8_t usart_data[EUSART_RX_BUFFER_SIZE];
uint8_t blk_dot = 0; // дозвіл на мигання кнопок
//int8_t en_ds_1; //  чи пок. температуру з датчика 1
//int8_t en_ds_2; //  чи пок. температуру з датчика 2
//int8_t en_bmp280; //  чи показуємо тиск
//int8_t en_dst; // перехід на літній час
//int8_t en_am2302; //датчик вологості
//uint8_t dst_yn; // чи відбувався перехід на літній час
//uint8_t count_min = 0; // лічильник пройдених хвилин
uint8_t day_mess = 0; // день в який буде виводитись повідомлення
uint8_t dst_flag = 0; // чи зараз літній час????
uint8_t const compile_date[12] = __DATE__; // Mmm dd yyyy
uint8_t const compile_time[9] = __TIME__; // hh:mm:ss
uint8_t valuesAM[5]; // масив байт з датчика AM2302
uint8_t ErrAM; //  помилка датчика AM2302
//uint16_t Rh, Td; // вологість, температура
//uint8_t RhH, RhL, TdH, TdL; //  вологість цілі - десяті, температура цілі - десяті
uint8_t ip_buf[16] = {"192.168.4.1"};
__bit flagUpdateTime;
uint8_t messageComplete = FALSE;

__EEPROM_DATA(4, 2, 1, 2, 1, 1, 1, 1); // ініціалізація еепром, 
// 0 - тип шрифту (від 1 до 5)
// 1 - тип годинника (1,2)
// 2 - тип яскравості(0, 1). 1 - автоматично, 0 - ручний
// 3 - значеняя яскравості для ручного
// 4 - щогодинний сигнал
// 5 - чи показувати температуру з датчика 1 (кімнатний)
// 6 - чи показувати температуру з датчика 2 (радіодатчик)
// 7 - чи показувати атмосферний тиск
__EEPROM_DATA(1, 2, 1, 0, ESP_ON, 0, 0, 0); // ініціалізація еепром, (слідуючі комірки пам'яті) 
// 0 - автоматичний перехід на літній час
// 1 - тип показу температури
// 2 - датчик AM2302
// 3 - чи відбувався перехід на літній час
// 4 - ESP8266

// читаємо з DS3231 години, хвилини, секунди та дату
// 

void GetTime(void) {
    oldsec = TTime.Ts;
    //    oldmin = TTime.Tmin;
    getTime(&TTime.Thr, &TTime.Tmin, &TTime.Ts);
    getDate(&TTime.Tdy, &TTime.Tdt, &TTime.Tmt, &TTime.Tyr);
    if (oldsec != TTime.Ts)
        oldsec_flag = 1;
    //    if (oldmin != TTime.Tmin)
    //        oldmin_flag = 1;
    if (TTime.Ts == 3)
        snd_flag = 1; //дозволяємо знову генерувати щогодинний сигнал

    if (setting_Val[ENABLE_DST])
        dst_time(&TTime, &dst_flag);
    else
        dst_flag = 0; // вимкнути признак літнього часу

    if ((TTime.Thr >= 7)&&(TTime.Thr <= 23)&&(TTime.Tmin == 0)&&(TTime.Ts == 0)&&(snd_flag))
        h_snd_t = 1; //щогодинний сигнал
}


//**************************************************
//      температура з кімнатного датчика
//*************************************************

void home_temp(void) {
    static uint8_t fptmp;
    uint8_t i;
    static uint16_t t_temp;

    switch (events) {
        case MAIN_EVENT:
            //   if (readTemp_Single(&temperature, &time_flag, &timer_val))
            //   {
            clear_matrix();

            if ((setting_Val[TYPE_TEMP] == TYPE_TEMP_1) || (data_Val[T_HOME] == 0)) {
                fptmp = data_Val[T_HOME] % 10; // остача
                t_temp = data_Val[T_HOME] / 10; // ціла частина
                if (fptmp >= 6) t_temp += 1;

                if (!(t_temp / 10)) // якщо перша цифра 0
                {
                    pic_to_led(3, 1, Dis_Buff);
                    putchar_down(13, (uint8_t) t_temp, pFont);
                    putchar_down(19, 176, Font);

                } else {

                    pic_to_led(3, 1, Dis_Buff);
                    putchar_down(11, (t_temp / 10) % 10, pFont);
                    putchar_down(17, t_temp % 10, pFont);
                    putchar_down(23, 176, Font);

                }
            } else { // якщо тип 2
                for (i = 32; i < BUF_SIZE_TEMP; i++)
                    Dis_Buff[i] = 0;

                //t_temp = temperature;
                //sprintf(text_buf, "%u", (uint16_t)(temperature / 100) % 10); // формуємо строку
                //text_buf[0] = ((temperature / 100) % 10) + 48;
                //text_buf[1] = ((temperature / 10) % 10) + 48;
                //text_buf[2] = (temperature % 10) + 48;
                text_buf[0] = ((data_Val[T_HOME] / 100) % 10) + 48;
                text_buf[1] = ((data_Val[T_HOME] / 10) % 10) + 48;
                text_buf[3] = (data_Val[T_HOME] % 10) + 48;
                text_buf[2] = '.';
                text_buf[4] = '°';
                text_buf[5] = 0;

                //sprintf(text_buf, "%c%c%c%c%c", tmp1, tmp2, '.', tmp3, '°'); // формуємо строку
                // t_temp =  temperature;
                //sprintf(text_buf, "%u", t_temp); // формуємо строку

                if (text_buf[0] != 48) { //якщо перша цифра не 0
                    pic_to_led(2, 1, Dis_Buff);
                    // putchar_down(11, minus_radio, &Font);
                    //      putchar_down(11, minus_radio, &Font);
                    putchar_down(11, text_buf[0] - 48, pFont); // цифра 1 - сотні
                    putchar_down(17, text_buf[1] - 48, pFont); // цифра 2 - десятки
                    putchar_down(23, text_buf[2], Font);
                    //putchar_down(32, text_buf[3] - 48, pFont);
                    putchar_down(26, text_buf[3] - 48, pFont); // цифра 3 
                    putchar_b_buf(32, text_buf[4], Font, Dis_Buff);
                    __delay_ms(1000);
                    scroll_text_temp(8);

                    //                    putchar_down(7, text_buf[0] - 48, pFont); // цифра 1 - сотні
                    //                    putchar_down(13, text_buf[1] - 48, pFont); // цифра 2 - десятки
                    //                    putchar_down(19, text_buf[2], &Font);
                    //                    putchar_down(22, text_buf[3] - 48, pFont);
                    //                    putchar_down(27, text_buf[4], &Font);

                } else {
                    pic_to_led(2, 1, Dis_Buff);
                    //  putchar_down(11, minus_radio, &Font);
                    putchar_down(11, text_buf[1] - 48, pFont); // цифра 1 - десятки
                    putchar_down(17, text_buf[2], Font); // крапка
                    putchar_down(20, text_buf[3] - 48, pFont); // цифра 2 
                    putchar_down(26, text_buf[4], Font); //знак градуса

                }

            }


            events = TEMP_EVENT;
            RTOS_SetTask(default_state, 650, 0); // 3,5 секунд для виходу
            break;
        case TEMP_EVENT:
            break;
        case KEY_EXIT_EVENT: // повертаємось в показ часу
            events = MAIN_EVENT;
            Rand_ef(); // випадковий ефект
            //scroll_right();
            //pre_ref_dis();
            RTOS_DeleteTask(default_state);
            RTOS_DeleteTask(home_temp);
            if (setting_Val[ENABLE_DS_2])
                RTOS_SetTask(radio_temp, 0, cycle_main);
            else {
                RTOS_SetTask(time_led, 0, cycle_main);
                pre_ref_dis();
            }
            break;

    }
}

//**************************************************
//      температура з радіо датчика
//*************************************************

void radio_temp(void) {
    static uint8_t fptmp;
    uint8_t i;
    static uint16_t t_temp;

    switch (events) {
        case MAIN_EVENT:
            clear_matrix();
            if (err_ds18) {
                pic_to_led(0, 2, Dis_Buff);
                putchar_down(9, 'E', Font);
                putchar_down(15, 'r', Font);
                putchar_down(21, 'r', Font);
            } else {
                // temperature_radio = 98; //&&&&&&&&&&&&&&&&&??????
                // minus_radio = '+';
                if (data_Val[T_RADIO] != 0xFFFF) {

                    if ((setting_Val[TYPE_TEMP] == TYPE_TEMP_1) || (data_Val[T_RADIO] == 0)) {
                        fptmp = data_Val[T_RADIO] % 10; // остача
                        t_temp = data_Val[T_RADIO] / 10; // ціла частина
                        if (fptmp >= 6) t_temp += 1;

                        if (!(t_temp / 10)) // якщо перша цифра 0
                        {
                            pic_to_led(3, 2, Dis_Buff);
                            if (t_temp != 0) {
                                putchar_down(13, minus_radio, Font);
                                putchar_down(19, t_temp % 10, pFont);
                                putchar_down(25, 176, Font);
                            } else {
                                //putchar_down(9, 0, &Font);
                                putchar_down(13, t_temp % 10, pFont);
                                putchar_down(19, 176, Font);
                            }
                        } else {

                            pic_to_led(0, 2, Dis_Buff);

                            putchar_down(9, minus_radio, Font);
                            putchar_down(15, (t_temp / 10) % 10, pFont);
                            putchar_down(21, t_temp % 10, pFont);
                            putchar_down(27, 176, Font);

                        }
                    } else { // якщо тип 2
                        for (i = 32; i < BUF_SIZE_TEMP; i++)
                            Dis_Buff[i] = 0;

                        //sprintf(text_buf, "%u%u%c%u%c", (temperature_radio / 100) % 10, (temperature_radio / 10) % 10, '.', temperature_radio % 10, '°'); // формуємо строку
                        text_buf[0] = ((data_Val[T_RADIO] / 100) % 10) + 48;
                        text_buf[1] = ((data_Val[T_RADIO] / 10) % 10) + 48;
                        text_buf[3] = (data_Val[T_RADIO] % 10) + 48;
                        text_buf[2] = '.';
                        text_buf[4] = '°';
                        text_buf[5] = 0;
                        if (text_buf[0] != 48) { //якщо перша цифра не 0
                            pic_to_led(3, 2, Dis_Buff);
                            putchar_down(11, minus_radio, Font);
                            putchar_down(17, text_buf[0] - 48, pFont); // цифра 1 - сотні
                            putchar_down(23, text_buf[1] - 48, pFont); // цифра 2 - десятки
                            putchar_down(29, text_buf[2], Font);
                            //putchar_down(32, text_buf[3] - 48, pFont);
                            putchar_b_buf(32, text_buf[3] - 48, pFont, Dis_Buff); // цифра 3 
                            putchar_b_buf(38, text_buf[4], Font, Dis_Buff);
                            __delay_ms(1000);
                            scroll_text_temp(10);
                        } else {
                            pic_to_led(1, 2, Dis_Buff);
                            putchar_down(11, minus_radio, Font);
                            putchar_down(17, text_buf[1] - 48, pFont); // цифра 1 - десятки
                            putchar_down(23, text_buf[2], Font); // крапка
                            putchar_down(26, text_buf[3] - 48, pFont); // цифра 2 
                            putchar_b_buf(32, text_buf[4], Font, Dis_Buff); //знак градуса
                            __delay_ms(1000);
                            scroll_text_temp(8);
                        }

                    }
                } else {
                    pic_to_led(0, 2, Dis_Buff);

                    putchar_down(9, '-', Font);
                    putchar_down(15, '-', Font);
                    putchar_down(21, '-', Font);
                }
            }
            events = TEMP_EVENT;
            RTOS_SetTask(default_state, 650, 0); // 3,5 секунд для виходу

            // }

            break;
        case TEMP_EVENT:
            break;
        case KEY_EXIT_EVENT: // повертаємось в показ часу
            events = MAIN_EVENT;

            Rand_ef(); // випадковий ефект
            //hide_two_side();
            //scroll_left();
            pre_ref_dis();
            RTOS_DeleteTask(default_state);
            RTOS_DeleteTask(radio_temp);
            RTOS_SetTask(time_led, 0, cycle_main);
            // clear_matrix();
            break;
    }
}


//=====================================================
//   Заміна шрифту
//=====================================================

void set_font(void) {
    switch (setting_Val[TYPE_FONT]) {
        case FONT_1:
            pFont = dFont1;
            break;
        case FONT_2:
            pFont = dFont2;
            break;
        case FONT_3:
            pFont = dFont3;
            break;
        case FONT_4:
            pFont = dFont4;
            break;
        case FONT_5:
            pFont = dFont5;
            break;
    }
}

//==================================================
//  вивід атмосферного тиску
// =================================================

void pressure(void) {
    //  uint16_t pr;
    uint8_t i;

    switch (events) {
        case MAIN_EVENT:
            //press = 75126;
            if (data_Val[PRESS]) {
                //pr = (uint16_t) press;

                fill_buff_t(data_Val[PRESS]);
                center_two_side();
                //                pic_to_led(3, 4, &Dis_Buff);
                //                putchar_down(11, (pr / 100) % 10, pFont);
                //                putchar_down(17, (pr / 10) % 10, pFont);
                //                putchar_down(23, pr % 10, pFont);
                for (i = 32; i < BUF_SIZE_TEMP; i++)
                    Dis_Buff[i] = 0;
                putchar_b_buf(32, data_Val[PRESS] % 10, pFont, Dis_Buff);
                //pic_to_led(38, 7, &Dis_Buff);
                //sprintf(text_buf, "мм");
                //putchar_b_buf(38, text_buf[0], &Font, &Dis_Buff); //  
                //putchar_b_buf(44, text_buf[1], &Font, &Dis_Buff);
                // putchar_b_buf(44, text_buf[2], &Font, &Dis_Buff);
                __delay_ms(1000);
                scroll_text_temp(8);

            } else {
                clear_matrix();
                pic_to_led(3, 4, Dis_Buff);
                putchar_down(11, 'E', Font);
                putchar_down(17, 'R', Font);
                putchar_down(23, 'R', Font);

            }

            events = TEMP_EVENT;
            RTOS_SetTask(default_state, 400, 0); // 2 секунд для виходу

            // }

            break;
        case TEMP_EVENT:
            break;
        case KEY_EXIT_EVENT: // повертаємось в показ часу
            events = MAIN_EVENT;
            //
            if (setting_Val[ENABLE_DHT]) {
                //sprintf(text_buf, "мм.рт.ст.");
                //interval_scroll_text(&text_buf);
                Rand_ef(); // випадковий ефект
                RTOS_DeleteTask(default_state);
                RTOS_DeleteTask(pressure);
                RTOS_SetTask(hum, 0, cycle_main);
            } else {
                Rand_ef(); // випадковий ефект

                pre_ref_dis();
                RTOS_DeleteTask(default_state);
                RTOS_DeleteTask(pressure);
                RTOS_SetTask(time_led, 0, cycle_main);
            }
            //clear_matrix();
            break;
    }
}

//==========================
//  Вивід вологості
//==========================

void hum(void) {
    //  uint8_t i;
    switch (events) {
        case MAIN_EVENT:
            if (data_Val[HUM] == ERROR_SI7021) {
                pic_to_led(1, 5, Dis_Buff);
                putchar_down(9, 'E', Font);
                putchar_down(15, 'R', Font);
                putchar_down(21, 'R', Font);
            } else {
                if (data_Val[HUM] < 1000) {
                    pic_to_led(1, 5, Dis_Buff);
                    putchar_down(9, (data_Val[HUM] / 100) % 10, pFont);
                    putchar_down(15, (data_Val[HUM] / 10) % 10, pFont);
                    putchar_down(21, '.', Font);
                    putchar_down(24, (data_Val[HUM] % 10), pFont);
                } else {
                    pic_to_led(2, 5, Dis_Buff);
                    putchar_down(11, (data_Val[HUM] / 1000) % 10, pFont);
                    putchar_down(17, (data_Val[HUM] / 100) % 10, pFont);
                    putchar_down(23, (data_Val[HUM] / 10) % 10, pFont);
                    //  putchar_down(26, '.', &Font);
                    //  putchar_down(28, (Rh % 10), pFont);
                }
            }
            events = TEMP_EVENT;
            RTOS_SetTask(default_state, 400, 0); // 2 секунд для виходу

            // }

            break;
        case TEMP_EVENT:
            break;
        case KEY_EXIT_EVENT: // повертаємось в показ часу
            events = MAIN_EVENT;

            Rand_ef(); // випадковий ефект
            pre_ref_dis();
            RTOS_DeleteTask(default_state);
            RTOS_DeleteTask(hum);
            RTOS_SetTask(time_led, 0, cycle_main);

            //clear_matrix();
            break;
    }
}

void pre_ref_dis(void) {


    clear_matrix();
    switch (setting_Val[TYPE_CLK]) {
        case TYPE_CLK_1:
            if ((TTime.Thr / 10) % 10)
                putchar_down(0, (TTime.Thr / 10) % 10, pFont);
            else putchar_down(0, 0, Font);
            putchar_down(6, TTime.Thr % 10, pFont);
            putchar_down(13, (TTime.Tmin / 10) % 10, pFont);
            putchar_down(19, TTime.Tmin % 10, pFont);
            GetTime();
            //getTime(&TTime.Thr, &TTime.Tmin, &TTime.Ts);
            putchar_down_s(25, (TTime.Ts / 10) % 10 + 1);
            putchar_down_s(29, TTime.Ts % 10 + 1);
            //if (en_dst)
            //    dst_time(&TTime, &dst_flag);
            blk_dot = 0;
            break;
        case TYPE_CLK_2:
            if ((TTime.Thr / 10) % 10)
                putchar_down(1, (TTime.Thr / 10) % 10, pFont);
            else putchar_down(1, 0, Font);
            putchar_down(7, TTime.Thr % 10, pFont);
            putchar_down(13, ':', Font);
            GetTime();
            //getTime(&TTime.Thr, &TTime.Tmin, &TTime.Ts);
            putchar_down(19, (TTime.Tmin / 10) % 10, pFont);
            putchar_down(25, TTime.Tmin % 10, pFont);
            //if (en_dst)
            //    dst_time(&TTime, &dst_flag);
            blk_dot = 1;
            idx_pnt = 0;
            x1 = (TTime.Thr / 10) % 10;
            x2 = TTime.Thr % 10;
            x3 = (TTime.Tmin / 10) % 10;
            x4 = TTime.Tmin % 10;

            break;

    }

}


//==================================================
//  виводимо годину на дисплей -:)
//==================================================

void time_led() {
    // uint8_t data_array[4] = {0, 0, 0, 0};

    flagUpdateTime = 1; // можна оновлювати час через уарт
    switch (events) {
        case MAIN_EVENT:
            FillBuf(setting_Val[TYPE_CLK]);
            if (setting_Val[TYPE_CLK] == TYPE_CLK_2)
                blk_dot = 1;
            else
                blk_dot = 0;
            if ((TTime.Ts > 5)&&(TTime.Ts < 7)) //прочитаємо температуру
            {
                readTemp_Single(&data_Val[T_HOME], &minus, &time_flag, &timer_val);
#ifdef DEBUG_TEMP
                temperature = 19;
#endif
            }
            if (((TTime.Ts > 14)&&(TTime.Ts < 16)))// ||((TTime.Ts>45)&&(TTime.Ts<47)))    //  виведемо температуру
                events = KEY_DOWN_EVENT;
            if ((TTime.Ts > 39)&&(TTime.Ts < 41)) { //  виведемо атмосферний тиск
                if ((setting_Val[ENABLE_BMP]) &&((TTime.Tmin % 2) == 0)) // якщо можна виводити атм. тиск
                    events = KEY_UP_EVENT;
                else
                    events = KEY_DOWN_EVENT;
                //  bmp_show = 0;
            }
            //            if (oldmin_flag) { // пройшла хвилина
            //                count_min++; // збільшуємо лічильник хвилин
            //                oldmin_flag = 0;
            //            }
            //            if(count_min == 2){
            //                count_min = 0;
            //                bmp_show = 1;
            //            }
            break;
        case KEY_OK_EVENT: // якщо натиснули кнопку ОК
            flagUpdateTime = 0; // не можна оновлювати час через уарт
            EUSART_Write('$');
            EUSART_Write('P');
            EUSART_Write('\r');
            EUSART_Write('\n');
            //            if (data_Val[T_HUM] == 65534) {
            //                ip_buf[0] = 'E';
            //                ip_buf[1] = 'R';
            //                ip_buf[2] = 'R';
            //                ip_buf[3] = 0;
            //            } else {
            //                ip_buf[0] = ((data_Val[T_HUM] / 100) % 10) + 48;
            //                ip_buf[1] = ((data_Val[T_HUM] / 10) % 10) + 48;
            //                ip_buf[2] = (data_Val[T_HUM] % 10) + 48;
            //                ip_buf[3] = 0;
            //            }
            //                    EUSART_Write(((data_Val[T_HUM] / 100) % 10) + 48); // передаємо першу цифру
            //                        EUSART_Write(((data_Val[T_HUM] / 10) % 10) + 48); //......
            //                        EUSART_Write((data_Val[T_HUM] % 10) + 48);
            RTOS_DeleteTask(time_led); // видаляємо задачу в якій сидимо
            RTOS_SetTask(time_set_min, 0, 50); //  ставимо задачу налаштування годинника
            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            TSTime = TTime;
            events = MAIN_EVENT;
            en_put = 0;
            if (setting_Val[TYPE_CLK] == TYPE_CLK_2) {
                blk_dot = 0;
                putchar_b_buf(13, 23, Font, Dis_Buff);
            }
            //clear_matrix();
            break;
        case KEY_UP_EVENT:
            //         asm("nop");
            blk_dot = 0;
            flagUpdateTime = 0; // не можна оновлювати час через уарт

            data_Val[HUM] = si7021_Hum();
            data_Val[T_HUM] = si7021_Temp();

            //bmp280Convert(&data_Val[PRESS], &data_Val[T_PRESS]);
            data_Val[T_PRESS] = (uint16_t) bmp280_compensate_T_int32();
            data_Val[PRESS] = (uint16_t) bmp280_compensate_P_int32();
            if (setting_Val[TYPE_CLK] == TYPE_CLK_2)
                putchar_b_buf(13, 23, Font, Dis_Buff);
            //scroll_right();
            Rand_ef(); // випадковий ефект
            if (setting_Val[ENABLE_BMP]) {
                RTOS_DeleteTask(time_led); //видаляємо задачу
                RTOS_SetTask(pressure, 0, cycle_main); //додаємо задачу 
                events = MAIN_EVENT;
                en_put = 0;
            } else {
                RTOS_DeleteTask(time_led); //видаляємо задачу
                RTOS_SetTask(hum, 0, cycle_main); //додаємо задачу 
                events = MAIN_EVENT;
                en_put = 0;
            }

            break;
        case KEY_DOWN_EVENT:
            flagUpdateTime = 0; // не можна оновлювати час через уарт
            if (setting_Val[ENABLE_DS_1]) {
                blk_dot = 0;
                // temperature_radio = data_array[1] | (uint16_t) (data_array[2] << 8);
                if (setting_Val[TYPE_CLK] == TYPE_CLK_2)
                    putchar_b_buf(13, 23, Font, Dis_Buff);
                //scroll_left();
                //dissolve();
                //scroll_down_one();
                Rand_ef();
                RTOS_DeleteTask(time_led); //видаляємо задачу
                RTOS_SetTask(home_temp, 0, cycle_main); //додаємо задачу 
                events = MAIN_EVENT;
                en_put = 0;
            } else if (setting_Val[ENABLE_DS_2]) {
                blk_dot = 0;
                if (setting_Val[TYPE_CLK] == TYPE_CLK_2)
                    putchar_b_buf(13, 23, Font, Dis_Buff);
                //scroll_left();
                //dissolve();
                Rand_ef();
                RTOS_DeleteTask(time_led); //видаляємо задачу
                RTOS_SetTask(radio_temp, 0, cycle_main); //додаємо задачу 
                events = MAIN_EVENT;
                en_put = 0;
            }
            events = MAIN_EVENT;
            break;
        case KEY_EXIT_EVENT:
            events = MAIN_EVENT;
            RTOS_DeleteTask(default_state);

        case TEMP_EVENT:

            break;
    }
    if (en_put)
        Update_Matrix(Dis_Buff); // обновити дані на дисплеї

    // яскравість
    if (setting_Val[TYPE_BRG]) {
        if ((TTime.Ts % 2 == 0)&&(oldsec_flag)) {
            oldsec_flag = 0;
            read_adc(); // прочитаємо дані з ацп
            adj_brig(); //  регулюємо яскравість
        }

    }
    en_put = 1;

    // читаємо радіодатчик
    //        if (nrf24_dataReady()) {
    //            nrf24_getData(&data_array);
    //            //spi_rw(FLUSH_RX); // очистити прийомний буфер
    //            nrf24_powerUpRx();
    //            temperature_radio = data_array[1] | (uint16_t) (data_array[2] << 8);
    //            minus_radio = data_array[0];
    //            err_ds_count = 0;
    //            err_ds18 = 0;
    //        } else
    //            err_ds_count++;
    //
    //
    //        if (err_ds_count > 1000) // чекаємо ~1.6 хвилини. Якщо не було ні одного зчитування
    //        {
    //            err_ds18 = 1; // то ставимо признак помилки радіодатчика
    //            //nrf24_powerUpRx(); // Переводимо датчик у режим прийому, та скидаємо всі переривання
    //            nrf24_init(100, 4); // Ще раз ініціалізуємо
    //        }
    if (TTime.Tdt == day_mess) { // будемо виводити строку. Наприклад - привітання.
        if (((TTime.Tmin % 5) == 0) && (TTime.Ts == 35) && mess_show) { // один раз в 5 хвилин
            blk_dot = 0;
            putchar_b_buf(13, 23, Font, Dis_Buff);
            interval_scroll_text(rs_text_buf);
            blk_dot = 1;
        }
    } else
        mess_show = 0;

    //#ifdef DEBUG
    //            if (nrf24_dataReady()) {
    //                test++;
    //                nrf24_getData(&data_array);
    //                printf("%u > ", test);
    //                printf("%u ", data_array[0]);
    //                printf("%c ", data_array[1]);
    //                printf("%2X ", data_array[2]);
    //                printf("%2X\r\n", data_array[3]);
    //            }
    //#endif  


}

//=======================================================
//          Робота з ком портом
//=======================================================

void usart_r() {

    static uint8_t i = 0;
    uint8_t j;

    //    if (EUSART_DataReady) {
    //
    //        usart_data[i++] = EUSART_Read();
    //        //        if (sizeof (usart_data) <= EUSART_DataReady)
    //        //            eusartRxCount = 0;
    //        return;
    //    }
    while (EUSART_DataReady) {
        usart_data[i++] = EUSART_Read();
        if (i >= 2 && usart_data[i - 1] == '\n' && usart_data[i - 2] == '\r') {
            messageComplete = TRUE;
            break; // Виходимо, якщо отримано \r\n
        }
    }
    if (!EUSART_DataReady) {
        reinit_rx();
        // i = 0;
    }
    if (messageComplete) {
        if (usart_data[0] == '$') {
            switch (usart_data[1]) {
                case 'D': // 
                    // 
                    sendDataSensors();
                    //                EUSART_Write('$');
                    //                EUSART_Write('D');
                    //                for (uint8_t j = 0; j < NUM_VALUES_DAT; j++) {
                    //                    EUSART_Write(data_Val[j]& 0xFF); //висилаємо молодший байт;
                    //                    EUSART_Write((data_Val[j] >> 8)& 0xFF); //висилаємо старший байт;
                    //                }
                    //                EUSART_Write(minus_radio);
                    //                EUSART_Write('\n');

                    //setting_Val[NUM_VALUES]
                    break;
                case 'J': // строка налаштувань
                    // 
                    EUSART_Write('$');
                    EUSART_Write('J');
                    for (j = 0; j < NUM_VALUES; j++)
                        EUSART_Write(setting_Val[j]);
                    EUSART_Write('\r');
                    EUSART_Write('\n');
                    //setting_Val[NUM_VALUES]
                    break;
                case 'P': // IP sdress
                    // формат 192.168.0.142
                    for (j = 0; j <= (strlen((const char *) usart_data)) - 2; j++)
                        ip_buf[j] = usart_data[j + 2];

                    break;
                case 't': // налаштування через синій зуб годин
                    // формат "$tHHMMss"
                    if (flagUpdateTime) {
                        TSTime.Thr = ((usart_data[2] - 48)*10)+(usart_data[3] - 48);
                        TSTime.Tmin = ((usart_data[4] - 48)*10)+(usart_data[5] - 48);
                        TSTime.Ts = ((usart_data[6] - 48)*10)+(usart_data[7] - 48);
                        if (dst_flag) {// якщо час літній
                            if (TSTime.Thr == 0)
                                setTime(23, TSTime.Tmin, TSTime.Ts);
                            else
                                setTime((TSTime.Thr - 1), TSTime.Tmin, TSTime.Ts);
                        } else
                            setTime(TSTime.Thr, TSTime.Tmin, TSTime.Ts);
                        // setTime(TSTime.Thr, TSTime.Tmin, 0);
                        usartOk();
                    } else {
                        usartEr();
                    }

                    break;
                case 'c': // тип годинника
                    // формат - ""$cN; N - тип, 1, або 2
                    if ((usart_data[2] - 48 == 1) || (usart_data[2] - 48 == 2)) {
                        setting_Val[TYPE_CLK] = usart_data[2] - 48;
                        write_eep(EE_TYPE_CLK, setting_Val[TYPE_CLK]);
                        clear_matrix();
                        if (setting_Val[TYPE_CLK] == 1)
                            blk_dot = 0;
                        else
                            blk_dot = 1;
                        idx_pnt = 0;
                        usartOk();
                        //                    EUSART_Write('O');
                        //                    EUSART_Write('K');
                        //                    // EUSART_Write('\r');
                        //                    EUSART_Write('\n');
                    } else {
                        usartEr();
                        //                    EUSART_Write('E');
                        //                    EUSART_Write('R');
                        //                    // EUSART_Write('\r');
                        //                    EUSART_Write('\n');

                    }

                    break;
                case 'f': // шрифт
                    // формат - $fN  N - номер шрифту від 1 до 5
                    switch (usart_data[2] - 48) {
                        case 1:
                            setting_Val[TYPE_FONT] = FONT_1;
                            write_eep(EE_FONT, setting_Val[TYPE_FONT]);
                            set_font();
                            break;
                        case 2:
                            setting_Val[TYPE_FONT] = FONT_2;
                            write_eep(EE_FONT, setting_Val[TYPE_FONT]);
                            set_font();
                            break;
                        case 3:
                            setting_Val[TYPE_FONT] = FONT_3;
                            write_eep(EE_FONT, setting_Val[TYPE_FONT]);
                            set_font();
                            break;
                        case 4:
                            setting_Val[TYPE_FONT] = FONT_4;
                            write_eep(EE_FONT, setting_Val[TYPE_FONT]);
                            set_font();
                            break;
                        case 5:
                            setting_Val[TYPE_FONT] = FONT_5;
                            write_eep(EE_FONT, setting_Val[TYPE_FONT]);
                            set_font();
                            break;
                    }
                    if (((usart_data[2] - 48) > 5) || (usart_data[2] - 48) == 0) {
                        usartEr();
                        //                    EUSART_Write('E'); // якщо не вірні дані
                        //                    EUSART_Write('R');
                        //                    // EUSART_Write('\r');
                        //                    EUSART_Write('\n');
                    } else {
                        usartOk();
                        //                    EUSART_Write('O');
                        //                    EUSART_Write('K');
                        //                    // EUSART_Write('\r');
                        //                    EUSART_Write('\n');
                    }

                    break;
                case 'd': // налаштування через синій зуб дата
                    // формат "$dDYDTMNYR  DY - день тижня(1 - неділя), DT - число, MN - місяць, YR - рік "
                    // наприклад - $d01270917 - неділя,27 вересня 17 року
                    setDate((((usart_data[2] - 48)*10) + usart_data[3] - 48),
                            (((usart_data[4] - 48)*10) + usart_data[5] - 48),
                            (((usart_data[6] - 48)*10) + usart_data[7] - 48),
                            (((usart_data[8] - 48)*10)+(usart_data[9] - 48)));
                    usartOk();
                    //                EUSART_Write('O');
                    //                EUSART_Write('K');
                    //                // EUSART_Write('\r');
                    //                EUSART_Write('\n');

                    break;
                case 'b': // налаштування через синій зуб яскравості
                    // формат "$bXN  X - тип(1 - автомат 0 - вручну, N - значення ручної - 0-7)
                    // 
                    if (((usart_data[2] - 48) == TYPE_BRG_MAN) || ((usart_data[2] - 48) == TYPE_BRG_AUTO)) {
                        setting_Val[TYPE_BRG] = usart_data[2] - 48;
                        setting_Val[VAL_BRG] = usart_data[3] - 48;
                        write_eep(EE_TYPE_BRG, setting_Val[TYPE_BRG]);
                    } else {
                        usartEr();
                        //                    EUSART_Write('E');
                        //                    EUSART_Write('R');
                        //                    // EUSART_Write('\r');
                        //                    EUSART_Write('\n');
                        break;
                    }
                    if (setting_Val[TYPE_BRG] == TYPE_BRG_MAN) {
                        write_eep(EE_DAT_BRG, setting_Val[VAL_BRG]);
                        Cmd7221(INTENSITY_R, setting_Val[VAL_BRG]); //Intensity Register
                    }
                    usartOk();
                    //                EUSART_Write('O');
                    //                EUSART_Write('K');
                    //                // EUSART_Write('\r');
                    //                EUSART_Write('\n');

                    break;
                case 's': // налаштування через синій зуб щогодинного сигналу
                    // формат "$sX  X - тип(1 - є 0 - нема)
                    // 
                    if (((usart_data[2] - 48) == ENABLE_SND_HR_OFF) || ((usart_data[2] - 48) == ENABLE_SND_HR_ON)) {
                        setting_Val[ENABLE_SND_HOUR] = usart_data[2] - 48;
                        write_eep(EE_EN_SND_H, setting_Val[ENABLE_SND_HOUR]);
                    } else {
                        usartEr();
                        //                    EUSART_Write('E');
                        //                    EUSART_Write('R');
                        //                    //   EUSART_Write('\r');
                        //                    EUSART_Write('\n');
                        break;
                    }
                    usartOk();
                    //                EUSART_Write('O');
                    //                EUSART_Write('K');
                    //                //  EUSART_Write('\r');
                    //                EUSART_Write('\n');

                    break;
                case 'a': // налаштування через синій зуб показу датчиків температури
                    // формат "$aXZ  XZ - датчики(X - кімнатний, Z - вуличний)(1 - показувати 0 - не показувати)
                    // 
                    if ((((usart_data[2] - 48) == 0) || ((usart_data[2] - 48) == 1)) && (((usart_data[3] - 48) == 0) || ((usart_data[3] - 48) == 1))) {
                        setting_Val[ENABLE_DS_1] = usart_data[2] - 48;
                        setting_Val[ENABLE_DS_2] = usart_data[3] - 48;
                        write_eep(EE_EN_DS1, setting_Val[ENABLE_DS_1]);
                        write_eep(EE_EN_DS2, setting_Val[ENABLE_DS_2]);
                    } else {
                        usartEr();
                        //                    EUSART_Write('E');
                        //                    EUSART_Write('R');
                        //                    //  EUSART_Write('\r');
                        //                    //  EUSART_Write('\n');
                        break;
                    }
                    usartOk();
                    //                EUSART_Write('O');
                    //                EUSART_Write('K');
                    //                // EUSART_Write('\r');
                    //                // EUSART_Write('\n');

                    break;
                case 'e': // налаштування через синій зуб показу датчика тиску
                    // формат "$eX  X - датчик(1 - показувати 0 - не показувати)
                    // 
                    if (((usart_data[2] - 48) == BMP_OFF) || ((usart_data[2] - 48) == BMP_ON)) {
                        setting_Val[ENABLE_BMP] = usart_data[2] - 48;
                        write_eep(EE_EN_BMP, setting_Val[ENABLE_BMP]);
                    } else {
                        usartEr();
                        //                    EUSART_Write('E');
                        //                    EUSART_Write('R');
                        //                    //  EUSART_Write('\r');
                        //                    //  EUSART_Write('\n');
                        break;
                    }
                    usartOk();
                    //                EUSART_Write('O');
                    //                EUSART_Write('K');
                    //                //  EUSART_Write('\r');
                    //                //  EUSART_Write('\n');

                    break;
                case 'w': // налаштування через синій літнього часу
                    // формат "$wX  X - 1 - літній час 0 - нема
                    // 
                    if (((usart_data[2] - 48) == DST_OFF) || ((usart_data[2] - 48) == DST_ON)) {
                        setting_Val[ENABLE_DST] = usart_data[2] - 48;
                        write_eep(EE_EN_DST, setting_Val[ENABLE_DST]); // запишемо в еепром
                    } else {
                        usartEr();
                        //                    EUSART_Write('E');
                        //                    EUSART_Write('R');
                        //                    //  EUSART_Write('\r');
                        //                    //  EUSART_Write('\n');
                        break;
                    }
                    usartOk();
                    //                EUSART_Write('O');
                    //                EUSART_Write('K');
                    //                //  EUSART_Write('\r');
                    //                //  EUSART_Write('\n');

                    break;
                case 'v': // налаштування через синій типу показу температури
                    // формат "$vX  X - 1 - тип 1 2 - тип 2
                    // 
                    if (((usart_data[2] - 48) == TYPE_TEMP_1) || ((usart_data[2] - 48) == TYPE_TEMP_2)) {
                        setting_Val[TYPE_TEMP] = usart_data[2] - 48;
                        write_eep(EE_TYPE_TEMP, setting_Val[TYPE_TEMP]); // запишемо в еепром
                    } else {
                        usartEr();
                        //                    EUSART_Write('E');
                        //                    EUSART_Write('R');
                        //                    //  EUSART_Write('\r');
                        //                    //  EUSART_Write('\n');
                        break;
                    }
                    usartOk();
                    //                EUSART_Write('O');
                    //                EUSART_Write('K');
                    //                // EUSART_Write('\r');
                    //                // EUSART_Write('\n');

                    break;
                case 'y': // налаштування через синій am2302
                    // формат "$yX  X - 1 - показ. 0 - не показ.
                    // 
                    if (((usart_data[2] - 48) == DHT_ON) || ((usart_data[2] - 48) == DHT_OFF)) {
                        setting_Val[ENABLE_DHT] = usart_data[2] - 48;
                        write_eep(EE_EN_AM2302, setting_Val[ENABLE_DHT]); // запишемо в еепром
                    } else {
                        usartEr();
                        //                    EUSART_Write('E');
                        //                    EUSART_Write('R');
                        //                    //  EUSART_Write('\r');
                        //                    //  EUSART_Write('\n');
                        break;
                    }
                    usartOk();
                    //                EUSART_Write('O');
                    //                EUSART_Write('K');
                    //                //  EUSART_Write('\r');
                    //                //  EUSART_Write('\n');

                    break;
                case 'z': // reset controller
                    // скидаємо контролер через 4 сек. після команди.
                    // потрібно для bootloader
                    usartOk();
                    //                EUSART_Write('O');
                    //                EUSART_Write('K');
                    //                // EUSART_Write('\r');
                    //                //  EUSART_Write('\n');
                    __delay_ms(1000);
                    __delay_ms(1000);
                    __delay_ms(1000);
                    __delay_ms(1000);
                    asm("reset");

                    break;
                    //                case 'q': // виводимо біг строку температура
                    //                    // формат "$qt - температура, якщо є дозвіл на показ датчиків
                    //                    // формат "$qp - атмосферний тиск
                    //                    if (usart_data[2] == 't') {
                    //                        events = KEY_DOWN_EVENT; // показати температуру
                    //                    } else if (usart_data[2] == 'p') {
                    //                        events = KEY_UP_EVENT; // показати тиск
                    //                    } else {
                    //                        usartEr();
                    //                        //                    EUSART_Write('E');
                    //                        //                    EUSART_Write('R');
                    //                        //                    //  EUSART_Write('\r');
                    //                        //                    //  EUSART_Write('\n');
                    //                        break;
                    //                    }
                    //                    usartOk();
                    //                    //                EUSART_Write('O');
                    //                    //                EUSART_Write('K');
                    //                    //                //  EUSART_Write('\r');
                    //                    //                //  EUSART_Write('\n');
                    //
                    //                    break;
                case 'S': // виводимо біг строку
                    // формат "$Sxtext -  текст, до 100 символів
                    // x - признак UTF8 or ASCII. u - utf8, a - ascii
                    if (usart_data[2] == 'a') {
                        rs_text_buf[0] = ' ';
                        for (j = 1; j <= (strlen((const char *) usart_data)) - 3; j++)
                            rs_text_buf[j] = usart_data[j + 2];
                    } else if (usart_data[2] == 'o') {
                        mess_show = 0; // вимикаємо вивід строки
                        usartOk();
                        //                    EUSART_Write('O');
                        //                    EUSART_Write('K');
                        //                    //  EUSART_Write('\r');
                        //                    EUSART_Write('\n');
                        break;
                    }
                    usartOk();
                    //                EUSART_Write('O');
                    //                EUSART_Write('K');
                    //                //  EUSART_Write('\r');
                    //                //  EUSART_Write('\n');

                    day_mess = TTime.Tdt;
                    mess_show = 1;
                    blk_dot = 0;
                    RTOS_DeleteTask(home_temp); //видаляємо задачу
                    RTOS_DeleteTask(radio_temp); //видаляємо задачу
                    RTOS_DeleteTask(pressure);
                    RTOS_DeleteTask(hum);
                    interval_scroll_text(rs_text_buf);
                    RTOS_SetTask(time_led, 0, cycle_main); //додаємо задачу
                    pre_ref_dis();
                    blk_dot = 1;
                    break;

                case 'r': // читаємо тестові значення
                    // 
                    switch (usart_data[2]) {
                            //                        case 'a': // $ra - показати значеняя АЦП
                            //                            EUSART_Write('$');
                            //                            EUSART_Write('a');
                            //                            EUSART_Write(((adc_res / 100) % 10) + 48); // передаємо першу цифру
                            //                            EUSART_Write(((adc_res / 10) % 10) + 48); //......
                            //                            EUSART_Write((adc_res % 10) + 48);
                            //                            EUSART_Write('\n');
                            //                            //EUSART_Write('_');
                            //                            break;
                        case 'v': // $rv - показати ID
                            //j = strlen(VERSION);
                            //for (j = 0; j <= (strlen(VERSION)) - 1; j++)
                            EUSART_Write('$');
                            EUSART_Write('v');

                            //                            EUSART_Write('V');
                            //                            EUSART_Write('e');
                            //                            EUSART_Write('r');
                            EUSART_Write('(');

                            for (j = 0; j < strlen(BuildVersion); j++)
                                EUSART_Write(BuildVersion[j]); //
                            EUSART_Write(')');
                            EUSART_Write(' '); // предаємо пробіл
                            EUSART_Write(' '); // предаємо пробіл

                            for (j = 0; j < strlen(BuildDate); j++)
                                EUSART_Write(BuildDate[j]); // 
                            EUSART_Write(' '); // предаємо пробіл

                            for (j = 0; j < strlen(BuildTime); j++)
                                EUSART_Write(BuildTime[j]); // передаємо першу цифру
                            EUSART_Write('\r');
                            EUSART_Write('\n');
                            break;
                            //                        case 't': // $rt - показати температуру 
                            //                            EUSART_Write(minus_radio); // передаємо першу цифру
                            //                            EUSART_Write(((data_Val[T_RADIO] / 100) % 10) + 48); // передаємо першу цифру
                            //                            EUSART_Write(((data_Val[T_RADIO] / 10) % 10) + 48); //......
                            //                            EUSART_Write((data_Val[T_RADIO] % 10) + 48);
                            //                            EUSART_Write('_');
                            //                            EUSART_Write(((data_Val[T_HOME] / 100) % 10) + 48); // передаємо першу цифру
                            //                            EUSART_Write(((data_Val[T_HOME] / 10) % 10) + 48); //......
                            //                            EUSART_Write((data_Val[T_HOME] % 10) + 48);
                            //                            EUSART_Write('_');
                            //                            break;
                            //                        case 'd': // $rd - показати температуру з датчика AM2302
                            //                            //                        EUSART_Write(minus_radio); // передаємо першу цифру
                            //                            EUSART_Write(((data_Val[T_HUM] / 100) % 10) + 48); // передаємо першу цифру
                            //                            EUSART_Write(((data_Val[T_HUM] / 10) % 10) + 48); //......
                            //                            EUSART_Write((data_Val[T_HUM] % 10) + 48);
                            //                            EUSART_Write('_');
                            //                            break;
                        default:
                            usartEr();
                            //                        EUSART_Write('E');
                            //                        EUSART_Write('R');
                            //                        //  EUSART_Write('\r');
                            //                        //  EUSART_Write('\n');
                            break;

                    }
                    //usartOk();
                    //                EUSART_Write('O');
                    //                EUSART_Write('K');
                    //                // EUSART_Write('\r');
                    //                // EUSART_Write('\n');

                    break;
            }
        }
        usart_data[0] = 0;
        usart_data[1] = 0;
        usart_data[2] = 0;
        i = 0;
        //      reinit_rx();
        messageComplete = FALSE;
    }

}

void usartOk(void) {
    EUSART_Write('O');
    EUSART_Write('K');
    EUSART_Write('\r');
    EUSART_Write('\n');
}

void usartEr(void) {
    EUSART_Write('E');
    EUSART_Write('R');
    EUSART_Write('\r');
    EUSART_Write('\n');
}

//  вивід версії

void version(void) {
    //  uint8_t i;

    //sprintf(text_buf,"%s %s %s", VERSION, compile_date, compile_time); // формуємо строку
    strcpy((char *) text_buf, (const char *) VERSION);
    strcat((char *) text_buf, (const char *) compile_date);
    strcat((char *) text_buf, (const char *) " ");
    strcat((char *) text_buf, (const char *) compile_time);
    interval_scroll_text(text_buf); // виводимо біг. строку

    //    while(scroll_text())
    //    {
    //        Update_Matrix(Dis_Buff);          // обновити дані на дисплеї
    //        for(i=0; i<SPEED_STRING; i++)
    //            __delay_ms(1);
    //
    //    };
}

//===========================================
//              АЦП перетворення
//===========================================

void read_adc(void) {
    ADCON0bits.GO = 1; // Вимірюємо
    while (ADCON0bits.GO); // чекаємо закінчення перетворення
    data_Val[ADC_RES] = (uint16_t) ((ADRESH << 8) + ADRESL);
    asm("nop");
    asm("nop");
#ifdef DEBUG

    printf("ADC_RESULT:%u\r\n", adc_res);
#endif  

}

//*****************************************
//  Перетворення кирилиці/ UTF8 --> ASCII
//*****************************************

//void convert_utf(uint8_t *buf) {
//    uint16_t temp;
//    uint8_t i, j = 0;
//    uint8_t len;
//
//    len = (uint8_t) (strlen((const char *) buf)) - 3; // довжина строки
//
//    for (i = 0; i <= (len); i++) { // проходимо всю строку
//        if (*(buf + j + 3) > 192) { // Якщо символ unicode
//            temp = (uint16_t) (*(buf + j + 3) << 8) | *(buf + j + 3 + 1);
//            if (temp == 53380)// літера Є
//                temp -= 53210;
//            else if (temp == 53382)// літера І
//                temp -= 53204;
//            else if (temp == 53383)// літера Ї
//                temp -= 53208;
//            else if (temp == 53652)// літера є
//                temp -= 53466;
//            else if (temp == 53654)// літера і
//                temp -= 53475;
//            else if (temp == 53655)// літера ї
//                temp -= 53464;
//            else if (temp <= 53439)
//                temp = temp - (uint16_t) 53200;
//            else if ((temp >= 53632)&&(temp <= 53647))
//                temp = temp - (uint16_t) 53392;
//            rs_text_buf[i] = (uint8_t) temp;
//            j += 2;
//            len -= 1;
//
//        } else {
//            rs_text_buf[i] = *(buf + j + 3);
//            j++;
//        }
//    }
//
//}


//===========================================
//       Автоматична яскравість матриці
//===========================================

void adj_brig(void) {

    if (data_Val[ADC_RES] >= 600)
        Cmd7221(INTENSITY_R, 0x05); //Intensity Register - 1/16
    else if (data_Val[ADC_RES] >= 58)
        Cmd7221(INTENSITY_R, 0x03); //Intensity Register - 1/16
    else if (data_Val[ADC_RES] <= 50)
        Cmd7221(INTENSITY_R, 0x00); //Intensity Register - 2/16


    //        Cmd7221(INTENSITY_R, 0x00); //Intensity Register - 2/16
    //    if (adc_res >= 600)
    //        Cmd7221(INTENSITY_R, 0x04); //Intensity Register - 1/16
    //    else if (adc_res >= 450)
    //        Cmd7221(INTENSITY_R, 0x03); //Intensity Register - 2/16
    //    else if (adc_res >= 300)
    //        Cmd7221(INTENSITY_R, 0x02); //Intensity Register - 2/16
    //    else if (adc_res >= 150)
    //        Cmd7221(INTENSITY_R, 0x01); //Intensity Register - 2/16
    //    else if (adc_res <= 100)
    //        Cmd7221(INTENSITY_R, 0x00); //Intensity Register - 2/16

}




// функція переривання по входу RB2
// то крапки мигають

void INT0_ISR(void) {
    if (blk_dot) {
        idx_pnt++;
        if (idx_pnt == 8) idx_pnt = 0;
        switch (idx_pnt) {
            case 0:
                Dis_Buff[15] = 0x36;
                Dis_Buff[16] = 0x36;
                break;
            case 1:
                Dis_Buff[15] = 0x24;
                Dis_Buff[16] = 0x36;
                break;
            case 2:
                Dis_Buff[15] = 0x24;
                Dis_Buff[16] = 0x24;
                break;
            case 3:
                Dis_Buff[15] = 0x24;
                Dis_Buff[16] = 0x00;
                break;
            case 4:
                Dis_Buff[15] = 0x00;
                Dis_Buff[16] = 0x00;
                break;
            case 5:
                Dis_Buff[15] = 0x12;
                Dis_Buff[16] = 0x00;
                break;
            case 6:
                Dis_Buff[15] = 0x12;
                Dis_Buff[16] = 0x12;
                break;
            case 7:
                Dis_Buff[15] = 0x12;
                Dis_Buff[16] = 0x36;
                break;
        }
    }
    //               putchar_b_buf(13, 23 + idx_pnt, &Font);
}


// функція переривання таймер1

void TMR1_ISR(void) {


    if (++timer_val >= 150) // затримка > 375мс
    {
        timer_val = 0;
        time_flag = 1;
        T1CONbits.TMR1ON = 0;

    }

    //     delay_digit++;
    //     if(delay_digit > 30)
    //     {
    //         delay_digit = 0;
    //         show_digit = ~show_digit;
    //     }

    //Update_Matrix(Dis_Buff);          // обновити дані на дисплеї
    //        RTOS_SetTask(GetTime, 0, 0); // додаємо одноразовий запуск задачі в диспетчер
    // кожні 500мс.
}

//=============================================================================
//   читаємо радіо датчик
//=============================================================================

void radioRead(void) {
    static uint8_t data_array[4] = {0, 0, 0, 0}; // робимо статичною, бо компілятор не вірно
    // читаємо радіодатчик                              ставить адреси перемінних
    if (nrf24_dataReady()) {
        nrf24_getData(data_array);
        //spi_rw(FLUSH_RX); // очистити прийомний буфер
        nrf24_powerUpRx();
        data_Val[T_RADIO] = data_array[1] | (uint16_t) (data_array[2] << 8);
        minus_radio = data_array[0];
        err_ds_count = 0;
        err_ds18 = 0;
    } else
        err_ds_count++;

    if (err_ds_count > 15000) // чекаємо ~5хв. Якщо не було ні одного зчитування
    {
        err_ds_count = 0; // починаємо знову рахувати
        err_ds18 = 1; // то ставимо признак помилки радіодатчика
        //nrf24_powerUpRx(); // Переводимо датчик у режим прийому, та скидаємо всі переривання
        nrf24_init(100, 4); // Ще раз ініціалізуємо
        data_Val[DS18_ERR]++;
    }

}

// передача по уарт показів датчиків

void sendDataSensors(void) {
    EUSART_Write('$');
    EUSART_Write('D');
    for (uint8_t j = 0; j < NUM_VALUES_DAT; j++) {
        EUSART_Write(data_Val[j]& 0xFF); //висилаємо молодший байт;
        EUSART_Write((data_Val[j] >> 8)& 0xFF); //висилаємо старший байт;
    }
    EUSART_Write(minus_radio);
    EUSART_Write('\r');
    EUSART_Write('\n');

}