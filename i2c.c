//====================================================
//       Налаштування I2C переферії
//
//====================================================



#include "i2c.h"

uint8_t i2cBuffer[10];

void I2C_Init(void)  // ініціалізація I2C
{
    TRISB |=  0b00000011;           // наналаштування порта
    SSPCON1 = 0b00101000;           // ввімкнути модуль MSSP, ведучий режим I2C
    SSPSTAT = 0b00000000;           // 
    SSPADD = 0x77;                  // частота 100кГц SSPADD = ((Fosc / Fclock) / 4) - 1 ; 1000 = I2C Master mode, clock = FOSC/(4 * (SSPADD + 1))
    //SSPADD = 0x1F;                  // частота 312.5кГц SSPADD = ((Fosc / Fclock) / 4) - 1
    PEN = 1;                        // формуємо стопбіт P
    while (PEN);                    // чекаємо закінчення формування стоп біту
}

// чекати закінчення активності

void I2C_Master_Wait()
{
  while ((SSPSTAT & 0x04) || (SSPCON2 & 0x1F)); //Transmit is in progress
}

// старт
void I2C_Master_Start()
{
  I2C_Master_Wait();    
  SEN = 1;             //Initiate start condition
}

// повторний старт
void I2C_Master_RepeatedStart()
{
  I2C_Master_Wait();
  RSEN = 1;           //Initiate repeated start condition
}

// стоп
void I2C_Master_Stop()
{
  I2C_Master_Wait();
  PEN = 1;           //Initiate stop condition
}

// пишемо дані
void I2C_Master_Write(uint8_t d)
{
  I2C_Master_Wait();
  SSPBUF = d;         //Write data to SSPBUF
}

// читаємо
uint8_t I2C_Master_Read(uint8_t a)
{
  uint8_t temp;
  I2C_Master_Wait();
  RCEN = 1;
  I2C_Master_Wait();
  temp = SSPBUF;      //Read data from SSPBUF
  I2C_Master_Wait();
  ACKDT = (a)?0:1;    //Acknowledge bit
  ACKEN = 1;          //Acknowledge sequence
  return temp;
}

// записуємо n - байт по адресі

void I2C_Write(uint8_t *byteAr, uint8_t i2cAdress, uint8_t n) {
    I2C_Master_Start();
    I2C_Master_Write(i2cAdress);
    for (uint8_t i = 0; i < n; i++)
        I2C_Master_Write(*(byteAr + i));
    // I2C_Master_Write(value);
    I2C_Master_Stop();
}




// читаємо n - байт по адресі
// enRegByte - чи потрібно слати байт регістра

void I2C_Read(uint8_t *byteAr, uint8_t i2cAdress, uint8_t n, uint8_t enRegByte) {
    I2C_Master_Start();
    if (enRegByte) {
        I2C_Master_Write(i2cAdress); // передаємо адресу для запису
        I2C_Master_Write(*byteAr); // регістр
        I2C_Master_RepeatedStart();
    }
    I2C_Master_Write(i2cAdress | 1); // передаємо адресу для читання

    for (uint8_t i = 0; i < n; i++) { // читаємо n байт
        if (i < (n - 1))
            *(byteAr + i) = I2C_Master_Read(1); // читаємо з підтвердженням
        else
            *(byteAr + i) = I2C_Master_Read(0); // читаємо без підтвердження
    }
    I2C_Master_Stop();
}













//// чекати закінчення активності
//void I2C_idle (void)
//{
//    while((SSPCON2 & 0x1F) || R_W);
//}//
//
//// Формування старт
//// adress - адреса відомого
//// _R_W - 0 - запис, 1 - читання
//uint8_t I2C_start (uint8_t adress, uint8_t _R_W)
//{
//	I2C_idle();
//	SEN = 1;    // формувати старт біт
//	I2C_idle(); //  чекаємо не активності
//	if(_R_W == 0) adress &= 0b11111110;	// якщо запис 
//              else adress |= 0b00000001;	//якщо читання
//	SSPBUF=adress;
//	I2C_idle();
//	if (ACKSTAT==0) return 0; 	// підтвердження від відомого отримано
//	else
//	{ PEN = 1; while (PEN); return 1;} // помилка
//}
//
//
////_t повторний старт
//uint8_t I2C_restart (uint8_t adres, uint8_t _R_W)
//{
//	I2C_idle();
//	RSEN = 1;
//	I2C_idle();
//	if(_R_W == 0) adres &= 0b11111110;	// 
//	      else adres |= 0b00000001;	//
//	SSPBUF = adres;
//	I2C_idle();
//	if (ACKSTAT == 0) return 0; // 
//	else
//	{ PEN = 1; while (PEN); return 1;} // 
//}//
//
////  запис байту в пристрій
//uint8_t I2C_write (uint8_t data)	
//{
//    I2C_idle();
//    SSPBUF = data;
//    SSPIF = 0;
//    I2C_idle();
//    if (ACKSTAT == 0) return 0; // підтвердження від відомого отримано
//    else
//    { PEN = 1; while (PEN); return 1;} // помилка
//}
//
//// читаємо байт без підтвердження
//uint8_t I2C_read_noack (void)	
//{
//	I2C_idle();
//	RCEN = 1;				//Дозволити прийом даних
//	I2C_idle();
//	ACKDT = 1;              //set the MASTER NOACK bit
//	ACKEN = 1;              //сформувати біт підтвердження
//    //	I2C_idle();
//	return  SSPBUF;         //читаємо буфер
//}
//
//uint8_t i2c_read_ack (void)	//формирования чтения байта из устройства с подтверждением приема
//{
//	I2C_idle();
//	RCEN=1;			//начать прием данных
//	I2C_idle();
//	ACKDT=0;		//установить бит подтвеждения приема
//	ACKEN=1;		//начать формировать бит
//	return  SSPBUF;		//чтение буфера
//}//
//
//// формування стоп біту
//void I2C_stop (void)
//{
//	I2C_idle(); //
//	PEN = 1;    //
//	while(PEN); //
//}//

