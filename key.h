
#ifndef KEY_H
#define	KEY_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>
#include "dispatcher.h"
#include "events.h" 

#define DELAYKEY 15     // debounce
#define DELAYKEY2 300

#define KEY_LONG 1
#define KEY_SHORT 0

#define KEY_PORT    PORTB
//  входи до яких підключені кнопки
#define KEY_OK 6
#define KEY_UP 7
#define KEY_DOWN 3
#define KEY_EXIT 4


//extern uint8_t keyLong;

void key_press(void);
//uint8_t key_GetKey(void);

#endif	/* XC_HEADER_TEMPLATE_H */

