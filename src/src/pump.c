/*
 * ECE 153B - Winter 2023
 *
 * Final Project - Perfect Pot
 * Torin Schlunk & Cameron Barrett
 */

#include "pump.h"

void Pump_Init(void) {
	// Enable GPIO Clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	
	// Initialize Pump
  GPIOA->MODER &= ~(3UL<<20);  // clear PA10 mode bits
  GPIOA->MODER |= (1UL<<20);   // set mode of PA10 to output
  GPIOA->OTYPER &= ~(1UL<<10); // set PA10 output type to push-pull
  GPIOA->PUPDR &= ~(3UL<<20);  // set PA10 to NO PUPD
}

void Pump_Off(void) {
	GPIOA->ODR &= ~(1UL<<10);
}

void Pump_On(void) {
	GPIOA->ODR |= 1UL<<10;
}

void Pump_Toggle(void) {
	GPIOA->ODR ^= 1UL<<10;
}
