#ifndef __UART_APP_H
#define __UART_APP_H

#include "main.h"

void UART_App_Init(void);
void UART_SendString(char *str);
void UART_Process_Command(void);

#endif /* __UART_APP_H */