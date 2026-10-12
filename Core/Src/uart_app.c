#include "uart_app.h"
#include "usart.h"
#include <string.h>

#define RX_BUFFER_SIZE 20

uint8_t rx_byte;                    
uint8_t rx_buffer[RX_BUFFER_SIZE];  
uint8_t rx_index = 0;               
volatile uint8_t cmd_ready_flag = 0; 

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
            } else {
                rx_index = 0;
            }
        }
        HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
    }
}

void UART_Process_Command(void) {
    if (cmd_ready_flag) {
        cmd_ready_flag = 0;
        
        if (strcmp((char*)rx_buffer, "MODE1") == 0) {
            UART_SendString("[UART] Switched to Mode 1: Automatic\r\n");
        } 
        else if (strcmp((char*)rx_buffer, "MODE2") == 0) {
            UART_SendString("[UART] Switched to Mode 2: Manual\r\n");
        } 
        else {
            UART_SendString("[UART] Unknown Command\r\n");
        }
    }
}