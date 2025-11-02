
#ifndef DISPLAY_H
#define	DISPLAY_H

#include <xc.h> // include processor files - each processor file is guarded. 
#include <stdint.h>
#include "common.h"
#include "dispatcher.h"
#include "ds3231.h"
#include "Font.h"
#include "FontS.h"
#include "pict.h"
#include "digits_f.h"
#include "max7221.h"
#include "time.h"


//#define COUNT_MATRIX 4

#define BUF_SIZE COUNT_MATRIX * 8
#define BUF_SIZE_TEMP BUF_SIZE + 17
#define SPEED_STRING 70     // швидкість бігучої строки, підібрати
#define SPEED_STRING_TEXT 13     // швидкість бігучої строки, підібрати
#define DELAY_SHIFT_DOWN 35 // швидкість ефекту зсуву вниз



extern uint8_t text_buf[]; // буфер для біг строки
extern uint8_t Dis_Buff[]; // буфер дисплея
extern const uint8_t(*pFont)[5];
extern uint8_t rs_text_buf[];
uint8_t i_char, i_bchar; // індекс літери та байт в літері

struct Time_Get // структура для годин
{
    uint8_t Ts; // секунди         
    uint8_t Tmin; // хвилини    
    uint8_t Thr; // години
    uint8_t Tdy; // день
    uint8_t Tdt; // число   
    uint8_t Tmt; // місяць         
    uint8_t Tyr; // рік
    uint8_t TyrC;// перші дві цифри року
} TTime, TSTime;

//typedef struct {
//    uint8_t Ts;   // секунди
//    uint8_t Tmin; // хвилини
//    uint8_t Thr;  // години
//    uint8_t Tdy;  // день тижня
//    uint8_t Tdt;  // число
//    uint8_t Tmt;  // місяць
//    uint8_t Tyr;  // рік
//    uint8_t TyrC; // перші дві цифри року
//} Time_Get;
//
//extern Time_Get TTime, TSTime;



//  конвертовані в десятки і одиниці        
//struct Time_Conv
//{
//   unsigned char s_10;
//   unsigned char s_1;
//   unsigned char min_10;
//   unsigned char min_1;
//   unsigned char hr_10;
//   unsigned char hr_1;
//} TTimeConv ;

typedef void (*p_MyFunc)();

typedef enum { // яка задача буде наступною піля виконання ефекту
    NEXT_NONE,
    NEXT_PRESSURE,
    NEXT_TEMP_HOME,
    NEXT_TEMP_VUL,
    NEXT_HUM
} NextAction;

NextAction nextAfterEffect = NEXT_NONE;

// Перелік усіх можливих ефектів

typedef enum {
    EFFECT_NONE = 0,
    EFFECT_SCROLL_LEFT,
    EFFECT_SCROLL_RIGHT,
    EFFECT_SCROLL_DOWN,
    EFFECT_DISSOLVE,
    EFFECT_HIDE_TWO_SIDE,
    // додати інші за потреби
} EffectType;

// Поточний активний ефект
EffectType currentEffect = EFFECT_NONE;



//void FillBufS(uint8_t *buf, uint8_t edit_Flag, uint8_t scr_flag);       
void FillBuf(uint8_t type);

void putchar_b_buf(uint8_t x, uint8_t symbol, const uint8_t(*pF)[5], uint8_t *buf);
//void putchar_b_buf(uint8_t x, uint8_t symbol, const uint8_t(*pF)[256][5], uint8_t *buf);
//void putchar_s_buf(uint8_t x, uint8_t symbol);
void pixel_on(uint8_t x, uint8_t y, uint8_t *buf);
void pixel_off(uint8_t x, uint8_t y, uint8_t *buf);
void clear_matrix(void);
uint8_t scroll_text(uint8_t *buf);
void putchar_down(uint8_t x, uint8_t symbol, const uint8_t(*pF)[5]);
void putchar_down_s(uint8_t x, uint8_t symbol);
void pic_to_led(uint8_t x, uint8_t pic, uint8_t *buf);
void scroll_left(void);
void dissolve(void);
void interval_scroll_text(uint8_t *buf);
void scroll_right(void);
void hide_two_side(void);
void Rand_ef(void);
void fill_buff_t(uint16_t data);
void center_two_side(void);
void scroll_down_one(void);
void scroll_text_temp(uint8_t pos);
void start_scroll_text(uint8_t *buf);
void task_scroll_text(void);
void start_scroll_left(void);
uint8_t update_scroll_left(void);
void start_hide_two_side(void);
uint8_t update_hide_two_side(void);
void start_scroll_right(void);
uint8_t update_scroll_right(void);
void start_dissolve(void);
uint8_t update_dissolve(void);
void start_scroll_down_one(void);
uint8_t update_scroll_down_one(void);
void task_effect_runner(void);

#endif	/* XC_HEADER_TEMPLATE_H */

