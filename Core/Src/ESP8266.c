#include "ESP8266.h"
#include "stdlib.h"

extern uint8_t HTTP_GET;

#define ESP8266_ACK_TIME 1000
#define MAX_TRANSMIT_COUNT 512

const uint8_t AT_RST[] = {"AT+RST\r\n"};                                                               // 重启
const uint8_t AT_TEST[] = {"AT\r\n"};                                                                  // 测试AT
const uint8_t AT_UART[] = {"AT+UART_CUR=256000,8,1,0,0\r\n"};                                          // 设置波特率
const uint8_t AT_RETURN_OFF[] = {"ATE0\r\n"};                                                          // 关闭回显
const uint8_t AT_OPEN_SOFTAP[] = {"AT+CWMODE=2\r\n"};                                                  // 开启SoftAP
const uint8_t AT_SOFTAPconfig[] = {"AT+CWSAP=\"Wireless LED\",\"12345678\",5,3,1\r\n"};                // 设置SoftAp
const uint8_t AT_SOFTAPdisconn[] = {"AT+CWQIF\r\n"};                                                   // 断开所有连入 ESP32 SoftAP 的 station
const uint8_t AT_TCP_CLOSE[] = {"AT+CIPCLOSE=0\r\n"};                                                  // 关闭 TCP/UDP/SSL 连接
const uint8_t AT_AP_IPconfig[] = {"AT+CIPAP=\"192.168.10.1\",\"192.168.10.1\",\"255.255.255.0\"\r\n"}; // 设置 ESP32 SoftAP 的 IPv4 地址
const uint8_t AT_TRANSMODE[] = {"AT+CIPMODE=0\r\n"};                                                   // 普通传输模式
const uint8_t AT_CONNMUX[] = {"AT+CIPMUX=1\r\n"};                                                      // 设置连接模式为多连接
const uint8_t AT_TCP_SERVER_MAXconn[] = {"AT+CIPSERVERMAXCONN=1\r\n"};                                 // 设置服务器最大连接数为1（禁止并发）
const uint8_t AT_TCP_SERVER_OPEN[] = {"AT+CIPSERVER=1,80\r\n"};                                        // 设置服务器端口为80（HTTP）
const uint8_t AT_TCP_TIMEOUT[] = {"AT+CIPSTO=5\r\n"};                                                  // 设置超时时间20s
const uint8_t AT_TCP_SERVER_CLOSE[] = {"AT+CIPSERVER=0,1\r\n"};                                        // 关闭服务器并断开所有连接
const uint8_t AT_TCP_SERVER_TRANSMIT[] = {"AT+CIPSEND=0,512\r\n"};

volatile ESP8266_UART_STATUS ESP8266_UART_status = ESP8266_TIMEOUT;
volatile AT_CMD ESP8266_ACK_status = ATCMD_RST;

const uint8_t *AT_allCMD_pointBuf[] = {
    AT_RST,
    AT_TEST,
    AT_UART,
    AT_RETURN_OFF,
    AT_OPEN_SOFTAP,
    AT_SOFTAPconfig,
    AT_SOFTAPdisconn,
    AT_TCP_CLOSE,
    AT_AP_IPconfig,
    AT_TRANSMODE,
    AT_CONNMUX,
    AT_TCP_SERVER_MAXconn,
    AT_TCP_SERVER_OPEN,
    AT_TCP_TIMEOUT,
    AT_TCP_SERVER_CLOSE,
    AT_TCP_SERVER_TRANSMIT,
};

void ESP8266_EN()
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
}

void ESP8266_DE()
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
}

void ESP8266_Receive(uint8_t *rx_buf, uint16_t size)
{
    if (ESP8266_ACK_status)
    {
        for (uint16_t i = 0; i < size; i++)
        {
            if (strncmp("OK", (const char *)&rx_buf[i], 2) == 0)
            {
                ESP8266_ACK_status = ATCMD_RST;
                ESP8266_UART_status = ESP8266_ACK_OK;
                break;
            }
        }
    }
    else
    {
        if (strncmp("\r\n+IPD,0,", (const char *)rx_buf, 7) == 0)
        {
            for (uint16_t i = 0; i < 100; i++)
            {
                if (strncmp("GET / HTTP/1.1", (const char *)&rx_buf[i], 14) == 0)
                {
                    HTTP_GET = 1;
                    break;
                }
                else if (strncmp("GET /index.css HTTP/1.1", (const char *)&rx_buf[i], 23) == 0)
                {
                    HTTP_GET = 2;
                    break;
                }
                else if (strncmp("GET /index.js HTTP/1.1", (const char *)&rx_buf[i], 22) == 0)
                {
                    HTTP_GET = 3;
                    break;
                }
                else if (strncmp("GET /min.js HTTP/1.1", (const char *)&rx_buf[i], 20) == 0)
                {
                    HTTP_GET = 4;
                    break;
                }
                else if (strncmp("GET /gz/asd.htm HTTP/1.1", (const char *)&rx_buf[i], 24) == 0)
                {
                    HTTP_GET = 6;
                    break;
                }
                else if (strncmp("POST /gz/asd.htm/MODE", (const char *)&rx_buf[i], 21) == 0)
                {
                    for (uint16_t j = i; j < 1000; j++)
                    {
                        if (strncmp("\r\n\r\nmode=", (const char *)&rx_buf[j], 9) == 0)
                        {
                            HTTP_GET = 7;
                            uint8_t a = rx_buf[j + 9];
                            rgb_mode = (uint8_t)atoi((const char *)&a);
                            break;
                        }
                    }
                }
                else if (strncmp("POST /gz/asd.htm/RGB", (const char *)&rx_buf[i], 20) == 0)
                {
                    uint8_t Rbuf[4] = {0}, Gbuf[4] = {0}, Bbuf[4] = {0};
                    for (uint16_t j = i; j < 1000; j++)
                    {
                        if (strncmp("\r\n\r\nr=", (const char *)&rx_buf[j], 6) == 0)
                        {
                            HTTP_GET = 5;
                            for (uint8_t k = 0; k < 3; k++)
                            {
                                if (rx_buf[j + 6 + k] == '&')
                                    break;
                                Rbuf[k] = rx_buf[j + 6 + k];
                            }
                            rgb[0].R = (uint8_t)atoi((const char *)Rbuf);
                        }
                        if (strncmp("&g=", (const char *)&rx_buf[j], 3) == 0)
                        {
                            for (uint8_t k = 0; k < 3; k++)
                            {
                                if (rx_buf[j + 3 + k] == '&')
                                    break;
                                Gbuf[k] = rx_buf[j + 3 + k];
                            }
                            rgb[0].G = (uint8_t)atoi((const char *)Gbuf);
                        }
                        if (strncmp("b=", (const char *)&rx_buf[j], 2) == 0)
                        {
                            for (uint8_t k = 0; k < 3; k++)
                            {
                                if (rx_buf[j + 2 + k] == '\r')
                                    break;
                                Bbuf[k] = rx_buf[j + 2 + k];
                            }
                            rgb[0].B = (uint8_t)atoi((const char *)Bbuf);
                            break;
                        }
                    }
                    // HAL_UART_Transmit_DMA(&huart2,"AT+CIPSEND=0,126\r\nHTTP/1.1 200 OK\r\nAccept-Ranges: bytes\r\nContent-Length: 2\r\nContent-Type: application/x-www-form-urlencoded; charset=utf-8\r\n\r\nOK",144);
                    break;
                }
            }
        }
        else if (strncmp("ERROR", (const char *)rx_buf, 7) == 0)
        {
            ESP8266_ACK_status = ATCMD_RST;
            ESP8266_UART_status = ESP8266_ACK_ERROR;
        }
    }
}

void ESP8266_AT(UART_HandleTypeDef *huart, AT_CMD cmd)
{
    ESP8266_UART_status = ESP8266_UART_WAIT4TRANS;
    ESP8266_ACK_status = cmd;
    HAL_UART_Transmit_DMA(huart, (uint8_t *)AT_allCMD_pointBuf[cmd], (uint16_t)strlen((const char *)AT_allCMD_pointBuf[cmd]));
    uint16_t i = 0;
    while (ESP8266_UART_status == ESP8266_UART_WAIT4TRANS ||
           ESP8266_UART_status == ESP8266_UART_WAIT4ACK)
    {
        if (i++ > ESP8266_ACK_TIME)
        {
            ESP8266_UART_status = ESP8266_TIMEOUT;
            break;
        }
        HAL_Delay(1);
    }
    if (ESP8266_UART_status == ESP8266_ACK_OK)
        ESP8266_UART_status = ESP8266_UART_READY;
}

void ESP8266_Init()
{
    ESP8266_DE();
    HAL_Delay(10);
    ESP8266_EN();
    HAL_Delay(500);
    ESP8266_AT(&huart2, ATCMD_TEST);
    // ESP8266_AT(&huart2,ATCMD_UART);
    // huart2.Init.BaudRate=256000;
    // HAL_UART_Init(&huart2);
    // HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx2_buf, sizeof(rx2_buf));
}

void ESP8266_HTTP_ON()
{
    ESP8266_AT(&huart2, ATCMD_OPEN_SOFTAP);
    ESP8266_AT(&huart2, ATCMD_SOFTAPconfig);
    ESP8266_AT(&huart2, ATCMD_AP_IPconfig);
    ESP8266_AT(&huart2, ATCMD_TRANSMODE);
    ESP8266_AT(&huart2, ATCMD_CONNMUX);
    ESP8266_AT(&huart2, ATCMD_TCP_SERVER_MAXconn);
    ESP8266_AT(&huart2, ATCMD_TCP_SERVER_OPEN);
    ESP8266_AT(&huart2, ATCMD_TCP_TIMEOUT);
}
