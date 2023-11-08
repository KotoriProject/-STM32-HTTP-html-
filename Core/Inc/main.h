/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
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
#define CONFIG1_Pin GPIO_PIN_1
#define CONFIG1_GPIO_Port GPIOC
#define CONFIG2_Pin GPIO_PIN_2
#define CONFIG2_GPIO_Port GPIOC
#define CONFIG3_Pin GPIO_PIN_3
#define CONFIG3_GPIO_Port GPIOC
#define ESP_EN_Pin GPIO_PIN_1
#define ESP_EN_GPIO_Port GPIOA
#define LED_R_Pin GPIO_PIN_6
#define LED_R_GPIO_Port GPIOA
#define LED_G_Pin GPIO_PIN_7
#define LED_G_GPIO_Port GPIOA
#define key1_Pin GPIO_PIN_4
#define key1_GPIO_Port GPIOC
#define key1_EXTI_IRQn EXTI4_IRQn
#define key2_Pin GPIO_PIN_5
#define key2_GPIO_Port GPIOC
#define key2_EXTI_IRQn EXTI9_5_IRQn
#define LED_B_Pin GPIO_PIN_0
#define LED_B_GPIO_Port GPIOB
#define key3_Pin GPIO_PIN_1
#define key3_GPIO_Port GPIOB
#define key3_EXTI_IRQn EXTI1_IRQn
#define EN485_Pin GPIO_PIN_2
#define EN485_GPIO_Port GPIOB
#define key4_Pin GPIO_PIN_12
#define key4_GPIO_Port GPIOB
#define key4_EXTI_IRQn EXTI15_10_IRQn
#define TF_CD_Pin GPIO_PIN_7
#define TF_CD_GPIO_Port GPIOC
#define OLED_RES_Pin GPIO_PIN_4
#define OLED_RES_GPIO_Port GPIOB
#define OLED_DC_Pin GPIO_PIN_6
#define OLED_DC_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
