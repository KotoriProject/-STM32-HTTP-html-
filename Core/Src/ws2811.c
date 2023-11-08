#include "ws2811.h"

#define WS2811Bit_0 0xC0
#define WS2811Bit_1 0xFC

extern SPI_HandleTypeDef hspi2;

volatile uint8_t RGB_ORDER = 0;
uint8_t RGB_spiData[24 * WS2811_COUNT] = {0};

void RGBtoSPI(uint8_t *index, RGB *rgb)
{
    if (RGB_ORDER == 0)
    {
        for (uint8_t i = 0; i < 8; i++)
        {
            if (rgb->R & 0x80 >> i)
                index[i] = WS2811Bit_1;
            else
                index[i] = WS2811Bit_0;
            if (rgb->G & 0x80 >> i)
                index[i + 8] = WS2811Bit_1;
            else
                index[i + 8] = WS2811Bit_0;
            if (rgb->B & 0x80 >> i)
                index[i + 16] = WS2811Bit_1;
            else
                index[i + 16] = WS2811Bit_0;
        }
    }
    else if (RGB_ORDER == 1)
    {
        for (uint8_t i = 0; i < 8; i++)
        {
            if (rgb->G & 0x80 >> i)
                index[i] = WS2811Bit_1;
            else
                index[i] = WS2811Bit_0;
            if (rgb->R & 0x80 >> i)
                index[i + 8] = WS2811Bit_1;
            else
                index[i + 8] = WS2811Bit_0;
            if (rgb->B & 0x80 >> i)
                index[i + 16] = WS2811Bit_1;
            else
                index[i + 16] = WS2811Bit_0;
        }
    }
}
void ws2811_Init()
{
    uint8_t reset[80] = {0};
    if (HAL_SPI_Transmit(&hspi2, reset, 80, 20) != HAL_OK)
        while (1)
            ;
    for (uint8_t i = 0; i < WS2811_COUNT; i++)
    {
        RGBtoSPI(&RGB_spiData[i * 24], &rgb[i + 1]);
    }
    HAL_SPI_Transmit(&hspi2, RGB_spiData, sizeof(RGB_spiData), 10);
}

RGB rgb[WS2811_COUNT + 1] = {[0] = {.R = 0x00,
                                    .G = 0x00,
                                    .B = 0x00},
                             [1] = {.R = 0x00,
                                    .G = 0x00,
                                    .B = 0x00},
                             [2] = {.R = 0x00,
                                    .G = 0x00,
                                    .B = 0x00}};
volatile uint8_t rgb_mode = 0;

void ws2811_breath()
{
    uint8_t reset[80] = {0};
    if (HAL_SPI_Transmit(&hspi2, reset, 80, 20) != HAL_OK)
        // while (1)
        ;
    static int direct = 1;
    static uint8_t add = 0;
    if (direct > 0)
    {
        if (rgb[add].R == 0xff)
        {
            direct = -1;
        }
        rgb[add].R += direct;
    }
    else
    {
        if (rgb[add].R == 0x00)
        {
            direct = 1;
            add++;
            if (add > WS2811_COUNT)
                add = 1;
        }
        rgb[add].R += direct;
    }
    RGBtoSPI(&RGB_spiData[add - 1 * 24], &rgb[add]);
    HAL_SPI_Transmit_DMA(&hspi2, RGB_spiData, sizeof(RGB_spiData));
}

void ws2811_refresh()
{
    // uint8_t reset[80] = {0};
    // if (HAL_SPI_Transmit(&hspi2, reset, 80, 20) != HAL_OK)
    //  while (1)
    ;
    static RGB all_rgb = {0};
    if (rgb_mode == 0)
    {
        if ((rgb[0].R != all_rgb.R) || (rgb[0].G != all_rgb.G) || (rgb[0].B != all_rgb.B))
        {

            if (all_rgb.R > rgb[0].R && all_rgb.R != 0)
                all_rgb.R -= (all_rgb.R - rgb[0].R) * 0.2 + 0.5;
            else if (all_rgb.R < rgb[0].R && all_rgb.R != 255)
                all_rgb.R += (rgb[0].R - all_rgb.R) * 0.2 + 0.5;
            if (all_rgb.G > rgb[0].G && all_rgb.G != 0)
                all_rgb.G -= (all_rgb.G - rgb[0].G) * 0.2 + 0.5;
            else if (all_rgb.G < rgb[0].G && all_rgb.G != 255)
                all_rgb.G += (rgb[0].G - all_rgb.G) * 0.2 + 0.5;
            if (all_rgb.B > rgb[0].B && all_rgb.B != 0)
                all_rgb.B -= (all_rgb.B - rgb[0].B) * 0.2 + 0.5;
            else if (all_rgb.B < rgb[0].B && all_rgb.B != 255)
                all_rgb.B += (rgb[0].B - all_rgb.B) * 0.2 + 0.5;
            for (uint8_t i = 1; i < WS2811_COUNT + 1; i++)
            {
                rgb[i].R = all_rgb.R;
                rgb[i].G = all_rgb.G;
                rgb[i].B = all_rgb.B;
                RGBtoSPI(&RGB_spiData[(i - 1) * 24], &rgb[i]);
            }
        }
    }
    else if (rgb_mode == 1)
    {
        if (rgb[0].R || rgb[0].G || rgb[0].B)
        {
            static uint8_t direct = 1;
            static float light = 0;
            if (direct)
                light += (float)1 / 255;
            else
                light -= (float)1 / 255;
            all_rgb.R = rgb[0].R * light + 0.5;
            all_rgb.G = rgb[0].G * light + 0.5;
            all_rgb.B = rgb[0].B * light + 0.5;
            if (light >= 1)
                direct = 0;
            else if (light <= 0)
                direct = 1;

            for (uint8_t i = 1; i < WS2811_COUNT + 1; i++)
            {
                rgb[i].R = all_rgb.R;
                rgb[i].G = all_rgb.G;
                rgb[i].B = all_rgb.B;
                RGBtoSPI(&RGB_spiData[(i - 1) * 24], &rgb[i]);
            }
        }
    }
    else if (rgb_mode == 2)
    {
        static uint16_t count = 0;
        if ((rgb[0].R || rgb[0].G || rgb[0].B) && count++ > 5)
        {
            count = 0;
            static uint8_t add = 0;
            if (add++ > WS2811_COUNT-1)
                add = 1;
            for (uint8_t i = 0; i <= 5; i++)
            {
                rgb[add + i > WS2811_COUNT ? add + i - WS2811_COUNT : add + i].R = rgb[0].R * (float)i / 5 + 0.5;
                rgb[add + i > WS2811_COUNT ? add + i - WS2811_COUNT : add + i].G = rgb[0].G * (float)i / 5 + 0.5;
                rgb[add + i > WS2811_COUNT ? add + i - WS2811_COUNT : add + i].B = rgb[0].B * (float)i / 5 + 0.5;
            }

            for (uint8_t i = 1; i < WS2811_COUNT + 1; i++)
            {
                RGBtoSPI(&RGB_spiData[(i - 1) * 24], &rgb[i]);
            }
        }
    }

    HAL_SPI_Transmit_DMA(&hspi2, RGB_spiData, sizeof(RGB_spiData));
}
