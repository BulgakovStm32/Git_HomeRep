
#ifndef _dma_ST_H
#define _dma_ST_H
//-----------------------------------------------------------------------------

#include "stm32f10x.h"

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void DMA1Ch2InitForTX(USART_TypeDef *usart);
void DMA1Ch2StartTX  (uint8_t *buf, uint16_t size);

//Работа с DMA. Не отлажено!!!
void    DMA1Ch5InitForRX (void);
void    StartDMA1Ch5ForRX(uint8_t *BuffAdr, uint16_t LengthBufer);
//-----------------------------------------------------------------------------
#endif /*_dma_ST_H*/


