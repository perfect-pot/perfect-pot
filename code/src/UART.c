/*
 * ECE 153B - Winter 2023
 *
 * Final Project - Perfect Pot
 * Torin Schlunk & Cameron Barrett
 */

#include "UART.h"

void UART1_Init(void) {
	// Enable the USART1 clock in the peripheral clock register
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

	// Select the system clock as the USART1 clock source 
	// in the peripheral independent clock configuration register.
	RCC->CCIPR &= ~(RCC_CCIPR_USART1SEL);
	RCC->CCIPR |= RCC_CCIPR_USART1SEL_0;
}

void UART1_GPIO_Init(void) {
	// Enable GPIO B clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	
	// Configure alternative function as USART1
  GPIOB->MODER &= ~(GPIO_MODER_MODE6 | GPIO_MODER_MODE7);
  GPIOB->MODER |= (GPIO_MODER_MODE6_1 | GPIO_MODER_MODE7_1);
	GPIOB->AFR[0] &= ~(GPIO_AFRL_AFSEL6 | GPIO_AFRL_AFSEL7);
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL6_2 | GPIO_AFRL_AFSEL6_1 | GPIO_AFRL_AFSEL6_0;
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL7_2 | GPIO_AFRL_AFSEL7_1 | GPIO_AFRL_AFSEL7_0;

  // 2 Both GPIO pins should operate at very high speed.
  GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED6 | GPIO_OSPEEDR_OSPEED7;

  // 2 Both GPIO pins should have a push-pull output type.
  GPIOB->OTYPER &= ~(GPIO_OTYPER_OT6 | GPIO_OTYPER_OT7);

  // 2 Configure both GPIO pins to use pull-up resistors for I/O.
  GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD6 | GPIO_PUPDR_PUPD7);
  GPIOB->PUPDR |= (GPIO_PUPDR_PUPD6_0 | GPIO_PUPDR_PUPD7_0);
}

void USART_Init(USART_TypeDef* USARTx) {
	// Disable USART for editing
	USARTx->CR1 &= ~(USART_CR1_UE);

	// In the control registers, set word length to 8 bits, oversampling mode to oversample
	// by 16, and number of stop bits to 1.
	USARTx->CR1 &= ~(USART_CR1_M);
	USARTx->CR1 &= ~(USART_CR1_OVER8);
	USARTx->CR2 &= ~(USART_CR2_STOP);

	// Set the baud rate to 9600.
	// What is system clock? 80 MHZ / 9600 = 8333.3333333  = 0x208D
	USARTx->BRR |= 0x208D;

	// In the control registers, enable both the transmitter and receiver
	USARTx->CR1 |= USART_CR1_TE | USART_CR1_RE;

	// Enable USART for editing
	USARTx->CR1 |= USART_CR1_UE;	
}

uint8_t USART_Read (USART_TypeDef * USARTx) {
	// SR_RXNE (Read data register not empty) bit is set by hardware
	while (!(USARTx->ISR & USART_ISR_RXNE));  // Wait until RXNE (RX not empty) bit is set
	// USART resets the RXNE flag automatically after reading DR
	return ((uint8_t)(USARTx->RDR & 0xFF));
	// Reading USART_DR automatically clears the RXNE flag 
}

void USART_Write(USART_TypeDef * USARTx, uint8_t *buffer, uint32_t nBytes) {
	uint32_t i;
	// TXE is cleared by a write to the USART_DR register.
	// TXE is set by hardware when the content of the TDR 
	// register has been transferred into the shift register.
	for (i = 0; i < nBytes; i++) {
		while (!(USARTx->ISR & USART_ISR_TXE));   	// wait until TXE (TX empty) bit is set
		// Writing USART_DR automatically clears the TXE flag 	
		USARTx->TDR = buffer[i] & 0xFF;
		USART_Delay(300);
	}
	while (!(USARTx->ISR & USART_ISR_TC));   		  // wait until TC bit is set
	USARTx->ISR &= ~USART_ISR_TC;
}   

void USART_Delay(uint32_t us) {
	uint32_t time = 100*us/7;    
	while(--time);   
}
