/*
 * fsm.h
 *
 *  Author: SV1 (Nhiệm vụ FSM & Timer - Đề tài D01)
 */

#ifndef INC_FSM_H_
#define INC_FSM_H_

#include "main.h"

// ==========================================
// CẤU HÌNH THỜI GIAN MẶC ĐỊNH (Đơn vị: Giây)
// Đảm bảo nguyên tắc: T_Đỏ = T_Xanh + T_Vàng (30 = 27 + 3)
// ==========================================
#define DEFAULT_TIME_YELLOW     3
#define DEFAULT_TIME_GREEN      27
#define DEFAULT_TIME_RED        30
#define TIME_PEDESTRIAN_CROSS   10   // Thời gian qua đường của người đi bộ: 10 giây

// ==========================================
// ĐỊNH NGHĨA CÁC TRẠNG THÁI HỆ THỐNG (FSM)
// ==========================================
typedef enum {
    STATE_INIT = 0,
    STATE_AUTO_RED_GREEN,    // Tuyến A: ĐỎ    - Tuyến B: XANH
    STATE_AUTO_RED_YELLOW,   // Tuyến A: ĐỎ    - Tuyến B: VÀNG
    STATE_AUTO_GREEN_RED,    // Tuyến A: XANH   - Tuyến B: ĐỎ
    STATE_AUTO_YELLOW_RED,   // Tuyến A: VÀNG   - Tuyến B: ĐỎ
    STATE_PEDESTRIAN,        // Ưu tiên người đi bộ (Đỏ cả 2 tuyến + Còi buzzer)
    STATE_NIGHT_MODE         // Cảnh báo ban đêm (Vàng nhấp nháy 1Hz)
} TrafficState_t;

// ==========================================
// BIẾN TOÀN CỤC CÔNG KHAI (Dành cho SV2 & SV4)
// ==========================================
extern TrafficState_t currentState;
extern uint8_t timeLaneA;
extern uint8_t timeLaneB;

extern uint8_t cfgRedTime;
extern uint8_t cfgGreenTime;
extern uint8_t cfgYellowTime;

// ==========================================
// KHAI BÁO CÁC HÀM XỬ LÝ (API)
// ==========================================
void FSM_Init(void);
void FSM_Run_1s(void);
void FSM_UpdateOutputs(void);

// Hàm nhận sự kiện từ SV3 (Nút bấm) và SV4 (UART)
void FSM_TriggerPedestrian(void);
void FSM_SetNightMode(void);
void FSM_SetAutoMode(void);
uint8_t FSM_ConfigNextCycle(uint8_t red, uint8_t green, uint8_t yellow);

#endif /* INC_FSM_H_ */
