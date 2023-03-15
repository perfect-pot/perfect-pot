/*
 * ECE 153B - Winter 2023
 *
 * Final Project - Perfect Pot
 * Torin Schlunk & Cameron Barrett
 */

#include "ADC.h"
#include "pump.h"
//#include "SysClock.h"
#include "stm32l476xx.h"
#include <stdio.h>

uint32_t data = 0;

int main(void) {
	// Initialization
	//System_Clock_Init(); // Switch System Clock = 8 MHz, needed for logic analyzer
	ADC_Init();
	Pump_Init();

	int i;
	while (1) {
		// 1. Through the ADC control register, start a regular conversion.
		ADC1->CR |= ADC_CR_ADSTART;

		// 2. Wait until the ADC conversion (i.e. regular conversion of the master ADC) is complete.
		// This information can be found in the ADC common status register.
		while(!(ADC123_COMMON->CSR & ADC_CSR_EOS_MST));

		// 3. Read the ADC data register to retrieve the measurement. The ADC should return a value
		// between 0 and roughly 4096.
		data = (uint16_t)ADC1->DR;

		// Respond to data
		if (data > 3300) {
			Pump_On();
		}
		else {
			Pump_Off();
		}
		
		// Some delay
		for(i=0; i<1000; ++i);
	}
}
