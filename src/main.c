/*
 * The source main routines implementing Lawicel SLCAN (CAN232) device
 *
 * main.c
 *
 *  Created on: June 5, 2018
 *      Author: Darau, blė
 *
 *  Credits: http://www.can232.com/docs/can232_v3.pdf
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

#include <stdio.h>
#include <string.h>

#include "makrosai.h"
#include "systick-utils.h"

#include "led.h"
#include "usart.h"
#include "can.h"
#include "buffers.h"
#include "slcan_shell.h"

#ifdef __DEBUG_UART__
#include "debug_uart.h"
#endif

const char *hello = "Starting \"Darau, Ble\" implementation of Lawicel SLCAN\r\n";

int main(int argc, char* argv[])
{
	if (argc) argv++; // Just to shut up warning on unused variables

	REMAP_JTAG_FULL_DISABLE;

	LED_PORT_INIT;
	LED_ON(LED_RED);

	init_systick_utils();
#ifdef __DEBUG_UART__
	open_dbg_uart();
	dbg_print("SLCAN debug started on USART3\r\n");
#endif

	/***********************************************************************************************************
	 * Could not get FT232 USB adapter working faster than this baud rate. Might be limitation of UART, as
	 * CTS and RTS pins are not employed for hardware RS232.
	 ***********************************************************************************************************/
	init_usart(460800);

	write_usart(hello, strlen(hello));

	int32_t cs = open_can();

#ifdef __DEBUG_UART__
	snprintf(dbg_buf, DBG_BUF_LEN, "CAN: %ld\r\n", cs);
	dbg_print_buf();
#endif

	if (cs != 0) {
		// Error, not possible to do anything meaningful, so shine all leds.
		LED_ON(LED_RED);
		LED_ON(LED_GREEN);
		LED_ON(LED_BLUE);
		while (1);
	}

	while (1) {
		handle_red_blink();

		handle_green_blink();

		handle_can_printing();

		handle_uart_output();

		handle_shell();
	}

	return 0;
}

