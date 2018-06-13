/*
 * The source implementing Lawicel SLCAN (CAN232) device.
 *
 * Implements ASSCII HEX to binary and back conversion using quick lookup tables.
 *
 * slcan_shell.c
 *
 *  Created on: Jun 8, 2018
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

#include "slcan_shell.h"
#include "buffers.h"
#include "usart.h"
#include "systick-utils.h"
#include "led.h"

#ifdef __DEBUG_UART__
#include <stdio.h>
#include "debug_uart.h"
#endif

#define SERIAL_NUM (*(uint32_t*) 0x1FFFF7E8)

//#define CAN_SEND_DEBUG

#ifdef CAN_SEND_DEBUG
extern char buf[100];
#endif

static char bin2hex[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
static uint32_t hex2bin[] = {
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Placeholders for converting ASCII to dec, i.e. gap between digits and letters
		0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
};
static char msg2type[] = { 't', 'r', 'T', 'R' };

// TODO: implement USART baud rate change via U command
static uint32_t u2baud[] = { 230400, 115200, 57600, 38400, 19200, 9600, 2400, 460800, 921600, 1000000 };

static volatile int32_t parsing = 0;

static volatile int32_t can_timestamp_on = 0;

static CanTxMsg tx;

int32_t print_can(CanRxMsg *rx, char* buf, int32_t len)
{
	if (len < 34) {
		return -1;
	}

	char *cbuf = buf+1;

	int32_t msgtype = (rx->RTR > 0);
	if (rx->IDE > 0) {
		msgtype += 2; // Upper case for Extended ID
	}

	*cbuf++ = msg2type[msgtype];

	int32_t i;
	if (rx->IDE > 0) {
		// Extended ID, CAN2B, print 8 Hex characters
		i = 8;
		while (i--) {
			*cbuf++ = bin2hex[(rx->ExtId >> (4*i)) & 0x0F];
		}
	} else {
		// Extended ID, CAN2A, print 3 Hex characters
		i = 3;
		while (i--) {
			*cbuf++ = bin2hex[(rx->StdId >> (4*i)) & 0x0F];
		}
	}

	if (rx->RTR == 0) {
		// Data
		*cbuf++ = bin2hex[rx->DLC & 0x0F];
		for (i=0; i<rx->DLC; i++) {
			*cbuf++ = bin2hex[rx->Data[i] >> 4];
			*cbuf++ = bin2hex[rx->Data[i] & 0x0F];
		}
	} else {
		*cbuf++ = '0';
	}

	if (can_timestamp_on) {
		uint32_t can_millis = can_timestamp();
		*cbuf++ = bin2hex[(can_millis >> 12) & 0x0F];
		*cbuf++ = bin2hex[(can_millis >> 8) & 0x0F];
		*cbuf++ = bin2hex[(can_millis >> 4) & 0x0F];
		*cbuf++ = bin2hex[(can_millis) & 0x0F];
	}

	*cbuf++ = '\r';
	*cbuf = 0;

	buf[0] = cbuf-&buf[1];

	return 0;
}

int32_t is_parsing()
{
	return parsing;
}


void handle_can_printing()
{
	if (!is_parsing()
		&& ((can_rx_buf.write - can_rx_buf.read) & CANBUFLEN) > 0)
	{
		print_can(&can_rx_buf.rx[can_rx_buf.read], uart_out.stream[uart_out.write], UBUFLEN);
		//write_usart(&buf[1], buf[0]);
		//write_usart("\n", 1);//*/
		uart_out.write = (uart_out.write + 1) & STREAM_UART;
		can_rx_buf.read = (can_rx_buf.read + 1) & CANBUFLEN;//*/
	}
}

void handle_uart_output()
{
	if (is_sending()) {
		return;
	}

	if (((uart_out.write - uart_out.read) & STREAM_UART) > 0) {
		// Printing queued
		start_tx_usart(&uart_out.stream[uart_out.read][1], uart_out.stream[uart_out.read][0]);
		uart_out.read = (uart_out.read + 1) & STREAM_UART;
	}
}

static void send_cr()
{
	char c = S_CR;
	write_usart(&c, 1);
}

static void send_bell()
{
	char c = S_BELL;
	write_usart(&c, 1);
}

static int32_t convert_hex2bin(const char *stream, uint32_t *res, uint32_t len)
{
	int32_t i;
	char *str = stream;
	uint32_t ret = 0;

	for (i=0; i<len; i++) {
		if (*str >= '0' && *str <= 'F') {
			ret += hex2bin[(*str) - '0'] << (4*(len-i-1));
		} else {
			return -1;
		}
		str++;
	}

	*res = ret;
	return 0;
}

void handle_shell()
{
	static int32_t bufpos = 0;
	static int32_t start_millis = 0;

	char c;
	int read;

	read = read_usart_nonblock(&c);

	if (read) {
		if (parsing == 0) {
			start_millis = millis();
		}
		parsing = 1;
		uart_in[bufpos++] = c;
		LED_ON(LED_BLUE);
	} else {
		return;
	}

	if (bufpos >= UARTINLEN) {
		// Error, should not happen
		send_bell();
		bufpos = 0;
		return;
	}

	int32_t res;
	int32_t dlc_pos;
	uint32_t conv_val;
	uint32_t i, timeout;
	uint8_t mbox;

	if (c == S_CR || c == S_NL) {
#ifdef __DEBUG_UART__
		uart_in[bufpos] = 0;
		dbg_print(uart_in);
		dbg_print("\n");

		uint32_t start, end;
		start = DWT_CYCCNT;
#endif
		// Handle also input "by hand" in terminal
		switch(uart_in[0]) {
			case 'T':
			case 't':
			case 'R':
			case 'r':
				// Parse and transmit message
				if (uart_in[0] < 'r') {
					// Extended ID
					tx.IDE = CAN_Id_Extended;
					res = convert_hex2bin(&uart_in[1], &tx.ExtId, 8);
					dlc_pos = 9;
				} else {
					tx.IDE = CAN_Id_Standard;
					res = convert_hex2bin(&uart_in[1], &tx.StdId, 3);
					dlc_pos = 4;
				}
				if (res == 0) {
					if (uart_in[0] == 't' || uart_in[0] == 'T') {
						tx.RTR = CAN_RTR_Data;
						res = convert_hex2bin(&uart_in[dlc_pos], &conv_val, 1);
						if (res == 0 && conv_val <= 8) {
							tx.DLC = (uint8_t) (conv_val & 0xFF);
							for (int32_t i = 0; i < tx.DLC; i++) {
								res = convert_hex2bin(&uart_in[(dlc_pos + 1)+(i*2)], &conv_val, 2);
								if (res == 0) {
									tx.Data[i] = (uint8_t) (conv_val & 0xFF);
								} else {
									break;
								}
							}
							if (res != 0) {
								send_bell();
								break;
							}
						} else {
							send_bell();
							break;
						}
					} else {
						tx.RTR = CAN_RTR_Remote;
					}
					/*********** DEBUG *************/
#ifdef __DEBUG_UART_ZZ__
					print_usart("\r\n");
					snprintf(dbg_buf, DBG_BUF_LEN, "CAN:\r\n");
					dbg_print_buf();
					snprintf(dbg_buf, DBG_BUF_LEN, "    ExtId: %08X\r\n", tx.ExtId);
					dbg_print_buf();
					snprintf(dbg_buf, DBG_BUF_LEN, "    StdId: %08X\r\n", tx.StdId);
					dbg_print_buf();
					snprintf(dbg_buf, DBG_BUF_LEN, "    RTR: %02X\r\n", tx.RTR);
					dbg_print_buf();
					snprintf(dbg_buf, DBG_BUF_LEN, "    DLC: %02X\r\n", tx.DLC);
					dbg_print_buf();
					snprintf(dbg_buf, DBG_BUF_LEN, "    Data: %02X %02X %02X %02X %02X %02X %02X %02X\r\n\r\n",
							tx.Data[0], tx.Data[1], tx.Data[2], tx.Data[3], tx.Data[4], tx.Data[5], tx.Data[6], tx.Data[7]);
					dbg_print_buf();
#endif
					/*******************************/
					for (i=0; i<CAN_RETRIES; i++) {
						timeout = 1000;
						mbox = CAN_Transmit(CAN1, &tx);
						while(CAN_TransmitStatus(CAN1, mbox) == CAN_TxStatus_Pending && --timeout);
						if (timeout > 0) {
							break;
						} else {
							CAN_CancelTransmit(CAN1, mbox);
						}
					}

					if (i < CAN_RETRIES) {
						send_cr();
					} else {
						send_bell(); // Ultimate transmission fail
					}
				} else {
					send_bell();
				}
			break;
			case 'N':
				// Report serial number (taken from STM32F103 ID register
				if (uart_in[1] == S_CR || uart_in[1] == S_NL) {
					uart_out.stream[uart_out.write][0] = 6;
					uart_out.stream[uart_out.write][1] = 'N';
					uart_out.stream[uart_out.write][2] = bin2hex[(SERIAL_NUM >> 12) & 0x0F];
					uart_out.stream[uart_out.write][3] = bin2hex[(SERIAL_NUM >> 8) & 0x0F];
					uart_out.stream[uart_out.write][4] = bin2hex[(SERIAL_NUM >> 4) & 0x0F];
					uart_out.stream[uart_out.write][5] = bin2hex[(SERIAL_NUM) & 0x0F];
					uart_out.stream[uart_out.write][6] = S_CR;
					uart_out.write = (uart_out.write + 1) & STREAM_UART;
				} else {
					// That's some gibberish, send error
					send_bell();
				}
			break;
			case 'U':
				// Set UART baud rate
			break;
			case 'V':
				if (uart_in[1] == S_CR || uart_in[1] == S_NL) {
					write_usart(DB_SLCAN_VERSION, DB_SLCAN_VER_LEN);
				} else {
					// That's some gibberish, send error
					send_bell();
				}
			break;
			case 'Z':
				// Turn the timestamp on/off
				if (uart_in[2] == S_CR || uart_in[2] == S_NL) {
					if (uart_in[1] == '1') {
						can_timestamp_on = 1;
						send_cr();
					} else if (uart_in[1] == '0') {
						can_timestamp_on = 0;
						send_cr();
					} else {
						// Something's not supported
						send_bell();
					}

				} else {
					send_bell();
				}
			break;
			default:
				// Something's not supported (yet?)
				send_bell();
			break;
		}

		bufpos = 0; // Reset buffer in any case when CR/NL received
		parsing = 0; // Also reset parsing flag so output could be enabled
		LED_OFF(LED_BLUE);
#ifdef __DEBUG_UART__
		end = DWT_CYCCNT;
		snprintf(dbg_buf, DBG_BUF_LEN, "Parsing cycles: %lu, %lu, %lu\r\n", start, end, end - start);
		dbg_print_buf();
#endif
	} else if (millis() - start_millis > 10) {
		// Timeout in parsing, maybe some error
		bufpos = 0;
		parsing = 0;
		send_bell();
		LED_OFF(LED_BLUE);
	}
}
