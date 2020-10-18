
#ifndef _LcdGraphic_H
#define _LcdGraphic_H

//-----------------------------------------------------------------------------

//#include "Rtc.h"
//#include "lcd12864.h"
#include "LM6063D.h"

//-----------------------------------------------------------------------------
#define	X0	0xb8
#define	Y0	0x40

#define PIXEL_OFF	0		//режимы отображения пикселя - используются в графических функциях
#define PIXEL_ON	1
#define PIXEL_XOR	2

#define LCD_X_RES 128	 //разрешение экрана    
#define LCD_Y_RES 64

#define LCD_CACHE_SIZE  ((LCD_X_RES * LCD_Y_RES)/8)  //
//-----------------------------------------------------------------------------
void Lcd_Init			  (void);
void Lcd_Update     (void);
void Lcd_Clear      (void);
void Lcd_Filling    (uint8_t byte);
void Lcd_Pixel      (uint8_t x, uint8_t y, uint8_t mode);
void Lcd_Line       (int x1, int y1, int x2, int y2, uint8_t mode);
void Lcd_Circle     (uint8_t x, uint8_t y, uint8_t radius, uint8_t mode);
void Lcd_Bar        (int x1, int y1, int x2, int y2, uint8_t persent);
void Lcd_GotoXYFont (uint8_t x, uint8_t y);
void Lcd_Chr        (uint16_t ch);
void Lcd_String     (uint8_t x, uint8_t y);
void Lcd_ChrBold    (int ch);
void Lcd_StringBold (uint8_t x, uint8_t y);
void Lcd_ChrBig     (int ch);
void Lcd_StringBig  (uint8_t x, uint8_t y);
uint8_t Lcd_Print   (char *txt);
void Lcd_Image      (const uint8_t *imageData);

uint8_t Lcd_PutUint (uint32_t data);
uint8_t Lcd_BinToDec(uint16_t var, uint8_t num);
void    Lcd_BinToDecBold(uint16_t var, uint8_t num);

//-----------------------------------------------------------------------------
#endif /*_LcdGraphic_H*/



