#include "spi.h"

void spi_init() {
    MISO_INPUT();
    MOSI_OUTPUT();
    CE_OUTPUT();
    CSN_OUTPUT();
    SCK_OUTPUT();
    IRQ_INPUT();
    SPI_CSN = 1;

}


//=======================================
// Функція передавання приймання даних
// по SPI
// value - байт передачі
// Повертає - прийняті дані
//=======================================

uint8_t spi_rw(uint8_t value) {
    uint8_t i;
    for (i = 0; i < 8; i++) // output 8-bit
    {
        if (value & 0x80) {
            SPI_MOSI = 1;
        } else {
            SPI_MOSI = 0;
        }

        value = (value << 1); // shift next bit into MSB..
        SPI_SCK = 1; // Set SCK high..
        value |= SPI_MISO; // capture current MISO bit
        SPI_SCK = 0; // ..then set SCK low again
    }
    return (value); // return read 
} 