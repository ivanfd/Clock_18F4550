#include "sound.h"


uint8_t count_sound = 0; // тривалість сигналу клавіш

uint8_t play_sound = 0; //  чи можна програвати

//для щогодинного сигналу
uint8_t cnt_snd_h = 0;
uint8_t cnt_b = 0;
uint16_t cnt_p = 0;
//uint8_t en_h_snd = 0; // якщо можна генерувати сигнал
uint8_t h_snd_t = 0; //година співпадає з дозволом
uint8_t snd_flag = 1; // один раз відтворювати

void sound_init(void) {
    SOUND_TRIS &= ~(1 << SOUND_PIN); // робимо вивід виходом
    SOUND_PORT &= ~(1 << SOUND_PIN); //x &= ~(1 << (y))  |= (1<<SOUND_PIN)
}

void TMR3_ISR(void) {

#ifndef ACTIVE_BUZZER  
    if (play_sound) {
        count_sound++;


        if (count_sound < SOUND_DELAY)
            SOUND_PORT ^= (1 << SOUND_PIN);
        else {
            count_sound = 0;
            play_sound = 0;
            SOUND_PORT &= ~(1 << SOUND_PIN);
        }
    }
    //--------------------------------------------------------

    if (setting_Val[ENABLE_SND_HOUR]) { // якщо можна генерувати щогодинний сигнал
        if (h_snd_t) { // якщо година співпала
            snd_flag = 0; // заборонити ще раз генерувати
            if (cnt_b == 0) {//генеруємо перший раз
                cnt_snd_h++;
                if (cnt_snd_h < SOUND_DELAY_H)
                    SOUND_PORT ^= (1 << SOUND_PIN);
                else {
                    cnt_snd_h = 0;
                    cnt_b = 1;
                    SOUND_PORT &= ~(1 << SOUND_PIN);
                }
            }
            if (cnt_b == 1)
                cnt_p++; // збільшуємо паузу паузи
            if (cnt_p > 300) {
                cnt_p = 0;
                cnt_b = 2;
            }
            if (cnt_b == 2) {//генеруємо другий раз
                cnt_snd_h++;
                if (cnt_snd_h < SOUND_DELAY_H)
                    SOUND_PORT ^= (1 << SOUND_PIN);
                else {
                    cnt_snd_h = 0;
                    cnt_b = 0;
                    h_snd_t = 0;
                    SOUND_PORT &= ~(1 << SOUND_PIN);
                }
            }
        }
    }
#else
    if (play_sound) { // якщо дозволено пікати при натисканні кнопок
        count_sound++; // збільшуємо лічильник


        if (count_sound < SOUND_DELAY)
            SetBit(SOUND_PORT,SOUND_PIN);
        else {
            count_sound = 0;
            play_sound = 0;
            ClrBit(SOUND_PORT,SOUND_PIN);
        }
    }
    
    //--------------------------------------------------------

    if (setting_Val[ENABLE_SND_HOUR]) { // якщо можна генерувати щогодинний сигнал
        if (h_snd_t) { // якщо година співпала
            snd_flag = 0; // заборонити ще раз генерувати
            if (cnt_b == 0) {//генеруємо перший раз
                cnt_snd_h++;
                if (cnt_snd_h < SOUND_DELAY_H)
                    SetBit(SOUND_PORT,SOUND_PIN);
                else {
                    cnt_snd_h = 0;
                    cnt_b = 1;
                    ClrBit(SOUND_PORT,SOUND_PIN);
                }
            }
            if (cnt_b == 1)
                cnt_p++; // збільшуємо паузу паузи
            if (cnt_p > 300) {
                cnt_p = 0;
                cnt_b = 2;
            }
            if (cnt_b == 2) {//генеруємо другий раз
                cnt_snd_h++;
                if (cnt_snd_h < SOUND_DELAY_H)
                    SetBit(SOUND_PORT,SOUND_PIN);
                else {
                    cnt_snd_h = 0;
                    cnt_b = 0;
                    h_snd_t = 0;
                    ClrBit(SOUND_PORT,SOUND_PIN);
                }
            }
        }
    }    

#endif
    //       if (count_sound <= 127)    (x)^=  (1 << (y))
    //        SOUND_PORT |= (1<<SOUND_PIN);
    //    else
    //        SOUND_PORT &= ~(1<<SOUND_PIN);

}
