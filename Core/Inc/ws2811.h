#ifndef _WS2811_
#define _WS2811_

#include "stm32f1xx_hal.h"

#define WS2811_COUNT 108

typedef struct
{
    uint8_t R;
    uint8_t G;
    uint8_t B;
} RGB;

enum ws2811
{
    ws2811_OK,
    ws2811_ERROR
};

void ws2811_refresh(void);
void ws2811_Init(void);
extern RGB rgb[WS2811_COUNT+1];
extern volatile uint8_t rgb_mode;
extern volatile uint8_t RGB_ORDER;
#endif
