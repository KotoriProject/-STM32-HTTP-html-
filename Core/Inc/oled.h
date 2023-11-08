/*
 * @Author: Snitro
 * @Date: 2021-02-23 17:42:27
 * @LastEditors: Snitro
 * @LastEditTime: 2021-02-24 15:30:55
 * @Description: SSD1306驱动
 */
#ifndef __SSD1306_H
#define __SSD1306_H

// #define OLED_USING_SOFTWARE_I2C //使用�?件i2c

#include <string.h>

#include "stm32f1xx_hal.h" //�?改为所用型�?

// #define OLED_INVERSE_COLOR  // OLED反色显示
#define OLED_Addr 0x78 // OLED IIC 地址

#define CmdReg 0x00    // 表示发送指�?
#define DataReg 0x40   // 表示发送数�?
#define OLED_ON 0xaf   // 打开显示�?
#define OLED_OFF 0xae  // 关闭显示�?
#define OLED_Width 128 // OLED宽度128像素
#define OLED_High 64   // OLED高度64像素

#define OLED_BOOL_Replace ((uint8_t)0x00)  // 替换
#define OLED_BOOL_ADD ((uint8_t)0x01)      // �?
#define OLED_BOOL_Subtract ((uint8_t)0x02) // �?
#define OLED_BOOL_XOR ((uint8_t)0x03)      // 异或

void OLED_Init(void);

HAL_StatusTypeDef OLED_Write_Byte(uint8_t, uint8_t);
void OLED_Fill(uint8_t);
void OLED_Reflash(void);
void OLED_SetPos(uint8_t, uint8_t);
HAL_StatusTypeDef OLED_BOOL_DrawColumn(uint8_t, uint8_t, uint8_t *, uint8_t, uint8_t);       // 以布尔运算方式绘制不定长列
HAL_StatusTypeDef OLED_BOOL_DrawChar(uint8_t, uint8_t, uint8_t, uint8_t);                    // 以布尔运算方式绘制字符
HAL_StatusTypeDef OLED_BOOL_DrawStr(uint8_t, uint8_t, uint8_t *, uint8_t);                   // 以布尔运算方式绘制字符串
HAL_StatusTypeDef OLED_BOOL_DrawBMP(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t *, uint8_t); // 以布尔运算方式绘制位图
HAL_StatusTypeDef OLED_DrawColumn(uint8_t, uint8_t, uint8_t *, uint8_t);                     // 直接绘制不定长列
HAL_StatusTypeDef OLED_DrawChar(uint8_t, uint8_t, uint8_t);                                  // 直接绘制字符
HAL_StatusTypeDef OLED_DrawStr(uint8_t, uint8_t, uint8_t *);                                 // 直接绘制字符串
HAL_StatusTypeDef OLED_DrawNum(uint8_t, uint8_t, float);
HAL_StatusTypeDef OLED_DrawBMP(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t *); // 直接绘制位图
void OLED_ChooseStr(uint8_t x, uint8_t y, uint8_t *data);
uint8_t *numtoascii(uint32_t num);
#endif
