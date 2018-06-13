/*
 * The source implementing Lawicel SLCAN (CAN232) device.
 *
 * Adds debugging capabilities on spare USART (USART3 in this case)
 *
 * debug_uart.h
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

#ifndef DEBUG_UART_H_
#define DEBUG_UART_H_

#include "stm32f10x.h"

#define DBG_BUF_LEN 128

extern char dbg_buf[DBG_BUF_LEN];

#define DBG_UART USART3
#define DBG_UART_BRR 0x0138 // 115200
#define DBG_UART_EN RCC->APB1ENR |= RCC_APB1ENR_USART3EN
#define DBG_UART_IO_EN RCC->APB2ENR |=  RCC_APB2ENR_IOPBEN;
#define DBG_UART_IO_CLEAR \
		GPIOB->CRH &= ~(GPIO_CRH_CNF10 | GPIO_CRH_MODE10 | GPIO_CRH_CNF11 | GPIO_CRH_MODE11)
#define DBG_UART_IO_CFG \
		GPIOB->CRH |= GPIO_CRH_CNF10_1 | GPIO_CRH_MODE10 | GPIO_CRH_CNF11_0


#define DWT_CTRL   (*(uint32_t*) 0xE0001000)
#define DWT_CYCCNT (*(uint32_t*) 0xE0001004)


void open_dbg_uart(void);

int32_t dbg_print(const char *str);

int32_t dbg_print_buf();

#endif /* DEBUG_UART_H_ */
