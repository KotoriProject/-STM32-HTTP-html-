/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "fatfs.h"
#include "sdio.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "oled.h"
#include "ws2811.h"
#include "myFatfs.h"
#include "GUI.h"
#include "ESP8266.h"
#include "stdio.h"
#include "arm_math.h"
#include "arm_const_structs.h"

#define FFT_LENGTH 32
volatile uint16_t adc_buf[FFT_LENGTH];
float fft_input[FFT_LENGTH * 2];
float fft_output[FFT_LENGTH];
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

int fputc(int ch, FILE *f)
{
  uint8_t data = (uint8_t)ch;
  HAL_UART_Transmit(&huart1, &data, 1, 10);
  return (ch);
}
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile GUI_InputKey gui_key = KEY_NONE;
volatile uint8_t OLED_TRANSMIT = 0;
volatile uint8_t HTTP_GET = 0;

extern volatile uint8_t adc_flag;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void FS_TRANS(uint8_t *pData, uint16_t Size)
{
  if (ESP8266_UART_status != ESP8266_ACK_ERROR)
  {
    uint8_t a[20] = {0};
    sprintf((char *)a, "AT+CIPSEND=0,%d\r\n", (int)Size);
    FS_uart_status = FS_UART_BUSY;
    HAL_UART_Transmit_DMA(&huart2, a, strlen((const char *)a));
    while (FS_uart_status)
      ;
    HAL_Delay(2);
    FS_uart_status = FS_UART_BUSY;
    HAL_UART_Transmit_DMA(&huart2, pData, Size);
    while (FS_uart_status)
      ;
    HAL_Delay(10);
  }
}

void dir_read(const TCHAR *path)
{
  FIL file;
  DIR dirs;
  static FILINFO finfo;
  if (f_opendir(&dirs, path) == FR_OK) // 打开文件夹成�?????????????????????????????
  {
    // printf("Now Dir:%s\r\n", finfo.fname);
    while (f_readdir(&dirs, &finfo) == FR_OK) // 按顺序读文件�?????????????????????????????
    {
      if (!finfo.fname[0])
        break; // 如果文件名为0，结�?????????????????????????????
      else
      {
        if (finfo.fattrib == AM_DIR)
        {
          printf("Now Dir:%s\r\n", finfo.fname);
          dir_read(finfo.fname);
        }
        else if (finfo.fattrib == AM_ARC) // 判断文件属�??
        {
          printf("FileName:%s\r\n", finfo.fname);
          myfatfs_open(&file, finfo.fname, FS_TRANS);
        }
      }
    }
  }
}

void FFT()
{
  HAL_TIM_Base_Start(&htim8);                                 // 开启定时器8
  HAL_ADCEx_Calibration_Start(&hadc1);                        // AD校准，F4不用校准没用这行函数。
  HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_buf, FFT_LENGTH); // 让ADC1去采集200个数，存放到adc_buff数组里
  while (!adc_flag)                                           // 等待转换完毕
    ;
  HAL_TIM_Base_Stop(&htim8);
  for (uint16_t i = 0; i < FFT_LENGTH; i++)
  {
    fft_input[i * 2] = (float)adc_buf[i] * 3.3 / 4095; // 实部赋�?�，* 3.3 / 4096是为了将ADC采集到的值转换成实际电压
    fft_input[i * 2 + 1] = 0;                          // 虚部赋�?�，固定�?0.
  }

  arm_cfft_f32(&arm_cfft_sR_f32_len32, fft_input, 0, 1); // 输出数组为原数组。虚实位置不变，但是有了对应的频率含�?
  arm_cmplx_mag_f32(fft_input, fft_output, FFT_LENGTH);  // 把运算结果复数求模得幅�??
  /**********************处理变换结果*******************************/
  fft_output[0] /= FFT_LENGTH;

  for (uint16_t i = 1; i < FFT_LENGTH; i++) // 输出各次谐波幅�??
  {
    if (fft_output[i])
      ;//fft_output[i] /= FFT_LENGTH / 2;
  }

  for (uint16_t i = 0; i < FFT_LENGTH / 2; i++)
  {
    printf("%d:%.3f\r\n", i, fft_output[i]); // 数据打印，查看结果
  }
  //OLED_Fill(0);
  uint8_t a[]={0xff};
  OLED_DrawBMP(0,0,1,1,a);

}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_TIM3_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_SDIO_SD_Init();
  MX_USART3_UART_Init();
  MX_TIM1_Init();
  MX_TIM4_Init();
  MX_TIM8_Init();
  MX_FATFS_Init();
  MX_USB_DEVICE_Init();
  MX_TIM5_Init();
  /* USER CODE BEGIN 2 */
  OLED_Init();
  GUI_REFRESH;
  HAL_TIM_Base_Start_IT(&htim4);

  ws2811_Init();
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
  // HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx1_buf, sizeof(rx1_buf));
  HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx2_buf, sizeof(rx2_buf));

  HAL_TIM_Base_Start_IT(&htim5);

  ESP8266_Init();
  ESP8266_HTTP_ON();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    //FFT();
    while (gui_key != KEY_NONE && OLED_TRANSMIT == 0)
    {
      GUI_Refresh(gui_key);
      gui_key = KEY_NONE;
    }
    switch (ESP8266_UART_status)
    {
    case ESP8266_UART_READY:
      OLED_DrawStr(97, 0, "Ready");
      break;
    case ESP8266_UART_WAIT4TRANS:
      OLED_DrawStr(97, 0, "W4TX ");
      break;
    case ESP8266_UART_WAIT4ACK:
      OLED_DrawStr(97, 0, "W4ACK");
      break;
    case ESP8266_ACK_OK:
      OLED_DrawStr(97, 0, "OK   ");
      break;
    case ESP8266_ACK_ERROR:
      OLED_DrawStr(97, 0, "Error");
      break;
    default:
      OLED_DrawStr(97, 0, "TIMEO");
      break;
    }
    HAL_Delay(10);
    if (HTTP_GET == 1)
    {
      HTTP_GET = 0;
      uint8_t a[60];
      HAL_Delay(100);
      ESP8266_UART_status = ESP8266_TIMEOUT;
      while (ESP8266_UART_status != ESP8266_UART_READY)
      {
        ESP8266_AT(&huart2, ATCMD_TEST);
      }
      sprintf((char *)a, "HTTP/1.1 404 NOT FOUND\r\nConnection: close\r\n\r\nNOT FOUND\r\n");
      FS_TRANS(a, strlen((const char *)a));
      HAL_Delay(100);
      ESP8266_AT(&huart2, ATCMD_TCP_CLOSE);
    }
    else if (HTTP_GET == 6)
    {
      HAL_Delay(500);
      ESP8266_UART_status = ESP8266_TIMEOUT;
      while (ESP8266_UART_status != ESP8266_UART_READY)
      {
        ESP8266_AT(&huart2, ATCMD_TEST);
      }
      HTTP_GET = 0;
      uint8_t a[200];
      sprintf((char *)a, "HTTP/1.1 200 OK\r\nAccept-Ranges: bytes\r\nContent-Length: %d\r\nContent-Type: text/html; charset=utf-8\r\nContent-Encoding: gzip\r\n\r\n", (int)fatfs_size("gz/asd.gz"));
      FS_TRANS(a, strlen((const char *)a));
      fatfs_open("gz/asd.gz");
      ESP8266_AT(&huart2, ATCMD_TCP_CLOSE);
    }
    else if (HTTP_GET == 2)
    {
      HTTP_GET = 0;
      uint8_t a[200];
      sprintf((char *)a, "HTTP/1.1 200 OK\r\nAccept-Ranges: bytes\r\nContent-Length: %d\r\nContent-Type: text/css; charset=utf-8\r\n\r\n", (int)fatfs_size("index.css"));
      FS_TRANS(a, strlen((const char *)a));
      fatfs_open("index.css");
    }
    else if (HTTP_GET == 3)
    {
      HTTP_GET = 0;
      uint8_t a[200];
      sprintf((char *)a, "HTTP/1.1 200 OK\r\nAccept-Ranges: bytes\r\nContent-Length: %d\r\nContent-Type: application/json; charset=utf-8\r\n\r\n", (int)fatfs_size("index.js"));
      FS_TRANS(a, strlen((const char *)a));
      fatfs_open("index.js");
    }
    else if (HTTP_GET == 4)
    {
      HTTP_GET = 0;
      uint8_t a[200];
      sprintf((char *)a, "HTTP/1.1 200 OK\r\nAccept-Ranges: bytes\r\nContent-Length: %d\r\nContent-Type: application/json; charset=utf-8\r\n\r\n", (int)fatfs_size("min.js"));
      FS_TRANS(a, strlen((const char *)a));
      fatfs_open("min.js");
    }
    else if (HTTP_GET == 5)
    {
      HTTP_GET = 0;
      uint8_t a[127];
      sprintf((char *)a,
              "HTTP/1.1 200 OK\r\nAccept-Ranges: bytes\r\nContent-Length: 2\r\nContent-Type: application/x-www-form-urlencoded; charset=utf-8\r\n\r\nOK");
      FS_TRANS(a, strlen((const char *)a));
    }
    else if (HTTP_GET == 7)
    {
      HTTP_GET = 0;
      uint8_t a[127];
      sprintf((char *)a,
              "HTTP/1.1 200 OK\r\nAccept-Ranges: bytes\r\nContent-Length: 2\r\nContent-Type: application/x-www-form-urlencoded; charset=utf-8\r\n\r\nOK");
      FS_TRANS(a, strlen((const char *)a));
    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV2;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC|RCC_PERIPHCLK_USB;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV4;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM2 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM2) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */
  if (htim == &htim1)
  {
    HAL_TIM_Base_Stop_IT(&htim1);
    extern GUI_InputKey press_key;
    static GUI_InputKey long_press = KEY_NONE;
    if (long_press == KEY_NONE)
    {
      long_press = press_key;
      press_key = KEY_NONE;
      htim1.Instance->ARR = 10000;
      HAL_TIM_Base_Start_IT(&htim1);
    }
    else
    {
      if (long_press == KEY_UP)
      {
        if (HAL_GPIO_ReadPin(key1_GPIO_Port, key1_Pin) == GPIO_PIN_RESET)
        {
          gui_key = long_press;
          htim1.Instance->ARR = 10000;
          HAL_TIM_Base_Start_IT(&htim1);
        }
        else
        {
          HAL_TIM_Base_Stop_IT(&htim1);
          htim1.Instance->ARR = 65535;
          long_press = KEY_NONE;
        }
      }
      else if (long_press == KEY_DOWN)
      {
        if (HAL_GPIO_ReadPin(key2_GPIO_Port, key2_Pin) == GPIO_PIN_RESET)
        {
          gui_key = long_press;
          htim1.Instance->ARR = 10000;
          HAL_TIM_Base_Start_IT(&htim1);
        }
        else
        {
          HAL_TIM_Base_Stop_IT(&htim1);
          htim1.Instance->ARR = 65535;
          long_press = KEY_NONE;
        }
      }
      else if (long_press == KEY_BACK)
      {
        if (HAL_GPIO_ReadPin(key3_GPIO_Port, key3_Pin) == GPIO_PIN_RESET)
        {
          gui_key = long_press;
          htim1.Instance->ARR = 5000;
          HAL_TIM_Base_Start_IT(&htim1);
        }
        else
        {
          HAL_TIM_Base_Stop_IT(&htim1);
          htim1.Instance->ARR = 65535;
          long_press = KEY_NONE;
        }
      }
      else if (long_press == KEY_OK)
      {
        if (HAL_GPIO_ReadPin(key4_GPIO_Port, key4_Pin) == GPIO_PIN_RESET)
        {
          gui_key = long_press;
          htim1.Instance->ARR = 2000;
          HAL_TIM_Base_Start_IT(&htim1);
        }
        else
        {
          HAL_TIM_Base_Stop_IT(&htim1);
          htim1.Instance->ARR = 65535;
          long_press = KEY_NONE;
        }
      }
    }
  }
  else if (htim == &htim4)
  {
    if (gui_key == KEY_NONE && OLED_TRANSMIT == 0)
    {
      OLED_TRANSMIT = 1;
      OLED_Reflash();
    }
  }
  else if (htim == &htim5)
  {
    ws2811_refresh();
    htim3.Instance->CCR1 = rgb[0].R;
    htim3.Instance->CCR2 = rgb[0].G;
    htim3.Instance->CCR3 = rgb[0].B;
  }
  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
