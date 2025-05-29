#include "eusart.h"



/**
  Section: Global Variables
 */

static uint8_t eusartTxHead = 0;
//static uint8_t eusartTxTail = 0;
static uint8_t eusartTxBuffer[EUSART_TX_BUFFER_SIZE];
volatile uint8_t eusartTxBufferRemaining;

static uint8_t eusartRxHead = 0;
static uint8_t eusartRxTail = 0;
static uint8_t eusartRxBuffer[EUSART_RX_BUFFER_SIZE];
volatile uint8_t eusartRxCount;

void putch(char data) {
    while (!TXIF)
        continue;
    TXREG = data;
}

void init_uart(void) {
    TRISCbits.RC6 = 1;
    TRISCbits.RC7 = 1;
    TXSTAbits.SYNC = 0; // 0 = Asynchronous mode
    TXSTAbits.TXEN = 1; // 1 = Transmit enabled
    RCSTAbits.CREN = 1; // 1 = Enables receiver
    RCSTAbits.SPEN = 1; // 1 = Serial port enabled (configures RX/DT and TX/CK pins as serial port pins)

#ifdef __18F4550__
    TXSTAbits.BRGH = 1; // 1 = High speed
    BAUDCONbits.BRG16 = 1; // 1 = 16-bit Baud Rate Generator Ц SPBRGH and SPBRG
//    SPBRGH = 0x04;
//    SPBRG = 0xE1;
    SPBRGH = 0x00;
    SPBRG = 0x67;  // 115200
#endif

#ifdef __18F452__
    TXSTAbits.BRGH = 0; // 1 = Low speed
    SPBRG = 64;
#endif

    // BAUD = FOSC/[4 (n + 1)]
    // n = value of SPBRGH:SPBRG register pair
    //  SPBRGH:SPBRG = ((48 000 000/9600)/4) - 1 = 1249
    // BAUDRATE = 48 000 000/(4*(1249 + 1)) = 9600
    // ERROR = (9600 - 9600)/9600 = 0%



    // initializing the driver state
    eusartTxHead = 0;
    // eusartTxTail = 0;
    eusartTxBufferRemaining = sizeof (eusartTxBuffer);

    eusartRxHead = 0;
    eusartRxTail = 0;
    eusartRxCount = 0;

    // enable receive interrupt
    PIE1bits.RCIE = 1;
}

uint8_t EUSART_Read(void) {
    uint8_t readValue = 0;

    while (0 == eusartRxCount) {
    }

    readValue = eusartRxBuffer[eusartRxTail++];
    if (sizeof (eusartRxBuffer) <= eusartRxTail) {
        eusartRxTail = 0;
    }
    PIE1bits.RCIE = 0;
    eusartRxCount--;
    PIE1bits.RCIE = 1;

    return readValue;
}

void EUSART_Write(uint8_t txData) {
    while (0 == eusartTxBufferRemaining) {
    }

    if (0 == PIE1bits.TXIE) {
        while (!TXIF)
            continue;
        TXREG = txData;
        // TXREG = txData;
    } else {
        PIE1bits.TXIE = 0;
        eusartTxBuffer[eusartTxHead++] = txData;
        if (sizeof (eusartTxBuffer) <= eusartTxHead) {
            eusartTxHead = 0;
        }
        eusartTxBufferRemaining--;
    }
    //  PIE1bits.TXIE = 1;
}

//=========================================
//  де≥н≥ц≥ал≥зувати прийомник (очистити)
//=========================================

void reinit_rx(void) {
    eusartRxHead = 0;
    eusartRxTail = 0;
    eusartRxCount = 0;
}



// перериванн€ по прийом≥ порта

void EUSART_Receive_ISR(void) {

    if (1 == RCSTAbits.OERR) {
        // EUSART error - restart

        RCSTAbits.CREN = 0;
        RCSTAbits.CREN = 1;
    }

    // buffer overruns are ignored
    eusartRxBuffer[eusartRxHead++] = RCREG;
    if (sizeof (eusartRxBuffer) <= eusartRxHead) {
        eusartRxHead = 0;
    }

    if ((eusartRxCount++) >= sizeof (eusartRxBuffer))
        eusartRxCount = 0;

}