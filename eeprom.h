#ifndef EEPROM_H
#define	EEPROM_H

#include <xc.h> // include processor files - each processor file is guarded.  


void write_eep( uint8_t address, uint8_t data );
unsigned char read_eep( uint8_t address );


#endif	/* XC_HEADER_TEMPLATE_H */

