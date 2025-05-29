#ifndef ONEWIRE_H
#define	ONEWIRE_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>
#include "common.h"


#define TRIS_DQ TRISDbits.RD6
#define DQ PORTDbits.RD6

uint8_t ow_reset(void);
uint8_t read_bit(void);
void write_bit(uint8_t bitval);
uint8_t read_byte(void);
void write_byte(uint8_t val);
//void Waiting_WR (void);

#endif	/* XC_HEADER_TEMPLATE_H */

