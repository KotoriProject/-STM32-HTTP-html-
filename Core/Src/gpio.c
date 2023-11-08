/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    gpio.c
 * @brief   This file provides code for the configuration
 *          of all used GPIO pins.
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

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */
#include "GUI.h"
#include "tim.h"
/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CONFIG1_GPIO_Port, CONFIG1_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, CONFIG2_Pin|CONFIG3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(ESP_EN_GPIO_Port, ESP_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, EN485_Pin|OLED_RES_Pin|OLED_DC_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = CONFIG1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(CONFIG1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PCPin PCPin */
  GPIO_InitStruct.Pin = CONFIG2_Pin|CONFIG3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = ESP_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ESP_EN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PCPin PCPin */
  GPIO_InitStruct.Pin = key1_Pin|key2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PBPin PBPin */
  GPIO_InitStruct.Pin = key3_Pin|key4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PBPin PBPin */
  GPIO_InitStruct.Pin = EN485_Pin|OLED_RES_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = TF_CD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(TF_CD_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = OLED_DC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(OLED_DC_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI1_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 2 */

/*CH224K CONFIG*/
void Voltage_Config(uint8_t vol_level)
{
  if (vol_level == 0)
  {
    HAL_GPIO_WritePin(CONFIG1_GPIO_Port, CONFIG1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(CONFIG1_GPIO_Port, CONFIG2_Pin | CONFIG2_Pin, GPIO_PIN_RESET);
  }
  else if (vol_level == 1)
  {
    HAL_GPIO_WritePin(CONFIG1_GPIO_Port, CONFIG1_Pin | CONFIG2_Pin | CONFIG3_Pin, GPIO_PIN_RESET);
  }
  else if (vol_level == 2)
  {
    HAL_GPIO_WritePin(CONFIG1_GPIO_Port, CONFIG1_Pin | CONFIG2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(CONFIG1_GPIO_Port, CONFIG3_Pin, GPIO_PIN_SET);
  }
  else if (vol_level == 3)
  {
    HAL_GPIO_WritePin(CONFIG1_GPIO_Port, CONFIG1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(CONFIG1_GPIO_Port, CONFIG2_Pin | CONFIG3_Pin, GPIO_PIN_SET);
  }
  else if (vol_level == 4)
  {
    HAL_GPIO_WritePin(CONFIG1_GPIO_Port, CONFIG1_Pin | CONFIG3_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(CONFIG1_GPIO_Port, CONFIG2_Pin, GPIO_PIN_SET);
  }
}
extern volatile GUI_InputKey gui_key;
GUI_InputKey press_key;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == key1_Pin)
  {
    if (HAL_GPIO_ReadPin(key1_GPIO_Port, key1_Pin) == GPIO_PIN_RESET)
    {
      press_key = KEY_UP;
      HAL_TIM_Base_Stop_IT(&htim1);
      htim1.Instance->ARR = 65535;
      HAL_TIM_Base_Start_IT(&htim1);
    }
    else if (press_key == KEY_UP)
    {
      HAL_TIM_Base_Stop_IT(&htim1);
      if (TIM1->CNT > 2000 && HAL_GPIO_ReadPin(key1_GPIO_Port, key1_Pin) == GPIO_PIN_SET)
        gui_key = press_key;
      press_key = KEY_NONE;
    }
  }
  else if (GPIO_Pin == key2_Pin)
  {
    if (HAL_GPIO_ReadPin(key2_GPIO_Port, key2_Pin) == GPIO_PIN_RESET)
    {
      press_key = KEY_DOWN;
      HAL_TIM_Base_Stop_IT(&htim1);
      htim1.Instance->ARR = 65535;
      HAL_TIM_Base_Start_IT(&htim1);
    }
    else if (press_key == KEY_DOWN)
    {
      HAL_TIM_Base_Stop_IT(&htim1);
      if (TIM1->CNT > 2000 && HAL_GPIO_ReadPin(key2_GPIO_Port, key2_Pin) == GPIO_PIN_SET)
        gui_key = press_key;
      press_key = KEY_NONE;
    }
  }
  else if (GPIO_Pin == key3_Pin)
  {
    if (HAL_GPIO_ReadPin(key3_GPIO_Port, key3_Pin) == GPIO_PIN_RESET)
    {
      press_key = KEY_BACK;
      HAL_TIM_Base_Stop_IT(&htim1);
      htim1.Instance->ARR = 65535;
      HAL_TIM_Base_Start_IT(&htim1);
    }
    else if (press_key == KEY_BACK)
    {
      HAL_TIM_Base_Stop_IT(&htim1);
      if (TIM1->CNT > 2000 && HAL_GPIO_ReadPin(key3_GPIO_Port, key3_Pin) == GPIO_PIN_SET)
        gui_key = press_key;
      press_key = KEY_NONE;
    }
  }
  else if (GPIO_Pin == key4_Pin)
  {
    if (HAL_GPIO_ReadPin(key4_GPIO_Port, key4_Pin) == GPIO_PIN_RESET)
    {
      press_key = KEY_OK;
      HAL_TIM_Base_Stop_IT(&htim1);
      htim1.Instance->ARR = 65535;
      HAL_TIM_Base_Start_IT(&htim1);
    }
    else if (press_key == KEY_OK)
    {
      HAL_TIM_Base_Stop_IT(&htim1);
      if (TIM1->CNT > 2000 && HAL_GPIO_ReadPin(key4_GPIO_Port, key4_Pin) == GPIO_PIN_SET)
        gui_key = press_key;
      press_key = KEY_NONE;
    }
  }
}
/* USER CODE END 2 */
