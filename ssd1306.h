#ifndef _SSD1306_H
#define _SSD1306_H
#include "stm32f10x.h"
#include "delay.h"
#include "usart.h"
#include "i2c.h"


#define SSD1306_I2C_ADDR        0x78
#define SSD1306_CMD_ADDR        0X00
#define SSD1306_DAT_ADDR        0X40



void SSD1306_Init(void);
void OLED_SetPos(uint8_t x, uint8_t y);
void OLED_DisplayOn(void);
void OLED_DisplayOff(void);



#define         MAX_COLUMN	  128
#define         MAX_ROW		  64
void OLED_Clear(void);
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size);
void OLED_ShowString(uint8_t x,uint8_t y,char *str,uint8_t Char_Size);//显示一个字符串
void OLED_ShowCN(uint8_t x,uint8_t y,uint8_t index);//显示汉字

void OLED_ShowLCN(uint8_t x,uint8_t y,uint8_t index);//显示小汉字
void OLED_ShowCNU(uint8_t x,uint8_t y,uint8_t index);//显示变化数字专用
void OLED_ShowCSNU(uint8_t x,uint8_t y,uint8_t integer,uint8_t decimal);


void SSD1306_Show_Change_Char(uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size);

#endif

