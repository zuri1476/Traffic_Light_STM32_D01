/*
 * fsm.h
 *
 *  Created on: Oct 12, 2026
 *      Author: admin
 */

#ifndef INC_FSM_H_
#define INC_FSM_H_

#include "main.h"

#define DEFAULT_TIME_YELLOW    3
#define DEFAULT_TIME_GREEN     27
#define DEFAULT_TIME_RED       30

typedef enum {
    STATE_INIT = 0,
    STATE_AUTO_RED_GREEN,    // Tuyến A Đỏ  - Tuyến B Xanh
    STATE_AUTO_RED_YELLOW,   // Tuyến A Đỏ  - Tuyến B Vàng
    STATE_AUTO_GREEN_RED,    // Tuyến A Xanh - Tuyến B Đỏ
    STATE_AUTO_YELLOW_RED,   // Tuyến A Vàng - Tuyến B Đỏ
    STATE_PEDESTRIAN,        // Ưu tiên người đi bộ
    STATE_NIGHT_MODE,        // Cảnh báo ban đêm (Vàng nhấp nháy)
    STATE_CONFIG_TIME        // Chế độ chỉnh thời gian
} TrafficState_t;

extern TrafficState_t currentState;
extern uint8_t timeLaneA;
extern uint8_t timeLaneB;

extern uint8_t cfgRedTime;
extern uint8_t cfgGreenTime;
extern uint8_t cfgYellowTime;

void FSM_Init(void);
void FSM_Run_1s(void);
void FSM_SetNightMode(void);
void FSM_SetAutoMode(void);
void FSM_TriggerPedestrian(void);
void FSM_UpdateOutputs(void);
#endif /* INC_FSM_H_ */
