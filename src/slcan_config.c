/*
 * The source implementing Lawicel SLCAN (CAN232) device.
 *
 * Adds configurable values.
 *
 * slcan_config.c
 *
 *  Created on: Jun 6, 2018
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
#include "slcan_config.h"

/********** Hard-coded, adjustment TBD ***********/

uint32_t cfg_can_mode = 0; // Normal mode
uint32_t cfg_can_sjw_quanta = 0; // 1q
uint32_t cfg_can_ts1_quanta = 0x00020000; // 3 tq
uint32_t cfg_can_ts2_quanta = 0x00400000; // 5 tq
uint32_t cfg_can_prescaler = 0x03; // 1 Mbit/s

