 
#ifndef TIMER_H
#define	TIMER_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include "common.h"
#include "init.h"
#include <stdint.h>

#define TMR1Val 5536        // 5мс при частот≥ 48м√ц
#define TMR3Val 61536

void TMRInit(void);        // ≥н≥т. таймера 1
//void TMR1Reload(uint16_t value);
#endif	/* XC_HEADER_TEMPLATE_H */

