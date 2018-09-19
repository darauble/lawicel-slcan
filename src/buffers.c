/*
 * The source implementing Lawicel SLCAN (CAN232) device.
 *
 * Placeholders of the buffers used in this application.
 *
 * buffers.c
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

#include "buffers.h"

can_buf_t can_rx_buf;
uart_in_buf_t uart_in_buf;
uart_str_buf uart_out;
