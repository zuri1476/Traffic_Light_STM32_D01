/*
 * led7seg_595.h
 *
 * Driver LED 7 doan 4 so (Anode chung) dung 1 IC 74HC595 + 4 transistor PNP.
 * Hien thi thoi gian dem nguoc 2 tuyen:
 *     LED1, LED2 : timeLaneA (chuc, don vi)
 *     LED3, LED4 : timeLaneB (chuc, don vi)
 *
 * Phan cung (khop voi main.h / gpio.c do CubeMX sinh ra):
 *     PB4 = DS   (74HC595 pin 14)   PB5 = SHCP (pin 11)   PB6 = STCP (pin 12)
 *     PB0 = DIG1, PB1 = DIG2, PB2 = DIG3, PB3 = DIG4  (muc 0 = bat transistor PNP)
 *
 * Su dung:
 *     led7seg_init();                        // 1 lan trong main(), sau MX_GPIO_Init()
 *     led7seg_scan();                        // goi moi 1 ms trong ngat TIM2
 *     led7seg_set_time(timeLaneA, timeLaneB);// goi moi khi so giay thay doi
 *     led7seg_enable(0);                     // tat hien thi (che do ban dem)
 */

#ifndef INC_LED7SEG_595_H_
#define INC_LED7SEG_595_H_

#include "main.h"
#include <stdint.h>

/* DS (PB4) khong co nhan define trong main.h nen khai bao tai day */
#define LED7_DS_Pin         GPIO_PIN_4
#define LED7_DS_GPIO_Port   GPIOB

/* Khoi tao: giai phong PB3/PB4 khoi JTAG, tat het LED, xoa bo dem */
void led7seg_init(void);

/* Dat thoi gian 2 huong (0..99 giay)
 * secA -> LED 1,2 (huong A) ; secB -> LED 3,4 (huong B) */
void led7seg_set_time(uint8_t secA, uint8_t secB);

/* Bat (1) / tat (0) toan bo hien thi. Khi tat, LED tat han, scan van an toan */
void led7seg_enable(uint8_t on);

/* Goi trong ngat TIM2 moi 1 ms: quet 1 LED moi lan goi (4 ms / chu ky, ~250 Hz) */
void led7seg_scan(void);

#endif /* INC_LED7SEG_595_H_ */