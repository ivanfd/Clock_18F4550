
#ifndef SPI_H
#define	SPI_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>


/* піни контролера для роботи з spi. Налаштовуємо під себе
 SPI_IRQ - це для модуля NRF24L01 */
#define SPI_MISO PORTDbits.RD3  //Прийом
#define SPI_MOSI LATDbits.LD4   //Передача
#define SPI_CE   LATCbits.LC1
#define SPI_CSN  LATCbits.LC2
#define SPI_SCK  LATDbits.LD5
#define SPI_IRQ  LATDbits.LD6

#define MISO_INPUT()   TRISDbits.RD3 = 1 
#define MOSI_OUTPUT()  TRISDbits.RD4 = 0
#define CE_OUTPUT()    TRISCbits.RC1 = 0
#define CSN_OUTPUT()   TRISCbits.RC2 = 0
#define SCK_OUTPUT()   TRISDbits.RD5 = 0
#define IRQ_INPUT()    TRISDbits.RD6 = 1

void spi_init(void);
uint8_t spi_rw(uint8_t value);



#endif	/* XC_HEADER_TEMPLATE_H */

