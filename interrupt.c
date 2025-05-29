#include "interrupt.h"


//void interrupt INTERRUPT_InterruptManager (void)
void __interrupt() INTERRUPT_InterruptManager(void)
{
    // interrupt handler
    if(INTCONbits.TMR0IE == 1 && INTCONbits.TMR0IF == 1) // таймер 0
    {
        TMR0_ISR();
    }
    else if (INTCON3bits.INT2IE == 1 && INTCON3bits.INT2IF == 1) // по входу RB2
    {
        INTCON3bits.INT2IF = 0;      // скидаємо признак переривання
        INTCON2bits.INTEDG2 = ~INTCON2bits.INTEDG2; // переводимо в переривання по задньому фронту
                                                    // щоб переривання було кожних 500мс
       INT0_ISR();
    } else if (PIE1bits.TMR1IE == 1 && PIR1bits.TMR1IF ==1) // від таймера 1
    {
        PIR1bits.TMR1IF = 0; // скинути признак переривання від таймера 1
         TMR1H = HIGH_BYTE(TMR1Val);
         TMR1L = LOW_BYTE(TMR1Val);
         //TMR1L = (uint8_t)TMR1Val;
        TMR1_ISR();
        
    } else if (PIE2bits.TMR3IE == 1 && PIR2bits.TMR3IF ==1) {
        PIR2bits.TMR3IF = 0;
        TMR3H = HIGH_BYTE(TMR3Val);
        TMR3L = LOW_BYTE((unsigned int)TMR3Val);
        //TMR3L = (uint8_t)TMR3Val;

        TMR3_ISR();
    } else if (INTCONbits.PEIE == 1 && PIE1bits.RCIE == 1 && PIR1bits.RCIF == 1) {
        EUSART_Receive_ISR();
    }
}
