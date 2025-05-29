#include "nrf24l01p.h"

uint8_t pay_len; // довжина посилки
const uint8_t nrf_tx_address[5] = {0x20, 0x01, 0x24, 0x05, 0x05};
const uint8_t nrf_rx_address[5] = {0x20, 0x01, 0x24, 0x05, 0x05};

/**********************************************
 *      Ініціалізація модуля
 *  channel -  канал модуля
 *  pay_length -  довжина пакету  
 *********************************************/

void nrf24_init(uint8_t channel, uint8_t pay_length) {

    SPI_CSN = 1;
    SPI_CE = 0;
    pay_len = pay_length;

    nrf24_tx_address(&nrf_tx_address);
    nrf24_rx_address(&nrf_rx_address);

    nrf24_write_reg(NRF24_RF_CH, channel); // пишемо номер каналу

    nrf24_write_reg(NRF24_RX_PW_P0, 0x00);
    nrf24_write_reg(NRF24_RX_PW_P1, pay_length);
    nrf24_write_reg(NRF24_RX_PW_P2, 0x00);
    nrf24_write_reg(NRF24_RX_PW_P3, 0x00);
    nrf24_write_reg(NRF24_RX_PW_P4, 0x00);
    nrf24_write_reg(NRF24_RX_PW_P5, 0x00);

    // 250 Kbps, потужність: 0dbm
    nrf24_write_reg(NRF24_RF_SETUP, (1 << RF_DR_LOW) | (0 << RF_DR) | ((0x03) << RF_PWR));

    // CRC активувати, довжина CRC 1 байт
    nrf24_write_reg(NRF24_CONFIG, CONFIG_SET);

    // Auto Acknowledgment
    nrf24_write_reg(NRF24_EN_AA, (1 << ENAA_P0) | (1 << ENAA_P1) | (0 << ENAA_P2) | (0 << ENAA_P3) | (0 << ENAA_P4) | (0 << ENAA_P5));

    // Enable RX addresses
    nrf24_write_reg(NRF24_EN_RXADDR, (1 << ERX_P0) | (1 << ERX_P1) | (0 << ERX_P2) | (0 << ERX_P3) | (0 << ERX_P4) | (0 << ERX_P5));

    // затримка автоповтору: 1000 мкс 15 спроб
    nrf24_write_reg(NRF24_SETUP_RETR, (0x0F << ARD) | (0x0F << ARC));

    // Dynamic length configurations: No dynamic length
    nrf24_write_reg(NRF24_DYNPD, (0 << DPL_P0) | (0 << DPL_P1) | (0 << DPL_P2) | (0 << DPL_P3) | (0 << DPL_P4) | (0 << DPL_P5));

    // Start listening
    nrf24_powerUpRx();

}

/*******************************
    Режим прийомника
 ******************************/

void nrf24_powerUpRx() {
    SPI_CSN = 0;
    spi_rw(FLUSH_RX); // очистити прийомний буфер
    SPI_CSN = 1;

    nrf24_write_reg(NRF24_STATUS, (1 << RX_DR) | (1 << TX_DS) | (1 << MAX_RT)); //скинути признаки переривання

    SPI_CE = 0;
    nrf24_write_reg(NRF24_CONFIG, CONFIG_SET | ((1 << PWR_UP) | (1 << PRIM_RX)));
    SPI_CE = 1;
    __delay_us(135);
}

/*******************************
    Режим передачі
 ******************************/
void nrf24_powerUpTx() {
    //nrf24_write_reg(FLUSH_TX, 0); //flush Tx

    nrf24_write_reg(NRF24_STATUS, (1 << RX_DR) | (1 << TX_DS) | (1 << MAX_RT));

    nrf24_write_reg(NRF24_CONFIG, CONFIG_SET | ((1 << PWR_UP) | (0 << PRIM_RX)));
    __delay_us(135);
}

void nrf24_powerDown() {
    SPI_CE = 0;
    nrf24_write_reg(NRF24_CONFIG, CONFIG_SET);
}

/***************************************
 Пишемо дані "value" в регітр "reg"
 ***************************************/
void nrf24_write_reg(uint8_t reg, uint8_t value) {
    SPI_CSN = 0; // CSN low, init SPI transaction
    spi_rw(W_REGISTER | reg); // select register
    spi_rw(value); // ..and write value to it..
    SPI_CSN = 1; // CSN high again
}

/***************************************
 Читаємо дані "value" з регітру "reg"
 ***************************************/
uint8_t nrf24_read_reg(uint8_t reg) {
    uint8_t value;
    SPI_CSN = 0; // CSN low, initialize SPI communication...
    spi_rw(reg); // Select register to read from..
    value = spi_rw(0); // ..then read register value
    SPI_CSN = 1; // CSN high, terminate SPI communication

    return (value); // return register value
}

/***************************************
 Пишемо length даних в регістр
 ***************************************/
void nrf24_write_buf(uint8_t reg, uint8_t *pBuf, uint8_t length) {
    uint8_t i;

    SPI_CSN = 0; // Set CSN low, init SPI tranaction
    spi_rw(reg); // Select register to write to and read status UINT8
    for (i = 0; i < length; i++) // then write all UINT8 in buffer(*pBuf) 
        spi_rw(*pBuf++);
    SPI_CSN = 1; // Set CSN high again      

}

/**************************************************         
    Читає 'length' з репгістру 'reg'         
 **************************************************/
void nrf24_read_buf(uint8_t reg, uint8_t *pBuf, uint8_t length) {
    uint8_t status, i;

    SPI_CSN = 0; // Set CSN l
    status = spi_rw(reg); // Select register to write, and read status UINT8

    for (i = 0; i < length; i++)
        pBuf[i] = spi_rw(0); // Perform SPI_RW to read UINT8 from RFM70 

    SPI_CSN = 1; // Set CSN high again

}

/* Встановлюємо адресу передачика */
void  nrf24_tx_address(uint8_t *adr) {
    //RX_ADDR_PO - повинен бути такий самий, як адреса передатчика
    nrf24_write_buf(W_REGISTER | NRF24_RX_ADDR_P0, adr, NRF24_ADDR_LEN);
    nrf24_write_buf(W_REGISTER | NRF24_TX_ADDR, adr, NRF24_ADDR_LEN);
}

/* Встановлюємо адресу приймача */
void nrf24_rx_address(uint8_t *adr) {

    nrf24_write_buf(W_REGISTER | NRF24_RX_ADDR_P1, adr, NRF24_ADDR_LEN);
}

/************************************
           Передача даних по єфіру
 ***********************************/
void nrf24_send(uint8_t *value) {

    SPI_CE = 0;
    nrf24_powerUpTx();
    SPI_CSN = 0; // очищаємо буфер передачі
    spi_rw(FLUSH_TX); //...........
    SPI_CSN = 1; //

    nrf24_write_buf(W_TX_PAYLOAD, value, pay_len);
   // __delay_ms(25);
    SPI_CE = 1;
}

/******************************
 *    Йде пердача посилки.
 *  Повертає 0, якщо передало або
 * перевищений ліміт ретрансляцій.
 *****************************/
uint8_t nrf24_isSending() {
    uint8_t status;

    // читаємо поточний стан
    status = nrf24_getStatus();

    if ((status & ((1 << TX_DS) | (1 << MAX_RT)))) {
        return 0; /* false */
    }

    return 1; /* true */

}

/********************************
 *  Отримати регістр STATUS
 ********************************/
uint8_t nrf24_getStatus() {
    uint8_t r_status;
    SPI_CSN = 0;
    r_status = spi_rw(NRF_NOP);
    SPI_CSN = 1;
    return r_status;
}

/***************************************
 *   Визначаємо стан переданого пакету
 **************************************/

uint8_t nrf24_messageStatus() {
    uint8_t r_status;

    r_status = nrf24_getStatus();

    // Пакет передано успішно
    if ((r_status & ((1 << TX_DS)))) {
        return NRF24_TRANSMISSON_OK;
    }
        // Перевищено ліміт ретрансляцій???
    else if ((r_status & ((1 << MAX_RT)))) {
        return NRF24_MESSAGE_LOST;
    }        // Можливо ще йде відправка
    else {
        return 0xFF;
    }
}

/* ***************************************
 *    Повертає кількість ретрансляцій
 *      пакету
 ****************************************/
uint8_t nrf24_retransmissionCount() {
    uint8_t r_count;
    r_count = nrf24_read_reg(NRF24_OBSERVE_TX);
    r_count = r_count & 0x0F;
    return r_count;
}

/********************************************
 *    Читаємо дані в масив
 ********************************************/
void nrf24_getData(uint8_t* data) {

    nrf24_read_buf(R_RX_PAYLOAD, data, pay_len);

    // скинути признак переривання
    nrf24_write_reg(NRF24_STATUS, (1 << RX_DR));
}

/* Повертає довжину пакету */
uint8_t nrf24_payloadLength() {
    uint8_t status;
    SPI_CSN = 0;
    spi_rw(R_RX_PL_WID);
    status = spi_rw(0x00);
    SPI_CSN = 1;
    return status;
}

/* Перевіряємо, чи порожній прийомний буфер */
uint8_t nrf24_rxFifoEmpty() {
    uint8_t fifoStatus;

    fifoStatus = nrf24_read_reg(NRF24_FIFO_STATUS);

    return (fifoStatus & (1 << RX_EMPTY));
}


/* Перевіряємо чи доступні дані для читання на прийомі */

/* Повертає 1, якщо доступні */
uint8_t nrf24_dataReady() {
    uint8_t status = nrf24_getStatus();


    if (status & (1 << RX_DR)) {
        return 1;
    }

    return !nrf24_rxFifoEmpty();
    ;
}