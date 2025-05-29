#include "settings.h"
#include "nrf24l01p.h"

//extern uint8_t(*pFont)[][5];
//extern uint8_t Dis_Buff[BUF_SIZE]; // буфер дисплея
//extern uint8_t type_clk; // вигляд годинника
//extern uint8_t events;
//extern bit en_put; // чи можна писати у буфер символи
//extern bit show_digit; // чи показувати цифри, в нал. мигання
//extern uint8_t type_font;
//extern uint8_t en_h_snd; // чи можна генерувати сигнал
//extern uint8_t brig;// значення яскравості
//extern int8_t brg_type;// яскравість по датчику, чи постійна
//extern uint8_t blk_dot; // дозвіл на мигання кнопок
//extern int8_t en_ds_1;    //  чи пок. температуру з датчика 1
//extern int8_t en_ds_2;    //  чи пок. температуру з датчика 2
//extern int8_t en_bmp280; //  чи показуємо тиск
//extern uint8_t dst_flag; // чи зараз літній час????
//extern int8_t en_dst; // перехід на літній час
//extern uint8_t type_temp;
//extern int8_t en_am2302; //датчик вологості
//****************************************
// налаштування годинника -  хвилини
//****************************************

void time_set_min(void) {
    show_digit = ~show_digit;
    switch (events) {
        case MAIN_EVENT:

            break;
        case KEY_OK_EVENT: // якщо натиснули ок

            RTOS_DeleteTask(time_set_min); // видаляємо задачу
            RTOS_SetTask(time_set_hr, 0, 50); // ставимо задачу, налаштування годин
            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            if (setting_Val[TYPE_CLK] == TYPE_CLK_1) {
                putchar_b_buf(13, (unsigned) (TSTime.Tmin / 10) % 10, pFont, Dis_Buff);
                putchar_b_buf(19, (unsigned) TSTime.Tmin % 10, pFont, Dis_Buff);
            } else {
                putchar_b_buf(19, (unsigned) (TTime.Tmin / 10) % 10, pFont, Dis_Buff);
                putchar_b_buf(25, (unsigned) TTime.Tmin % 10, pFont, Dis_Buff);
            }
            events = MAIN_EVENT;
            en_put = 0;
            break;

        case KEY_EXIT_EVENT: // кнопка EXIT
            exitButSet(time_set_min);
            //            RTOS_DeleteTask(time_set_min); // переходимо в основний режим
            //            RTOS_DeleteTask(default_state);
            //            RTOS_SetTask(time_led, 0, cycle_main);
            //            events = MAIN_EVENT;
            //            clear_matrix();
            break;
        case KEY_UP_EVENT:
            TSTime.Tmin++;
            if (TSTime.Tmin > 59) TSTime.Tmin = 0;
            RTOS_SetTask(default_state, 2000, 0); // 5 секунд для виходу
            setTime(TSTime.Thr, TSTime.Tmin, 0);
            events = MAIN_EVENT;
            break;
        case KEY_DOWN_EVENT:
            TSTime.Tmin--;
            if (TSTime.Tmin == 255) TSTime.Tmin = 59;
            RTOS_SetTask(default_state, 2000, 0); // 5 секунд для виходу
            setTime(TSTime.Thr, TSTime.Tmin, 0);
            events = MAIN_EVENT;
            break;
    }
    if (en_put) {
        if (show_digit) // чи показувати цифри
        {
            if (setting_Val[TYPE_CLK] == TYPE_CLK_1) {
                putchar_b_buf(13, (unsigned) (TSTime.Tmin / 10) % 10, pFont, Dis_Buff);
                putchar_b_buf(19, (unsigned) TSTime.Tmin % 10, pFont, Dis_Buff);
            } else {
                putchar_b_buf(19, (unsigned) (TSTime.Tmin / 10) % 10, pFont, Dis_Buff);
                putchar_b_buf(25, (unsigned) TSTime.Tmin % 10, pFont, Dis_Buff);
            }
        } else {
            if (setting_Val[TYPE_CLK] == TYPE_CLK_1) {
                putchar_b_buf(13, 0, (void *) Font, Dis_Buff);
                putchar_b_buf(19, 0, (void *) Font, Dis_Buff);
            } else {
                putchar_b_buf(19, 0, (void *) Font, Dis_Buff);
                putchar_b_buf(25, 0, (void *) Font, Dis_Buff);
            }
        }
    }
    Update_Matrix(Dis_Buff); // обновити дані на дисплеї
    en_put = 1;
}

//****************************************
// налаштування годинника -  годин
//****************************************

void time_set_hr(void) {
    show_digit = ~show_digit;
    switch (events) {
        case MAIN_EVENT:

            break;
        case KEY_OK_EVENT:
            okButSet(time_set_hr, time_set_yr);
            //            RTOS_DeleteTask(time_set_hr);
            //            RTOS_SetTask(time_set_yr, 0, cycle_main);
            //            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            //            //RTOS_DeleteTask(default_state);
            //            events = MAIN_EVENT;
            //            clear_matrix();
            //            en_put = 0; // заборонити малювати
            break;
        case KEY_EXIT_EVENT:
            exitButSet(time_set_hr);
            //            RTOS_DeleteTask(time_set_hr);
            //            RTOS_DeleteTask(default_state);
            //            RTOS_SetTask(time_led, 0, cycle_main);
            //            events = MAIN_EVENT;
            //            clear_matrix();
            break;
        case KEY_UP_EVENT:
            TSTime.Thr++;
            if (TSTime.Thr > 23) TSTime.Thr = 0;
            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            events = MAIN_EVENT;
            if (dst_flag) {// якщо час літній
                if (TSTime.Thr == 0)
                    setTime(23, TSTime.Tmin, 0);
                else
                    setTime((uint8_t) (TSTime.Thr - 1), TSTime.Tmin, 0);
            } else
                setTime(TSTime.Thr, TSTime.Tmin, 0);
            break;
        case KEY_DOWN_EVENT:
            TSTime.Thr--;

            if (TSTime.Thr == 255) TSTime.Thr = 23;
            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            events = MAIN_EVENT;
            if (dst_flag) {// якщо час літній
                if (TSTime.Thr == 0)
                    setTime(23, TSTime.Tmin, 0);
                else
                    setTime((uint8_t) (TSTime.Thr - 1), TSTime.Tmin, 0);
            } else
                setTime(TSTime.Thr, TSTime.Tmin, 0);
            break;
    }
    if (en_put) {
        if (show_digit) // чи показувати цифри
        {

            // putchar_b_buf(13,(TSTime.Tmin/10) % 10, pFont);
            // putchar_b_buf(19,TSTime.Tmin % 10, pFont);
            if (setting_Val[TYPE_CLK] == TYPE_CLK_1) {
                if ((TSTime.Thr / 10) % 10)
                    putchar_b_buf(0, (unsigned) (TSTime.Thr / 10) % 10, pFont, Dis_Buff);
                else
                    putchar_b_buf(0, 0, (void *) Font, Dis_Buff);
                putchar_b_buf(6, (unsigned) TSTime.Thr % 10, pFont, Dis_Buff);
            } else {
                if ((TSTime.Thr / 10) % 10)
                    putchar_b_buf(1, (unsigned) (TSTime.Thr / 10) % 10, pFont, Dis_Buff);
                else
                    putchar_b_buf(1, 0, (void *) Font, Dis_Buff);
                putchar_b_buf(7, (unsigned) TSTime.Thr % 10, pFont, Dis_Buff);
            }
        } else {
            if (setting_Val[TYPE_CLK] == TYPE_CLK_1) {
                putchar_b_buf(0, 0, (void *) Font, Dis_Buff);
                putchar_b_buf(6, 0, (void *) Font, Dis_Buff);
            } else {

                putchar_b_buf(1, 0, (void *) Font, Dis_Buff);
                putchar_b_buf(7, 0, (void *) Font, Dis_Buff);
            }
        }
    }
    Update_Matrix(Dis_Buff); // обновити дані на дисплеї
    en_put = 1; // дозволити малювати символи
}
//***************************************************************
//  встановлення року
//***************************************************************

void time_set_yr(void) {
    switch (events) {
        case MAIN_EVENT:

            break;
        case KEY_OK_EVENT:
            okButSet(time_set_yr, time_set_mt);
            //            RTOS_DeleteTask(time_set_yr);
            //            RTOS_SetTask(time_set_mt, 0, cycle_main);
            //            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            //            events = MAIN_EVENT;
            //            en_put = 0;
            break;
        case KEY_EXIT_EVENT:
            exitButSet(time_set_yr);
            //            RTOS_DeleteTask(time_set_yr);
            //            RTOS_DeleteTask(default_state);
            //            RTOS_SetTask(time_led, 0, cycle_main);
            //            events = MAIN_EVENT;
            //            en_put = 0;
            //            clear_matrix();
            break;
        case KEY_UP_EVENT:
            TSTime.Tyr++;
            if (TSTime.Tyr > 99) TSTime.Tyr = 0;
            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            events = MAIN_EVENT;
            setDate(TSTime.Tdy, TSTime.Tdt, TSTime.Tmt, TSTime.Tyr);
            break;
        case KEY_DOWN_EVENT:
            TSTime.Tyr--;
            if (TSTime.Tyr == 255) TSTime.Tyr = 99;
            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            events = MAIN_EVENT;
            setDate(TSTime.Tdy, TSTime.Tdt, TSTime.Tmt, TSTime.Tyr);
            break;
    }
    if (en_put) {
        putchar_b_buf(0, STR_YR[0], (void *) Font, Dis_Buff);
        putchar_b_buf(6, STR_YR[1], (void *) Font, Dis_Buff);
        putchar_b_buf(12, STR_YR[2], (void *) Font, Dis_Buff);
        putchar_b_buf(18, (unsigned) (TSTime.Tyr / 10) % 10, pFont, Dis_Buff);
        putchar_b_buf(24, (unsigned) TSTime.Tyr % 10, pFont, Dis_Buff);
    }
    Update_Matrix(Dis_Buff); // обновити дані на дисплеї
    en_put = 1;
}


//***************************************************************
//  встановлення місяць
//***************************************************************

void time_set_mt(void) {
    switch (events) {
        case MAIN_EVENT:

            break;
        case KEY_OK_EVENT:
            okButSet(time_set_mt, time_set_dt);
            //            RTOS_DeleteTask(time_set_mt);
            //            RTOS_SetTask(time_set_dt, 0, cycle_main);
            //            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            //            events = MAIN_EVENT;
            //            en_put = 1;
            break;
        case KEY_EXIT_EVENT:
            exitButSet(time_set_mt);
            //            RTOS_DeleteTask(time_set_mt);
            //            RTOS_DeleteTask(default_state);
            //            RTOS_SetTask(time_led, 0, cycle_main);
            //            events = MAIN_EVENT;
            //            en_put = 0;
            //            clear_matrix();
            break;
        case KEY_UP_EVENT:
            TSTime.Tmt++;
            if (TSTime.Tmt > 12) TSTime.Tmt = 1;
            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            events = MAIN_EVENT;
            setDate(TSTime.Tdy, TSTime.Tdt, TSTime.Tmt, TSTime.Tyr);
            break;
        case KEY_DOWN_EVENT:
            TSTime.Tmt--;
            if (TSTime.Tmt == 0) TSTime.Tmt = 12;
            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            events = MAIN_EVENT;
            setDate(TSTime.Tdy, TSTime.Tdt, TSTime.Tmt, TSTime.Tyr);
            break;
    }
    if (en_put) {
        putchar_b_buf(0, STR_MNT[0], (void *) Font, Dis_Buff);
        putchar_b_buf(6, STR_MNT[1], (void *) Font, Dis_Buff);
        putchar_b_buf(12, STR_MNT[2], (void *) Font, Dis_Buff);
        switch (TSTime.Tmt) {
            case 1:
                putchar_b_buf(18, STR_MNT_1[0], (void *) Font, Dis_Buff);
                putchar_b_buf(24, STR_MNT_1[1], (void *) Font, Dis_Buff);
                break;
            case 2:
                putchar_b_buf(18, STR_MNT_2[0], (void *) Font, Dis_Buff);
                putchar_b_buf(24, STR_MNT_2[1], (void *) Font, Dis_Buff);
                break;
            case 3:
                putchar_b_buf(18, STR_MNT_3[0], (void *) Font, Dis_Buff);
                putchar_b_buf(24, STR_MNT_3[1], (void *) Font, Dis_Buff);
                break;
            case 4:
                putchar_b_buf(18, STR_MNT_4[0], (void *) Font, Dis_Buff);
                putchar_b_buf(24, STR_MNT_4[1], (void *) Font, Dis_Buff);
                break;
            case 5:
                putchar_b_buf(18, STR_MNT_5[0], (void *) Font, Dis_Buff);
                putchar_b_buf(24, STR_MNT_5[1], (void *) Font, Dis_Buff);
                break;
            case 6:
                putchar_b_buf(18, STR_MNT_6[0], (void *) Font, Dis_Buff);
                putchar_b_buf(24, STR_MNT_6[1], (void *) Font, Dis_Buff);
                break;
            case 7:
                putchar_b_buf(18, STR_MNT_7[0], (void *) Font, Dis_Buff);
                putchar_b_buf(24, STR_MNT_7[1], (void *) Font, Dis_Buff);
                break;
            case 8:
                putchar_b_buf(18, STR_MNT_8[0], (void *) Font, Dis_Buff);
                putchar_b_buf(24, STR_MNT_8[1], (void *) Font, Dis_Buff);
                break;
            case 9:
                putchar_b_buf(18, STR_MNT_9[0], (void *) Font, Dis_Buff);
                putchar_b_buf(24, STR_MNT_9[1], (void *) Font, Dis_Buff);
                break;
            case 10:
                putchar_b_buf(18, STR_MNT_10[0], (void *) Font, Dis_Buff);
                putchar_b_buf(24, STR_MNT_10[1], (void *) Font, Dis_Buff);
                break;
            case 11:
                putchar_b_buf(18, STR_MNT_11[0], (void *) Font, Dis_Buff);
                putchar_b_buf(24, STR_MNT_11[1], (void *) Font, Dis_Buff);
                break;
            case 12:
                putchar_b_buf(18, STR_MNT_12[0], (void *) Font, Dis_Buff);
                putchar_b_buf(24, STR_MNT_12[1], (void *) Font, Dis_Buff);
                break;

        }
    }
    Update_Matrix(Dis_Buff); // обновити дані на дисплеї
    en_put = 1;
}

//***************************************************************
//  встановлення число
//***************************************************************

void time_set_dt(void) {
    switch (events) {
        case MAIN_EVENT:

            break;
        case KEY_OK_EVENT:
            okButSet(time_set_dt, time_set_dy);
            //            RTOS_DeleteTask(time_set_dt);
            //            RTOS_SetTask(time_set_dy, 0, cycle_main);
            //            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            //            events = MAIN_EVENT;
            //            en_put = 1;
            break;
        case KEY_EXIT_EVENT:
            exitButSet(time_set_dt);
            //            RTOS_DeleteTask(time_set_dt);
            //            RTOS_DeleteTask(default_state);
            //            RTOS_SetTask(time_led, 0, cycle_main);
            //            events = MAIN_EVENT;
            //            en_put = 0;
            //            clear_matrix();
            break;
        case KEY_UP_EVENT:
            TSTime.Tdt++;
            if (TSTime.Tyr % 4) { //  якщо не високосний
                if ((TSTime.Tdt > day_in_m[TSTime.Tmt - (unsigned) 1]) | ((TSTime.Tmt == 2) && (TSTime.Tdt > (day_in_m[TSTime.Tmt - (unsigned) 1]) - 1)))
                    TSTime.Tdt = 1;
            } else { // якщо високосний
                if (TSTime.Tdt > day_in_m[TSTime.Tmt - (unsigned) 1])
                    TSTime.Tdt = 1;
            }
            //if (TSTime.Tdt > 31) TSTime.Tdt = 1;
            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            events = MAIN_EVENT;
            setDate(TSTime.Tdy, TSTime.Tdt, TSTime.Tmt, TSTime.Tyr);
            break;
        case KEY_DOWN_EVENT:
            TSTime.Tdt--;
            if (TSTime.Tyr % 4) { //  якщо не високосний
                if ((TSTime.Tdt == 0) && (TSTime.Tmt != 2)) {
                    TSTime.Tdt = day_in_m[TSTime.Tmt - (unsigned) 1];
                } else {
                    if ((TSTime.Tmt == 2) && (TSTime.Tdt == 0))
                        TSTime.Tdt = (day_in_m[TSTime.Tmt - (unsigned) 1]) - (unsigned) 1;
                }
            } else { // якщо високосний
                if (TSTime.Tdt == 0)
                    TSTime.Tdt = day_in_m[TSTime.Tmt - (unsigned) 1];
            }
            //if (TSTime.Tdt == 0) TSTime.Tdt = 31;
            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            events = MAIN_EVENT;
            setDate(TSTime.Tdy, TSTime.Tdt, TSTime.Tmt, TSTime.Tyr);
            break;
    }
    if (en_put) {
        putchar_b_buf(0, STR_DATE[0], (void *) Font, Dis_Buff);
        putchar_b_buf(6, STR_DATE[1], (void *) Font, Dis_Buff);
        putchar_b_buf(12, STR_DATE[2], (void *) Font, Dis_Buff);
        putchar_b_buf(18, (unsigned) (TSTime.Tdt / 10) % 10, pFont, Dis_Buff);
        putchar_b_buf(24, (unsigned) TSTime.Tdt % 10, pFont, Dis_Buff);
    }
    Update_Matrix(Dis_Buff); // обновити дані на дисплеї
    en_put = 1;
}


//***************************************************************
//  встановлення день
//***************************************************************

void time_set_dy(void) {
    switch (events) {
        case MAIN_EVENT:

            break;
        case KEY_OK_EVENT:
            okButSet(time_set_dy, set_font_set);
            //            RTOS_DeleteTask(time_set_dy);
            //            RTOS_SetTask(set_font_set, 0, cycle_main);
            //            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            //            events = MAIN_EVENT;
            //            en_put = 1;
            break;
        case KEY_EXIT_EVENT:
            exitButSet(time_set_dy);
            //            RTOS_DeleteTask(time_set_dy);
            //            RTOS_DeleteTask(default_state);
            //            RTOS_SetTask(time_led, 0, cycle_main);
            //            events = MAIN_EVENT;
            //            en_put = 0;
            //            clear_matrix();
            break;
        case KEY_UP_EVENT:
            TSTime.Tdy++;
            if (TSTime.Tdy > 7) TSTime.Tdy = 1;
            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            events = MAIN_EVENT;
            setDate(TSTime.Tdy, TSTime.Tdt, TSTime.Tmt, TSTime.Tyr);
            break;
        case KEY_DOWN_EVENT:
            TSTime.Tdy--;
            if (TSTime.Tdy == 0) TSTime.Tdy = 7;
            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            events = MAIN_EVENT;
            setDate(TSTime.Tdy, TSTime.Tdt, TSTime.Tmt, TSTime.Tyr);
            break;
    }
    if (en_put) {
        putchar_b_buf(0, STR_DAY[0], (void *) Font, Dis_Buff);
        putchar_b_buf(6, STR_DAY[1], (void *) Font, Dis_Buff);
        putchar_b_buf(12, STR_DAY[2], (void *) Font, Dis_Buff);
        switch (TSTime.Tdy) {
            case 2:
                putchar_b_buf(18, STR_DAY_1[0], (void *) Font, Dis_Buff);
                putchar_b_buf(24, STR_DAY_1[1], (void *) Font, Dis_Buff);
                break;
            case 3:
                putchar_b_buf(18, STR_DAY_2[0], (void *) Font, Dis_Buff);
                putchar_b_buf(24, STR_DAY_2[1], (void *) Font, Dis_Buff);
                break;
            case 4:
                putchar_b_buf(18, STR_DAY_3[0], (void *) Font, Dis_Buff);
                putchar_b_buf(24, STR_DAY_3[1], (void *) Font, Dis_Buff);
                break;
            case 5:
                putchar_b_buf(18, STR_DAY_4[0], (void *) Font, Dis_Buff);
                putchar_b_buf(24, STR_DAY_4[1], (void *) Font, Dis_Buff);
                break;
            case 6:
                putchar_b_buf(18, STR_DAY_5[0], (void *) Font, Dis_Buff);
                putchar_b_buf(24, STR_DAY_5[1], (void *) Font, Dis_Buff);
                break;
            case 7:
                putchar_b_buf(18, STR_DAY_6[0], (void *) Font, Dis_Buff);
                putchar_b_buf(24, STR_DAY_6[1], (void *) Font, Dis_Buff);
                break;
            case 1:
                putchar_b_buf(18, STR_DAY_7[0], (void *) Font, Dis_Buff);
                putchar_b_buf(24, STR_DAY_7[1], (void *) Font, Dis_Buff);
                break;
        }
    }
    Update_Matrix(Dis_Buff); // обновити дані на дисплеї
    en_put = 1;
}

//***************************************************************
//  встановлення шрифту
//***************************************************************

void set_font_set(void) {
    switch (events) {
        case MAIN_EVENT:

            break;
        case KEY_OK_EVENT:
            okButSet(set_font_set, set_type_clk);
            //            RTOS_DeleteTask(set_font_set);
            //            RTOS_SetTask(set_type_clk, 0, cycle_main);
            //            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            //            events = MAIN_EVENT;
            //            en_put = 0;
            //            clear_matrix();
            break;
        case KEY_EXIT_EVENT:
            exitButSet(set_font_set);
            //            RTOS_DeleteTask(set_font_set);
            //            RTOS_DeleteTask(default_state);
            //            RTOS_SetTask(time_led, 0, cycle_main);
            //            events = MAIN_EVENT;
            //            en_put = 0;
            //            clear_matrix();
            break;
        case KEY_UP_EVENT:
            setting_Val[TYPE_FONT]++;
            if (setting_Val[TYPE_FONT] > 5) setting_Val[TYPE_FONT] = FONT_1;
            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            events = MAIN_EVENT;
            write_eep(EE_FONT, setting_Val[TYPE_FONT]);
            set_font();
            break;
        case KEY_DOWN_EVENT:
            setting_Val[TYPE_FONT]--;
            if (setting_Val[TYPE_FONT] == 0) setting_Val[TYPE_FONT] = FONT_5;
            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            events = MAIN_EVENT;
            write_eep(EE_FONT, setting_Val[TYPE_FONT]);
            set_font();
            break;
    }
    if (en_put) {
        putchar_b_buf(0, STR_FONT[0], (void *) Font, Dis_Buff);
        putchar_b_buf(6, STR_FONT[1], (void *) Font, Dis_Buff);
        putchar_b_buf(12, STR_FONT[2], (void *) Font, Dis_Buff);
        putchar_b_buf(18, (unsigned) setting_Val[TYPE_FONT] % 10, pFont, Dis_Buff);
        putchar_b_buf(24, 0, (void *) Font, Dis_Buff);
    }
    Update_Matrix(Dis_Buff); // обновити дані на дисплеї
    en_put = 1;
}

//***************************************************************
//  встановлення вигляду годинника
//***************************************************************

void set_type_clk(void) {
    switch (events) {
        case MAIN_EVENT:

            break;
        case KEY_OK_EVENT:
            okButSet(set_type_clk, brg_set);
            //            RTOS_DeleteTask(set_type_clk);
            //            RTOS_SetTask(brg_set, 0, cycle_main);
            //            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            //            events = MAIN_EVENT;
            //            en_put = 0;
            //            clear_matrix();
            break;
        case KEY_EXIT_EVENT:
            exitButSet(set_type_clk);
            //            RTOS_DeleteTask(set_type_clk);
            //            RTOS_DeleteTask(default_state);
            //            RTOS_SetTask(time_led, 0, cycle_main);
            //            events = MAIN_EVENT;
            //            en_put = 0;
            //            clear_matrix();
            //            if (setting_Val[TYPE_CLK] == 1)
            //                blk_dot = 0;
            //            else
            //                blk_dot = 1;

            break;
        case KEY_UP_EVENT:
            setting_Val[TYPE_CLK]++;
            if (setting_Val[TYPE_CLK] > 2) setting_Val[TYPE_CLK] = 1;
            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            events = MAIN_EVENT;
            write_eep(EE_TYPE_CLK, setting_Val[TYPE_CLK]);
            break;
        case KEY_DOWN_EVENT:
            setting_Val[TYPE_CLK]--;
            if (setting_Val[TYPE_CLK] == 0) setting_Val[TYPE_CLK] = 2;
            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            events = MAIN_EVENT;
            write_eep(EE_TYPE_CLK, setting_Val[TYPE_CLK]);
            break;
    }
    if (en_put) {
        putchar_b_buf(0, STR_TP_CLK[0], (void *) Font, Dis_Buff);
        putchar_b_buf(6, STR_TP_CLK[1], (void *) Font, Dis_Buff);
        putchar_b_buf(12, STR_TP_CLK[2], (void *) Font, Dis_Buff);
        putchar_b_buf(18, (unsigned) setting_Val[TYPE_CLK] % 10, pFont, Dis_Buff);
        putchar_b_buf(24, 0, (void *) Font, Dis_Buff);
    }
    Update_Matrix(Dis_Buff); // обновити дані на дисплеї
    en_put = 1;

}

//=====================================================
//   Налаштування - яскравість авто чи постійна
//=====================================================

void brg_set() {
    switch (events) {
        case MAIN_EVENT:

            break;
        case KEY_OK_EVENT:
            if (setting_Val[TYPE_BRG]) {
                okButSet(brg_set, set_sound_h);
                //                RTOS_DeleteTask(brg_set);
                //                RTOS_SetTask(set_sound_h, 0, cycle_main);
                //                RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
                //                events = MAIN_EVENT;
                //                en_put = 0;
                //                clear_matrix();
            } else {
                okButSet(brg_set, set_brg_manual);
                //                RTOS_DeleteTask(brg_set);
                //                RTOS_SetTask(set_brg_manual, 0, cycle_main);
                setting_Val[VAL_BRG] = read_eep(EE_DAT_BRG);
                Cmd7221(INTENSITY_R, setting_Val[VAL_BRG]); //Intensity Register
                //                RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
                //                events = MAIN_EVENT;
                //                en_put = 0;
                //                clear_matrix();
            }

            break;
        case KEY_EXIT_EVENT:
            exitButSet(brg_set);
            //            RTOS_DeleteTask(brg_set);
            //            RTOS_DeleteTask(default_state);
            //            RTOS_SetTask(time_led, 0, cycle_main);
            //            events = MAIN_EVENT;
            //            en_put = 0;
            //            clear_matrix();
            break;
        case KEY_UP_EVENT:
            setting_Val[TYPE_BRG] = !setting_Val[TYPE_BRG];
            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            events = MAIN_EVENT;
            write_eep(EE_TYPE_BRG, (uint8_t) setting_Val[TYPE_BRG]);
            break;
        case KEY_DOWN_EVENT:
            setting_Val[TYPE_BRG] = !setting_Val[TYPE_BRG];
            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            events = MAIN_EVENT;
            write_eep(EE_TYPE_BRG, (uint8_t) setting_Val[TYPE_BRG]);
            break;
    }
    if (en_put) {
        putchar_b_buf(0, STR_BRG[0], (void *) Font, Dis_Buff);
        putchar_b_buf(6, STR_BRG[1], (void *) Font, Dis_Buff);
        putchar_b_buf(12, STR_BRG[2], (void *) Font, Dis_Buff);
        if (setting_Val[TYPE_BRG]) {
            putchar_b_buf(18, STR_BRG_A[0], (void *) Font, Dis_Buff);
            putchar_b_buf(24, STR_BRG_A[1], (void *) Font, Dis_Buff);
        } else {
            putchar_b_buf(18, STR_BRG_M[0], (void *) Font, Dis_Buff);
            putchar_b_buf(24, STR_BRG_M[1], (void *) Font, Dis_Buff);
        }

    }
    Update_Matrix(Dis_Buff); // оновити дані на дисплеї
    en_put = 1;

}


//**************************************************
//      ручна яскравість - налаштування
//*************************************************

void set_brg_manual() {
    switch (events) {
        case MAIN_EVENT:

            break;
        case KEY_OK_EVENT:
            okButSet(set_brg_manual, set_sound_h);
            //            RTOS_DeleteTask(set_brg_manual);
            //            RTOS_SetTask(set_sound_h, 0, cycle_main);
            //            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            //            events = MAIN_EVENT;
            //            en_put = 0;
            //            clear_matrix();
            break;
        case KEY_EXIT_EVENT:
            exitButSet(set_brg_manual);
            //            RTOS_DeleteTask(set_brg_manual);
            //            RTOS_DeleteTask(default_state);
            //            RTOS_SetTask(time_led, 0, cycle_main);
            //            events = MAIN_EVENT;
            //            en_put = 0;
            //            clear_matrix();
            break;
        case KEY_UP_EVENT:
            setting_Val[VAL_BRG]++;
            if (setting_Val[VAL_BRG] > VAL_BRG_NUM_8) setting_Val[VAL_BRG] = VAL_BRG_NUM_1;
            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            events = MAIN_EVENT;
            write_eep(EE_DAT_BRG, setting_Val[VAL_BRG]);
            Cmd7221(INTENSITY_R, setting_Val[VAL_BRG]); //Intensity Register
            break;
        case KEY_DOWN_EVENT:
            setting_Val[VAL_BRG]--;
            if (setting_Val[VAL_BRG] == 255) setting_Val[VAL_BRG] = VAL_BRG_NUM_8;
            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            events = MAIN_EVENT;
            write_eep(EE_DAT_BRG, setting_Val[VAL_BRG]);
            Cmd7221(INTENSITY_R, setting_Val[VAL_BRG]); //Intensity Register
            break;
    }
    if (en_put) {
        putchar_b_buf(0, STR_BRG[0], (void *) Font, Dis_Buff);
        putchar_b_buf(6, STR_BRG[1], (void *) Font, Dis_Buff);
        putchar_b_buf(12, STR_BRG[2], (void *) Font, Dis_Buff);
        putchar_b_buf(18, (unsigned) setting_Val[VAL_BRG] % 10, (void *) pFont, Dis_Buff);
        putchar_b_buf(24, 0, (void *) Font, Dis_Buff);
    }
    Update_Matrix(Dis_Buff); // обновити дані на дисплеї
    en_put = 1;

}


//============================================================
// Налаштування - чи можна відтворювати щогодинний сигнал. 
//============================================================

void set_sound_h(void) {
    switch (events) {
        case MAIN_EVENT:

            break;
        case KEY_OK_EVENT:
            okButSet(set_sound_h, set_en_ds1);
            //            RTOS_DeleteTask(set_sound_h);
            //            RTOS_SetTask(set_en_ds1, 0, cycle_main);
            //            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу 
            //            events = MAIN_EVENT;
            //            en_put = 0;
            //            clear_matrix();
            break;
        case KEY_EXIT_EVENT:
            exitButSet(set_sound_h);
            //            RTOS_DeleteTask(set_sound_h);
            //            RTOS_SetTask(time_led, 0, cycle_main);
            //            RTOS_DeleteTask(default_state);
            //            events = MAIN_EVENT;
            //            en_put = 0;
            //            clear_matrix();
            break;
        case KEY_UP_EVENT:
            setting_Val[ENABLE_SND_HOUR]++;
            if (setting_Val[ENABLE_SND_HOUR] > ENABLE_SND_HR_ON) setting_Val[ENABLE_SND_HOUR] = ENABLE_SND_HR_OFF;
            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            events = MAIN_EVENT;
            write_eep(EE_EN_SND_H, setting_Val[ENABLE_SND_HOUR]);
            break;
        case KEY_DOWN_EVENT:
            setting_Val[ENABLE_SND_HOUR]--;
            if (setting_Val[ENABLE_SND_HOUR] == 255) setting_Val[ENABLE_SND_HOUR] = ENABLE_SND_HR_ON;
            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            events = MAIN_EVENT;
            write_eep(EE_EN_SND_H, setting_Val[ENABLE_SND_HOUR]);
            break;
    }
    if (en_put) {
        putchar_b_buf(0, STR_SND_H[0], (void *) Font, Dis_Buff);
        putchar_b_buf(6, STR_SND_H[1], (void *) Font, Dis_Buff);
        putchar_b_buf(12, STR_SND_H[2], (void *) Font, Dis_Buff);
        putchar_b_buf(18, (unsigned) setting_Val[ENABLE_SND_HOUR] % 10, (void *) pFont, Dis_Buff);
        putchar_b_buf(24, 0, (void *) Font, Dis_Buff);
    }
    Update_Matrix(Dis_Buff); // обновити дані на дисплеї
    en_put = 1;

}

//============================================================
// Налаштування - чи можна показувати температуру з 1 датчика. 
//============================================================

void set_en_ds1(void) {
    switch (events) {
        case MAIN_EVENT:

            break;
        case KEY_OK_EVENT:
            okButSet(set_en_ds1, set_en_ds2);
            //            RTOS_DeleteTask(set_en_ds1);
            //            RTOS_SetTask(set_en_ds2, 0, cycle_main);
            //            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            //            events = MAIN_EVENT;
            //            en_put = 0;
            //            clear_matrix();
            break;
        case KEY_EXIT_EVENT:
            exitButSet(set_en_ds1);
            //            RTOS_DeleteTask(set_en_ds1);
            //            RTOS_SetTask(time_led, 0, cycle_main);
            //            RTOS_DeleteTask(default_state);
            //            events = MAIN_EVENT;
            //            en_put = 0;
            //            clear_matrix();
            break;
        case KEY_UP_EVENT:
            setting_Val[ENABLE_DS_1] = !(setting_Val[ENABLE_DS_1]);
            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            events = MAIN_EVENT;
            write_eep(EE_EN_DS1, (uint8_t) setting_Val[ENABLE_DS_1]);
            break;
        case KEY_DOWN_EVENT:
            //setting_Val[ENABLE_DS_1] = !(setting_Val[ENABLE_DS_1]);
            //RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            events = KEY_UP_EVENT;
            //write_eep(EE_EN_DS1, (uint8_t) setting_Val[ENABLE_DS_1]);
            break;

    }
    if (en_put) {
        putchar_b_buf(0, STR_DS1[0], (void *) Font, Dis_Buff);
        putchar_b_buf(6, STR_DS1[1], (void *) Font, Dis_Buff);
        putchar_b_buf(12, STR_DS1[2], (void *) Font, Dis_Buff);
        if (setting_Val[ENABLE_DS_1])
            putchar_b_buf(18, '+', (void *) Font, Dis_Buff);
        else
            putchar_b_buf(18, '-', (void *) Font, Dis_Buff);
        putchar_b_buf(24, 0, (void *) Font, Dis_Buff);
    }
    Update_Matrix(Dis_Buff); // обновити дані на дисплеї
    en_put = 1;

}

//============================================================
// Налаштування - чи можна показувати температуру з 2 датчика. 
//============================================================

void set_en_ds2(void) {
    switch (events) {
        case MAIN_EVENT:

            break;
        case KEY_OK_EVENT:
            okButSet(set_en_ds2, set_en_bmp);
            //            RTOS_DeleteTask(set_en_ds2);
            //            RTOS_SetTask(set_en_bmp, 0, cycle_main);
            //            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            //            events = MAIN_EVENT;
            //            en_put = 0;
            //            clear_matrix();
            break;
        case KEY_EXIT_EVENT:
            exitButSet(set_en_ds2);
            //            RTOS_DeleteTask(set_en_ds2);
            //            RTOS_SetTask(time_led, 0, cycle_main);
            //            RTOS_DeleteTask(default_state);
            //            events = MAIN_EVENT;
            //            en_put = 0;
            //            clear_matrix();
            break;
        case KEY_UP_EVENT:
            setting_Val[ENABLE_DS_2] = !(setting_Val[ENABLE_DS_2]);
            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            events = MAIN_EVENT;
            write_eep(EE_EN_DS2, (uint8_t) setting_Val[ENABLE_DS_2]);
            break;
        case KEY_DOWN_EVENT:
            // setting_Val[ENABLE_DS_2] = !(setting_Val[ENABLE_DS_2]);
            // RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            events = KEY_UP_EVENT;
            // write_eep(EE_EN_DS2, (uint8_t) setting_Val[ENABLE_DS_2]);
            break;

    }
    if (en_put) {
        putchar_b_buf(0, STR_DS2[0], (void *) Font, Dis_Buff);
        putchar_b_buf(6, STR_DS2[1], (void *) Font, Dis_Buff);
        putchar_b_buf(12, STR_DS2[2], (void *) Font, Dis_Buff);
        if (setting_Val[ENABLE_DS_2])
            putchar_b_buf(18, '+', (void *) Font, Dis_Buff);
        else
            putchar_b_buf(18, '-', (void *) Font, Dis_Buff);
        putchar_b_buf(24, 0, (void *) Font, Dis_Buff);
    }
    Update_Matrix(Dis_Buff); // обновити дані на дисплеї
    en_put = 1;

}

//============================================================
// Налаштування - чи можна показувати тиск 
//============================================================

void set_en_bmp(void) {
    switch (events) {
        case MAIN_EVENT:

            break;
        case KEY_OK_EVENT:
            okButSet(set_en_bmp, set_en_dst);
            //            RTOS_DeleteTask(set_en_bmp);
            //            RTOS_SetTask(set_en_dst, 0, cycle_main);
            //            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу 
            //            events = MAIN_EVENT;
            //            en_put = 0;
            //            clear_matrix();
            break;
        case KEY_EXIT_EVENT:
            exitButSet(set_en_bmp);
            //            RTOS_DeleteTask(set_en_bmp);
            //            RTOS_SetTask(time_led, 0, cycle_main);
            //            RTOS_DeleteTask(default_state);
            //            events = MAIN_EVENT;
            //            en_put = 0;
            //            clear_matrix();
            break;
        case KEY_UP_EVENT:
            setting_Val[ENABLE_BMP] = !(setting_Val[ENABLE_BMP]);
            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            events = MAIN_EVENT;
            write_eep(EE_EN_BMP, (uint8_t) setting_Val[ENABLE_BMP]);
            break;
        case KEY_DOWN_EVENT:
            // setting_Val[ENABLE_BMP] = !(setting_Val[ENABLE_BMP]);
            // RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            events = KEY_UP_EVENT;
            //write_eep(EE_EN_BMP, (uint8_t) setting_Val[ENABLE_BMP]);
            break;

    }
    if (en_put) {
        putchar_b_buf(0, STR_BMP[0], (void *) Font, Dis_Buff);
        putchar_b_buf(6, STR_BMP[1], (void *) Font, Dis_Buff);
        putchar_b_buf(12, STR_BMP[2], (void *) Font, Dis_Buff);
        if (setting_Val[ENABLE_BMP])
            putchar_b_buf(18, '+', (void *) Font, Dis_Buff);
        else
            putchar_b_buf(18, '-', (void *) Font, Dis_Buff);
        putchar_b_buf(24, 0, (void *) Font, Dis_Buff);
    }
    Update_Matrix(Dis_Buff); // обновити дані на дисплеї
    en_put = 1;

}


//============================================================
// Налаштування - літній час 
//============================================================

void set_en_dst(void) {
    switch (events) {
        case MAIN_EVENT:

            break;
        case KEY_OK_EVENT:
            okButSet(set_en_dst, set_type_temp);
            //            RTOS_DeleteTask(set_en_dst);
            //            RTOS_SetTask(set_type_temp, 0, cycle_main);
            //            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            //            events = MAIN_EVENT;
            //            en_put = 0;
            //            clear_matrix();
            break;
        case KEY_EXIT_EVENT:
            exitButSet(set_en_dst);
            //            RTOS_DeleteTask(set_en_dst);
            //            RTOS_SetTask(time_led, 0, cycle_main);
            //            RTOS_DeleteTask(default_state);
            //            events = MAIN_EVENT;
            //            en_put = 0;
            //            clear_matrix();
            break;
        case KEY_UP_EVENT:
            setting_Val[ENABLE_DST] = !(setting_Val[ENABLE_DST]);
            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            events = MAIN_EVENT;
            write_eep(EE_EN_DST, (uint8_t) setting_Val[ENABLE_DST]);
            break;
        case KEY_DOWN_EVENT:
            //            setting_Val[ENABLE_DST] = !(setting_Val[ENABLE_DST]);
            //            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            events = KEY_UP_EVENT;
            //            write_eep(EE_EN_DST, (uint8_t) setting_Val[ENABLE_DST]);
            break;

    }
    if (en_put) {
        putchar_b_buf(0, STR_DST[0], (void *) Font, Dis_Buff);
        putchar_b_buf(6, STR_DST[1], (void *) Font, Dis_Buff);
        putchar_b_buf(12, STR_DST[2], (void *) Font, Dis_Buff);
        putchar_b_buf(18, STR_DST[3], (void *) Font, Dis_Buff);
        if (setting_Val[ENABLE_DST])
            putchar_b_buf(24, '+', (void *) Font, Dis_Buff);
        else
            putchar_b_buf(24, '-', (void *) Font, Dis_Buff);
        //        putchar_b_buf(24, 0, &Font, &Dis_Buff);
    }
    Update_Matrix(Dis_Buff); // обновити дані на дисплеї
    en_put = 1;



}

//============================================================
// Налаштування - тип відображення температури з радіодатчика
// Є два типи. 1 - Округлена, 2 - температура з десятими
//============================================================

void set_type_temp(void) {
    switch (events) {
        case MAIN_EVENT:

            break;
        case KEY_OK_EVENT:
            okButSet(set_type_temp, set_en_am2302);
            //            RTOS_DeleteTask(set_type_temp);
            //            RTOS_SetTask(set_en_am2302, 0, cycle_main);
            //            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            //            events = MAIN_EVENT;
            //            en_put = 0;
            //            clear_matrix();
            break;
        case KEY_EXIT_EVENT:
            exitButSet(set_type_temp);
            //            RTOS_DeleteTask(set_type_temp);
            //            RTOS_SetTask(time_led, 0, cycle_main);
            //            RTOS_DeleteTask(default_state);
            //            events = MAIN_EVENT;
            //            en_put = 0;
            //            clear_matrix();
            break;
        case KEY_UP_EVENT:
            setting_Val[TYPE_TEMP]++;
            if (setting_Val[TYPE_TEMP] == 3) setting_Val[TYPE_TEMP] = TYPE_TEMP_1;
            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            events = MAIN_EVENT;
            write_eep(EE_TYPE_TEMP, setting_Val[TYPE_TEMP]);
            break;
        case KEY_DOWN_EVENT:
            setting_Val[TYPE_TEMP]--;
            if (setting_Val[TYPE_TEMP] == 0) setting_Val[TYPE_TEMP] = TYPE_TEMP_2;
            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            events = MAIN_EVENT;
            write_eep(EE_TYPE_TEMP, setting_Val[TYPE_TEMP]);
            break;

    }
    if (en_put) {
        putchar_b_buf(0, STR_TYPE_TEMP[0], (void *) Font, Dis_Buff);
        putchar_b_buf(6, STR_TYPE_TEMP[1], (void *) Font, Dis_Buff);
        putchar_b_buf(12, STR_TYPE_TEMP[2], (void *) Font, Dis_Buff);
        putchar_b_buf(18, STR_TYPE_TEMP[3], (void *) Font, Dis_Buff);
        putchar_b_buf(24, setting_Val[TYPE_TEMP], pFont, Dis_Buff);

    }
    Update_Matrix(Dis_Buff); // оновити дані на дисплеї
    en_put = 1;


}

//============================================================
// Налаштування - показ датчика. Вологість.
//============================================================

void set_en_am2302(void) {
    switch (events) {
        case MAIN_EVENT:

            break;
        case KEY_OK_EVENT:
            okButSet(set_en_am2302, ip_esp);
            RTOS_DeleteTask(default_state);
            //            RTOS_DeleteTask(set_en_am2302);
            //            RTOS_SetTask(ip_esp, 0, 14);
            //            RTOS_DeleteTask(default_state);
            //            events = MAIN_EVENT;
            //            en_put = 0;
            if (!(setting_Val[ENABLE_ESP]))
                memcpy(ip_buf, "WEB Вимкнено", sizeof (ip_buf));
            i_char = 0;
            i_bchar = 0;
            //clear_matrix();
            break;
        case KEY_EXIT_EVENT:
            exitButSet(set_en_am2302);
            //            RTOS_DeleteTask(set_en_am2302);
            //            RTOS_SetTask(time_led, 0, cycle_main);
            //            RTOS_DeleteTask(default_state);
            //            events = MAIN_EVENT;
            //            en_put = 0;
            //            clear_matrix();
            //            if (setting_Val[TYPE_CLK] == 1)
            //                blk_dot = 0;
            //            else
            //                blk_dot = 1;
            break;
        case KEY_UP_EVENT:
            setting_Val[ENABLE_DHT] = !(setting_Val[ENABLE_DHT]);
            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            events = MAIN_EVENT;
            write_eep(EE_EN_AM2302, (uint8_t) setting_Val[ENABLE_DHT]);
            break;
        case KEY_DOWN_EVENT:
            events = KEY_UP_EVENT;
            //            setting_Val[ENABLE_DHT] = !(setting_Val[ENABLE_DHT]);
            //            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            //            events = MAIN_EVENT;
            //            write_eep(EE_EN_AM2302, (uint8_t) setting_Val[ENABLE_DHT]);
            break;

    }
    if (en_put) {
        putchar_b_buf(0, STR_AM2302[0], (void *) Font, Dis_Buff);
        putchar_b_buf(6, STR_AM2302[1], (void *) Font, Dis_Buff);
        putchar_b_buf(12, STR_AM2302[2], (void *) Font, Dis_Buff);
        putchar_b_buf(18, STR_AM2302[3], (void *) Font, Dis_Buff);
        if (setting_Val[ENABLE_DHT])
            putchar_b_buf(24, '+', (void *) Font, Dis_Buff);
        else
            putchar_b_buf(24, '-', (void *) Font, Dis_Buff);

    }
    Update_Matrix(Dis_Buff); // обновити дані на дисплеї
    en_put = 1;


}
//=================================================
// IP адреса WEB інтерфейсу
//=================================================

void ip_esp(void) {

    switch (events) {
        case MAIN_EVENT:

            scroll_text(ip_buf);
            break;
        case KEY_OK_EVENT:
            okButSet(ip_esp, en_esp);
            //            RTOS_DeleteTask(ip_esp);
            //            RTOS_SetTask(en_esp, 0, cycle_main);
            //            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            //            // RTOS_DeleteTask(default_state);
            //            events = MAIN_EVENT;
            //            en_put = 0;
            //            clear_matrix();
            break;
        case KEY_EXIT_EVENT:
            exitButSet(ip_esp);
            //            RTOS_DeleteTask(ip_esp);
            //            RTOS_SetTask(time_led, 0, cycle_main);
            //            // RTOS_DeleteTask(default_state);
            //            events = MAIN_EVENT;
            //            en_put = 0;
            //            clear_matrix();
            //            if (setting_Val[TYPE_CLK] == 1)
            //                blk_dot = 0;
            //            else
            //                blk_dot = 1;
            //            break;
            //        case KEY_UP_EVENT:
            //            break;
            //        case KEY_DOWN_EVENT:
            break;

    }


    Update_Matrix(Dis_Buff); // обновити дані на дисплеї

}

void en_esp(void) {
    switch (events) {
        case MAIN_EVENT:

            break;
        case KEY_OK_EVENT:
            //            RTOS_DeleteTask(en_esp);
            okButSet(en_esp, time_led);
            RTOS_DeleteTask(default_state);
            //            RTOS_SetTask(time_led, 0, cycle_main);
            //            events = MAIN_EVENT;
            //            en_put = 0;
            //            clear_matrix();
            break;
        case KEY_EXIT_EVENT:
            exitButSet(en_esp);

            //            RTOS_DeleteTask(en_esp);
            //            RTOS_SetTask(time_led, 0, cycle_main);
            //            RTOS_DeleteTask(default_state);
            //            events = MAIN_EVENT;
            //            en_put = 0;
            //            clear_matrix();
            //            if (setting_Val[TYPE_CLK] == 1)
            //                blk_dot = 0;
            //            else
            //                blk_dot = 1;
            break;
        case KEY_UP_EVENT:
            events = KEY_DOWN_EVENT;
            break;
        case KEY_DOWN_EVENT:
            setting_Val[ENABLE_ESP] = !(setting_Val[ENABLE_ESP]);
            RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
            events = MAIN_EVENT;
            write_eep(EE_ESP_EN, (uint8_t) setting_Val[ENABLE_ESP]);
            if (setting_Val[ENABLE_ESP]) {
                RCSTAbits.SPEN = 0;
                TRISCbits.RC6 = 0;
                TRISCbits.RC7 = 0;
                PORTCbits.RC7 = 0;
                PORTCbits.RC6 = 0;
                __delay_ms(10);
                EN_ESP8266 = 1;
                __delay_ms(100); //даємо esp8266 час на завантаження
                init_uart();
            } else
                EN_ESP8266 = 0;
            break;

    }
    if (en_put) {
        putchar_b_buf(0, STR_ESP[0], (void *) Font, Dis_Buff);
        putchar_b_buf(6, STR_ESP[1], (void *) Font, Dis_Buff);
        putchar_b_buf(12, STR_ESP[2], (void *) Font, Dis_Buff);
        putchar_b_buf(18, STR_ESP[3], (void *) Font, Dis_Buff);
        if (setting_Val[ENABLE_ESP])
            putchar_b_buf(24, '+', (void *) Font, Dis_Buff);
        else
            putchar_b_buf(24, '-', (void *) Font, Dis_Buff);

    }
    Update_Matrix(Dis_Buff); // оновити дані на дисплеї
    en_put = 1;


}

void default_state(void) {
    events = KEY_EXIT_EVENT;

}

void exitButSet(void (*taskFunc)(void)) {
    RTOS_DeleteTask(taskFunc);
    RTOS_SetTask(time_led, 0, cycle_main);
    RTOS_DeleteTask(default_state);
    events = MAIN_EVENT;
    en_put = 0;
    clear_matrix();
    if (setting_Val[TYPE_CLK] == 1)
        blk_dot = 0;
    else
        blk_dot = 1;
}

void okButSet(void (*DeleteFunc)(void), void (*setFunc)(void)) {
    RTOS_DeleteTask(DeleteFunc);
    RTOS_SetTask(setFunc, 0, cycle_main);
    RTOS_SetTask(default_state, 2000, 0); // 10 секунд для виходу
    events = MAIN_EVENT;
    en_put = 0;
    clear_matrix();
}