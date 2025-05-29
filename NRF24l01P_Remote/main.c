/*
 * File:   main.c
 * Author: Ivan_fd
 *
 * 
 */


#include <xc.h>
#include "main.h"

uint8_t data_array[4];
uint8_t temp;

uint16_t temperature;
uint8_t minus;
uint8_t count = 0; // скільки сидимо в сні

void main(void) {
    uint8_t temp_flag; // чи присутній датчик
    init_Cpu();
    data_array[0] = 0x75;
    data_array[1] = 0xDC;
    data_array[2] = 0x19;
    data_array[3] = 0x79;
    while (1) {
        CLRWDT();
        //  printf("> ....\r\n");
        if (readTemp_Single(&temperature, &minus)) {
            //data_array[0] = temperature; // пишемо в буфер температуру
            data_array[0] = minus;
            data_array[1] = temperature & 0xFF;
            data_array[2] = (uint8_t) (temperature >> 8);
        } else {
            //temperature = 0xFF;
            data_array[0] = 0xFF;
            data_array[1] = 0xFF;
            data_array[2] = 0xFF;
        }
        // data_array[2] = 0x19;
        data_array[3] = 0x79;
        //LED = 0; // засвітити світлодіод
        CLRWDT();
        nrf24_send(&data_array);

        while (nrf24_isSending()); // чекаємо поки передасть

        /* Make analysis on last tranmission attempt */
        temp = nrf24_messageStatus();

        if (temp == NRF24_TRANSMISSON_OK) {
            LED = 0;
            __delay_ms(8);
            LED = 1;
            //            printf("> Tranmission went OK\r\n");
        } else if (temp == NRF24_MESSAGE_LOST) {
            LED = 0;
            __delay_ms(2);
            LED = 1;
            __delay_ms(150);
            LED = 0;
            __delay_ms(2);
            LED = 1;
            __delay_ms(20);
            //            printf("> Message is lost ...\r\n");
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
loop:
        SLEEP();
        count++;
        if (count < 15) goto loop;
        count = 0;
        /* Wait a little ... */
        //_delay_ms(10);
        spi_init();
        //        CLRWDT();
        //        __delay_ms(250);
        //        CLRWDT();
        //        __delay_ms(250);
        //        CLRWDT();
        //        __delay_ms(250);
        //        CLRWDT();
        //        __delay_ms(250);
        //        CLRWDT();
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
    nrf24_init(100, 4);
    init_uart();
#ifdef DEBUG
    printf("-USART READY- \n\r");
#endif
}