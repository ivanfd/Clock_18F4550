#include "time.h"
#include "display.h"

uint8_t day_in_m[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; // к≥льк≥сть дн≥в в м≥с€ц€х
//*******************************************************
//            ¬изначаЇмо чи зараз л≥тн≥й час
//*******************************************************

void dst_time(struct Time_Get *pTime, uint8_t *dst) {
    
uint8_t Mrt, Oct; // останн≥ дн≥ м≥с€ц≥в, число

    Mrt = (unsigned)31 - DayOfWeek(31, 3, (pTime->Tyr)); //останн€ нед≥л€ березн€, число
    Oct = (unsigned)31 - DayOfWeek(31, 10, (pTime->Tyr)); //останн€ нед≥л€ жовтн€, число

        // перев≥р€Їмо, чи час л≥тн≥й
    if (((pTime->Tmt > 3) && (pTime->Tmt < 10)) ||
            ((pTime->Tmt == 3) && (pTime->Tdt > Mrt)) ||
            ((pTime->Tmt == 3) && (pTime->Tdt == Mrt) && (pTime->Thr >= 3)) ||
            ((pTime->Tmt == 10) && (pTime->Tdt < Oct)) ||
            ((pTime->Tmt == 10) && (pTime->Tdt == Oct) && (pTime->Thr <= 1)) ||
            ((pTime->Tmt == 10) && (pTime->Tdt == Oct) && (pTime->Thr == 23)))
        *dst = 1; // €кщо так, то ставимо признак
    else
        *dst = 0;

    //    pTime->Tdt = 29;
//    pTime->Thr = 23;
//    pTime->Tmt = 2;
//    pTime->Tyr = 20;

    if (*dst) {
        if (pTime->Thr == 23) { // €кщо година п≥сл€ 23
            pTime->Thr = 0;
            pTime->Tdy++;
            if (pTime->Tdy == 8)
                pTime->Tdy = 1;
            if ((pTime->Tyr) % 4) { // €кщо р≥к не високосний
                if ((pTime->Tdt == day_in_m[(pTime->Tmt)-(unsigned) 1]) | ((pTime->Tmt == 2)&&(pTime->Tdt == 28))) { // €кщо число р≥вне останньому числу м≥с€ц€
                    if (pTime->Tmt == 12) {// €кщо м≥с€ць грудень
                        pTime->Tyr++; // зб≥льшуЇмо р≥к
                        pTime->Tdt = 1; // ставимо число 1
                        pTime->Tmt = 1;
                    } else {
                        pTime->Tdt = 1; // ставимо число 1
                        pTime->Tmt++;
                    }
                } else {
                    pTime->Tdt++; // зб≥льшуЇмо на 1
                    //pTime->Tmt++;
                }
            } else { //€кщо високосний
                if (pTime->Tdt == (day_in_m[(pTime->Tmt) - (unsigned)1])) { // €кщо число р≥вне останньому числу м≥с€ц€
                    if (pTime->Tmt == 12) {// €кщо м≥с€ць грудень
                        pTime->Tyr++; // зб≥льшуЇмо р≥к
                        pTime->Tdt = 1; // ставимо число 1
                        pTime->Tmt = 1; // м≥с€ць с≥чень
                    } else {
                        pTime->Tdt = 1; // ставимо число 1
                        pTime->Tmt++;
                    }
                } else {
                    pTime->Tdt++; // зб≥льшуЇмо на 1
                }
            }

        } else
            pTime->Thr = (uint8_t) pTime->Thr + 1; // додаЇмо до години 1

    }
}

//**********************************************************
//                        день тижн€
//**********************************************************

uint8_t DayOfWeek (uint8_t day, uint8_t month, uint8_t year)
 {
  uint8_t a = (uint8_t)((14 - month) / 12);
  uint8_t y = (uint8_t)(year - a);
  uint8_t m = (uint8_t)(month + 12 * a - 2);
  return (uint8_t)((7000 + (day + y + y / 4 - y / 100 + y / 400 + (31 * m) / 12)) % 7);
}