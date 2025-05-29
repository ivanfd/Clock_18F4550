#include "usart.h"



void putch(char data)
{
 while( ! TXIF)
 continue;
 TXREG = data;
}


void init_uart(void)
{
    TRISBbits.TRISB1 = 1;
    TRISBbits.TRISB2 = 1;
    TXSTAbits.BRGH = 0; // 1 = High speed
    TXSTAbits.SYNC = 0; // 0 = Asynchronous mode
    TXSTAbits.TXEN = 1; // 1 = Transmit enabled
    TXSTAbits.CSRC = 1;
    RCSTAbits.CREN = 1; // 1 = Enables receiver
    RCSTAbits.SPEN = 1; // 1 = Serial port enabled (configures RX/DT and TX/CK pins as serial port pins)
    
    // BAUD = FOSC/[4 (n + 1)]
    // n = value of SPBRGH:SPBRG register pair
    //  SPBRGH:SPBRG = ((4000000/4800)/64) - 1 = 12
    // BAUDRATE = 4000000/(64*(12 + 1)) = 4808
    // ERROR = 100*(4808 - 4800)/4800 = 0.17%
    
    SPBRG = 12;
}
