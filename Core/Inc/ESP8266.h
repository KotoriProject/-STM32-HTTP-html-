#ifndef _ESP8266_
#define _ESP8266_

#include "gpio.h"
#include "usart.h"
#include "string.h"
#include "stm32f1xx_hal.h"
#include "ws2811.h"

typedef enum
{
    ATCMD_RST,
    ATCMD_TEST,
    ATCMD_UART,
    ATCMD_RETURN_OFF,
    ATCMD_OPEN_SOFTAP,
    ATCMD_SOFTAPconfig,
    ATCMD_SOFTAPdisconn,
    ATCMD_TCP_CLOSE,
    ATCMD_AP_IPconfig,
    ATCMD_TRANSMODE,
    ATCMD_CONNMUX,
    ATCMD_TCP_SERVER_MAXconn,
    ATCMD_TCP_SERVER_OPEN,
    ATCMD_TCP_TIMEOUT,
    ATCMD_TCP_SERVER_CLOSE,
    ATCMD_TCP_SERVER_TRANSMIT,
} AT_CMD;

typedef enum
{
    ESP8266_UART_READY,
    ESP8266_UART_WAIT4TRANS,
    ESP8266_UART_WAIT4ACK,
    ESP8266_ACK_OK,
    ESP8266_ACK_ERROR,
    ESP8266_TIMEOUT,
} ESP8266_UART_STATUS;

extern const uint8_t *AT_allCMD_pointBuf[16];
extern volatile ESP8266_UART_STATUS ESP8266_UART_status;

void ESP8266_EN(void);
void ESP8266_DE(void);
void ESP8266_AT(UART_HandleTypeDef *huart, AT_CMD cmd);
void ESP8266_Init(void);
void ESP8266_Receive(uint8_t *rx_buf, uint16_t size);
void ESP8266_HTTP_ON(void);
#endif
