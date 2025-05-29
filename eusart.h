
#ifndef EUSART_H
#define	EUSART_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdio.h>
#include <stdint.h>
#include "common.h"
#include "dispatcher.h"
#include "max7221.h"

/**
  Section: Macro Declarations
*/
#define EUSART_TX_BUFFER_SIZE 16
#define EUSART_RX_BUFFER_SIZE 150

#define EUSART_DataReady  (eusartRxCount)

extern volatile uint8_t eusartRxCount;

void init_uart(void);
void putch(char data);
void EUSART_Receive_ISR(void);
uint8_t EUSART_Read(void);
void EUSART_Write(uint8_t txData);
void reinit_rx(void);


#endif	/* XC_HEADER_TEMPLATE_H */

