#ifndef UART_APP_H
#define UART_APP_H

#include "main.h"

void UART_App_Init(void);
void UART_Process_Command(void);
void UART_SendString(char *str);

#endif /* UART_APP_H */