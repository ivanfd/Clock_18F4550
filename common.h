
#ifndef COMMON_H
#define	COMMON_H

#include <xc.h> // include processor files - each processor file is guarded. 
#include <stdint.h>
#include <stdio.h>
//#include "max7221.h"
#include "Font.h"
#include "dispatcher.h"
#include "display.h"
#include "interrupt.h"
#include "key.h"
#include "events.h"
#include "newmain.h"
#include "onewire.h"
#include "ds18b20.h"
#include "BMP_280.h"
#include "eusart.h"
#include "strings.h"
#include "settings.h"
#include <stdint.h>
#include <string.h>
#include "time.h"
#include "SI7021.h"
#include "build.h"





#define SetBit(x,y)    do{ x |=  (1 << (y));} while(0)
#define ClrBit(x,y)    do{ x &= ~(1 << (y));} while(0)
#define InvBit(x,y)    do{(x)^=  (1 << (y));} while(0)
#define IsBit(x,y)        (x &   (1 << (y)))

//#define LOW_BYTES(x) ((unsigned char)((x) & 0xFF))

#define TRUE 1
#define FALSE 0

#define HIGH  1
#define LOW   0

#define ON    1
#define OFF   0

#define cycle_main 20       // період виконання задачі, основна
//#define NULL  0

#define EE_FONT 0 // адреса в еепром типу шрифту
#define EE_TYPE_CLK 1 // адреса в еепром типу годинника
#define EE_TYPE_BRG 2 // адреса в еепром типу яскравості
#define EE_DAT_BRG 3 // адреса в еепром значення яскравості
#define EE_EN_SND_H 4 //адреса в еепром, чи можна відтворювати щогодинний сигнал
#define EE_EN_DS1 5 // еепром, чи показувати температуру з датчика 1
#define EE_EN_DS2 6 // еепром, чи показувати температуру з датчика 2
#define EE_EN_BMP 7 // еепром, датчик тиску
#define EE_EN_DST 8 // еепром, літній час (активація переходу на літній час)
#define EE_TYPE_TEMP 9 // еепром, тип показу температури
#define EE_EN_AM2302 10 //  еепром, чи виводити вологість
#define EE_DST_YN 11 // чи відбувся перехід на літній час
#define EE_ESP_EN 12 // ввімкнення/вимкнення ESP8266
#define EE_CENTURY_YEAR 13 // перші цифри року в еепром
#define EE_EN_DATE 14 // чи виводити біг строку з датою

#define TYPE_CLK_1 1  // вигляд годинника
#define TYPE_CLK_2 2  // вигляд годинника
#define MAX_BRIG 10 // максимальна яскравість
#define TYPE_TEMP_1 1 //тип показу температури
#define TYPE_TEMP_2 2 //тип показу температури
#define EN_AM2302 1
#define DIS_AM2302 0
#define TYPE_BRG_AUTO 1
#define TYPE_BRG_MAN 0
#define ENABLE_SND_HR_ON 1
#define ENABLE_SND_HR_OFF 0
#define DS_ON 1
#define DS_OFF 0
#define BMP_OFF 0
#define BMP_ON 1
#define DST_ON 1
#define DST_OFF 0
#define ESP_ON 1
#define ESP_OFF 0
#define DHT_ON 1
#define DHT_OFF 0
#define DATE_ON 1
#define DATE_OFF 0

enum datIdx { // покази датчиків
    T_RADIO, // температура вулиця
    T_HOME, // Температура дім
    PRESS, // атмосферний тиск
    T_PRESS, // Температура датчика тиску
    HUM, // вологість
    T_HUM, // температура з датчика вологості
    ADC_RES, // АЦП
    DS18_ERR, // кількість помилок радіоканалу по прийому
    NUM_VALUES_DAT
};

enum setIdx {
    TYPE_FONT, // тип шрифту
    TYPE_CLK, // тип годинника
    TYPE_BRG, // тип яскравості, ручна чи автоматична
    VAL_BRG, // значення ручної яскравості
    ENABLE_SND_HOUR, // чи пікати кожну годину
    ENABLE_DS_1, // чи показувати кімнатний датчик температури
    ENABLE_DS_2, // чи показувати вуличний датчик температури
    ENABLE_BMP, // чи показувати атмосферний тиск
    ENABLE_DST, // чи переходити на літній час
    TYPE_TEMP, // тип відображення температури
    ENABLE_ESP, // чи включений esp8266
    ENABLE_DHT, // датчик вологості
    ENABLE_DATE, // чи виводити дату
    NUM_VALUES
};

enum fontNum {
    FONT_1 = 1,
    FONT_2 = 2,
    FONT_3 = 3,
    FONT_4 = 4,
    FONT_5 = 5
};

enum brgMan {
    VAL_BRG_NUM_1,
    VAL_BRG_NUM_2,
    VAL_BRG_NUM_3,
    VAL_BRG_NUM_4,
    VAL_BRG_NUM_5,
    VAL_BRG_NUM_6,
    VAL_BRG_NUM_7,
    VAL_BRG_NUM_8
};



extern uint8_t setting_Val[NUM_VALUES];
extern uint8_t blk_dot; // дозвіл на мигання кнопок
//extern int8_t brg_type;// яскравість по датчику, чи постійна
//extern uint8_t brig;// значення яскравості
extern uint8_t dst_flag; // чи зараз літній час????
//extern int8_t en_am2302; //датчик вологості
//extern int8_t en_bmp280; //  чи показуємо тиск
//extern int8_t en_ds_1;    //  чи пок. температуру з датчика 1
//extern int8_t en_ds_2;    //  чи пок. температуру з датчика 2
//extern int8_t en_dst; // перехід на літній час
extern __bit en_put; // чи можна писати у буфер символи
extern uint8_t events;
extern uint8_t idx_pnt;
extern __bit show_digit; // чи показувати цифри, в нал. мигання
//extern uint8_t type_clk; // вигляд годинника
//extern uint8_t type_font;
//extern uint8_t type_temp;
extern volatile uint8_t x1, x2, x3, x4, y1, y2, y3, y4; //Для зсуву стовбця вниз
extern uint8_t ip_buf[16];
//extern uint8_t dst_yn; // чи відбувався перехід на літній час


void INT0_ISR(void);
void GetTime(void);



void TMR1_ISR(void);
void time_led();
void version(void);

void home_temp(void);
void set_font(void);

void pressure(void);
void pre_ref_dis(void);

void radio_temp(void);
void read_adc(void);
void adj_brig(void);

void usart_r();
//void convert_utf(uint8_t *buf);
void hum(void);
void radioRead(void);
void usartOk(void);
void usartEr(void);
void sendDataSensors(void);
void appendNumber(char* buffer, int number);
void buildDateString(uint8_t* dest, uint8_t dayOfWeek, uint8_t day, uint8_t month, int year);
uint8_t crc8(const uint8_t *data, uint8_t len);
void readTemp(void);
void convertTemp(void);
void buildDateStringSafe(uint8_t* txt_buf_date);


#endif	/* XC_HEADER_TEMPLATE_H */

