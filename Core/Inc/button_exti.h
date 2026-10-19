#ifndef BUTTON_EXTI_H
#define BUTTON_EXTI_H

#include "main.h"

/* Khai báo chân và hàm xử lý hai nút nhấn. */
/* Active-low buttons connected from these pins to GND. */
#define BUTTON_PEDESTRIAN_PORT BTN_PED_GPIO_Port
#define BUTTON_PEDESTRIAN_PIN  BTN_PED_Pin
#define BUTTON_NIGHT_PORT      BTN_NIGHT_GPIO_Port
#define BUTTON_NIGHT_PIN       BTN_NIGHT_Pin

/* Call Button_DebounceTask1ms() from the 1 ms TIM2 callback. */
#define BUTTON_DEBOUNCE_TIME_MS 20U

void Button_Init(void);
void Button_DebounceTask1ms(void);
void Button_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

#endif /* BUTTON_EXTI_H */
