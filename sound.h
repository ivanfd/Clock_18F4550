
#ifndef SOUND_H
#define	SOUND_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>
#include "display.h"

#define SOUND_PORT LATD
#define SOUND_TRIS TRISD
#define SOUND_PIN 7

#define SOUND_DELAY 30
#define SOUND_DELAY_H 20

#define ACTIVE_BUZZER  // якщо використовуємо активний бузер, якщо ні то закоментувати


//extern uint8_t en_h_snd;
extern uint8_t h_snd_t; //година співпадає з дозволом
extern uint8_t play_sound; //  чи можна програвати
extern uint8_t snd_flag; // один раз відтворювати


void sound_init(void);

void TMR3_ISR(void);

#endif	/* XC_HEADER_TEMPLATE_H */

