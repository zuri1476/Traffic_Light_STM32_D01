#include "uart_app.h"
#include <stdio.h>
#include <string.h>

#define RX_BUFFER_SIZE 64

static uint8_t rx_byte;
static char rx_buffer[RX_BUFFER_SIZE];
static uint8_t rx_index = 0;
static volatile uint8_t cmd_ready_flag = 0;

extern int red_duration;
extern int green_duration;
extern int yellow_duration;
extern int current_state;
extern int remaining_time;

extern UART_HandleTypeDef huart1;

void UART_App_Init(void) {
    HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
}

void UART_SendString(char *str) {
    HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        if (rx_byte == '\n' || rx_byte == '\r') {
            rx_buffer[rx_index] = '\0';
            if (rx_index > 0) {
                cmd_ready_flag = 1; 
            }
            rx_index = 0;
        } else {
            if (rx_index < RX_BUFFER_SIZE - 1) {
                rx_buffer[rx_index++] = rx_byte;
            }
        }
        HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
    }
}

void UART_Process_Command(void) {
    if (cmd_ready_flag) {
        cmd_ready_flag = 0;
        char response_buf[128];
        int val = 0;

			if (sscanf(rx_buffer, "SET RED %d", &val) == 1) {
            if (val > 0) {
                red_duration = val;
                sprintf(response_buf, "[UART] OK: Red duration updated to %d s\r\n", red_duration);
                UART_SendString(response_buf);
            } else {
                UART_SendString("[UART] ERROR: Invalid time value\r\n");
            }
        }

        else if (sscanf(rx_buffer, "SET GREEN %d", &val) == 1) {
            if (val > 0) {
                green_duration = val;
                sprintf(response_buf, "[UART] OK: Green duration updated to %d s\r\n", green_duration);
                UART_SendString(response_buf);
            } else {
                UART_SendString("[UART] ERROR: Invalid time value\r\n");
            }
        }

        else if (sscanf(rx_buffer, "SET YELLOW %d", &val) == 1) {
            if (val > 0) {
                yellow_duration = val;
                sprintf(response_buf, "[UART] OK: Yellow duration updated to %d s\r\n", yellow_duration);
                UART_SendString(response_buf);
            } else {
                UART_SendString("[UART] ERROR: Invalid time value\r\n");
            }
        }

        else if (strcmp(rx_buffer, "GET STATUS") == 0) {
            sprintf(response_buf, "[STATUS] State: %d | Time Left: %ds | Red: %ds, Green: %ds, Yellow: %ds\r\n", 
                    current_state, remaining_time, red_duration, green_duration, yellow_duration);
            UART_SendString(response_buf);
        }

        else if (strcmp(rx_buffer, "MODE1") == 0) {
            UART_SendString("[UART] Switched to Mode 1 (Automatic)\r\n");
        } 
        else if (strcmp(rx_buffer, "MODE2") == 0) {
            UART_SendString("[UART] Switched to Mode 2 (Manual)\r\n");
        } 
        else {
            UART_SendString("[UART] ERROR: Unknown Command\r\n");
        }
    }
}