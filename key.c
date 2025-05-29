#include "key.h"
//uint8_t key_event = NULL;
//extern uint8_t events;
//extern uint8_t play_sound; //  чи можна програвати
volatile uint8_t key_pressed;

//uint8_t keyLong = KEY_SHORT;

void key_press(void) {
    static uint16_t count = 0;
    uint8_t key;

    if ((KEY_PORT & (1 << KEY_OK)) == 0)
        key = KEY_OK_EVENT;
    else if ((KEY_PORT & (1 << KEY_UP)) == 0)
        key = KEY_UP_EVENT;
    else if ((KEY_PORT & (1 << KEY_DOWN)) == 0)
        key = KEY_DOWN_EVENT;
    else if ((KEY_PORT & (1 << KEY_EXIT)) == 0)
        key = KEY_EXIT_EVENT;
    else
        key = 0;


    if (key) {// якщо перемінна key не нульова
        if (count > DELAYKEY2) {
            count = DELAYKEY2 - 20;
            //key_pressed = key;
            events = key;
         //   keyLong = KEY_LONG;
            play_sound = 1;
            return;
        } else count++;

        if (count == DELAYKEY) {
            //key_pressed = key;
            events = key;
         //   keyLong = KEY_SHORT;
            play_sound = 1;
            return;
        }
    } else count = 0;
}



////Отримати код кнопки
//
//uint8_t key_GetKey(void) {
//    uint8_t key = key_pressed;
//
//    key_pressed = NULL;
//    return key;
//}