  
#ifndef MAX7221_H
#define	MAX7221_H

#include <xc.h> // include processor files - each processor file is guarded.
#include <stdint.h>
//#include "common.h"

#define CS_MAX  LATDbits.LD0
#define CLK_MAX LATDbits.LD1
#define DAT_MAX LATDbits.LD2

#define COUNT_MATRIX 4

#define INTENSITY_R 0x0A //Intensity Register
#define SHUTDOWN_R  0x0C //Shutdown Register 
#define DECODE_R    0x09 //Decode-Mode Register
#define SCAN_R      0x0B //Scan-Limit Register 
#define TEST_R      0x0F //test 

// прототипи функцій
void Send_Byte_7221(uint8_t dat);
void Cmd7221(uint8_t adr, uint8_t val );
void Update_Matrix(uint8_t *buf);
void Init7221(void);


#endif	/* XC_HEADER_TEMPLATE_H */

