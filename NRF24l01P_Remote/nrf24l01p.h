
#ifndef NRF24L01P_H
#define	NRF24L01P_H

/* Опис пінів підключення до модуля знаходиться в файлі spi.h!!!!!!*/

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>
#include "spi.h"
#include "main.h"

//

#define CONFIG_SET    ((1 << EN_CRC) | (0 << CRCO))
#define NRF24_ADDR_LEN 5
//#define PAY_LEN 2

#define NRF24_TRANSMISSON_OK 0
#define NRF24_MESSAGE_LOST   1


// команди датчика
#define R_REGISTER          0b00000000  //Команда читання з датчика. Біти 0-4 - адреса регістра.
#define W_REGISTER          0b00100000  //Команда запису в датчик. Біти 0-4 - адреса регістра.
#define R_RX_PAYLOAD        0b01100001  //Читаємо з RX-Payload: 1- 32 біти. 
#define W_TX_PAYLOAD        0b10100000  //Пишемо в TX-Payload: 1- 32 біти.
#define FLUSH_TX            0b11100001  //*Очистка TX FIFO, використовується в режимі TX.
#define FLUSH_RX            0b11100010  //*Очистка RX FIFO, використовується в режимі RX.
#define REUSE_TX_PL         0b11100011  //Повторне використання останньої передачі.
#define R_RX_PL_WID         0b01100000  //Прочитати ширину пакета.
#define W_ACK_PAYLOAD       0b10101000  //
#define W_TX_PAYLOAD_NOACK  0b10110000  //
#define NRF_NOP                 0b11111111  //

// регістри модуля

// регістр конфігурації
#define NRF24_CONFIG 0x00       // адреса регістру    
#define MASK_RX_DR  6           // ввімкнути\вимкнути переривання від RX_DR. 0 - on; 1 - 1 off.
#define MASK_TX_DS  5           // ввімкнути\вимкнути переривання від TX_DS. 0 - on; 1 - 1 off.
#define MASK_MAX_RT 4           // ввімкнути\вимкнути переривання від MAX_RT. 0 - on; 1 - 1 off.
#define EN_CRC      3           // CRC. Вмкню, якщо ввімкнено хоч один з бітів EN_AA
#define CRCO        2           // Режим CRC. 0 - 1 байт; 1 - 2 байти.
#define PWR_UP      1           // 1 - POWER_UP; 0 - POWER_DOWN;
#define PRIM_RX     0           // 1 - прийом; 0 - передача.

#define NRF24_EN_AA       0x01  // регістр вмкн. автопідтвердження
/* enable auto acknowledgment */
#define ENAA_P5     5
#define ENAA_P4     4
#define ENAA_P3     3
#define ENAA_P2     2
#define ENAA_P1     1
#define ENAA_P0     0

#define NRF24_EN_RXADDR   0x02  // вмкн. адреси RX
/* enable rx addresses */
#define ERX_P5      5
#define ERX_P4      4
#define ERX_P3      3
#define ERX_P2      2
#define ERX_P1      1
#define ERX_P0      0

#define NRF24_SETUP_AW    0x03  // ширина адреси. 01 - 3 байти; 10 - 4 байти; 11 - 5 байт.
#define NRF24_SETUP_RETR  0x04  // Налаштування автоматичної повторної передачі
/* setup of auto re-transmission */
#define ARD         4 /* 4 bits */
#define ARC         0 /* 4 bits */

#define NRF24_RF_CH       0x05  // Встановлює частотний канал 
#define NRF24_RF_SETUP    0x06  // Регістр налаштувань RF
/* RF setup register */
#define RF_DR_LOW   5
#define PLL_LOCK    4
#define RF_DR       3
#define RF_PWR      1 /* 2 bits */ 



#define NRF24_STATUS      0x07  // Регістр стану модуля. Дані з цього регістру завжди видаються по SPI після першого записаного байту.
#define RX_DR             6     // Відбулося переривання по прийому. Для скидання записати 1.
#define TX_DS             5     // Перивання відбулося по відправлених даних TX FIFO. Встановлюэться, коли пакет, переданий. Якщо AUTO_ACK активовано, цей біт встановлюється лише тоді, коли є ACK отримано.
#define MAX_RT            4     // Переривання максимальної кількісті повторів передачі. Записати 1, щоб очистити біт. Якщо заявлено MAX_RT, має бути очищеним для подальшого спілкування.
#define RX_P_NO_2         3
#define RX_P_NO_1         2
#define RX_P_NO_0         1
#define TX_FULL           0     // стан переповнення TX FIFO

#define NRF24_OBSERVE_TX  0x08
#define NRF24_RPD         0x09
#define NRF24_RX_ADDR_P0  0x0A  // Адреса прийомника 0. Довжина 5 байт максимум. Визначається в рег. SETUP_AW.___
// По замовчуванню 0xE7E7E7E7E7
#define NRF24_RX_ADDR_P1  0x0B  // Адреса прийомника 1. Довжина 5 байт максимум. Визначається в рег. SETUP_AW.___
// По замовчуванню 0xC2C2C2C2C2
#define NRF24_RX_ADDR_P2  0x0C  // Адреса прийомника 2. Довжина 5 байт максимум. Визначається в рег. SETUP_AW.___
// Тільки молодший байт. Всі інші такі як в P1. По замовчуванню 0xC3
#define NRF24_RX_ADDR_P3  0x0D  // Адреса прийомника 2. Довжина 5 байт максимум. Визначається в рег. SETUP_AW.___
// Тільки молодший байт. Всі інші такі як в P1. По замовчуванню 0xC4
#define NRF24_RX_ADDR_P4  0x0E  // Адреса прийомника 2. Довжина 5 байт максимум. Визначається в рег. SETUP_AW.___
// Тільки молодший байт. Всі інші такі як в P1. По замовчуванню 0xC5
#define NRF24_RX_ADDR_P5  0x0F  // Адреса прийомника 2. Довжина 5 байт максимум. Визначається в рег. SETUP_AW.___
// Тільки молодший байт. Всі інші такі як в P1. По замовчуванню 0xC6

#define NRF24_TX_ADDR     0x10  // Адреса передатчика. По замовчуванню 0xE7E7E7E7E7

#define NRF24_RX_PW_P0  0x11    // Кількість байт поля даних. Від 1 до 32.
#define NRF24_RX_PW_P1  0x12    // Кількість байт поля даних. Від 1 до 32.
#define NRF24_RX_PW_P2  0x13    // Кількість байт поля даних. Від 1 до 32.
#define NRF24_RX_PW_P3  0x14    // Кількість байт поля даних. Від 1 до 32.
#define NRF24_RX_PW_P4  0x15    // Кількість байт поля даних. Від 1 до 32.
#define NRF24_RX_PW_P5  0x16    // Кількість байт поля даних. Від 1 до 32.

#define NRF24_FIFO_STATUS  0x17 // Регістр стану FIFO
#define TX_REUSE            6
#define TX_FULL_FIFO        5
#define TX_EMPTY            4
#define RX_FULL             1
#define RX_EMPTY            0

#define NRF24_DYNPD  0x1C    // Динамічна довжина даних. Потрібно встановити EN_DPL та ENAA_Px
/* dynamic length */
#define DPL_P0      0
#define DPL_P1      1
#define DPL_P2      2
#define DPL_P3      3
#define DPL_P4      4
#define DPL_P5      5
#define NRF24_FEATURE  0x1D     // 59 сторінка даташиту


//#define NRF24_TX_ADDRESS 0x1979270904
//#define NRF24_RX_ADDRESS NRF24_TX_ADDRESS



void nrf24_init(uint8_t channel, uint8_t pay_length);
void nrf24_write_reg(uint8_t reg, uint8_t value);
uint8_t nrf24_read_reg(uint8_t reg);
void nrf24_powerUpRx();
void nrf24_powerUpTx();
void nrf24_powerDown();
void nrf24_write_buf(uint8_t reg, uint8_t *pBuf, uint8_t length);
void nrf24_read_buf(uint8_t reg, uint8_t *pBuf, uint8_t length);
void nrf24_tx_address(uint8_t *adr);
void nrf24_rx_address(uint8_t *adr);
void nrf24_send(uint8_t *value);
uint8_t nrf24_isSending();
uint8_t nrf24_getStatus();
uint8_t nrf24_messageStatus();
uint8_t nrf24_retransmissionCount();
void nrf24_getData(uint8_t* data);
uint8_t nrf24_payloadLength();
uint8_t nrf24_rxFifoEmpty();
uint8_t nrf24_dataReady();


#endif	/* XC_HEADER_TEMPLATE_H */

