/* Проект годинника на матричних індикаторах.
 * На драйверах MAX7221
 * Процесор: PIC18F4550
 * Тактова частота 48Мгц
 * Контролер годинника: DS3231 
 * Author: Ivan_fd
 * Version: 3.0.1 
 * Nov 02 2018
 * 10:33:19
 */

#include "newmain.h"


//extern uint8_t TxtBuf[6];          // буфер дл¤ цифр
//extern uint8_t Dis_Buff[BUF_SIZE]; // буфер дисплея
//extern uint8_t blk_dot; // дозвіл на мигання крапок
//int8_t tst1 = -125, tst2 = 93;
//int16_t tstr;

void main(void) {

    __delay_ms(10);

    SYSTEM_Initialize(); // ініціалізація контролера
    //    tstr = (tst1 << 8) | tst2;
    //    printf("AC1: %d\n\r", tstr);
    //    printf("AC1: %d\n\r", tst1);
    //    printf("AC1: %d\n\r", tst2);
    clear_matrix();
#ifdef SHOW_VERSION
    blk_dot = 0;
    version();
    blk_dot = 1;
#endif

    INTERRUPT_GlobalInterruptDisable();
    RTOS_SetTask(time_led, 100, cycle_main); // додаємо задачу, запуск кожних 100мс, затримка перед запуском 500ms
    RTOS_SetTask(radioRead, 150, 200); // додаємо задачу, запуск кожних 20мс, затримка перед запуском 750ms
    RTOS_SetTask(key_press, 0, 1); // опитування кнопок кожні 5 мс.
    //RTOS_SetTask(sendDataSensors, 4000, 2000); // відсилання показів датчиків на web server, кожні 20 сек, затримка 20 сек
        RTOS_SetTask(readTemp, 1000, 10000); // вимірювання температури 1 раз в хвилину  
    INTERRUPT_GlobalInterruptEnable();

    while (1) {
        RTOS_DispatchTask();

    }
    return;
}
