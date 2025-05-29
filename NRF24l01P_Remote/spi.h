
#ifndef SPI_H
#define	SPI_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>


/* піни контролера для роботи з spi. Налаштовуємо під себе
 SPI_IRQ - це для модуля NRF24L01 */
#define SPI_MISO PORTBbits.RB0  //Прийом
#define SPI_MOSI PORTAbits.RA1   //Передача
#define SPI_CE   PORTAbits.RA2
#define SPI_CSN  PORTBbits.RB3
#define SPI_SCK  PORTBbits.RB4
#define SPI_IRQ  PORTBbits.RB5

#define MISO_INPUT()   TRISBbits.TRISB0 = 1 
#define MOSI_OUTPUT()  TRISAbits.TRISA1 = 0
#define CE_OUTPUT()    TRISAbits.TRISA2 = 0
#define CSN_OUTPUT()   TRISBbits.TRISB3 = 0
#define SCK_OUTPUT()   TRISBbits.TRISB4 = 0
#define IRQ_INPUT()    TRISBbits.TRISB5 = 1

void spi_init();
uint8_t spi_rw(uint8_t value);



#endif	/* XC_HEADER_TEMPLATE_H */

