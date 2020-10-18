
#ifndef _uart_ST_H
#define _uart_ST_H
//-----------------------------------------------------------------------------

#include "stm32f10x.h"

//-----------------------------------------------------------------------------
//Указатели обработчиков событий
typedef struct{
	//--------
	uint8_t (*ReceiveByte)        (uint8_t byte);//Обработчик приема байта.
  void    (*ReceiveByteBroken)  (void);   		 //Обработчик битого байта.
	void    (*BufTransferComplete)(void);        //Окончание передачи буфера.
	//--------
}UxHandlers_t;
//-----------------------------------------------------------------------------
//Аппатартный уровень.
void Uart1Init  (void);
void Uart1StarTx(uint8_t *TxBuf, uint8_t size);

void 					Uart3Init   (void);
UxHandlers_t* Uart3Handler(void);
//void 					Uart3StarTx(uint8_t *TxBuf, uint8_t size);
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif 

















