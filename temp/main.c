 /*
 * MAIN Generated Driver File
 * 
 * @file main.c
 * 
 * @defgroup main MAIN
 * 
 * @brief This is the generated driver implementation file for the MAIN driver.
 *
 * @version MAIN Driver Version 1.0.2
 *
 * @version Package Version: 3.1.2
*/

/*
© [2025] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/
#include "mcc_generated_files/system/system.h"
#include "mcc_generated_files/i2c_host/i2c_host_interface.h"

/*
    Main application
*/

int main(void)
{

    uint8_t clientAddr = 0x40; // 7-bit Client address
    uint8_t deviceRegAddrMsb = 0x00;    // MSB of Client location address
    uint8_t deviceRegAddrLsb = 0x20;    // LSB of Client location address

    // Desired write and read lengths
    uint8_t writeLength;
    uint8_t readLength;

    // Write and read data buffers (buffersize is set for some overhead)
    uint8_t transmitData[10] = {};
    uint8_t readData[10] = {};

    uint8_t waitCounter = 0;

    // Initializes Clock, Pins, Interrupts and I2C Initialize
    SYSTEM_Initialize();
//I2C1_StopSend();
    
    //I2C1_EVENT_STOP();

    // Write to client device
    transmitData[0] = 0x00;  // load MSB of Client location address
    transmitData[1] = 0x20;  // load LSB of Client location address
    transmitData[2] = 0x55;  // load data
    writeLength = 3; // 2 bytes of location address + 1 byte data

    if (I2C1_Host.Write(clientAddr, transmitData, writeLength))
    {
        waitCounter = 100; // This value depends on the system clock, I2C clock and data length.                                                                                          
        while (I2C1_Host.IsBusy())
        {
            I2C1_Host.Tasks();
            waitCounter--;              
        }

        if ( I2C1_Host.ErrorGet() == I2C_ERROR_NONE)
        {
            // Write operation is successful
        }
        else
        {
            // Error handling
        }
    }

    // Write which register we wish to read from
    writeLength = 2; // 2 bytes of location address
    if (I2C1_Host.Write(clientAddr, transmitData, writeLength))
    {
        waitCounter = 100; // This value depends on the system clock, I2C clock and data length.                                                                                          
        while (I2C1_Host.IsBusy())
        {
            I2C1_Host.Tasks();
            waitCounter--;              
        }

        if ( I2C1_Host.ErrorGet() == I2C_ERROR_NONE)
        {
            // Write operation is successful
        }
        else
        {
            // Error handling
        }
    }

    // Read from client device
    readLength = 1; // 1 byte read
    if (I2C1_Host.Read(clientAddr, readData, readLength))
    {
        waitCounter = 100; // This value depends on the system clock, I2C clock and data length.                                                                                          
        while (I2C1_Host.IsBusy())
        {
            I2C1_Host.Tasks();
            waitCounter--;              
        }

        if ( I2C1_Host.ErrorGet() == I2C_ERROR_NONE)
        {
            // Read operation is successful
        }
        else
        {
            // Error handling
        }
    }
}  
