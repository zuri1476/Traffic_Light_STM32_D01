#include "uart_app.h"
#include "fsm.h"
#include <string.h>
#include <stdio.h>

extern UART_HandleTypeDef huart1;

#define RX_BUFFER_SIZE 128

static char rx_buffer[RX_BUFFER_SIZE];    /* ng?t ghi vào dây */
static char cmd_buffer[RX_BUFFER_SIZE];   /* main d?c t? dây */
static uint8_t rx_index = 0;
static uint8_t rx_byte;
static volatile uint8_t cmd_ready_flag = 0;

void UART_App_Init(void)
{
    HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
}

void UART_SendString(const char *str)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)str, strlen(str), HAL_MAX_DELAY);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        if (rx_byte == '\r' || rx_byte == '\n')
        {
            if (rx_index > 0)
            {
                rx_buffer[rx_index] = '\0';
                if (cmd_ready_flag == 0)   /* ch? chuy?n khi l?nh tru?c dã x? lý xong */
                {
                    strcpy(cmd_buffer, rx_buffer);
                    cmd_ready_flag = 1;
                }
                rx_index = 0;
            }
        }
        else
        {
            if (rx_index < (RX_BUFFER_SIZE - 1))
                rx_buffer[rx_index++] = rx_byte;
            else
                rx_index = 0;              /* tràn b? d?m: b? dòng này */
        }

        HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        HAL_UART_Receive_IT(&huart1, &rx_byte, 1);   /* b?t nh?n l?i sau l?i */
    }
}

void UART_Process_Command(void)
{
    if (!cmd_ready_flag) return;

    int r = 0, g = 0, y = 0;
    char msg[160];

    if (sscanf(cmd_buffer, "SET %d %d %d", &r, &g, &y) == 3)
    {
        if (r < 0 || r > 255 || g < 0 || g > 255 || y < 0 || y > 255)
        {
            UART_SendString("[UART] ERROR: Value out of range (0-255)\r\n");
        }
        else if (FSM_ConfigNextCycle((uint8_t)r, (uint8_t)g, (uint8_t)y) == 1)
        {
            snprintf(msg, sizeof(msg),
                     "[UART] OK: Configured -> Red:%d, Green:%d, Yellow:%d\r\n", r, g, y);
            UART_SendString(msg);
        }
        else
        {
            UART_SendString("[UART] ERROR: Invalid logic! (Red must equal Green + Yellow)\r\n");
        }
    }
    else if (strcmp(cmd_buffer, "GET STATUS") == 0)
    {
        snprintf(msg, sizeof(msg),
                 "[STATUS] State: %d | LaneA: %ds | LaneB: %ds | Red: %ds, Green: %ds, Yellow: %ds\r\n",
                 (int)currentState, (int)timeLaneA, (int)timeLaneB,
                 (int)cfgRedTime, (int)cfgGreenTime, (int)cfgYellowTime);
        UART_SendString(msg);
    }
    else
    {
        snprintf(msg, sizeof(msg), "[UART] ERROR: Invalid command -> %s\r\n", cmd_buffer);
        UART_SendString(msg);
    }

    cmd_ready_flag = 0;   /* d?t cu?i d? ng?t không ghi dè cmd_buffer khi dang x? lý */
}