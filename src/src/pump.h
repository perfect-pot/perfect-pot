/*
 * ECE 153B - Winter 2023
 *
 * Final Project - Perfect Pot
 * Torin Schlunk & Cameron Barrett
 */
 
#ifndef __STM32L476G_NUCLEO_LED_H
#define __STM32L476G_NUCLEO_LED_H

#include "stm32l476xx.h"

void Pump_Init(void);

void Pump_Off(void);
void Pump_On(void);
void Pump_Toggle(void);

#endif
