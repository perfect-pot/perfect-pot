#include "SysClock.h"

void System_Clock_Init(void) {
	// Select MSI as system clock source
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_MSI;

	// Set MSI clock range
	RCC->CR &= ~RCC_CR_MSIRANGE;
	RCC->CR |= RCC_CR_MSIRANGE_7; // Make range 8 MHz

	// Use the MSI clock range that is defined in RCC_CR
	RCC->CR |= RCC_CR_MSIRGSEL;

	// Enable MSI oscillator
	RCC->CR |= RCC_CR_MSIPLLEN;

	// Wait until MSI is ready
	while((RCC->CR & RCC_CR_MSIRDY) == 0);
}
