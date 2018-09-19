/*
 * The source implementing Lawicel SLCAN (CAN232) device.
 *
 * Implements ASSCII HEX to binary and back conversion using quick lookup tables.
 *
 * slcan_config.h
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
#ifndef PRINTER_PARSER_H_
#define PRINTER_PARSER_H_

#define S_CR '\r'
#define S_NL '\n'
#define S_BELL '\a'

#define CAN_RETRIES 3

#define DB_SLCAN_VERSION "V3001\r"
#define DB_SLCAN_VER_LEN 6

#include "stm32f10x_can.h"
#include "buffers.h"

/**
 * Convert incoming CAN message into Lawicel representation. First byte of the buffer
 * contains length of the converted string, like in Pascal. This makes DMA-UART operation
 * quicker, as there's no need to count string's length.
 * @param pointer to incoming (RX) message
 * @param pointer to the buffer where to put message
 * @param max lenght of the buffer (should be >=34 bytes)
 * @return 0 if ok, error code otherwise
 */
int32_t print_can(union CanBuffer *, char*, int32_t);

/**
 * Checks if there's ongoing UART input parsing. During parsing output buffer should not be filled,
 * even if there are incoming CAN messages.
 * @return 1 if parsing's ongoing, 0 if there's no parsing activity
 */
int32_t is_parsing();

/**
 * Function to be called periodically to process buffer of incoming CAN messages and filling
 * in the output buffer with appropriate Lawicel representation.
 */
void handle_can_printing();

void handle_uart_output();

/**
 * Function to be called periodically to read input from UART and put it into incomming buffer
 * for further processing and sending out CAN messages.
 */
void handle_shell();

/**
 * Function to be called periodically to check if there is UART buffer filled with messages to be sent.
 */
void handle_can_send();

#endif /* PRINTER_PARSER_H_ */
