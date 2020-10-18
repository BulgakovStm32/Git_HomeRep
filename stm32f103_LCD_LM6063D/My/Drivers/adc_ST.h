
#ifndef _adc_ST_H
#define _adc_ST_H
//-----------------------------------------------------------------------------

#include "stm32f10x.h"

//-----------------------------------------------------------------------------
#define AdcRef						3300UL //Опорное напряжение
#define AdcQuant					((AdcRef * 10000UL) / 4096UL)
#define AdcMeasQuantity   16 //Количество измерений для усреднения
#define AdcMeasShiftDev   4  //Количество сдвигов для деления
//--------------------
//Каналы АЦП.
#define Poj1AdcCh   0
#define Poj2AdcCh   1
#define Poj3AdcCh   2
#define ChSAdcCh    3
#define BatAdcCh    4
#define LcAdcCh     5
//-----------------------------------------------------------------------------
//Аппатартный уровень.
void     AdcInit(void);
uint16_t AdcGetMeas(uint8_t channel);
//-----------------------------------------------------------------------------
#endif /*_adc_ST_HH*/
