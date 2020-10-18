
#include "uart_ST.h"
//-----------------------------------------------------------------------------
typedef struct{
	//--------
	volatile uint16_t CounterByte;//Счетчик переданных байт. 
	volatile uint8_t  BufSize;    //Количество передаваемых байт.
	volatile uint8_t *BufPtr;     //Указатель на передающий буфер.
	//--------
}UsartWorkReg_t;
//----------------------------------------
static UsartWorkReg_t	U1WorkReg;
//-------------
//static UsartWorkReg_t	U3WorkReg;
static UxHandlers_t	Usart3Handlers;
//***************************************************************************************************
//************************Аппаратный уровень USART1.*************************************************
void Uart1Init(void){

 	//Инициализация портов. 
	RCC->APB2ENR |=   RCC_APB2ENR_IOPAEN;             //Включение тактирование GPIOA.
	GPIOA->CRH   &= ~(GPIO_CRH_CNF9 | GPIO_CRH_CNF10);	
	GPIOA->CRH   |=   GPIO_CRH_CNF9_1 | GPIO_CRH_MODE9;//PA9(U1TX) - выход, альтернативный режим push-pull.
                                                     //PA9(U1TX) - тактирование 50МГц.
	GPIOA->CRH |= GPIO_CRH_CNF10_0;								     //PA10(U1RX) - Floating input.
  //Инициализация USART1. APB2 тактируется 72МГц.
  RCC->APB2ENR |= RCC_APB2ENR_USART1EN;//Включение тактирование USART1.
  USART1->CR1 &= ~USART_CR1_M; 		     //8 бит данных.
  USART1->CR2 &= ~USART_CR2_STOP;      //1 стоп-бит.
  USART1->BRR =   0x1d44;  				     //BaudRate 9600.
  USART1->CR1 =   USART_CR1_RE    |    //Включение RX USART1.	
                  USART_CR1_TE    |    //Включение TX USART1.
                  //USART_CR1_RXNEIE|    //Включение прерывания от приемника USART1.		
                  USART_CR1_UE;        //Включение USART1.   
	NVIC_SetPriority(USART1_IRQn, 15);   //Приоритет прерывания USART1.
	NVIC_EnableIRQ(USART1_IRQn);         //Разрешаем прерывание от приемника USART1.                
  //--------------------
  //Инициализация DMA1.
//  if (!(RCC->AHBENR & RCC_AHBENR_DMA1EN)) RCC->AHBENR |= RCC_AHBENR_DMA1EN;//Включить тактирование DMA1
//  DMA1Ch4InitForTX();  //DMA1Ch4 - 
//  DMA1Ch5InitForRX();  //DMA1Ch5 - 
//  USART1->CR3 |= (USART_CR3_DMAT | USART_CR3_DMAR);//разрешить передачу и прием USART1 через DMA.
}
//-----------------------------------------------------------------------------
//Запуск передачи буфера по прерыванию.
void Uart1StarTx(uint8_t *TxBuf, uint8_t size){

	U1WorkReg.CounterByte = 0;		//Cброс счетчика байт.
	U1WorkReg.BufSize			= size; //Количество байт на передачу.	
	U1WorkReg.BufPtr		  = TxBuf;//Указатель на передающий буфер.
  //----------------
  USART1->CR1 &= ~USART_CR1_RE; //Отлючение RX USART1.
	USART1->DR   = *TxBuf;        //Передача первого байта.
	USART1->CR1 |= USART_CR1_TE  |//Включение TX USART1.
                 USART_CR1_TCIE;//Включение прерывания от TX USART1.
}
//-----------------------------------------------------------------------------
//сброс флагов ошибки 
static void Usart1ClrErrFlag(void){

	USART1->SR;//Такая последовательность сбрасывает флаги ошибки 
	USART1->DR;//
}
//-----------------------------------------------------------------------------
//Прерывание от USART1.
void USART1_IRQHandler(void){

  //--------------------
	/*!< Read Data Register Not Empty */
	if (USART1->SR & USART_SR_RXNE)
		{
      //проверяем нет ли ошибок при приеме байта.
      if ((USART1->SR & (USART_SR_NE | USART_SR_FE | USART_SR_PE | USART_SR_ORE)) == 0)
        {
         //Принимаем пакета.
         //if(RS485RxPacket((uint8_t)USART3->DR)) USART3->CR1 &= ~USART_CR1_RE;//Отлючение RX.        
        }
      //--------------------
      //Если байт битый, то пропускаем его
      else
        {
					Usart1ClrErrFlag();//сброс флагов ошибки 
        }
      //--------------------
		}
  //--------------------
  /*!< Overrun error */
  if (USART1->SR & USART_SR_ORE)
		{
			Usart1ClrErrFlag();//сброс флагов ошибки 
    }
  //--------------------
  	/*!< Transmission Complete */
	if (USART1->SR & USART_SR_TC)
		{
			if(++U1WorkReg.CounterByte == U1WorkReg.BufSize)
        {
           //Запуск приема нового пакета.        
           USART1->CR1 &= ~USART_CR1_TCIE;//Отлючение прерывания по окончании передачи.  USART1.     
           USART1->CR1 |=  USART_CR1_RE;  //Включение RX USART1.
        }
      //Передача очередного байта.
			else USART1->DR = *(U1WorkReg.BufPtr + U1WorkReg.CounterByte);
      //---------      
		}
  //-------------------- 
 	/*!< Transmit Data Register Empty */
  if (USART1->SR & USART_SR_TXE)
 		{
      //USART1->SR &= ~USART_SR_TXE; //Сброс флага прерывания.
      //---------
 		}
	//--------------------
}
//***************************************************************************************************
//************************Аппаратный уровень USART3.*************************************************
void Uart3Init(void){
  
 	//Инициализация портов. 
  RCC->APB2ENR |=   RCC_APB2ENR_IOPBEN;                //Включение тактирование GPIOB.
	GPIOB->CRH   &= ~(GPIO_CRH_CNF10 | GPIO_CRH_CNF11);
	GPIOB->CRH   |=   GPIO_CRH_CNF10_1 | GPIO_CRH_MODE10;//PB10(U3TX) - выход, альтернативный режим push-pull.
																								       //PB10(U3TX) - тактирование 50МГц.
	GPIOB->CRH   |= GPIO_CRH_CNF11_0;								     //PB11(U3RX) - Floating input.		
//  //Инициализация USART3. APB1 тактируется 36МГц.
//  RCC->APB1ENR |= RCC_APB1ENR_USART3EN ;//Включение тактирование USART3. 
//  USART3->CR1 &= ~USART_CR1_M; 		      //8 бит данных.
//  USART3->CR2 &= ~USART_CR2_STOP;       //1 стоп-бит.
//  USART3->BRR =   0x138;  				      //BaudRate 115200.
//  USART3->CR1 =   USART_CR1_RE    |     //Включение RX USART1.	
//                  USART_CR1_TE;   |     //Включение TX USART1.
//                  USART_CR1_RXNEIE|     //Включение прерывания от приемника USART1.		
//                  USART_CR1_UE;         //Включение USART1.
//	NVIC_SetPriority(USART3_IRQn, 14);    //Приоритет прерывания USART3.
//	NVIC_EnableIRQ  (USART3_IRQn);        //Разрешаем прерывание от приемника USART3.                
  //--------------------
  //Инициализация USART3 для работы с ДМА1. APB1 тактируется 36МГц.
  RCC->APB1ENR |= RCC_APB1ENR_USART3EN ;//Включение тактирование USART3.
	
//	USART3->BRR =  USART3BRR_Const;  //Set baudrate
  USART3->CR2 &= ~USART_CR2_STOP;  //1 стоп-бит.
	USART3->CR1 &= ~USART_CR1_M; 		 //8 бит данных.
  USART3->CR1 =  USART_CR1_RE |    //Включение RX USART3.	
                 USART_CR1_TE |    //Включение TX USART3.
								 USART_CR1_RXNEIE| //Включение прерывания от приемника USART1.		
                 USART_CR1_UE;     //Включение USART1.
	USART3->CR3 |= USART_CR3_DMAT;   //Подключение TX USART3 к DMA.
	NVIC_SetPriority(USART3_IRQn, 15);//Приоритет прерывания USART3.
	NVIC_EnableIRQ  (USART3_IRQn);    //Разрешаем прерывание от приемника USART3.
	//--------------------
//  //Инициализация DMA. TX USART3 работает с DMA1 Ch2.
//	RCC->AHBENR |= RCC_AHBENR_DMA1EN;//Включить тактирование DMA1
//	DMA1Ch2InitForTX(USART3);  			 //DMA1Ch2 - 
 	
//  DMA1Ch5InitForRX();  //DMA1Ch5 - 
//  USART1->CR3 |= (USART_CR3_DMAT | USART_CR3_DMAR);//разрешить передачу и прием USART1 через DMA.
}
//-----------------------------------------------------------------------------
//Указатели обработчиков событий
UxHandlers_t* Uart3Handler(void){
	
	return &Usart3Handlers;
}
//-----------------------------------------------------------------------------
//Запуск передачи буфера по прерыванию.
//void Uart3StarTx(uint8_t *TxBuf, uint8_t size){

//	U3WorkReg.CounterByte = 0;		//Cброс счетчика байт.
//	U3WorkReg.BufSize			= size; //Количество байт на передачу.	
//	U3WorkReg.BufPtr		  = TxBuf;//Указатель на передающий буфер.
//  //----------------
//  USART3->CR1 &= ~USART_CR1_RE; //Отлючение RX USART1.
//	USART3->DR   = *TxBuf;        //Передача первого байта.
////	USART3->CR1 |= USART_CR1_TE | //Включение TX USART1.
////                 USART_CR1_TCIE;//Включение прерывания от TX USART1.
//	
//	USART3->CR1 |= USART_CR1_TCIE;//Включение прерывания от TX USART1.
//}
//-----------------------------------------------------------------------------
//сброс флагов ошибки 
static void Usart3ClrErrFlag(void){

	USART3->SR;//Такая последовательность сбрасывает флаги ошибки 
	USART3->DR;//
}
//-----------------------------------------------------------------------------
//Прерывание от USART3.
void USART3_IRQHandler(void){

  //--------------------
	/*!< Read Data Register Not Empty */
	if (USART3->SR & USART_SR_RXNE)
		{
      //проверяем нет ли ошибок при приеме байта.
      if ((USART3->SR & (USART_SR_NE | USART_SR_FE | USART_SR_PE | USART_SR_ORE)) == 0)
        {
          //Принимаем пакета.              
					Usart3Handlers.ReceiveByte(USART3->DR);
				}
      //--------------------
      //Если байт битый, то пропускаем его и 
      //очищаем приемный буфер для запуска приема нового пакета.
      else
        {
					Usart3ClrErrFlag();//сброс флагов ошибки 
					Usart3Handlers.ReceiveByteBroken();
        }
      //--------------------
		}
  //--------------------
  /*!< Overrun error */
  if(USART3->SR & USART_SR_ORE)
		{
 			Usart3ClrErrFlag();//сброс флагов ошибки 
			Usart3Handlers.ReceiveByteBroken();
    }
  //--------------------
  //Transmission Complete */
	if (USART3->SR & USART_SR_TC)
		{
//			if(++U3WorkReg.CounterByte == U3WorkReg.BufSize)
//        {
//           //Запуск приема нового пакета. 
//           RS485Direction(Rx);            //Микросхему на прием.        
//           USART3->CR1 &= ~USART_CR1_TCIE;//Отлючение прерывания по окончании передачи.  USART1.            
//           USART3->CR1 |=  USART_CR1_RE;  //Включение RX USART1. 
//        }
//      //Передача очередного байта.
//			else USART3->DR = *(U3WorkReg.BufPtr + U3WorkReg.CounterByte);
      //---------
      //Работа с ДМА1 канал 2.
			Usart3Handlers.BufTransferComplete();//Обработка окнчания предачи буфера.
			USART3->CR1 &= ~USART_CR1_TCIE;      //Отлючение прерывания по окончанию передачи			
      //---------      
		}
  //-------------------- 
 	//Transmit Data Register Empty */
  if (USART3->SR & USART_SR_TXE)
 		{
      //LC2SostGreenLedToggel;
 		}
	//--------------------
}
//***************************************************************************************************
//***************************************************************************************************
