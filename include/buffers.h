/*
 * The source implementing Lawicel SLCAN (CAN232) device.
 *
 * Placeholders of the buffers used in this application.
 *
 * buffers.h
 *
 *  Created on: Jun 7, 2018
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

#ifndef BUFFERS_H_
#define BUFFERS_H_

#include "stm32f10x_can.h"

#define CANBUFLEN 255
#define STREAM_UART 255
#define UARTINLEN 64
// (sizeof("T1111222281122334455667788EA5F\r")+1) and plus first member is length (like in Pascal, heh)
#define UBUFLEN 34

#define CAN_RW_DIFF(w, r) ((w-r) & CANBUFLEN)
#define UART_RW_DIFF(w, r) ((w-r) & UBUFLEN)

typedef struct {
	uint32_t RIR;
	uint32_t RDTR;
	uint8_t data[8];
} can_data_layout_t;

union CanBuffer {
	CAN_FIFOMailBox_TypeDef mailbox;
	can_data_layout_t layout;
};


typedef struct {
	union CanBuffer rx[CANBUFLEN+1];
	volatile uint32_t read;
	volatile uint32_t write;
} can_buf_t;

typedef struct {
	char stream[STREAM_UART+1][UBUFLEN];
	uint32_t read;
	volatile uint32_t write;
	volatile uint32_t curpos;
} uart_str_buf;

extern can_buf_t can_rx_buf;
extern char uart_in[UARTINLEN];
extern uart_str_buf uart_out;


#endif /* BUFFERS_H_ */
