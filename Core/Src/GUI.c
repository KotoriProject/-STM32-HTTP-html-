#include "GUI.h"
#include "ESP8266.h"
#include "gpio.h"
#include "string.h"
#include "usart.h"
#include "QR_Encode.h"
#include "stdlib.h"
static page *now_page;

volatile uint16_t GUI_REFRESH_BUSY = 0;

void GUI_RGB_Rconf_FUNC()
{
    static uint8_t Red = 0;
    Red++;
    rgb[0].R = Red;
    now_page->items[0].value_text[0] = Red / 100 + 48;
    now_page->items[0].value_text[1] = Red / 10 % 10 + 48;
    now_page->items[0].value_text[2] = Red % 10 + 48;
    OLED_DrawStr(109, 8, now_page->items[0].value_text);
}
void GUI_RGB_Gconf_FUNC()
{
    static uint8_t Green = 0;
    Green++;
    rgb[0].G = Green;
    now_page->items[1].value_text[0] = Green / 100 + 48;
    now_page->items[1].value_text[1] = Green / 10 % 10 + 48;
    now_page->items[1].value_text[2] = Green % 10 + 48;
    OLED_DrawStr(109, 16, now_page->items[1].value_text);
}
void GUI_RGB_Bconf_FUNC()
{
    static uint8_t Blue = 0;
    Blue++;
    rgb[0].B = Blue;
    now_page->items[2].value_text[0] = Blue / 100 + 48;
    now_page->items[2].value_text[1] = Blue / 10 % 10 + 48;
    now_page->items[2].value_text[2] = Blue % 10 + 48;
    OLED_DrawStr(109, 24, now_page->items[2].value_text);
}
void GUI_RGB_MODE_FUNC()
{
    rgb_mode++;
    if (rgb_mode > 2)
        rgb_mode = 0;
    now_page->items[3].value_text[0] = rgb_mode + 48;
    OLED_DrawStr(109, 32, now_page->items[3].value_text);
}
void GUI_RGB_ORDER_FUNC()
{
    RGB_ORDER++;
    if (RGB_ORDER > 6)
        RGB_ORDER = 0;
    now_page->items[4].value_text[0] = RGB_ORDER + 48;
    OLED_DrawStr(109, 40, now_page->items[4].value_text);
}
void GUI_Volconf_FUNC()
{
    static uint8_t voltage = 0;
    if (voltage++ > 4)
        voltage = 0;
    Voltage_Config(voltage);
    switch (voltage)
    {
    case 0:
        memcpy(now_page->items[0].value_text, "5V ", 3);
        break;
    case 1:
        memcpy(now_page->items[0].value_text, "9V ", 3);
        break;
    case 2:
        memcpy(now_page->items[0].value_text, "12V", 3);
        break;
    case 3:
        memcpy(now_page->items[0].value_text, "15V", 3);
        break;
    case 4:
        memcpy(now_page->items[0].value_text, "20V", 3);
        break;
    default:
        break;
    }
    OLED_DrawStr(109, 8, now_page->items[0].value_text);
}
void GUI_WIFIEN_FUNC()
{
    static uint8_t ESP_EN = 1;
    ESP_EN ^= 1;
    if (ESP_EN)
    {
        ESP8266_EN();
        memcpy(now_page->items[0].value_text, "on ", 3);
    }
    else
    {
        ESP8266_DE();
        memcpy(now_page->items[0].value_text, "off", 3);
    }
    OLED_DrawStr(109, 8, now_page->items[0].value_text);
}

void GUI_ESP8266_AT()
{
    ESP8266_AT(&huart2, ATCMD_TEST);
}
void GUI_ESP8266_ReturnOff()
{
    ESP8266_AT(&huart2, ATCMD_RETURN_OFF);
}
void GUI_ESP8266_OpenAP()
{
    ESP8266_AT(&huart2, ATCMD_OPEN_SOFTAP);
}
void GUI_ESP8266_APConfig()
{
    ESP8266_AT(&huart2, ATCMD_SOFTAPconfig);
}
void WIFI_QR()
{
    uint8_t code1[64][64] = {0};
    extern uint8_t OLED_buffer[8][128];
    // uint8_t *code;
    // code = malloc((int)(m_nSymbleSize * (m_nSymbleSize / 8 + 0.5)));
    OLED_Fill(0);
    if (EncodeData("WIFI:T:WPA;P:12345678;S:Wireless LED;H:false;") == 1)
    {
        for (uint8_t i = 0; i < m_nSymbleSize; i++) // 行
        {
            for (uint16_t j = 0; j < m_nSymbleSize; j++) // 列
            {
                code1[i * 2][j * 2] = m_byModuleData[i][j];
                code1[i * 2][j * 2 + 1] = m_byModuleData[i][j];
                code1[i * 2 + 1][j * 2] = m_byModuleData[i][j];
                code1[i * 2 + 1][j * 2 + 1] = m_byModuleData[i][j];
            }
        }
        for (uint8_t i = 0; i < 8; i++) // 每页
        {
            for (uint16_t j = 0; j < 63; j++) // 每列
            {
                for (uint8_t k = 0; k < 8; k++)
                {
                    OLED_buffer[i][j] |= code1[i * 8 + k][j] << (k);
                }
            }
        }
    }

    // OLED_DrawBMP(0, 0, m_nSymbleSize, m_nSymbleSize, code);
    // free(code);
}
void HTML_QR()
{
    uint8_t code1[64][64] = {0};
    extern uint8_t OLED_buffer[8][128];
    // uint8_t *code;
    // code = malloc((int)(m_nSymbleSize * (m_nSymbleSize / 8 + 0.5)));
    OLED_Fill(0);
    if (EncodeData("http://192.168.10.1/gz/asd.htm") == 1)
    {
        for (uint8_t i = 0; i < m_nSymbleSize; i++) // 行
        {
            for (uint16_t j = 0; j < m_nSymbleSize; j++) // 列
            {
                code1[i * 2][j * 2] = m_byModuleData[i][j];
                code1[i * 2][j * 2 + 1] = m_byModuleData[i][j];
                code1[i * 2 + 1][j * 2] = m_byModuleData[i][j];
                code1[i * 2 + 1][j * 2 + 1] = m_byModuleData[i][j];
            }
        }
        for (uint8_t i = 0; i < 8; i++) // 每页
        {
            for (uint16_t j = 0; j < 63; j++) // 每列
            {
                for (uint8_t k = 0; k < 8; k++)
                {
                    OLED_buffer[i][j] |= code1[i * 8 + k][j] << (k);
                }
            }
        }
    }

    // OLED_DrawBMP(0, 0, m_nSymbleSize, m_nSymbleSize, code);
    // free(code);
}
void HTML1_QR()
{
    uint8_t code[64][64] = {0};
    extern uint8_t OLED_buffer[8][128];
    // uint8_t *code;
    // code = malloc((int)(m_nSymbleSize * (m_nSymbleSize / 8 + 0.5)));
    OLED_Fill(0);
    if (EncodeData("http://192.168.10.1") == 1)
    {
        for (uint8_t i = 0; i < 21; i++) // 行
        {
            for (uint16_t j = 0; j < 21; j++) // 列
            {
                code[i * 3][j * 3] = m_byModuleData[i][j];
                code[i * 3][j * 3 + 1] = m_byModuleData[i][j];
                code[i * 3][j * 3 + 2] = m_byModuleData[i][j];
                code[i * 3 + 1][j * 3] = m_byModuleData[i][j];
                code[i * 3 + 1][j * 3 + 1] = m_byModuleData[i][j];
                code[i * 3 + 1][j * 3 + 2] = m_byModuleData[i][j];
                code[i * 3 + 2][j * 3] = m_byModuleData[i][j];
                code[i * 3 + 2][j * 3 + 1] = m_byModuleData[i][j];
                code[i * 3 + 2][j * 3 + 2] = m_byModuleData[i][j];
            }
        }                               /*
                                           for (uint8_t i = 0; i <= m_nSymbleSize / 8 * 2 + 1; i++) // 每页
                                           {
                                               for (uint16_t j = 0; j < m_nSymbleSize; j++) // 每列
                                               {
                                                   for (uint8_t k = 0; k < 4; k++)
                                                   {
                                                       OLED_buffer[i][j * 2] |= m_byModuleData[i * 8 / 2 + k][j] << (k * 2);
                                                       OLED_buffer[i][j * 2 + 1] |= m_byModuleData[i * 8 / 2 + k][j] << (k * 2);
                                                       OLED_buffer[i][j * 2] |= m_byModuleData[i * 8 / 2 + k][j] << (k * 2 + 1);
                                                       OLED_buffer[i][j * 2 + 1] |= m_byModuleData[i * 8 / 2 + k][j] << (k * 2 + 1);
                                                   }
                                               }
                                           }*/
        for (uint8_t i = 0; i < 8; i++) // 每页
        {
            for (uint16_t j = 0; j < 63; j++) // 每列
            {
                for (uint8_t k = 0; k < 8; k++)
                {
                    OLED_buffer[i][j] |= code[i * 8 + k][j] << (k);
                }
            }
        }
    }

    // OLED_DrawBMP(0, 0, m_nSymbleSize, m_nSymbleSize, code);
    // free(code);
}

enum
{
    GUI_PAGE_WELCOME,
    GUI_PAGE_HOME,
    GUI_PAGE_WIFICONF,
    GUI_PAGE_RGBCONF,
    GUI_PAGE_VOLCONF,
    GUI_PAGE_AT,
    GUI_PAGE_ATTCP,
};
page pages[10] = {
    [GUI_PAGE_WELCOME] = {
        .page_title = "Wireless LED Controller",
        .last_page = &pages[GUI_PAGE_WELCOME],
    },
    [1] = {
        .page_title = "Welcome",
        .items = {
            [0] = {
                .text = "WIFI config",
                .item_type = item_type_page,
                .value = &pages[GUI_PAGE_WIFICONF],
            },
            [1] = {
                .text = "RGB config",
                .item_type = item_type_page,
                .value = &pages[GUI_PAGE_RGBCONF],
            },
            [2] = {
                .text = "Voltage config",
                .item_type = item_type_page,
                .value = &pages[GUI_PAGE_VOLCONF],
            },
        },
        .last_page = &pages[GUI_PAGE_WELCOME],
    },
    [2] = {
        .page_title = "WIFI conf",
        .items[0] = {
            .text = "WIFI switch",
            .item_type = item_type_func,
            .value = GUI_WIFIEN_FUNC,
            .value_text = "on",
        },
        .items[1] = {
            .text = "AT TEST",
            .item_type = item_type_page,
            .value = &pages[GUI_PAGE_AT],
        },
        .items[2] = {
            .text = "WIFI QR",
            .item_type = item_type_func,
            .value = WIFI_QR,
        },
        .items[3] = {
            .text = "HTML QR",
            .item_type = item_type_func,
            .value = HTML_QR,
        },
        .last_page = &pages[GUI_PAGE_HOME],
    },
    [3] = {
        .page_title = "RGB conf",
        .items = {
            [0] = {
                .text = "RED",
                .item_type = item_type_func,
                .value = GUI_RGB_Rconf_FUNC,
                .value_text = "000",
            },
            [1] = {
                .text = "GREEN",
                .item_type = item_type_func,
                .value = GUI_RGB_Gconf_FUNC,
                .value_text = "000",
            },
            [2] = {
                .text = "BLUE",
                .item_type = item_type_func,
                .value = GUI_RGB_Bconf_FUNC,
                .value_text = "000",
            },
            [3] = {
                .text = "MODE",
                .item_type = item_type_func,
                .value = GUI_RGB_MODE_FUNC,
                .value_text = "0",
            },
            [4] = {
                .text = "RGB ORDER",
                .item_type = item_type_func,
                .value = GUI_RGB_ORDER_FUNC,
                .value_text = "0",
            },
        },
        .last_page = &pages[GUI_PAGE_HOME],
    },
    [4] = {
        .page_title = "Voltage conf",
        .items = {
            [0] = {
                .text = "Voltage set",
                .item_type = item_type_func,
                .value = GUI_Volconf_FUNC,
                .value_text = "5V",
            },
        },
        .last_page = &pages[GUI_PAGE_HOME],
    },
    [5] = {
        .page_title = "AT TEST",
        .items = {
            [0] = {
                .text = "AT",
                .item_type = item_type_func,
                .value = GUI_ESP8266_AT,
            },
            [1] = {
                .text = "return off",
                .item_type = item_type_func,
                .value = GUI_ESP8266_ReturnOff,
            },
            [2] = {
                .text = "open AP",
                .item_type = item_type_func,
                .value = GUI_ESP8266_OpenAP,
            },
            [3] = {
                .text = "AP config",
                .item_type = item_type_func,
                .value = GUI_ESP8266_APConfig,
            },
            [4] = {
                .text = "AP disconnect",
                .item_type = item_type_func,
                .value = GUI_ESP8266_AT,
            },
            [5] = {
                .text = "AP IPconfig",
                .item_type = item_type_func,
                .value = GUI_ESP8266_AT,
            },
            [6] = {
                .text = "AT TCP",
                .item_type = item_type_page,
                .value = &pages[GUI_PAGE_ATTCP],
            },
        },
        .last_page = &pages[GUI_PAGE_WIFICONF],
    },
    [6] = {
        .page_title = "AT TCP",
        .items = {
            [0] = {
                .text = "transmode 1",
                .item_type = item_type_func,
                .value = GUI_ESP8266_AT,
            },
            [1] = {
                .text = "connect mux",
                .item_type = item_type_func,
                .value = GUI_ESP8266_AT,
            },
            [2] = {
                .text = "Server maxConnect",
                .item_type = item_type_func,
                .value = GUI_ESP8266_AT,
            },
            [3] = {
                .text = "Server open",
                .item_type = item_type_func,
                .value = GUI_ESP8266_AT,
            },
            [4] = {
                .text = "TCP timeout",
                .item_type = item_type_func,
                .value = GUI_ESP8266_AT,
            },
            [5] = {
                .text = "Server close",
                .item_type = item_type_func,
                .value = GUI_ESP8266_AT,
            },
            [6] = {
                .text = "Server Transmit",
                .item_type = item_type_func,
                .value = GUI_ESP8266_AT,
            },
        },
        .last_page = &pages[GUI_PAGE_AT],
    },
};
static page *now_page = &pages[GUI_PAGE_WELCOME]; // 当前页

GUI_StatusTypeDef GUI_Refresh(GUI_InputKey key)
{
    if (now_page == &pages[0])
    {
        if (key == KEY_NONE)
        {
            OLED_Fill(0);
            OLED_DrawStr(0, 32, now_page->page_title);
        }
        else if (key == KEY_OK)
        {
            now_page = &pages[GUI_PAGE_HOME];
            GUI_Refresh(KEY_NONE);
        }
    }
    else
    {
        if (key == KEY_NONE)
        {
            OLED_Fill(0);
            OLED_DrawStr(0, 0, now_page->page_title);
            for (uint8_t i = 0; i < 7; i++)
            {
                if (now_page->items[i].text != 0)
                {
                    OLED_DrawStr(10, 8 * (i + 1), now_page->items[i].text);
                    if (now_page->items[i].value_text != 0)
                        OLED_DrawStr(109, 8 * (i + 1), now_page->items[i].value_text);
                }
            }
            OLED_ChooseStr(7, 8 + 8 * now_page->choose, now_page->items[now_page->choose].text);
        }
        else if (key == KEY_OK)
        {
            if (now_page->items[now_page->choose].item_type == item_type_func)
            {
                void (*func)() = (void (*)())now_page->items[now_page->choose].value;
                func();
            }
            else if (now_page->items[now_page->choose].item_type == item_type_page)
            {
                now_page = now_page->items[now_page->choose].value;
                OLED_Fill(0);
                GUI_Refresh(KEY_NONE);
            }
        }
        else if (key == KEY_BACK)
        {
            now_page = now_page->last_page;
            OLED_Fill(0);
            GUI_Refresh(KEY_NONE);
        }
        else if (key == KEY_DOWN)
        {
            now_page->choose++;
            if (now_page->items[now_page->choose].text == 0 || now_page->choose == 7)
                now_page->choose = 0;
            GUI_Refresh(KEY_NONE);
        }
        else if (key == KEY_UP)
        {
            if (now_page->choose == 0)
            {
                for (uint8_t i = 7; i != 0; i--)
                {
                    if (now_page->items[i].text != 0)
                    {
                        now_page->choose = i;
                        break;
                    }
                }
            }
            else
                now_page->choose--;

            GUI_Refresh(KEY_NONE);
        }
    }
    return GUI_OK;
}
