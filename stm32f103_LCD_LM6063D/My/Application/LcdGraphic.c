
#include "LcdGraphic.h"

//*********************************************************************************************
unsigned char lcd_buf[32];		          //текстовый буфер для вывода на LCD
unsigned char LcdCache[LCD_CACHE_SIZE];	//Видео память.Cache buffer in SRAM 128*64 bits or 1024 bytes
unsigned int  LcdCacheIdx = 0;          //Cache index

extern const unsigned char Ascii_Tab_12864[];//Рабочая.
#define TabAscii	         Ascii_Tab_12864   //Рабочая.
//*********************************************************************************************
//***************************Работа с выводом изображения на дисплей.**************************
void Lcd_Init(void){

	Lm6063LcdInit();
	Lcd_Clear();
	Lcd_Update();
}
//*****************************************************************************
//Вывод буфера на дисплей.
void Lcd_Update(void){
		
	Lm6063LcdUpdate(LcdCache);
}
//*****************************************************************************
//Clears the display
void Lcd_Clear(void){
  
  uint16_t i;
  //--------------------
  for (i = 0; i < LCD_CACHE_SIZE; i++) LcdCache[i] = 0;	//забиваем всю память 0
  //LcdUpdate ();
}
//*****************************************************************************
void Lcd_Filling(uint8_t byte){
  
  uint16_t i;
  //--------------------
  for (i = 0; i < LCD_CACHE_SIZE; i++) LcdCache[i] = byte;	//забиваем всю память 0
  //LcdUpdate ();
}
//*****************************************************************************
//управление пикселем с координатами x,y. mode -> Off, On or Xor.
void Lcd_Pixel(uint8_t x, uint8_t y, uint8_t mode){
  
  uint16_t index;
  uint16_t offset, data;
  //--------------------
  //если передали в функцию некорректные данные - выходим.
  if ( (x > LCD_X_RES) || (y > LCD_Y_RES) ) return;	
  //-------------------- 
	index = (((int)(y)/8)*128)+x;//считаем номер байта в массиве памяти дисплея
	offset  = y-((y/8)*8);       //считаем номер бита в этом байте
  data = LcdCache[index];      //берем байт по найденному индексу.
  //редактируем бит в этом байте
       if( mode == PIXEL_OFF ) data &= (~(0x01 << offset));	
  else if( mode == PIXEL_ON  ) data |= (  0x01 << offset);
  else if( mode == PIXEL_XOR ) data ^= (  0x01 << offset);
  //--------------------
  LcdCache[index] = data;		//загружаем байт назад
}
//*****************************************************************************
//Draws a line between two points on the display - по Брезенхейму
void Lcd_Line(int x1, int y1, int x2, int y2, uint8_t mode){
  
  signed int dy = 0;
  signed int dx = 0;
  signed int stepx = 0;
  signed int stepy = 0;
  signed int fraction = 0;
  //--------------------
  //если передали в функцию некорректные данные - выходим.
  if (x1>LCD_X_RES || x2>LCD_X_RES || y1>LCD_Y_RES || y2>LCD_Y_RES) return;
 //--------------------
  dy = y2 - y1;
  dx = x2 - x1;
  if (dy < 0) 
    {
      dy = -dy;
      stepy = -1;
    }
  else stepy = 1;
  if (dx < 0)
    {
      dx = -dx;
      stepx = -1;
    }
  else stepx = 1;
  dy <<= 1;
  dx <<= 1;
  Lcd_Pixel(x1,y1,mode);
  if (dx > dy)
    {
      fraction = dy - (dx >> 1); 
      while (x1 != x2)
        {
          if (fraction >= 0)
            {
              y1 += stepy;
              fraction -= dx;
            }
          x1 += stepx;
          fraction += dy;  
          Lcd_Pixel(x1,y1,mode);
        }
    }
  else
    {
      fraction = dx - (dy >> 1);
      while (y1 != y2)
        {
          if (fraction >= 0)
            {
              x1 += stepx;
              fraction -= dy;
            }
          y1 += stepy;
          fraction += dx;
          Lcd_Pixel(x1,y1,mode);
        }
    }
}
//*****************************************************************************
//рисуем круг по координатам с радиусом - по Брезенхейму
void Lcd_Circle(uint8_t x, uint8_t y, uint8_t radius, uint8_t mode){
  
  signed char xc = 0;
  signed char yc = 0;
  signed char p = 0;
  //--------------------
  if (x > LCD_X_RES && y > LCD_Y_RES) return;
 //--------------------
  yc = radius;
  p  = 3 - (radius<<1);
  while (xc <= yc)  
    {
      Lcd_Pixel(x + xc, y + yc, mode);
      Lcd_Pixel(x + xc, y - yc, mode);
      Lcd_Pixel(x - xc, y + yc, mode);
      Lcd_Pixel(x - xc, y - yc, mode);
      Lcd_Pixel(x + yc, y + xc, mode);
      Lcd_Pixel(x + yc, y - xc, mode);
      Lcd_Pixel(x - yc, y + xc, mode);
      Lcd_Pixel(x - yc, y - xc, mode);
        if (p < 0) p += (xc++ << 2) + 6;
      else  p += ((xc++ - yc--)<<2) + 10;
    }
}
//*****************************************************************************
//рисуем батарейку с заполнением в %
void Lcd_Bar(int x1, int y1, int x2, int y2, uint8_t persent){
  
  unsigned char horizon_line,horizon_line2,i;
  //--------------------
  if(persent>100)return;
  
  Lcd_Line(x1,y2,x2,y2,1);  //down
  Lcd_Line(x2,y1,x2,y2,1);  //right
  Lcd_Line(x1,y1,x1,y2,1);  //left
  Lcd_Line(x1,y1,x2,y1,1);  //up
  Lcd_Line(x1+7,y1-1,x2-7,y1-1,1);
  Lcd_Line(x1+7,y1-2,x2-7,y1-2,1);

  horizon_line = persent*(y2-y1-3)/100;
  for(i=0;i<horizon_line;i++) Lcd_Line(x1+2,y2-2-i,x2-2,y2-2-i,1);

  horizon_line2 = (y2-y1-3);
  for(i=horizon_line2;i>horizon_line;i--) Lcd_Line(x1+2,y2-2-i,x2-2,y2-2-i,0);
}
//*****************************************************************************
//Установка курсора в положение Х,У. Диапазон значений Х,У: 1,1 .. 14,8.
void Lcd_GotoXYFont(uint8_t x, uint8_t y){
  
  if ((x <= 24) && (y <= 8)) 
    {
      LcdCacheIdx = (uint16_t)((y-1) * 128) + (uint16_t)((x-1) * 6);
    }
}
//*****************************************************************************
//Очистка текстового буфера.
void clean_lcd_buf(void){
  
	uint8_t i;
	//-------------------- 
	for (i=0; i<32; i++) lcd_buf[i] = 0;
}   
//***************************************************************************** 
//Displays a character at current cursor location and increment cursor location
void Lcd_Chr(uint16_t ch){
  
  unsigned char i;
  //--------------------
  //Проверка на максимум.
  //if(ch > LCD_CACHE_SIZE) return;
  //--------------------
  for (i=0; i<5; i++)
    {
      //выделяем байт-столбик из символа и грузим в массив - 5 раз
      LcdCache[LcdCacheIdx++] = TabAscii[ch*5+i];	
    }
  LcdCache[LcdCacheIdx++] = 0x00;	//добавляем пробел между символами
}
//*****************************************************************************
//Печать строки.
void Lcd_String(uint8_t x, uint8_t y){
  
	unsigned char i;
	//--------------------
	//if (x > 15 || y > 8) return;
  if (x > 22 || y > 8) 
    {
      x = 22;
      y = 8;
    }
  //--------------------
	Lcd_GotoXYFont (x, y);
 
	for (i=0; i<(22-x); i++)
    {
      if(lcd_buf[i]) 
        {
          Lcd_Chr(lcd_buf[i]);
        }
    }
	clean_lcd_buf(); 
}
//*****************************************************************************
//Displays a bold character at current cursor location and increment cursor location
void Lcd_ChrBold(int ch){
  
  unsigned char i;
  unsigned char a = 0, b = 0, c = 0;
  //--------------------    	
  for (i = 0; i < 5; i++)
    {
      c = TabAscii[(ch*5+i)];		//выделяем столбец из символа

      b  = (c & 0x01) * 3;    //"растягиваем" столбец на два байта 
      b |= (c & 0x02) * 6;
      b |= (c & 0x04) * 12;
      b |= (c & 0x08) * 24;

      c >>= 4;
      a  = (c & 0x01) * 3;
      a |= (c & 0x02) * 6;
      a |= (c & 0x04) * 12;
      a |= (c & 0x08) * 24;

      LcdCache[LcdCacheIdx]   = b;	  //копируем байты в экранный буфер
      LcdCache[LcdCacheIdx+1] = b;  //дублируем для получения жирного шрифта
      LcdCache[LcdCacheIdx+128] = a;
      LcdCache[LcdCacheIdx+129] = a;
      LcdCacheIdx = LcdCacheIdx+2;
    }

  LcdCache[LcdCacheIdx++] = 0x00;	//для пробела между символами
  LcdCache[LcdCacheIdx++] = 0x00;
}
//*****************************************************************************
//Печатает символ на текущем месте, большой и жирный.
void Lcd_StringBold (unsigned char x, unsigned char y){
  
	unsigned char i;
	//-------------------- 
	Lcd_GotoXYFont (x, y);
  
	for ( i = 0; i < 12-x; i++ ) if (lcd_buf[i]) Lcd_ChrBold(lcd_buf[i]); 
	clean_lcd_buf();
}
//*****************************************************************************	
//Displays a character at current cursor location and increment cursor location
void Lcd_ChrBig (int ch){
  
  unsigned char i;
  unsigned char a = 0, b = 0, c = 0;
  //-------------------- 
  for ( i = 0; i < 5; i++ )
    {
      c = TabAscii[(ch*5+i)];		//выделяем столбец из символа

      b  = (c & 0x01) * 3;            //"растягиваем" столбец на два байта 
      b |= (c & 0x02) * 6;
      b |= (c & 0x04) * 12;
      b |= (c & 0x08) * 24;

      c >>= 4;
      a  = (c & 0x01) * 3;
      a |= (c & 0x02) * 6;
      a |= (c & 0x04) * 12;
      a |= (c & 0x08) * 24;
      LcdCache[LcdCacheIdx] = b;
      LcdCache[LcdCacheIdx+128] = a;
      LcdCacheIdx = LcdCacheIdx+1;
    }
  LcdCache[LcdCacheIdx++] = 0x00;
}
//*****************************************************************************
//Displays a string at current cursor location
void Lcd_StringBig (unsigned char x, unsigned char y){
  
	unsigned char i;
	//-------------------- 
	if (x > 22 && y > 5) return;
  
	Lcd_GotoXYFont (x, y);
  
	for (i=0; i<(22-x); i++) if (lcd_buf[i]) Lcd_ChrBig(lcd_buf[i]); 
  
	clean_lcd_buf();
}
//*****************************************************************************
uint8_t Lcd_Print(char *txt){
  
  uint8_t i = 0;
  //-------------------- 
	clean_lcd_buf();
  while(*txt)
    {
      Lcd_Chr(*txt++);
      i++;  
    } 
  return i;
}
//*****************************************************************************
//вывод изображения.
void Lcd_Image(const uint8_t *imageData){
  
  uint16_t i;
  //--------------------
  for (i = 0; i < LCD_CACHE_SIZE; i++) 
    {
      LcdCache[i] = imageData[1023 - i];	//грузим данные
      //LcdCache[i] = imageData[i];	//грузим данные
    }
}
//*****************************************************************************
//void LcdTime(uint8_t x, uint8_t y, RtcTimeSTR_TypeDef *time){

//  LcdString(x, y);
//  LcdChr(time->hour);
//  LcdChr(':');
//  LcdChr(time->min);
//  LcdChr(':');
//  LcdChr(time->sec);
//}
//*****************************************************************************
uint8_t Lcd_PutUint(uint32_t data){
  
  uint8_t temp[10];
  uint8_t count = 0;
  volatile uint8_t countTemp;
  //--------------------
  if(data)
    {
      while(data)
        {
          temp[count++]=data%10+'0';
          data/=10;        
          countTemp = count;     
        } 
      while(count)   
        {
          Lcd_Chr(temp[--count]);                
        }
    }
  else 
    {
      Lcd_Chr('0');
      ++countTemp;
    }
  return countTemp;
}
//*****************************************************************************
uint8_t Lcd_BinToDec(uint16_t var, uint8_t num){
  
  uint8_t	DecArray[5];
  uint8_t i;
  //--------------------
  if((var <= 0xFFFF) && (num < 5))
    {
      DecArray[4] = (uint8_t)(var / 10000);
      var  %= 10000;

      DecArray[3] = (uint8_t)(var / 1000);
      var  %= 1000;

      DecArray[2] = (uint8_t)(var / 100);
      var  %= 100;

      DecArray[1] = (uint8_t)(var / 10);
      DecArray[0] = (uint8_t)(var % 10);   
      //--------------------
      //Вывод на дисплей
      for(i=0; i<num; i++)
        {
          Lcd_Chr(0x30 + DecArray[(num - 1) - i]);
        }
    }
  return num+1;
}
//*****************************************************************************
void Lcd_BinToDecBold(uint16_t var, uint8_t num){
  
  uint8_t   DecArray[5];
  uint8_t   i;
  //--------------------
  if((var <= 0xFFFF) && (num < 5))
    {
      DecArray[4] = (uint8_t)(var / 10000);
      var  %= 10000;

      DecArray[3] = (uint8_t)(var / 1000);
      var  %= 1000;

      DecArray[2] = (uint8_t)(var / 100);
      var  %= 100;

      DecArray[1] = (uint8_t)(var / 10);
      DecArray[0] = (uint8_t)(var % 10);   
      //--------------------
      //Вывод на дисплей
      for(i=0; i < num; i++)
        {
          Lcd_ChrBold(0x30 + DecArray[(num - 1) - i]);
        }
    }
}
//*****************************************************************************




