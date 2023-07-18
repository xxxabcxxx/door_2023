#ifndef LCD_H_
#define LCD_H_
#include <stdint.h>

#include "drv_gpio.h"
#include "ch32v30x.h"
#include <rtthread.h>
#include <rthw.h>
#include "drivers/pin.h"


#define USE_HORIZONTAL 3  


#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 128
#define LCD_H 128

#else
#define LCD_W 128
#define LCD_H 128
#endif

#define LCD_RES_Clr()  GPIO_ResetBits(GPIOB,GPIO_Pin_4)//RES
#define LCD_RES_Set()  GPIO_SetBits(GPIOB,GPIO_Pin_4)

#define LCD_DC_Clr()   GPIO_ResetBits(GPIOD,GPIO_Pin_3)//DC
#define LCD_DC_Set()   GPIO_SetBits(GPIOD,GPIO_Pin_3)

#define LCD_CS_Clr()   GPIO_ResetBits(GPIOD,GPIO_Pin_4)//CS
#define LCD_CS_Set()   GPIO_SetBits(GPIOD,GPIO_Pin_4)

#define LCD_BLK_Clr()  GPIO_ResetBits(GPIOD,GPIO_Pin_5)//BLK
#define LCD_BLK_Set()  GPIO_SetBits(GPIOD,GPIO_Pin_5)


#define WHITE                0xFFFF
#define BLACK                0x0000
#define BLUE                 0x001F
#define BRED                 0XF81F
#define GRED                 0XFFE0
#define GBLUE                0X07FF
#define RED                  0xF800
#define MAGENTA              0xF81F
#define GREEN                0x07E0
#define CYAN                 0x7FFF
#define YELLOW               0xFFE0
#define BROWN                0XBC40 
#define BRRED                0XFC07 
#define GRAY                 0X8430 
#define DARKBLUE             0X01CF 
#define LIGHTBLUE            0X7D7C 
#define GRAYBLUE             0X5458 
#define LIGHTGREEN           0X841F 
#define LGRAY                0XC618 
#define LGRAYBLUE            0XA651 
#define LBBLUE               0X2B12 



void LCD_GPIO_Init(void);
//void LCD_Writ_Bus(uint8_t dat);
void LCD_WR_DATA8(uint8_t dat);
void LCD_WR_DATA(uint16_t dat);
void LCD_WR_REG(uint8_t dat);
void LCD_Address_Set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
void LCD_Init(void);

void LCD_Fill(uint16_t xsta,uint16_t ysta,uint16_t xend,uint16_t yend,uint16_t color);
void LCD_DrawPoint(uint16_t x,uint16_t y,uint16_t color);
void LCD_DrawLine(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color);
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color);
void Draw_Circle(uint16_t x0,uint16_t y0,uint8_t r,uint16_t color);

// void LCD_ShowChinese(uint16_t x,uint16_t y,uint8_t *s,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode);
// void LCD_ShowChinese16x16(uint16_t x,uint16_t y,uint8_t *s,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode);
// void LCD_ShowChinese24x24(uint16_t x,uint16_t y,uint8_t *s,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode);
// void LCD_ShowChinese32x32(uint16_t x,uint16_t y,uint8_t *s,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode);

void LCD_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode);
void LCD_ShowString(uint16_t x,uint16_t y,const uint8_t *p,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode);
uint32_t mypow(uint8_t m,uint8_t n);
void LCD_ShowIntNum(uint16_t x,uint16_t y,uint16_t num,uint8_t len,uint16_t fc,uint16_t bc,uint8_t sizey);
void LCD_ShowFloatNum1(uint16_t x,uint16_t y,float num,uint8_t len,uint16_t fc,uint16_t bc,uint8_t sizey);

void LCD_ShowPicture(uint16_t x,uint16_t y,uint16_t length,uint16_t width,const uint8_t pic[]);




#endif /* !LCD_H_ */
