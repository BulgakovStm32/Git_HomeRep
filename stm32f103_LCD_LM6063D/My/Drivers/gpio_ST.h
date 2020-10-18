
#ifndef _gpio_ST_H
#define _gpio_ST_H
//-----------------------------------------------------------------------------

#include "stm32f10x.h"

//-----------------------------------------------------------------------------
#define Gpio_mSConst   10//50//10

#define GpioAIDR       1
#define GpioBIDR       2
#define GpioCIDR       3 
//****************************************************
//Светодиод на плате подключен к PC13.
#define Led_Off()  		 (GPIOC->BSRR = GPIO_BSRR_BS13) 
#define Led_On()   		 (GPIOC->BSRR = GPIO_BSRR_BR13) 
#define Led_Toggle()   (GPIOC->ODR  = GPIOC->ODR ^ GPIO_ODR_ODR13)
//****************************************************
//W5500
#define W5500_CSHight()  (GPIOC->BSRR = GPIO_BSRR_BS4) 
#define W5500_CSLow()    (GPIOC->BSRR = GPIO_BSRR_BR4) 

#define W5500_RSTHight() (GPIOC->BSRR = GPIO_BSRR_BS5) 
#define W5500_RSTLow()   (GPIOC->BSRR = GPIO_BSRR_BR5)                   
//-----------------------------------------------------------------------------
void     GpioInit     (void);
void     GpioCheckLoop(void);
uint16_t GpioGetState (uint8_t port);
//-----------------------------------------------------------------------------
#endif /*_gpio_ST_H*/


