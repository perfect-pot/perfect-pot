/*
 * ECE 153B - Winter 2023
 *
 * Final Project - Perfect Pot
 * Torin Schlunk & Cameron Barrett
 */

#include "ADC.h"

#include "stm32l476xx.h"

#include <stdint.h>

void ADC_Wakeup(void) {
	int wait_time;

	// To start ADC operations, the following sequence should be applied
	// DEEPPWD = 0: ADC not in deep-power down
	// DEEPPWD = 1: ADC in deep-power-down (default reset state)
	if ((ADC1->CR & ADC_CR_DEEPPWD) == ADC_CR_DEEPPWD)
		ADC1->CR &= ~ADC_CR_DEEPPWD; // Exit deep power down mode if still in that state

	// Enable the ADC internal voltage regulator
	// Before performing any operation such as launching a calibration or enabling the ADC, the ADC
	// voltage regulator must first be enabled and the software must wait for the regulator start-up
	// time.
	ADC1->CR |= ADC_CR_ADVREGEN;

	// Wait for ADC voltage regulator start-up time
	// The software must wait for the startup time of the ADC voltage regulator (T_ADCVREG_STUP)
	// before launching a calibration or enabling the ADC.
	// T_ADCVREG_STUP = 20 us
	wait_time = 20 * (80000000 / 1000000);
	while (wait_time != 0) {
		wait_time--;
	}
}

void ADC_Common_Configuration() {
	SYSCFG->CFGR1 |= SYSCFG_CFGR1_BOOSTEN;  // enable I/O analog switch voltage booster
	ADC123_COMMON->CCR |= ADC_CCR_VREFEN;            // Enable VREFINT
	ADC123_COMMON->CCR &= ~(ADC_CCR_PRESC);          // Ensure that the clock is not divided
	ADC123_COMMON->CCR &= ~(ADC_CCR_CKMODE);         // Ensure that the ADC clock scheme is set to HCLK/1 
	ADC123_COMMON->CCR |= ADC_CCR_CKMODE_0;          // synchronous clock mode
	ADC123_COMMON->CCR &= ~(ADC_CCR_DUAL);           // Ensure that all ADCs are operating in independent mode
}

void ADC_Pin_Init(void) {
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;    // enable GPIOA clock

	GPIOA->MODER |= GPIO_MODER_MODE1;       // mode = analog
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD1);    // no pull up, no pull down
	GPIOA->ASCR |= GPIO_ASCR_ASC1;          // connect PA1 to input of ADC
}

void ADC_Init(void) {
	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;          // enable ADC Clock
	RCC->AHB2RSTR |= RCC_AHB2RSTR_ADCRST;       // reset ADC
	RCC->AHB2RSTR &= ~(RCC_AHB2RSTR_ADCRST);    // clear reset bit


	// Other ADC Initialization
	ADC_Pin_Init();
	ADC_Common_Configuration();
	ADC_Wakeup();

	ADC1->CR &= ~ADC_CR_ADEN;                   // disable ADC to configure

	// In the ADC configuration register, configure the ADC to have 12-bit resolution and rightalignment.
	ADC1->CFGR &= ~(ADC_CFGR_RES);
	ADC1->CFGR &= ~(ADC_CFGR_ALIGN);

	// In the ADC regular sequence register, set the sequence length to 1. That is, there will be
	// only 1 conversion in the conversion sequence. Then, ensure that channel 6 is used for the
	// first conversion.
	ADC1->SQR1 &= ~(ADC_SQR1_L);
	ADC1->SQR1 &= ~(ADC_SQR1_SQ1);
	ADC1->SQR1 |= (ADC_SQR1_SQ1 & 6<<6);

	// In the ADC differential mode selection register, ensure that channel 6 is set to single-ended
	// mode.
	ADC1->DIFSEL &= ~(ADC_DIFSEL_DIFSEL_6);

	// In the ADC sample time register, set the sampling time for channel 6 to be 24.5 ADC
	// clock cycles.
	ADC1->SMPR1 &= ~(ADC_SMPR1_SMP6);
	ADC1->SMPR1 |= 3<<18;

	// In the ADC configuration register, ensure that the ADC is in single conversion mode and
	// that hardware trigger detection is disabled.
	ADC1->CFGR &= ~ADC_CFGR_CONT;

	// Now that we are done with the configuration, enable the ADC. Before moving on, wait
	// until the ADC is ready (this information is available in the interrupt and status register).
	ADC1->CR |= ADC_CR_ADEN;
	while(!(ADC1->ISR & ADC_ISR_ADRDY));
}
