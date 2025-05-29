#include "timer.h" 

//volatile uint16_t RelTMR = 16536;

void TMRInit(void)
{
  T1CONbits.TMR1ON	 = 0;
  TMR1H = HIGH_BYTE(TMR1Val);
  //TMR1L = (uint8_t)TMR1Val;
  TMR1L = LOW_BYTE(TMR1Val); 
  PIE1bits.TMR1IE = 1; // переривання від таймера 1
  
  T3CONbits.TMR3CS = 0;
  T3CONbits.TMR3ON = 1; // вмк. таймер 3
  PIE2bits.TMR3IE = 1;  // переривання від таймера 3
  TMR3H = HIGH_BYTE(TMR3Val);
  //TMR3L = (uint8_t)TMR3Val;
  TMR3L = LOW_BYTE(TMR3Val);
  //TMR3L = LOW_BYTE(61536u);
}


//void TMR1Reload(uint16_t value)
//{
//       TMR1H = (value >> 8);
//       TMR1L = value; 
//}

