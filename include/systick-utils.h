/*
 * The source implementing Lawicel SLCAN (CAN232) device.
 *
 * Implements timestamp handling for incoming CAN messages and miliseconds counters.
 *
 * systick-utils.h
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

#ifndef SYSTICK_UTILS_H
#define SYSTICK_UTILS_H

#define F_CPU 		72000000UL
#define TIMER_TICK	F_CPU/1000-1

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"

void init_systick_utils(void);

uint32_t millis(void);

uint32_t can_timestamp(void);


#ifdef __cplusplus
}
#endif

#endif
