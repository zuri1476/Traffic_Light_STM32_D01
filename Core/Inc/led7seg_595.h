#ifndef __LED7SEG_595_H
#define __LED7SEG_595_H

#include "main.h"
#include <stdint.h>

/* Khoi tao: tat het LED */
void led7seg_init(void);

/* Dat thoi gian 2 huong (0..99 giay)
 * secA -> LED 1,2 (huong A) ; secB -> LED 3,4 (huong B) */
void led7seg_set_time(uint8_t secA, uint8_t secB);

/* Goi trong ngat TIM2 moi 1ms: quet 1 LED moi lan goi */
void led7seg_scan(void);

#endif