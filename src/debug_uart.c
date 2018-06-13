/*
 * The source implementing Lawicel SLCAN (CAN232) device.
 *
 * Adds debugging capabilities on spare USART (USART3 in this case)
 *
 * debug_uart.c
 *
 *  Created on: Jun 10, 2018
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

#include "debug_uart.h"

char dbg_buf[DBG_BUF_LEN];

void open_dbg_uart(void)
{
	RCC->APB2ENR |=  RCC_APB2ENR_AFIOEN;
	DBG_UART_EN;
	DBG_UART_IO_EN
	DBG_UART_IO_CLEAR;
	DBG_UART_IO_CFG;

	DBG_UART->CR1 = 0;
	DBG_UART->CR2 = 0;
	DBG_UART->CR3 = 0;
	DBG_UART->CR1 |= USART_CR1_UE | USART_CR1_RE | USART_CR1_TE;
	DBG_UART->BRR = DBG_UART_BRR;

	// Enable debug registers for profiling
	if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) {
		CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
		DWT_CYCCNT = 0;
		DWT_CTRL |= 0x01;
	}
}

int32_t dbg_print(const char *str)
{
	int32_t i = 0;
	while (*str) {
		// Write a character to the USART
		DBG_UART->DR = (*str & USART_DR_DR);

		while(!(DBG_UART->SR & USART_SR_TXE));

		i++;
		str++;
	}
	return i;
}

int32_t dbg_print_buf()
{
	return dbg_print(dbg_buf);
}
