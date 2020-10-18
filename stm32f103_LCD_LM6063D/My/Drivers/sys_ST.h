
#ifndef _sys_ST_H
#define _sys_ST_H
//-----------------------------------------------------------------------------

#include "stm32f10x.h"

#include "gpio_ST.h"

//-----------------------------------------------------------------------------
//Как же определить свободную страницу? Это зависит от размера прошивки. 
//Обычно используют последнюю страницу в памяти. 
//Её адрес можно вычислить как [начальный адрес] + [номер страницы - 1] * [размер страницы].
//Выберем 16 страницу: 0x0800 0000 + 15 * 0x0000 0400 = 0x0800 3C00

#define FLASH_SIZE				128
#define FLASH_PAGE_SIZE		1024

#define FLASH_PAGE_127 (FLASH_BASE + (FLASH_PAGE_SIZE * (FLASH_SIZE	 - 1)))   
#define FLASH_PAGE_126 (FLASH_BASE + (FLASH_PAGE_SIZE * (FLASH_SIZE	 - 2)))   

#define FLASH_PAGE127_END_ADDR 	(FLASH_PAGE_127 + FLASH_PAGE_SIZE)
#define FLASH_PAGE126_END_ADDR 	(FLASH_PAGE_126 + FLASH_PAGE_SIZE)
//-----------------------------------------------------------------------------
uint32_t InitRCC(void);
void     SysInit(void);

void     Stm32FlashLock     (void);
void     Stm32FlashUnlock   (void);
void     Stm32FlashErasePage(uint32_t pageAddress);
void     Stm32FlashWriteWord(uint32_t word, uint32_t address);
uint32_t Stm32FlashReadWord (uint32_t address);

//void     Stm32FlashEraseAll(void);//неотлажено
//void     Stm32FlashWritePage(unsigned char* data, uint32_t Address, uint16_t size);
//void     Stm32FlashWriteBuf (unsigned char* buf, uint16_t size);
//-----------------------------------------------------------------------------
#endif /*_sys_ST_H*/



