#include <stm32f10x.h>
//-----------------------------------------------------------------------------
// Назначение порта и его выводов для работы с ЖК дисплеем по 3-х проводной шине.
#define PORT_LCD  GPIOA	  // Порт, к которому подключен LCD.
	//-------------------
#define E_LCD  	  (1<<8)  // ножка к которой подключенеа линия E ЖКИ.
#define DAT_LCD   (1<<9)  // ножка к которой подключенеа линия CLK ЖКИ.
#define CLK_LCD   (1<<10) // ножка к которой подключенеа линия D_In ЖКИ.
//-----------------------------------------------------------------------------
// Прототипы функций работы с LCD.
void LCD_Init (void);
void Delay_LCD(uint32_t del);
void STROB_CLK (void);
void LCD_CMD_SEND (unsigned char cmd);
void LCD_DATA_SEND (unsigned char  data);
void LCD_OUT(char *txt);
void LCD_OUT_VARIABLE(uint32_t var);
void LCD_USER_SUMBOL(void);
void LCD_OUT_STRING(char *txt);
//-----------------------------------------------------------------------------
