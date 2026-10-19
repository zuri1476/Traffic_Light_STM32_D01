/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define RED_A_Pin GPIO_PIN_0
#define RED_A_GPIO_Port GPIOA
#define YEL_A_Pin GPIO_PIN_1
#define YEL_A_GPIO_Port GPIOA
#define GRE_A_Pin GPIO_PIN_2
#define GRE_A_GPIO_Port GPIOA
#define RED_B_Pin GPIO_PIN_3
#define RED_B_GPIO_Port GPIOA
#define YEL_B_Pin GPIO_PIN_4
#define YEL_B_GPIO_Port GPIOA
#define GRE_B_Pin GPIO_PIN_5
#define GRE_B_GPIO_Port GPIOA
#define DIG1_Pin GPIO_PIN_0
#define DIG1_GPIO_Port GPIOB
#define DIG2_Pin GPIO_PIN_1
#define DIG2_GPIO_Port GPIOB
#define DIG3_Pin GPIO_PIN_2
#define DIG3_GPIO_Port GPIOB
#define BTN_PED_Pin GPIO_PIN_12
#define BTN_PED_GPIO_Port GPIOB
#define BTN_PED_EXTI_IRQn EXTI15_10_IRQn
#define BTN_NIGHT_Pin GPIO_PIN_13
#define BTN_NIGHT_GPIO_Port GPIOB
#define BTN_NIGHT_EXTI_IRQn EXTI15_10_IRQn
#define BUZZER_Pin GPIO_PIN_14
#define BUZZER_GPIO_Port GPIOB
#define DIG4_Pin GPIO_PIN_3
#define DIG4_GPIO_Port GPIOB
#define DS_Pin GPIO_PIN_4
#define DS_GPIO_Port GPIOB
#define SHCP_Pin GPIO_PIN_5
#define SHCP_GPIO_Port GPIOB
#define STCP_Pin GPIO_PIN_6
#define STCP_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
