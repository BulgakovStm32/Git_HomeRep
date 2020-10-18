
#include "sys_ST.h"

//-----------------------------------------------------------------------------
//Настройка тактирования 72МГц. Внешний кварц 8 МГц.
uint32_t InitRCC(void){
	
    uint32_t StartUpCounter = 0, HSEStatus = 0;
    //--------------------
    //SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------*/        
    RCC->CR |= ((uint32_t)RCC_CR_HSEON);//Enable HSE.
    /* Wait till HSE is ready and if Time out is reached exit */
    do
      {
				HSEStatus = RCC->CR & RCC_CR_HSERDY;
				StartUpCounter++;  
      } 
    while((HSEStatus == 0) && (StartUpCounter != HSEStartUp_TimeOut));
    //--------------------
    if ((RCC->CR & RCC_CR_HSERDY) != RESET)
      {
        HSEStatus = (uint32_t)0x01;
      }
    else
      {
        HSEStatus = (uint32_t)0x00;
      }  
    //--------------------
    if (HSEStatus == (uint32_t)0x01)
      {
        /* Enable Prefetch Buffer */
        FLASH->ACR |= FLASH_ACR_PRFTBE;
        /* Flash 2 wait state */
        FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
        FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;  
        //--------------------
        //Настройка тактирования шин AHB, APB1 и APB2.
        RCC->CFGR |= (uint32_t)(RCC_CFGR_HPRE_DIV1  | // AHB prescaler. SYSCLK not divided
                                RCC_CFGR_PPRE2_DIV1 | // APB2 = AHB. APB2 Fmax = 72MHz.
                                RCC_CFGR_PPRE1_DIV2); // APB1 = (AHB / 2). APB1 Fmax = 36MHz.

        //--------------------
        //PLL configuration:  = HSE * 9 = 72 MHz */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL));
        RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE        |  // HSE oscillator clock selected as PLL input clock
                                //RCC_CFGR_PLLXTPRE_HSE_Div2 |  // HSE clock divided by 2                                
                                RCC_CFGR_PLLMULL9);
        /* Enable PLL */
        RCC->CR |= RCC_CR_PLLON;
        /* Wait till PLL is ready */
        while((RCC->CR & RCC_CR_PLLRDY) == 0) {}
        /* Select PLL as system clock source */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
        RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;    
        /* Wait till PLL is used as system clock source */
        while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08)  {}        
        RCC->CR |= RCC_CR_CSSON; //Включаем защиту HSE от сбоев - CSS.
      }
    else
      {
          /* If HSE fails to start-up, the application will have wrong clock 
           configuration. User can add here some code to deal with this error */
      } 
    return HSEStatus;
}
//-----------------------------------------------------------------------------
void SysInit (void){
	
  //Reset the RCC clock configuration to the default reset state(for debug purpose).
  RCC->CR 	|= (uint32_t)0x00000001;	/* Set HSION bit */
  RCC->CFGR &= (uint32_t)0xF0FF0000;	/* Reset SW, HPRE, PPRE1, PPRE2, ADCPRE and MCO bits */
  RCC->CR 	&= (uint32_t)0xFEF6FFFF;  /* Reset HSEON, CSSON and PLLON bits */
  RCC->CR 	&= (uint32_t)0xFFFBFFFF;  /* Reset HSEBYP bit */
  RCC->CFGR &= (uint32_t)0xFF80FFFF;	/* Reset PLLSRC, PLLXTPRE, PLLMUL and USBPRE/OTGFSPRE bits */
  RCC->CIR 	 = 0x009F0000;  				  /* Disable all interrupts and clear pending bits  */
  //RCC->CFGR2 = 0x00000000;      			/* Reset CFGR2 register */  
	
  InitRCC();//Настройка тактирования микроконтроллера. Внешний кварц 8 МГц.
}
//**********************************************************************************************
//**********************************************************************************************
//Блокировка flash.
void Stm32FlashLock(void){

  FLASH->CR |= FLASH_CR_LOCK;
}
//-----------------------------------------------------------------------------
//Разблокировка работы с flash.
void Stm32FlashUnlock(void){

  FLASH->KEYR = 0x45670123;
  FLASH->KEYR = 0xCDEF89AB;
}
//-----------------------------------------------------------------------------
//Стирание страницы flash.
//Перед записью необходимо стереть данные по нужным адресам, 
//это особенность флеша.
//pageAddress - любой адрес, принадлежащий стираемой странице
void Stm32FlashErasePage(uint32_t pageAddress){

	while(FLASH->SR & FLASH_SR_BSY);                         //Ждем окончания работы с памятю.
	   if(FLASH->SR & FLASH_SR_EOP) FLASH->SR = FLASH_SR_EOP;//Сбрасывается бит EOP записью в него единицы.
	FLASH->CR |= FLASH_CR_PER; //стирания страницы.
	FLASH->AR  = pageAddress;  //адрес стираемой страницы.
	FLASH->CR |= FLASH_CR_STRT;//Запуск выбранной операции
	while (!(FLASH->SR & FLASH_SR_EOP));
	FLASH->SR  =  FLASH_SR_EOP;
	FLASH->CR &= ~FLASH_CR_PER;
}
//-----------------------------------------------------------------------------
void Stm32FlashWriteWord(uint32_t word, uint32_t address){

	while(FLASH->SR & FLASH_SR_BSY);                         //Ждем окончания работы с памятю.
	   if(FLASH->SR & FLASH_SR_EOP) FLASH->SR = FLASH_SR_EOP;//Сбрасывается бит EOP записью в него единицы.
	FLASH->CR |= FLASH_CR_PG;                                //разрешение зиписи во флеш.
	
	*(volatile unsigned short*)address = (uint16_t)word;
	while(!(FLASH->SR & FLASH_SR_EOP)); 
	FLASH->SR = FLASH_SR_EOP;
		
	*(volatile unsigned short*)(address + 2) = (uint16_t)(word >> 16);
	while(!(FLASH->SR & FLASH_SR_EOP)){}; 
  FLASH->SR = FLASH_SR_EOP;
		
  FLASH->CR &= ~(FLASH_CR_PG);
}
//-----------------------------------------------------------------------------
//Чтения 32-хбитного слова из FLASH.
uint32_t Stm32FlashReadWord(uint32_t address){

	return (*(__IO uint32_t*)address);
}
//-----------------------------------------------------------------------------
////Функция стирает ВСЕ страницы. При её вызове прошивка самоуничтожается 
//void Stm32FlashEraseAll(void){

//	while(FLASH->SR & FLASH_SR_BSY);                         //Ждем окончания работы с памятю.
//	   if(FLASH->SR & FLASH_SR_EOP) FLASH->SR = FLASH_SR_EOP;//Сбрасывается бит EOP записью в него единицы.
//  FLASH->CR |= FLASH_CR_MER;                               //Устанавливаем бит стирания ВСЕХ страниц
//  FLASH->CR |= FLASH_CR_STRT;                              //Начать стирание
//  while(!(FLASH->SR & FLASH_SR_EOP));//Ожидание готовности.. Хотя оно уже наверное ни к чему здесь...
//  //FLASH->CR &= ~FLASH_CR_MER;
//}
////-----------------------------------------------------------------------------
////Запись страницы flash.
////data - указатель на записываемые данные
////address - адрес во flash
////count - количество записываемых байт, должно быть кратно 2
//void Stm32FlashWritePage(unsigned char* data, uint32_t address, uint16_t size){

//	unsigned int i;
//  //--------------------
//	while(FLASH->SR & FLASH_SR_BSY);                         //Ждем окончания работы с памятю.
//	   if(FLASH->SR & FLASH_SR_EOP) FLASH->SR = FLASH_SR_EOP;//Сбрасывается бит EOP записью в него единицы.
//	FLASH->CR |= FLASH_CR_PG;//разрешение зиписи во флеш.
//	for (i = 0; i < size; i += 2)
//    {
//      *(volatile unsigned short*)(address + i) = (((unsigned short)data[i + 1]) << 8) + data[i];
//      while(!(FLASH->SR & FLASH_SR_EOP));
//      FLASH->SR = FLASH_SR_EOP;
//    }
//  FLASH->CR &= ~(FLASH_CR_PG);
//}
////-----------------------------------------------------------------------------
////Запись буфера во флеш.
//void Stm32FlashWriteBuf(unsigned char* buf, uint16_t size){

//  __disable_irq();
//  Stm32FlashUnlock();
//  Stm32FlashErasePage(FLASH_PAGE_1);
//  Stm32FlashWritePage(buf, FLASH_PAGE_1, size);
//  Stm32FlashLock();
//  __enable_irq();
//}
//**********************************************************************************************
//**********************************************************************************************
//Прерываени блока CSS. Возникает при сбое тактирования HSE.
void NMI_Handler( void){

  //RCC->CIR |= RCC_CIR_CSSC;//Очищаем флаг прерывания CSS иначе прерывание будет генерироваться бесконечно.
	//NVIC_SystemReset();
  //LC1SostRedLedOn;
}
//*****************************************************************************
//*****************************************************************************
void HardFault_Handler( void){

//  if(SCB->HFSR & SCB_HFSR_VECTTBL)  LC1SostGreenLedOn();
//  if(SCB->HFSR & SCB_HFSR_FORCED)   LC1SostRedLedOn();
//  if(SCB->HFSR & SCB_HFSR_DEBUGEVT) LC2SostRedLedOn();
	
}
//*****************************************************************************
//*****************************************************************************
void MemManage_Handler( void){

}
//*****************************************************************************
//*****************************************************************************
void BusFault_Handler( void){

}
//*****************************************************************************
//*****************************************************************************
void UsageFault_Handler( void){

  //LC2SostRedLedOn;
}
//*****************************************************************************
//*****************************************************************************
void SVC_Handler( void){

  //LC2SostRedLedOn;
}
//*****************************************************************************
//*****************************************************************************








