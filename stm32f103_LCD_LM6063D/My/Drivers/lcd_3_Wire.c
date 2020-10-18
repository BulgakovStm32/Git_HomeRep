
#include "lcd_3_Wire.h"

//=============================================================================
// Функция задержки
void Delay_LCD(uint32_t del) {
  uint32_t i = 0;
  for (i=0; i<del; i++);
 }
//=============================================================================
//Строб
void STROB_CLK (void){
	PORT_LCD->ODR |= CLK_LCD; // выставляем 1.
	PORT_LCD->ODR &= ~CLK_LCD;// выставляем 0.
}
//=============================================================================
//ПП передачи команды в LCD. Самая быстрая команда выполняется за 38 мкСек (по даьашиту).
void LCD_CMD_SEND (uint8_t cmd) {
	uint8_t i;
	
//	 GPIOC->BSRR =GPIO_BSRR_BS14;//Отладка.	
 
	
//	PORT_LCD->ODR &= ~CLK_LCD; //Выставляем все линии в нули.
//	PORT_LCD->ODR &= ~E_LCD;   //
//	PORT_LCD->ODR &= ~DAT_LCD;
	  PORT_LCD->BSRR = (CLK_LCD) << 16;
		PORT_LCD->BSRR = (E_LCD) << 16;
		PORT_LCD->BSRR = (DAT_LCD) << 16;
	//Меняем местами тетрады в cmd.
	cmd = ((cmd & 0x0F)<<4) | ((cmd & 0xF0)>>4) ;
	//----------------------------
// Передаем старшую тетраду.
	for	(i=0; i<4; i++)
	{
		if ((cmd & (1<<i)) != 0)
		{ PORT_LCD->BSRR = DAT_LCD;}//PORT_LCD->ODR |= DAT_LCD;} // выставляем 1.
		else
		{ PORT_LCD->BSRR = (DAT_LCD) << 16;}//PORT_LCD->ODR &= ~DAT_LCD;} // или выставляем 0.
		STROB_CLK();
	}
				//Записываем бит в здвиговый регистр - управление ножкой RS.
				PORT_LCD->BSRR = (DAT_LCD) << 16;//PORT_LCD->ODR &= ~DAT_LCD; // Записываем нолик.
				STROB_CLK();
				//Выводим информацию со здвигового регистра на выводы.
				STROB_CLK();
				// Сбрасываем ножку D_In на всякий случай.
				PORT_LCD->BSRR = (DAT_LCD) << 16;//PORT_LCD->ODR &= ~DAT_LCD;
				// Шлем строб в LCD дисплей.
				//Delay_LCD(3);
				PORT_LCD->BSRR = E_LCD;//PORT_LCD->ODR |= E_LCD; // выставляем 1.
				//Delay_LCD(5);
				PORT_LCD->BSRR = (E_LCD) << 16;//PORT_LCD->ODR &= ~E_LCD;// выставляем 0
				// Передаем младшую тетраду.
	for	(i=4; i<8; i++)
	{
		if ((cmd & (1<<i)) != 0)
		{ PORT_LCD->BSRR = DAT_LCD;}//PORT_LCD->ODR |= DAT_LCD;} // выставляем 1.
		else
		{ PORT_LCD->BSRR = (DAT_LCD) << 16;}//PORT_LCD->ODR &= ~DAT_LCD;} // или выставляем 0.
		STROB_CLK();
	}
				//Записываем бит в здвиговый регистр - управление ножкой RS.
				PORT_LCD->BSRR = (DAT_LCD) << 16;//PORT_LCD->ODR &= ~DAT_LCD; // Записываем нолик.
				STROB_CLK();
				//Выводим информацию со здвигового регистра на выводы.
				STROB_CLK();
				// Сбрасываем ножку D_In на всякий случай.
				PORT_LCD->BSRR = (DAT_LCD) << 16;//PORT_LCD->ODR &= ~DAT_LCD;
				// Шлем строб в LCD дисплей.
				//Delay_LCD(3);
				PORT_LCD->BSRR = E_LCD;//PORT_LCD->ODR |= E_LCD; // выставляем 1.
				//Delay_LCD(5);
				PORT_LCD->BSRR = (E_LCD) << 16;//PORT_LCD->ODR &= ~E_LCD;// выставляем 0
				Delay_LCD(200);
	
//	GPIOC->BSRR =GPIO_BSRR_BR14;//Отладка.	
}
//=============================================================================
//ПП передачи данных в LCD.
void LCD_DATA_SEND (unsigned char  data) {
	uint8_t i;
	
	 //GPIOC->BSRR = GPIO_BSRR_BS14; //Отладка.	
	
	PORT_LCD->ODR &= ~CLK_LCD; //Выставляем в на все линии нули.
	PORT_LCD->ODR &= ~E_LCD;   //
	PORT_LCD->ODR &= ~DAT_LCD;
	//Меняем местами тетрады в data.
	data = ((data & 0x0F)<<4) | ((data & 0xF0)>>4) ;
	//----------------------------
// Передаем старшую тетраду.
	for	(i=0; i<4; i++)
	{
		if ((data & (1<<i)) != 0)
		{PORT_LCD->ODR |= DAT_LCD;} // выставляем 1.
		else
		{PORT_LCD->ODR &= ~DAT_LCD;} // или выставляем 0.
		STROB_CLK(); // Стробируем
	}
	//Записываем бит в здвиговый регистр - управление ножкой RS.
	PORT_LCD->ODR |= DAT_LCD; // Записываем единичку.
	STROB_CLK();
	//Выводим информацию со здвигового регистра на выводы.
	STROB_CLK();
	// Сбрасываем ножку D_In на всякий случай.
	PORT_LCD->ODR &= ~DAT_LCD;
	// Шлем строб в LCD дисплей.
//	Delay_LCD(3);
	PORT_LCD->ODR |= E_LCD; // выставляем 1.
//	Delay_LCD(5);
	PORT_LCD->ODR &= ~E_LCD;// выставляем 0
// Передаем младшую тетраду.
	for	(i=4; i<8; i++)
	{
		if ((data & (1<<i)) != 0)
		{PORT_LCD->ODR |= DAT_LCD;} // выставляем 1.
		else
		{PORT_LCD->ODR &= ~DAT_LCD;} // или выставляем 0.
		STROB_CLK(); // Стробируем
	}
		//Записываем пятый бит в здвиговый регистр - управление ножкой RS.
		PORT_LCD->ODR |= DAT_LCD; // Записываем единичку.
		STROB_CLK();
		//Выводим информацию со здвигового регистра на выводы.
		STROB_CLK();
		// Сбрасываем ножку D_In на всякий случай.
	//	PORT_LCD->ODR &= ~DAT_LCD;
		// Шлем строб в LCD дисплей.
	//	Delay_LCD(3);
		PORT_LCD->ODR |= E_LCD; // выставляем 1.
	//	Delay_LCD(5);
		PORT_LCD->ODR &= ~E_LCD;// выставляем 0
		Delay_LCD(200);
	
		//GPIOC->BSRR = GPIO_BSRR_BR14;// Отладка.	
	
}
//=============================================================================
// ПП передачи текста в LCD.
void LCD_OUT_STRING(char *txt) {
	while(*txt) {
		LCD_DATA_SEND(*txt);
		txt++;
	}
}
//=============================================================================
// Разбиение числа на тысяси, сотни, десятки и единицы.
void LCD_OUT_VARIABLE(uint32_t var) {
	char mil=0, s_tis=0, d_tis=0, tis = 0, sot = 0, des = 0, ed = 0;
	//===========================================
	if(var < 10000000)
	{
			while(var >= 1000000)
			{
				var = var - 1000000;
				mil++;
			}
			while(var >= 100000)
			{
				var = var - 100000;
				s_tis++;
			}
			while(var >= 10000)
			{
				var = var - 10000;
				d_tis++;
			}
			while(var >= 1000)
			{
				var = var - 1000;
				tis++;
			}
			while(var >= 100)
			{
				sot++;
				var = var - 100;
			}
			while(var >= 10)
			{
				des++;
				var = var - 10;
			}
			while(var != 0)
			{
				ed++;
				var--;
			}


	LCD_DATA_SEND(0x30 + mil);
	LCD_DATA_SEND(0x30 + s_tis);
	LCD_DATA_SEND(0x30 + d_tis);
	LCD_DATA_SEND(0x30 + tis);
	LCD_DATA_SEND(0x30 + sot);
	LCD_DATA_SEND(0x30 + des);
	LCD_DATA_SEND(0x30 + ed);
	mil = 0;
	s_tis =0;
	d_tis =0;
	tis = 0;
	sot = 0;
	des = 0;
	ed  = 0;
	}
}
//=============================================================================
//ПП инициалации LCD.
void LCD_Init (void) {
	//Включаем тактирование порта .
	 	RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN);
		// RCC->BDCR &= ~(1<<0); //Отключение генератора LSE для освобождения ножек PC14 И PC15.
		//===============================
		// Инициализайия порта PORT_LCD для работы с LCD.
	 	PORT_LCD->CRH |= (GPIO_CRH_MODE8 | GPIO_CRH_MODE9 | GPIO_CRH_MODE10);//PA8,PA9,PA10-выход, тактирование 50МГц.
	 	PORT_LCD->CRH &= ~(GPIO_CRH_CNF8 | GPIO_CRH_CNF9 | GPIO_CRH_CNF10);   //PA8,PA9,PA10-General purpose output push-pull

	// Сброс.
	LCD_CMD_SEND (0x03);
	Delay_LCD(8000);
	LCD_CMD_SEND (0x03);
	Delay_LCD(8000);
	LCD_CMD_SEND (0x03);
	Delay_LCD(8000);
	// Формат обмена 4 разр. , 5х7, 2 строки.
	LCD_CMD_SEND (0x28);
	Delay_LCD(8000);
	LCD_CMD_SEND (0x28);
	Delay_LCD(8000);
	// Запрет сдвига изображения.
	LCD_CMD_SEND (0x10);
	Delay_LCD(8000);
	// Инкриментированеи позиции курсора после каждого записанного байта.
	LCD_CMD_SEND (0x06);
	Delay_LCD(8000);
	// Стирание пямяти дисплея
	LCD_CMD_SEND (0x01);
	Delay_LCD(16000);
	// Курсор невиден, запрет мерцания курсора.
	LCD_CMD_SEND (0x0c);
	Delay_LCD(80000);
}
//=============================================================================
//ПП создания символа в LCD.
void LCD_USER_SUMBOL(void){
	//----------------------------
		//Символ градуса Цельсия.
//		LCD_CMD_SEND(0b01000000);

//		LCD_DATA_SEND(0b00001100);
//		LCD_DATA_SEND(0b00010010);
//		LCD_DATA_SEND(0b00010010);
//		LCD_DATA_SEND(0b00001100);
//		LCD_DATA_SEND(0b00000000);
//		LCD_DATA_SEND(0b00000000);
//		LCD_DATA_SEND(0b00000000);
//		LCD_DATA_SEND(0b00000000);
//		//----------------------------
//		LCD_DATA_SEND(0b00001110);
//		LCD_DATA_SEND(0b00010101);
//		LCD_DATA_SEND(0b00011011);
//		LCD_DATA_SEND(0b00001110);
//		LCD_DATA_SEND(0b00000000);
//		LCD_DATA_SEND(0b00010001);
//		LCD_DATA_SEND(0b00001110);
//		LCD_DATA_SEND(0b00010001);
	//----------------------------

	//----------------------------

	//----------------------------

	//----------------------------

	//----------------------------

	//----------------------------

	//----------------------------
}
//=============================================================================

