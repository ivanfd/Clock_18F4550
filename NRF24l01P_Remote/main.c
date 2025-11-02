/*
 *  File:   main.c
 *  Author: Ivan_fd
 *  Передавач по радіо каналу температури    
 *  Використовується модуль NRF24L01+
 *  Канал 123, довжина пакету 5 байт
 *  0 - знак температури + чи -
 *  1,2 - МОЛОДШИЙ І СТАРШИЙ БАЙТ ТЕМПЕРАТУРИ   
 *  3,4 - МОЛОДШИЙ І СТАРШИЙ БАЙТ ПОМИЛОК
 * 
 *  Остання модифікація 02.11.2025
 */


#include <xc.h>
#include "main.h"

uint8_t data_array[5];
uint8_t temp;

uint16_t temperature;
uint16_t error_send = 0; // будемо рахувати і передавати на сервер кількість помилок
uint8_t minus;
uint8_t count = 0; // скільки сидимо в сні

#define NRF_CHANNEL 123
#define NRF_LEN 5

void main(void) {
    uint8_t temp_flag; // чи присутній датчик
    init_Cpu();
    data_array[0] = 0xFF;
    data_array[1] = 0xFF;
    data_array[2] = 0xFF;
    data_array[3] = 0x00;
    data_array[4] = 0x00;
    while (1) {
        CLRWDT();
        //  printf("> ....\r\n");
        data_array[3] = error_send & 0xFF;
        data_array[4] = (uint8_t) (error_send >> 8);
        if (readTemp_Single(&temperature, &minus)) {
            data_array[0] = minus;
            data_array[1] = temperature & 0xFF;
            data_array[2] = (uint8_t) (temperature >> 8);
        } else {
            //temperature = 0xFF;
            data_array[0] = 0xFF;
            data_array[1] = 0xFF;
            data_array[2] = 0xFF;
        }
        CLRWDT();

        //  Розбудити NRF24 перед відправкою
        spi_init(); //  Ініціалізація SPI (після сну лінії неактивні)
        nrf24_init(NRF_CHANNEL, NRF_LEN); //  Повна ініціалізація радіо (канал 100, адреса 5)
        __delay_ms(5); //  Дати NRF24 стабілізуватися після power-up
        
//        nrf24_send(data_array);
//
//        while (nrf24_isSending()); // чекаємо поки передасть
//
//        /* Make analysis on last tranmission attempt */
//        temp = nrf24_messageStatus();

        if (reliable_send(data_array)) { // якщо передало
            LED = 0;
            __delay_ms(8); // то мигнемо раз діодом
            LED = 1;
            //            printf("> Tranmission went OK\r\n");
        } else if (temp == NRF24_MESSAGE_LOST) { // якщо не передало
            LED = 0;
            __delay_ms(2);  // то мигнемо два рази
            LED = 1;
            __delay_ms(150);
            LED = 0;
            __delay_ms(2);
            LED = 1;
            __delay_ms(20);
            //            printf("> Message is lost ...\r\n");
            error_send ++; // додаємо 1 до помилки передач
        }

        /* Retranmission count indicates the tranmission quality */
        // temp = nrf24_retransmissionCount();
        //     //   printf("> Retranmission count: %d\r\n", temp);

        /* Optionally, go back to RX mode ... */
        ////nrf24_powerUpRx();

        /* Or you might want to power down after TX */
        //        nrf24_powerUpRx();
        //        __delay_ms(300);

        nrf24_powerDown();

        LED = 1;
        TRISB = 0;
        PORTB = 0;
        TRISA = 0b00001000;
        PORTA = 0b00000001;
       
        count = 0;
        do {
            SLEEP();
            count++;
        } while (count < 10);

//loop:
//        SLEEP();
//        count++;
//        if (count < 10) goto loop;
//        count = 0;
//        /* Wait a little ... */
//        //_delay_ms(10);
//        spi_init();

    }
    return;
}

void init_Cpu(void) {
    PORTA = 0b00000001;
    CMCON = 0x07;
    TRISA = 0;
    OPTION_REGbits.PSA = 1; // дільник перед песиком
    OPTION_REGbits.PS0 = 1;
    OPTION_REGbits.PS1 = 1;
    OPTION_REGbits.PS2 = 1;

    DQ = 1;

    spi_init();
    init_ds18b20();
    nrf24_init(NRF_CHANNEL, NRF_LEN);
    init_uart();
#ifdef DEBUG
    printf("-USART READY- \n\r");
#endif
}

//==================================================
//  передаємо пакет до 3-х раз, якщо невдало
//==================================================

uint8_t reliable_send(uint8_t *data) {
    for (uint8_t i = 0; i < 3; i++) {
        nrf24_send(data);
        while (nrf24_isSending()); // чекаємо поки передасть
        temp = nrf24_messageStatus();
        if (temp == NRF24_TRANSMISSON_OK)
            return 1;
        else if (temp == NRF24_MESSAGE_LOST)
        __delay_ms(100);
    }
    return 0;
}