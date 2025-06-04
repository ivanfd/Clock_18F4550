


#include "init.h"

//extern const unsigned char userID[ 8 ] @ 0x200000;
//extern uint8_t type_font;
//extern uint8_t type_clk;
//extern int8_t brg_type;
//extern uint8_t brig;// значення яскравості
//extern uint8_t blk_dot; // дозвіл на мигання кнопок
//extern uint8_t en_h_snd;
//extern int8_t en_ds_1;    //  чи пок. температуру з датчика 1
//extern int8_t en_ds_2;    //  чи пок. температуру з датчика 2
//extern int8_t en_bmp280; //  чи показуємо тиск
//extern int8_t en_dst; // перехід на літній час
//extern uint8_t type_temp;
//extern int8_t en_am2302; //датчик вологості
//extern const unsigned char userID[8] @ 0x200000;
//volatile unsigned char usrID[8];

void SYSTEM_Initialize(void) // ініціалізація контролера
{
    //  uint8_t i;

    // for (i = 0; i <= 7; i++)
    //     usrID[i] = userID[i];


    Port_Init();
    setting_Val[ENABLE_ESP] = read_eep(EE_ESP_EN); // чи вмикати ESP
    EN_ESP8266 = setting_Val[ENABLE_ESP]; //  8266
    __delay_ms(100); //даємо esp8266 час на завантаження
    I2C_Init();
    Init7221();
    TMRInit();
    Interrupt_Init();
    DS3231_init();
    init_ds18b20();
    RTOS_Init();
    init_uart();
    //bmp085Calibration();
    bmp280_Init();
    si7021_init();
    setting_Val[TYPE_FONT] = read_eep(EE_FONT);
    setting_Val[TYPE_CLK] = read_eep(EE_TYPE_CLK);
    if (setting_Val[TYPE_CLK] == 1)
        blk_dot = 0;
    else
        blk_dot = 1;
    setting_Val[TYPE_BRG] = read_eep(EE_TYPE_BRG);
    setting_Val[VAL_BRG] = read_eep(EE_DAT_BRG);
    Cmd7221(INTENSITY_R, setting_Val[VAL_BRG]); //Intensity Register
    set_font();
    sound_init();
    spi_init();
    nrf24_init(100, 5);
    RTOS_SetTask(usart_r, 40, cycle_main); // ЗАДАЧА ОПИТУВАННЯ КОМ ПОРТА
    RTOS_SetTask(GetTime, 80, cycle_main); // Задача зчитування даних з RTC
    setting_Val[ENABLE_SND_HOUR] = read_eep(EE_EN_SND_H);
    setting_Val[ENABLE_DS_1] = read_eep(EE_EN_DS1);
    setting_Val[ENABLE_DS_2] = read_eep(EE_EN_DS2);
    setting_Val[ENABLE_BMP] = read_eep(EE_EN_BMP);
    setting_Val[ENABLE_DST] = read_eep(EE_EN_DST); // перехід на літній час
    setting_Val[TYPE_TEMP] = read_eep(EE_TYPE_TEMP); // тип показу температури

    setting_Val[ENABLE_DHT] = read_eep(EE_EN_AM2302);
    //    if (en_am2302)

    //    else
    //        EN_ESP8266 = 0;
    // dst_yn = read_eep(EE_DST_YN);
    srand(3);
}

void Port_Init(void) // ініціалізація портів
{
    LATC = 0x00;
    TRISC = 0b00111000;
    LATB = 0x00;
    TRISB = 0b11011111;
    INTCON2 &= (~(1 << 7)); // підтягуючі резистори
    UCONbits.USBEN = 0; // вимкнути USB
    LATD = 0x00;
    TRISD = 0x00;
    LATA = 0;
    TRISA = 0b00000001;
    TRISE = 0;
    PORTE = 0;
    LATE = 0;
    CMCON = 0x07;

    ADCON0bits.CHS = 0b0000; // аналоговий вхід - 0
    ADCON1bits.PCFG = 0b1110; // порт RA0 - аналоговий
    ADCON1bits.VCFG = 0b00; // Voltage Reference - до VSS, VDD
    ADCON2bits.ADCS = 0b110; //FOSC/64
    ADCON2bits.ACQT = 0b010; // 4 Tad 
    ADCON2bits.ADFM = 1; // праве вирівнювання
    ADCON0bits.ADON = 1; // вмк. модуль АЦП

    INTCONbits.INT0IE = 0;
    INTCON3bits.INT1IE = 0;
    INTCON3bits.INT2IE = 1; // переривання по входу RB2
    INTCON3bits.INT2IF = 0; //скинути признак переривання

    DQ = 1;
}

void Interrupt_Init(void) // ініт переривання
{
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
}