 
#ifndef NEWMAIN_H
#define	NEWMAIN_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>
#include <stdio.h>
#include "max7221.h"
#include "init.h"
#include "interrupt.h"
#include "dispatcher.h"
#include "display.h"
#include "common.h"
#include "ds3231.h"
#include "key.h"
#include "events.h"
#include "onewire.h"
#include "build.h"

#define _XTAL_FREQ  48000000 // чатота контролера 48м√ц


#define VERSION "Clock v3.0.1 by Ivan_fd "
//#define SHOW_VERSION
//#define DEBUG_TEMP
//#define DEBUG   // дл€ в≥дладки, розкоментувати.


#endif	/* XC_HEADER_TEMPLATE_H */

