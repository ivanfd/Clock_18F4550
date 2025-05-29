
#include "display.h"

uint8_t Dis_Buff[BUF_SIZE_TEMP]; // буфер дисплея
uint8_t Dis_Buff_t[BUF_SIZE]; // буфер дисплея temp
//uint8_t TxtBuf[COUNT_MATRIX];          // буфер дл¤ цифр
uint8_t text_buf[50]; // буфер для біг строки
uint8_t rs_text_buf[100]; // буфер для біг строки з компорта
uint8_t i_char, i_bchar; // індекс літери та байт в літері
const uint8_t(*pFont)[5] = dFont1; // вказівник на шрифт цифр для показу годин     
//extern uint8_t idx_pnt;
//extern volatile uint8_t x1 = 0, x2 = 0, x3 = 0, x4 = 0, y1, y2, y3, y4; //Для зсуву стовбця вниз
//extern uint8_t blk_dot; // дозвіл на мигання кнопок

//uint8_t Hours, Min, Sec;

static void (*function) (void); // вказівник на функцію
//void (*p_MyFunc[4])(void) = {dissolve, scroll_left, scroll_right, hide_two_side};
const p_MyFunc my_func[5] = {dissolve, scroll_left, scroll_right, hide_two_side, scroll_down_one}; //масив вказівників на функції
//*****************************************
//       засвітити піксель на матриці
//*****************************************

void pixel_on(uint8_t x, uint8_t y, uint8_t *buf) {
    *(buf + x) |= (1 << y);
    //Dis_Buff[x] |= (1<<y);
}

//*****************************************
//      загасити піксель на матриці
//*****************************************

void pixel_off(uint8_t x, uint8_t y, uint8_t *buf) {
    *(buf + x) &= ~(1 << y);
    //Dis_Buff[x] &= ~(1<<y);
}

//**********************************************
//  вивід картинки на дисплей у позицію x
//**********************************************

void pic_to_led(uint8_t x, uint8_t pic, uint8_t *buf) {
    uint8_t i, j, mask = 0x01;

    for (i = 0; i < 7; i++) { //стобчики
        for (j = 0; j < 8; j++) { //строки 
            if (pic7x8[pic][i] & mask) {
                pixel_on(x + i, j, buf);
            } else {
                pixel_off(x + i, j, buf);
            };
            mask = (uint8_t) (mask <<  1);
        };
        mask = 0x01;
    };
}

//***********************************************
//вставити символ у відповідну позицію на матрицю
//***********************************************

void putchar_b_buf(uint8_t x, uint8_t symbol, const uint8_t(*pF)[5], uint8_t *buf) {
    uint8_t i, j, mask = 0x01;


    for (i = 0; i < 5; i++) // перебір стовбців, шрифт 5х8
    {
        for (j = 0; j < 8; j++)//перебір строк - 8біт
        {
            if (pF[symbol] [i] & mask) // якщо відповідний біт не = 0
                pixel_on(x + i, j, buf); //засвічуємо піксель
            else //якщо ні
                pixel_off(x + i, j, buf); //гасимо

            mask = (uint8_t) (mask <<  1); // 
        };
        //пройшли всі 8 біт стовбця, переходимо на наступний
        mask = 0x01;
    };
}

//*******************************************************
//вставити маленькі цифри у відповідну позицію на матрицю
//*******************************************************
//void putchar_s_buf(uint8_t x, uint8_t symbol)
//{
//uint8_t i, j, mask=0x01;
//
//symbol -= 47; 
//    for(i=0; i<3; i++)  // перебір стовбців,маленький шрифт 3х8
//    {        
//        for(j=0;j<8;j++)//перебір строк - 8біт
//        {      
//            if(FontS[symbol] [i] & mask) // якщо відповідний біт не = 0
//                pixel_on(x+i,j,&Dis_Buff);        //засвічуємо піксель
//            else                        //якщо ні
//                pixel_off(x+i,j,&Dis_Buff);       //гасимо
//                
//                mask = mask<<1;         // 
//        };
//        //пройшли всі 8 біт стовбця, переходимо на наступний
//    mask=0x01; 
//    };    
//}

//*************************************************************
//вставити символ у відповідну позицію на матрицю з зсувом вниз
//*************************************************************

void putchar_down(uint8_t x, uint8_t symbol, const uint8_t(*pF)[5]) {
    uint8_t i, j, k;

    for (j = 0; j <= 7; j++) //проходимо всі 8 строк
    {
        for (i = 0; i < 5; i++) // сунемо вниз на 1 біт символ
        {
            Dis_Buff[x + i] = (uint8_t) (Dis_Buff[x + i] << 1) | (pF[symbol][i] >> (8 - 1 - j));
        }

        for (k = 0; k < DELAY_SHIFT_DOWN; k++)
            __delay_ms(1);
        Update_Matrix(Dis_Buff); // обновити дані на дисплеї

    }

}

//***********************************************************************
//вставити маленький символ у відповідну позицію на матрицю з зсувом вниз
//***********************************************************************

void putchar_down_s(uint8_t x, uint8_t symbol) {
    uint8_t i, j, k;

    //if(symbol != 0)
    //   symbol -= 47;

    for (j = 0; j <= 7; j++) //проходимо всі 8 строк
    {
        for (i = 0; i < 3; i++) // сунемо вниз на 1 біт символ
        {
            Dis_Buff[x + i] = (uint8_t) (Dis_Buff[x + i] << 1) | (FontS[symbol][i] >> (8 - 1 - j));
        }

        for (k = 0; k < DELAY_SHIFT_DOWN; k++)
            __delay_ms(1);
        Update_Matrix(Dis_Buff); // обновити дані на дисплеї

    }

}

//********************
//   очистити матрицю
//********************

void clear_matrix(void) {
    uint8_t i;
    for (i = 0; i < BUF_SIZE; i++)
        Dis_Buff[i] = 0;
}


//***********************************
// Заповнення буфера байтами шрифту
//***********************************

void FillBuf(uint8_t type) {
    uint8_t i;
    switch (type) {
        case TYPE_CLK_1:
            blk_dot = 0;
            for (i = 0; i < 5; ++i) {
                if ((TTime.Thr / 10) % 10)
                    Dis_Buff[i] = pFont[(TTime.Thr / 10) % 10][i];
                else
                    Dis_Buff[i] = 0x00;


                Dis_Buff[i + 6] = pFont[TTime.Thr % 10 ][i];
                Dis_Buff[i + 13] = pFont[(TTime.Tmin / 10) % 10 ][i];
                Dis_Buff[i + 19] = pFont[TTime.Tmin % 10 ][i];

            }

            for (i = 0; i < 3; ++i) {
                Dis_Buff[i + 25] = FontS[(TTime.Ts / 10) % 10 + 1][i];
                Dis_Buff[i + 29] = FontS[TTime.Ts % 10 + 1][i];
            }

            Dis_Buff[5] = 0;
            Dis_Buff[11] = 0x80;
            Dis_Buff[12] = 0x80;
            Dis_Buff[18] = 0;
            Dis_Buff[24] = 0;
            Dis_Buff[28] = 0;
            break;
        case TYPE_CLK_2:// якщо тип годинника 2
            blk_dot = 1;
            y1 = (TTime.Thr / 10) % 10;
            y2 = TTime.Thr % 10;
            y3 = (TTime.Tmin / 10) % 10;
            y4 = TTime.Tmin % 10;
            //Якщо люба з цифр змінилася
            if ((x1 != y1) || (x2 != y2) || (x3 != y3) || (x4 != y4)) {
                //putchar_b_buf(13, ':', &Font);
                if (x4 != y4)
                    putchar_down(25, TTime.Tmin % 10, pFont);
                else
                    putchar_b_buf(25, TTime.Tmin % 10, pFont, Dis_Buff);

                if (x3 != y3)
                    putchar_down(19, (TTime.Tmin / 10) % 10, pFont);
                else
                    putchar_b_buf(19, (TTime.Tmin / 10) % 10, pFont, Dis_Buff);

                if (x2 != y2)
                    putchar_down(7, TTime.Thr % 10, pFont);
                else
                    putchar_b_buf(7, TTime.Thr % 10, pFont, Dis_Buff);

                if ((TTime.Thr / 10) % 10) {
                    if (x1 != y1)
                        putchar_down(1, (TTime.Thr / 10) % 10, pFont);
                    else
                        putchar_b_buf(1, (TTime.Thr / 10) % 10, pFont, Dis_Buff);

                } else
                    putchar_down(1, 0, Font);
                // putchar_b_buf(13, 23 + idx_pnt, &Font);
                //idx_pnt = 0;


                x1 = y1;
                x2 = y2;
                x3 = y3;
                x4 = y4;

            } else {
                if ((TTime.Thr / 10) % 10)
                    putchar_b_buf(1, (TTime.Thr / 10) % 10, pFont, Dis_Buff);
                else
                    putchar_b_buf(1, 0, Font, Dis_Buff);
                putchar_b_buf(7, TTime.Thr % 10, pFont, Dis_Buff);
                // putchar_b_buf(13, 23 + idx_pnt, &Font);
                putchar_b_buf(19, (TTime.Tmin / 10) % 10, pFont, Dis_Buff);
                putchar_b_buf(25, TTime.Tmin % 10, pFont, Dis_Buff);
            }
            break;
    }


}

//================================================
//           Заповнюємо буфер (5 байт) 
//  *buf - вказівник на масив цифр
//  edit_Flsg - якщо 1, то під 4-5-ю цифрою буде підкреслення
//  scr_flag - якщо 1, то потрібно вивести два останні стобвбці
//================================================

//void FillBufS(uint8_t *buf, uint8_t edit_Flag, uint8_t scr_flag)
//   {
//      uint8_t i, j, d, nch = 0;
//      d = 0;
//      
//      for (j=0;j<=4;++j)  
//      {
//         for (i=0;i<=4;++i)  
//         {
//            if((nch >= 3) && edit_Flag)
//               Dis_Buff[d] = (Font[*(buf + j)] [i]) + 0x80;
//            else
//               Dis_Buff[d] = Font[*(buf + j)] [i];
//            d++;
//         }  
//
//           Dis_Buff[d] = 0;
//           d++;
//           nch++;
//         //}
//      }
//      if (scr_flag)
//      {
//         Dis_Buff[30] = Font[*(buf + 5)] [0];
//         Dis_Buff[31] = Font[*(buf + 5)] [1];
//      }
//      else
//      {
//         Dis_Buff[30] = 0;
//         Dis_Buff[31] = 0;
//      }
//   }


//*******************************
//   Вивід бігучого рядка
//*******************************

uint8_t scroll_text(uint8_t *buf) {
    uint8_t i;

    for (i = 0; i < 31; i++) // зсуваємо всі пікселі на матриці вліво на 1 біт
        Dis_Buff[i] = Dis_Buff[i + 1];

    if ((i_bchar) >= 5) // якщо вивели один символ
    {
        if (*(buf + i_char) == 0) // якщо строка закінчилась
        {
            i_bchar++;
            if (i_bchar >= 32) // якщо добили буфер до кінця нулями
            {
                i_char = 0;
                i_bchar = 0;
                return 0; // виходимо
            }
        } else {
            Dis_Buff[31] = 0; // розділити букви
            i_bchar = 0;
            i_char++; // наступний байт (літера) строки
        }
    } else {
        Dis_Buff[31] = Font[ *(buf + i_char)][i_bchar++]; // у 31 позицію нову колонку

    }



    return 1;

}

//******************************************
//   Вивід бігучого рядка з температурою
//      pos - на скільки будемо зсувати
//******************************************

void scroll_text_temp(uint8_t pos) {
    uint8_t i, j, k;

    for (k = 0; k <= pos; k++) {
        for (i = 0; i < 31; i++) // зсуваємо всі пікселі на матриці вліво на 1 біт
            Dis_Buff[i] = Dis_Buff[i + 1];

        Dis_Buff[31] = Dis_Buff[32 + k ]; // у 31 позицію нову колонку
        Update_Matrix(Dis_Buff); // обновити дані на дисплеї
        for (j = 0; j < SPEED_STRING - 20; j++)
            __delay_ms(1);
    }
    return;

}

//********************************************
//  зсовуємо текст зправа наліво
//********************************************

void scroll_left(void) {
    uint8_t i, j, k, speed = 100;


    for (k = 0; k <= 31; k++) {
        for (i = 0; i <= 31; i++)
            Dis_Buff[i] = Dis_Buff[i + 1];

        Dis_Buff[31] = 0;
        Update_Matrix(Dis_Buff); // обновити дані на дисплеї
        for (j = 0; j < speed; j++)
            __delay_ms(1);
        if (speed > 10) speed -= 10;

    }


}

//********************************************
//  зсовуємо текст зліва направо
//********************************************

void scroll_right(void) {
    uint8_t i, j, k, speed = 100;


    for (k = 0; k <= 31; k++) {

        for (i = 31; i > 0; i--)
            Dis_Buff[i] = Dis_Buff[i - 1];

        Dis_Buff[0] = 0;
        Update_Matrix(Dis_Buff); // обновити дані на дисплеї
        for (j = 0; j < speed; j++)
            __delay_ms(1);
        if (speed > 10) speed -= 10;

    }


}

//===============================================
//      інтервал виводу для біг. строки
//===============================================

void interval_scroll_text(uint8_t *buf) {
    uint8_t i;
    INTERRUPT_GlobalInterruptDisable();
    while (scroll_text(buf)) {
        Update_Matrix(Dis_Buff); // обновити дані на дисплеї
        for (i = 0; i < SPEED_STRING; i++)
            __delay_ms(1);

    }
    INTERRUPT_GlobalInterruptEnable();

}

//**************************************************
//             Ефект згасання з двох сторін :-)
//**************************************************

void hide_two_side(void) {
    uint8_t a = 0, b = 31;
    uint8_t i, j;

    for (i = 0; i <= 15; i++) {
        Dis_Buff[a++] = 0;
        Dis_Buff[b--] = 0;
        Update_Matrix(Dis_Buff); // обновити дані на дисплеї
        for (j = 0; j < 30; j++) // пауза
            __delay_ms(1);

    }
}


//**************************************************
//             Ефект затухання :-)
//**************************************************

void dissolve(void) {
    uint8_t i, n = 0, z, k;
    //    const uint8_t dissolve_arr[16] = {253,191,239,127,251,223,247,254,253,191,239,127,251,223,247,254}; //масив для ефекту затухання
    const uint8_t dissolve_arr[8] = {253, 191, 239, 127, 251, 223, 247, 254}; //масив для ефекту затухання



    for (z = 0; z <= 7; z++) {
        for (i = 0; i <= 31; i++) {
            Dis_Buff[i] &= dissolve_arr[n + z];
            n++;
            if (n > 7)
                n = 0;
        }
        Update_Matrix(Dis_Buff); // обновити дані на дисплеї
        for (k = 0; k < 100; k++) // пауза
            __delay_ms(1);
    }
}


//**************************************************
//             Ефект спадання вниз :-)
//**************************************************

void scroll_down_one(void) {
    uint8_t i, j, k;


    for (i = 0; i <= 31; i++) {
        if (Dis_Buff[i] == 0)
            continue;
        for (j = 0; j <= 7; j++) {
            //Dis_Buff[x+i] = (Dis_Buff[x+i]<<1) | ((*pF)[symbol][i] >> (8-1-j));
            Dis_Buff[i] = (uint8_t) (Dis_Buff[i] << 1);
            Update_Matrix(Dis_Buff); // оновити дані на дисплеї
            for (k = 0; k < 5; k++) // пауза
                __delay_ms(1);
        }

    }
}


//=========================================
//  Вибір випадкового ефекту
//=========================================

void Rand_ef(void) {
    static uint8_t eff = 0;
    static uint8_t old_eff = 0;

    while (old_eff == eff) {
        eff = (0 + rand() % 5);
    }
    old_eff = eff;
    function = my_func[eff];
    (*function)(); // виконуємо задачу
}

//*******************************************
//  Заповнення тимчасового буфера екрану
//*******************************************

void fill_buff_t(uint16_t data) {
    uint8_t i;


    for (i = 0; i <= BUF_SIZE; i++)
        Dis_Buff_t[i] = 0;

    pic_to_led(3, 4, Dis_Buff_t);
    putchar_b_buf(11, (data / 1000) % 10, pFont, Dis_Buff_t);
    putchar_b_buf(17, (data / 100) % 10, pFont, Dis_Buff_t);
    putchar_b_buf(23, (data / 10) % 10, pFont, Dis_Buff_t);
    putchar_b_buf(29, '.', Font, Dis_Buff_t);
}

//*******************************************
//  Ефект виводу тексту зсередини
//*******************************************

void center_two_side(void) {
    uint8_t i, k, shiftr = 16, shiftl = 15;

    for (i = 0; i <= 15; i++) {
        Dis_Buff[shiftr] = Dis_Buff_t[shiftr];
        shiftr++;
        //Dis_Buff[shiftr++] = Dis_Buff_t[shiftr];
        Dis_Buff[shiftl] = Dis_Buff_t[shiftl];
        shiftl--;
        //Dis_Buff[shiftl--] = Dis_Buff_t[shiftl];


        Update_Matrix(Dis_Buff); // оновити дані на дисплеї
        for (k = 0; k < 30; k++) // пауза
            __delay_ms(1);
    }
}