/*
 * fsm.c
 *
 *  Author: SV1 (Nhiệm vụ FSM & Timer - Đề tài D01)
 */

#include "fsm.h"

// Biến trạng thái và bộ đếm lùi thời gian
TrafficState_t currentState = STATE_INIT;
uint8_t timeLaneA = 0;
uint8_t timeLaneB = 0;

// Cấu hình thời gian đang chạy
uint8_t cfgRedTime    = DEFAULT_TIME_RED;     // 30s
uint8_t cfgGreenTime  = DEFAULT_TIME_GREEN;   // 27s
uint8_t cfgYellowTime = DEFAULT_TIME_YELLOW;  //  3s

// Bộ đệm cấu hình thời gian mới từ UART (áp dụng ở chu kỳ sau)
static uint8_t nextRedTime    = DEFAULT_TIME_RED;
static uint8_t nextGreenTime  = DEFAULT_TIME_GREEN;
static uint8_t nextYellowTime = DEFAULT_TIME_YELLOW;
static uint8_t isConfigPending = 0;

// Cờ người đi bộ và trạng thái phục hồi công bằng giao thông
static uint8_t pedestrianRequested = 0;
static TrafficState_t nextStateAfterPed = STATE_AUTO_RED_GREEN;

/**
 * @brief Điều khiển 6 LED giao thông (PA0 - PA5) và Còi Buzzer (PB14)
 */
void FSM_UpdateOutputs(void) {
    // SỬA LỖI 1: Xử lý riêng cho Night Mode để đèn Vàng nhấp nháy chuẩn 1Hz
    if (currentState == STATE_NIGHT_MODE) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_5, GPIO_PIN_RESET);
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_1 | GPIO_PIN_4); // PA1, PA4 đảo trạng thái bật/tắt mỗi giây
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET); // Tắt còi
        return;
    }

    // Tắt toàn bộ 6 chân đèn trước khi kích hoạt trạng thái mới
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 |
                             GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, GPIO_PIN_RESET);

    switch (currentState) {
        case STATE_AUTO_RED_GREEN:
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);   // Tuyến A: ĐỎ (PA0)
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);   // Tuyến B: XANH (PA5)
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
            break;

        case STATE_AUTO_RED_YELLOW:
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);   // Tuyến A: ĐỎ (PA0)
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);   // Tuyến B: VÀNG (PA4)
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
            break;

        case STATE_AUTO_GREEN_RED:
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);   // Tuyến A: XANH (PA2)
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);   // Tuyến B: ĐỎ (PA3)
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
            break;

        case STATE_AUTO_YELLOW_RED:
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);   // Tuyến A: VÀNG (PA1)
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);   // Tuyến B: ĐỎ (PA3)
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
            break;

        case STATE_PEDESTRIAN:
            // Đỏ cả 2 tuyến cho phương tiện dừng hẳn
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);   // Tuyến A: ĐỎ
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);   // Tuyến B: ĐỎ
            HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);               // Còi Buzzer kêu ngắt quãng (PB14)
            break;

        default:
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
            break;
    }
}

/**
 * @brief Áp dụng cấu hình thời gian mới nếu có lệnh chờ từ UART
 */
static void FSM_ApplyPendingConfig(void) {
    if (isConfigPending) {
        cfgRedTime    = nextRedTime;
        cfgGreenTime  = nextGreenTime;
        cfgYellowTime = nextYellowTime;
        isConfigPending = 0;
    }
}

/**
 * @brief Khởi tạo trạng thái ban đầu của hệ thống
 */
void FSM_Init(void) {
    currentState = STATE_AUTO_RED_GREEN;
    timeLaneA = cfgRedTime;
    timeLaneB = cfgGreenTime;
    pedestrianRequested = 0;
    nextStateAfterPed = STATE_AUTO_RED_GREEN;
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
    FSM_UpdateOutputs();
}

/**
 * @brief Hàm chạy chu kỳ 1 giây trong ngắt phần cứng TIM2
 */
void FSM_Run_1s(void) {
    if (currentState == STATE_NIGHT_MODE) {
        FSM_UpdateOutputs();
        return;
    }

    if (timeLaneA > 0) timeLaneA--;
    if (timeLaneB > 0) timeLaneB--;

    switch (currentState) {
        // --- PHA 1: A ĐỎ - B XANH ---
        case STATE_AUTO_RED_GREEN:
            if (pedestrianRequested && timeLaneB > 3) {
                timeLaneB = 3;
                timeLaneA = 3 + cfgYellowTime;
            }
            if (timeLaneB == 0) {
                currentState = STATE_AUTO_RED_YELLOW;
                timeLaneB = cfgYellowTime;
            }
            break;

        // --- PHA 2: A ĐỎ - B VÀNG ---
        case STATE_AUTO_RED_YELLOW:
            if (timeLaneB == 0) {
                if (pedestrianRequested) {
                    currentState = STATE_PEDESTRIAN;
                    nextStateAfterPed = STATE_AUTO_GREEN_RED; // SỬA LỖI 2: Sau đi bộ, Tuyến A được XANH
                    timeLaneA = TIME_PEDESTRIAN_CROSS;
                    timeLaneB = TIME_PEDESTRIAN_CROSS;
                } else {
                    currentState = STATE_AUTO_GREEN_RED;
                    timeLaneA = cfgGreenTime;
                    timeLaneB = cfgRedTime;
                }
            }
            break;

        // --- PHA 3: A XANH - B ĐỎ ---
        case STATE_AUTO_GREEN_RED:
            if (pedestrianRequested && timeLaneA > 3) {
                timeLaneA = 3;
                timeLaneB = 3 + cfgYellowTime;
            }
            if (timeLaneA == 0) {
                currentState = STATE_AUTO_YELLOW_RED;
                timeLaneA = cfgYellowTime;
            }
            break;

        // --- PHA 4: A VÀNG - B ĐỎ ---
        case STATE_AUTO_YELLOW_RED:
            if (timeLaneA == 0) {
                if (pedestrianRequested) {
                    currentState = STATE_PEDESTRIAN;
                    nextStateAfterPed = STATE_AUTO_RED_GREEN; // SỬA LỖI 2: Sau đi bộ, Tuyến B được XANH
                    timeLaneA = TIME_PEDESTRIAN_CROSS;
                    timeLaneB = TIME_PEDESTRIAN_CROSS;
                } else {
                    FSM_ApplyPendingConfig();
                    currentState = STATE_AUTO_RED_GREEN;
                    timeLaneA = cfgRedTime;
                    timeLaneB = cfgGreenTime;
                }
            }
            break;

        // --- CHẾ ĐỘ ƯU TIÊN NGƯỜI ĐI BỘ (10 GIÂY) ---
        case STATE_PEDESTRIAN:
            if (timeLaneA == 0 && timeLaneB == 0) {
                pedestrianRequested = 0;
                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);

                // Chuyển sang tuyến đúng lượt ưu tiên tiếp theo
                currentState = nextStateAfterPed;
                if (currentState == STATE_AUTO_GREEN_RED) {
                    timeLaneA = cfgGreenTime;
                    timeLaneB = cfgRedTime;
                } else {
                    FSM_ApplyPendingConfig();
                    timeLaneA = cfgRedTime;
                    timeLaneB = cfgGreenTime;
                }
            }
            break;

        default:
            FSM_Init();
            break;
    }

    FSM_UpdateOutputs();
}

void FSM_TriggerPedestrian(void) {
    pedestrianRequested = 1;
}

void FSM_SetNightMode(void) {
    currentState = STATE_NIGHT_MODE;
    timeLaneA = 0;
    timeLaneB = 0;
    FSM_UpdateOutputs();
}

void FSM_SetAutoMode(void) {
    FSM_Init();
}

uint8_t FSM_ConfigNextCycle(uint8_t red, uint8_t green, uint8_t yellow) {
    if (red != (green + yellow)) {
        return 0;
    }
    nextRedTime    = red;
    nextGreenTime  = green;
    nextYellowTime = yellow;
    isConfigPending = 1;
    return 1;
}
