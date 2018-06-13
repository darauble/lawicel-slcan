/*
 * The source implementing Lawicel SLCAN (CAN232) device
 *
 * Implements USART printing via DMA and non-blocking reading.
 *
 * usart.h
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

#include <stdint.h>

#ifndef USART_H_
#define USART_H_

void init_usart(uint32_t);

void set_baud_rate(uint32_t baud_rate);

void write_usart(const char*, int32_t);

void start_tx_usart(const char*, int32_t);

uint32_t is_sending(void);

int read_usart_nonblock(char *c);

#endif /* USART_H_ */
