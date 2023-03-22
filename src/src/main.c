/*
 * ECE 153B - Winter 2023
 *
 * Final Project - Perfect Pot
 * Torin Schlunk & Cameron Barrett
 */

#include "SysClock.h"
#include "pump.h"
#include "ADC.h"
#include "UART.h"
#include "I2C.h"
#include "SysTimer.h"
#include "stm32l476xx.h"
#include <stdio.h>
#include <string.h>

int main(void) {
	// Initialization
	System_Clock_Init();
	ADC_Init();
	Pump_Init();
	UART1_Init();
	UART1_GPIO_Init();
	USART_Init(USART1);
	I2C_GPIO_Init();
	I2C_Initialization();
	SysTick_Init();

	int i;
	uint32_t moistureData = 0;
	uint8_t SlaveAddress = 0x48 << 1;
	uint8_t temperatureData;
	uint8_t Data_Send = 0x00;
	char rxByte;
	int enableWatering = 1;
	while (1) {
		// Some delay
		delay(5000);
		
		// Through the ADC control register, start a regular conversion.
		ADC1->CR |= ADC_CR_ADSTART;

		// Wait until the ADC conversion (i.e. regular conversion of the master ADC) is complete.
		// This information can be found in the ADC common status register.
		while(!(ADC123_COMMON->CSR & ADC_CSR_EOS_MST));

		// Read the ADC data register to retrieve the measurement. The ADC should return a value
		// between 0 and roughly 4096.
		moistureData = (uint16_t)ADC1->DR;
		
		I2C_SendData(I2C1, SlaveAddress, &Data_Send, 1);
		I2C_ReceiveData(I2C1, SlaveAddress, &temperatureData, 1);
		
		printf("T%u M%u", temperatureData, moistureData);
		printf("\n");

		// decide whether to water
		if (moistureData > 3300) { // dry
			enableWatering += 1;
		}
		else if (moistureData < 3300) { // moist
			enableWatering = 0;
		}
		if (enableWatering > 3) { // try watering up to 3 times
			printf("Watering failed. Please add more water and check tubes.\n");
		}
		else if (enableWatering) {
			Pump_On();
			printf("Watering...\n");
			delay(7000);
			Pump_Off();
		}
	}
}
