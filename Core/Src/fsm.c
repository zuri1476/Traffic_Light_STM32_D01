/*
 * fsm.c
 *
 *  Created on: Oct 12, 2026
 *      Author: admin
 */

#include "fsm.h"

TrafficState_t currentState = STATE_INIT;
uint8_t timeLaneA = 0;
uint8_t timeLaneB = 0;

uint8_t cfgRedTime    = DEFAULT_TIME_RED;     // 30 giây
uint8_t cfgGreenTime  = DEFAULT_TIME_GREEN;   // 27 giây
uint8_t cfgYellowTime = DEFAULT_TIME_YELLOW;  //  3 giây

/**
 * @brief Xuất mức logic ra các chân PA0 - PA5 để điều khiển 6 đèn LED
 */
void FSM_UpdateOutputs(void) {
    // Tắt toàn bộ 6 đèn trước khi bật trạng thái mới
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 |
                             GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, GPIO_PIN_RESET);

    switch (currentState) {
        case STATE_AUTO_RED_GREEN:
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET); // Tuyến A: ĐỎ (PA0)
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET); // Tuyến B: XANH (PA5)
            break;

        case STATE_AUTO_RED_YELLOW:
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET); // Tuyến A: ĐỎ (PA0)
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); // Tuyến B: VÀNG (PA4)
            break;

        case STATE_AUTO_GREEN_RED:
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET); // Tuyến A: XANH (PA2)
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET); // Tuyến B: ĐỎ (PA3)
            break;

        case STATE_AUTO_YELLOW_RED:
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET); // Tuyến A: VÀNG (PA1)
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET); // Tuyến B: ĐỎ (PA3)
            break;

        case STATE_NIGHT_MODE:
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_1 | GPIO_PIN_4); // Cả hai tuyến nháy VÀNG
            break;

        default:
            break;
    }
}

void FSM_Init(void) {
    currentState = STATE_AUTO_RED_GREEN;
    timeLaneA = cfgRedTime;
    timeLaneB = cfgGreenTime;
    FSM_UpdateOutputs(); // Cập nhật đèn sáng ngay khi bật nguồn
}

void FSM_Run_1s(void) {
    if (currentState == STATE_NIGHT_MODE) {
        FSM_UpdateOutputs();
        return;
    }

    if (timeLaneA > 0) timeLaneA--;
    if (timeLaneB > 0) timeLaneB--;

    switch (currentState) {
        case STATE_AUTO_RED_GREEN:
            if (timeLaneB == 0) {
                currentState = STATE_AUTO_RED_YELLOW;
                timeLaneB = cfgYellowTime;
            }
            break;

        case STATE_AUTO_RED_YELLOW:
            if (timeLaneB == 0) {
                currentState = STATE_AUTO_GREEN_RED;
                timeLaneA = cfgGreenTime;
                timeLaneB = cfgRedTime;
            }
            break;

        case STATE_AUTO_GREEN_RED:
            if (timeLaneA == 0) {
                currentState = STATE_AUTO_YELLOW_RED;
                timeLaneA = cfgYellowTime;
            }
            break;

        case STATE_AUTO_YELLOW_RED:
            if (timeLaneA == 0) {
                currentState = STATE_AUTO_RED_GREEN;
                timeLaneA = cfgRedTime;
                timeLaneB = cfgGreenTime;
            }
            break;

        case STATE_PEDESTRIAN:
            if (timeLaneA == 0 && timeLaneB == 0) {
                FSM_Init();
            }
            break;

        default:
            FSM_Init();
            break;
    }

    FSM_UpdateOutputs(); // Cập nhật đèn sau khi đếm giây / chuyển pha
}

void FSM_SetNightMode(void) {
    currentState = STATE_NIGHT_MODE;
}

void FSM_SetAutoMode(void) {
    FSM_Init();
}

void FSM_TriggerPedestrian(void) {
    currentState = STATE_PEDESTRIAN;
    timeLaneA = 15;
    timeLaneB = 15;
}
