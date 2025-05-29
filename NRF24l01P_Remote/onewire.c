#include "onewire.h"


// повертає 0, якщо датчик присутній і 1, якщо ні
uint8_t ow_reset(void)
{
   uint8_t presence;
    DQ = 0;
    TRIS_DQ = 0;
    __delay_us(500);          // Min. 480uS
    DQ = 1;
    TRIS_DQ = 1;
   __delay_us(100);           // Takes 15 to 60uS for devices to respond
   presence = DQ;
   __delay_us(424);          // Wait for end of timeslot
   return(presence);
}


//******************************************************************************
// Read bit on one wire bus
uint8_t read_bit(void)
{
    DQ = 0;
    TRIS_DQ = 0;
    __delay_us(1);         // Added, 1uS min. Code relied on 8051 being slow.
    DQ = 1;
    TRIS_DQ = 1;
    __delay_us(6);        // Read within 15uS from start of time slot
   return(DQ);   
}   


void write_bit(uint8_t bitval)
{
    DQ = 0;
    TRIS_DQ = 0;
   if(bitval == 1) {
      __delay_us(2);      // 1uS min. Code relied on 8051 being slow.
      DQ = 1;
      TRIS_DQ = 1;
   }
   __delay_us(105);       // Wait for end of timeslot
      DQ = 1;
      TRIS_DQ = 1;
}


//******************************************************************************
uint8_t read_byte(void)
{
   uint8_t i;
   uint8_t val = 0;

   for(i=0;i<8;i++)
   {
      if(read_bit()) val |= (0x01 << i);
      __delay_us(120);  // To finish time slot
   }

   return val;
}


//******************************************************************************
void write_byte(uint8_t val)
{
   uint8_t i;
   uint8_t temp;

   for (i=0;i<8;i++)
   {
      temp = val >> i;
      temp &= 0x01;
      write_bit(temp);
   }

   __delay_us(105);
}

void Waiting_WR (void)
{
   __delay_us(52);         //
}//---------------------------------------------------------------------------------------
