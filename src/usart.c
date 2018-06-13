/*
 * The source implementing Lawicel SLCAN (CAN232) device
 *
 * Implements USART printing via DMA and non-blocking reading.
 *
 * usart.c
 *
 *  Created on: June 5, 2018
 *      Author: Darau, blė
 *
 *  This file is a part of personal use libraries and applications.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 */

#include "stm32f10x.h"
#include "usart.h"

const uint32_t bauds[] = {   1200,   2400,  4800,   9600,   19200,  38400,  57600,  115200, 230400, 460800, 921600, 1000000, 0 };
const uint16_t brr_u[] = { 0xEA60, 0x7530, 0x3A98, 0x1D4C, 0x0EA6, 0x0753, 0x04E2, 0x0271, 0x0138, 0x009C, 0x004E, 0x0048 };

#define USART USART1
#define DMA DMA1
#define DMA_CH DMA1_Channel4
#define DMA_CLK RCC_AHBENR_DMA1EN
#define DMA_IFCR_CLEAR (DMA_ISR_GIF4 | DMA_ISR_TCIF4 | DMA_ISR_HTIF4 | DMA_ISR_TEIF4)
#define DMA_TX_FLAG DMA_ISR_TCIF4
#define DMA_IRQ_H DMA1_Channel4_IRQHandler
#define DMA_IRQ DMA1_Channel4_IRQn

static volatile int32_t sending = 0;

void init_usart(uint32_t baud_rate)
{
	RCC->APB2ENR |=  RCC_APB2ENR_AFIOEN;
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	RCC->APB2ENR |=  RCC_APB2ENR_IOPAEN;
	GPIOA->CRH &= ~(GPIO_CRH_CNF9 | GPIO_CRH_MODE9 | GPIO_CRH_CNF10 | GPIO_CRH_MODE10); // Clear configuration
	GPIOA->CRH |= GPIO_CRH_CNF9_1 | GPIO_CRH_MODE9 | GPIO_CRH_CNF10_0; // Alternative Push-pull, 50 MHz; in floating

	// Clear config
	USART->CR1 = 0;
	USART->CR2 = 0;
	USART->CR3 = 0;
	USART->CR1 |= USART_CR1_UE | USART_CR1_RE | USART_CR1_TE;

	set_baud_rate(baud_rate);

	RCC->AHBENR |= DMA_CLK;

	// Configure DMA NVIC
	NVIC->IP[DMA_IRQ] = 0x00;
	NVIC->ISER[DMA_IRQ >> 0x05] = (uint32_t) 0x001 << (DMA_IRQ & (uint8_t) 0x1F);

	// Configure Memory to UART DMA
	DMA_CH->CPAR = (uint32_t) &(USART1->DR);
	DMA_CH->CCR = 0;
	DMA_CH->CCR = (DMA_CCR1_MINC | DMA_CCR1_DIR | DMA_CCR1_TCIE); // Increment memory buffer | Memory to peripheral

}

void set_baud_rate(uint32_t baud_rate)
{
	int32_t i = 0;
	while (bauds[i]) {
		if (bauds[i] == baud_rate) {
			USART->BRR = brr_u[i];
			break;
		}
		i++;
	}
	if (bauds[i] == 0) {
		// default to 9600
		USART->BRR = brr_u[2];
	}
}

uint32_t is_sending(void)
{
	return sending;
}

void write_usart(const char *str, int32_t len)
{
	while (sending);

	start_tx_usart(str, len);
}

void start_tx_usart(const char *str, int32_t len)
{
	sending = 1;
	USART->SR = ~( USART_FLAG_TC | USART_FLAG_RXNE);

	DMA_CH->CMAR = (uint32_t) str;
	DMA_CH->CNDTR = len;

	DMA->IFCR |= DMA_IFCR_CLEAR;
	USART->CR3 |= USART_CR3_DMAT;
	DMA_CH->CCR |= DMA_CCR1_EN;
}

int read_usart_nonblock(char *c)
{
	if (!(USART->SR & USART_SR_RXNE)) {
		return 0; // Nothing to read
	} else {
		*c = USART->DR;
		return 1;
	}
}

void DMA_IRQ_H(void)
{
	if (((DMA->ISR & DMA_TX_FLAG) > 0)) {
		// Wait for USART to finish sending last byte, it still works for some cycles
		// after DMA transfer is done.
		while ((USART->SR & USART_SR_TC) == 0);

		DMA_CH->CCR &= ~DMA_CCR1_EN;
		USART->CR3 &= (uint16_t) ~USART_CR3_DMAT;
		DMA->IFCR |= DMA_IFCR_CLEAR;
		sending = 0;
	}
}
