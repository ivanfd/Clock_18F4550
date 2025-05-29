
#ifndef INTERRUPT_H
#define	INTERRUPT_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include "common.h"
#include "dispatcher.h"
#include "display.h"
#include "timer.h"
#include "sound.h"
#include "eusart.h"
#include "max7221.h"

#define INTERRUPT_GlobalInterruptEnable() (INTCONbits.GIE = 1)
#define INTERRUPT_GlobalInterruptDisable() (INTCONbits.GIE = 0)
#define INTERRUPT_PeripheralInterruptEnable() (INTCONbits.PEIE = 1)
#define INTERRUPT_PeripheralInterruptDisable() (INTCONbits.PEIE = 0)

//void interrupt INTERRUPT_InterruptManager(void);
void __interrupt() INTERRUPT_InterruptManager(void);


#endif	/* XC_HEADER_TEMPLATE_H */

