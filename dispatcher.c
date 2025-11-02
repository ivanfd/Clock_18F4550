//*********************************************************************************************************
//                                            Диспетчер задач.
// Запозичено з сайту: 
// http://chipenable.ru/index.php/embedded-programming/item/219-planirovschik-dlya-mikrokontrollera.html
// Диспетчер під AVR. Мною перероблено для PIC, плюс деякі зміни. 
// 
//*********************************************************************************************************

#include "dispatcher.h"

volatile static task TaskArray[MAX_TASKS];      // задачі
volatile static uint8_t arrayTail;                  // "хвіст" черги задач
uint8_t Timer0Interrupt;        // чи було переривання від таймера 0

/******************************************************************************************
 * ініт
 */
 void RTOS_Init()
{
   T0CON = 0b11000111;                         // дільник 1:256, таймер вмк. 8-біт
   INTCONbits.TMR0IE = 1;                      // переривання від переповнення таймера
   TMR0 = TIMER_START;                         // початкове значення в регістр таймера

   arrayTail = 0;                               // "хвіст" в 0
}
 
/******************************************************************************************
 * Додати задачу в список
 */
void RTOS_SetTask (void (*taskFunc)(void), uint16_t taskDelay, uint16_t taskPeriod)
{
   uint8_t i;
   
   if(!taskFunc) return;
   for(i = 0; i < arrayTail; i++)                     // пошук задачі в списку
   {
      if(TaskArray[i].pFunc == taskFunc)              // якщо така вже є, то обновляємо її
      {
         INTERRUPT_GlobalInterruptDisable();

         TaskArray[i].delay  = taskDelay;
         TaskArray[i].period = taskPeriod;
         TaskArray[i].run    = 0;   

         INTERRUPT_GlobalInterruptEnable();
           return;                                      // обновивши, выходимо
      }
   }

   if (arrayTail < MAX_TASKS)                         // якщо такої задачі немає 
   {                                                  // і є місце, то добавимо її
         INTERRUPT_GlobalInterruptDisable();
      
      TaskArray[arrayTail].pFunc  = taskFunc;
      TaskArray[arrayTail].delay  = taskDelay;
      TaskArray[arrayTail].period = taskPeriod;
      TaskArray[arrayTail].run    = 0;   

      arrayTail++;                                    // збільшити хвіст
         INTERRUPT_GlobalInterruptEnable();
   } 
} 

/******************************************************************************************
 * видалити задачу зі списку
 */
void RTOS_DeleteTask (void (*taskFunc)(void))
{
   uint8_t i;
   
   for (i=0; i<arrayTail; i++)                        // проходимо по списку задач
   {
      if(TaskArray[i].pFunc == taskFunc)              // якщо знайшли задачу
      {
         
                  INTERRUPT_GlobalInterruptDisable();
         if(i != (arrayTail - 1))                     // переносимо останню задачу
         {                                            // на місце видаленої
            TaskArray[i] = TaskArray[arrayTail - 1];
         }
         arrayTail--;                                 // зменшуємо хвіст
                  INTERRUPT_GlobalInterruptEnable();
         return;
      }
   }
}

/******************************************************************************************
 * Диспетчер ОС
 ******************************************************************************************/

void RTOS_DispatchTask()
{
   uint8_t i, j;
   void (*function) (void);
   
   if (Timer0Interrupt)
   {
       Timer0Interrupt = 0;
       for (j=0; j<arrayTail; j++)                        // проходимо по списку задач
        {
            if  (TaskArray[j].delay == 0)                   // якщо час до виконання вийшов
                TaskArray[j].run = 1;                      // ставимо біт запуску
            else TaskArray[j].delay--;                      // якщо ні, то зменшуємо його
        }
   }

   for (i=0; i<arrayTail; i++)                        // проходимо по списку задач
   {
      if (TaskArray[i].run == 1)                      // якщо біт на виконання задачі = 1
      {                                               // запам'ятовуємо задачу, т.як під
         function = TaskArray[i].pFunc;               // виконання може 
                                                      // змінитися індекс
         if(TaskArray[i].period == 0)                 
         {                                            // якщо період рівний нулю
            RTOS_DeleteTask(TaskArray[i].pFunc);      // видаляємо задачу зі списку
            
         }
         else
         {
            TaskArray[i].run = 0;                     // якщо ні, то знімаємо біт запуску
            if(!TaskArray[i].delay)                   // якщо задача не змінила затримку
            {                                         // задаємо її
               TaskArray[i].delay = TaskArray[i].period-1; 
            }                                         //   
         }
         (*function)();                               // виконуємо задачу
      
      }
      
   }
}

// переривання від таймера 0
// налаштовано 5мс
 void TMR0_ISR(void)
{

    // Clear the TMR0 interrupt flag
    INTCONbits.TMR0IF = 0;
    TMR0 = TIMER_START;
    Timer0Interrupt = 1; // ставимо признак переривання
   
   

    }