
#include "gpio_ST.h"

//-----------------------------------------------------------------------------

volatile uint16_t GpioAState = 0; //
volatile uint16_t GpioBState = 0; //
volatile uint16_t GpioCState = 0; //

//-----------------------------------------------------------------------------
//Инициализация переферии.
void GpioInit (void){
  
  //Включаем тактирование порта A, B, C, D и модуля альтернативных функций.
  RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN |
                   RCC_APB2ENR_IOPBEN |
                   RCC_APB2ENR_IOPCEN |
                   RCC_APB2ENR_IOPDEN |
                   RCC_APB2ENR_AFIOEN);
	//Отключение JTAG-D от порта PA15, отладка через SWD активна.
  AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE; 
  //--------------------
  //Настройка вывода MCO(PA8) для вывода на нее системной частоты SYSCLK.
  //GPIOA->CRH &= ~GPIO_CRH_CNF8;

  //GPIOA->CRH |= GPIO_CRH_CNF8_1; 	 //PA8 -выход, альтернативный режим push-pull.
  //GPIOA->CRH |= GPIO_CRH_MODE8;	   //PA8 -выход, тактирование 50МГц.
  
  //RCC->CFGR |= RCC_CFGR_MCO_SYSCLK;//Подключение к выводу PA8 системную частоту.
  //RCC->CFGR |= RCC_CFGR_MCO_HSI;   //Подключение к выводу PA8 частоту HSI.
  //RCC->CFGR |= RCC_CFGR_MCO_HSE;      //Подключение к выводу PA8 частоту HSE.
  //RCC->CFGR |= RCC_CFGR_MCO_PLL;   //Подключение к выводу PA8 частоту PLL/2. 
  //--------------------
	//Светодиод на плате подключен к PC13.	
	GPIOC->CRH &= ~GPIO_CRH_CNF13; //PC13 - выход, режим - push-pull.
  GPIOC->CRH |=  GPIO_CRH_MODE13;//PC13 - тактирование 50МГц.  	
	//--------------------
  //W5500
  // CS  - PC4 - выход.
  // RST - PC5 - выход.
	GPIOC->CRL &= ~GPIO_CRL_CNF4; //PC4 - выход, режим - push-pull.
  GPIOC->CRL |=  GPIO_CRL_MODE4;//PC4 - тактирование 50МГц.  

	GPIOC->CRL &= ~GPIO_CRL_CNF5; //PC5 - выход, режим - push-pull.
  GPIOC->CRL |=  GPIO_CRL_MODE5;//PC5 - тактирование 50МГц. 
  //--------------------
}
//-----------------------------------------------------------------------------
void GpioCheckLoop(void){

 	static uint8_t  mScount         = 0;
 	static uint8_t  cycle           = 0;
 	static uint16_t GpioAIDRtemp[3] = {0, 0, 0};
 	static uint16_t GpioBIDRtemp[3] = {0, 0, 0};
 	static uint16_t GpioCIDRtemp[3] = {0, 0, 0};
  //-------------------------
  if(++mScount >= Gpio_mSConst)
    { 
      mScount = 0;
      //-------------------------
      if (cycle < 3)
        {
          GpioAIDRtemp[cycle] = GPIOA->IDR;//Считывание выводов.
          GpioBIDRtemp[cycle] = GPIOB->IDR;//Считывание выводов.
          GpioCIDRtemp[cycle] = GPIOC->IDR;//Считывание выводов.
          cycle++;
        } 
      //-------------------------
      else
        {
          cycle = 0;
          //Мажоритарное определение состояния выводов.
          GpioAState = ((GpioAIDRtemp[0] & GpioAIDRtemp[1]) |
                        (GpioAIDRtemp[1] & GpioAIDRtemp[2]) |
                        (GpioAIDRtemp[0] & GpioAIDRtemp[2]));

          GpioBState = ((GpioBIDRtemp[0] & GpioBIDRtemp[1]) |
                        (GpioBIDRtemp[1] & GpioBIDRtemp[2]) |
                        (GpioBIDRtemp[0] & GpioBIDRtemp[2]));

          GpioCState = ((GpioCIDRtemp[0] & GpioCIDRtemp[1]) |
                        (GpioCIDRtemp[1] & GpioCIDRtemp[2]) |
                        (GpioCIDRtemp[0] & GpioCIDRtemp[2]));
        }
      //-------------------------
    }
}
//-----------------------------------------------------------------------------
uint16_t GpioGetState(uint8_t port){
  
  if(port == GpioAIDR) return GpioAState;
  if(port == GpioBIDR) return GpioBState;
                       return GpioCState;
}
//-----------------------------------------------------------------------------





