
#ifndef TIME_H
#define	TIME_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include "display.h"
#include "common.h"
#include <stdint.h>
#include "settings.h"
#include "time.h"

struct Time_Get;

extern uint8_t day_in_m[];

void dst_time(struct Time_Get *pTime, uint8_t *dst);
uint8_t DayOfWeek (uint8_t day, uint8_t month, uint8_t year);


#endif	/* XC_HEADER_TEMPLATE_H */

