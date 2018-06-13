/*
 * The source implementing Lawicel SLCAN (CAN232) device.
 *
 * Implements timestamp handling for incoming CAN messages and miliseconds counters.
 *
 * systick-utils.c
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
#include "systick-utils.h"

// As per Lawicel, CAN timestamp overflows every minute and that's enough for most applications. Ok, whatever.
#define CAN_RX_OVF 60000

static volatile uint32_t miliseconds;
static volatile uint32_t can_rx_ts;

void SysTick_Handler(void)
{
	miliseconds++;
	if (++can_rx_ts >= CAN_RX_OVF) {
		can_rx_ts = 0;
	}
}

void init_systick_utils(void)
{
	miliseconds = 0;
	can_rx_ts = 0;

	SysTick->LOAD=TIMER_TICK;
	NVIC_SetPriority (SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL);
	SysTick->VAL=0UL;
	SysTick->CTRL=	SysTick_CTRL_CLKSOURCE_Msk |
	                SysTick_CTRL_TICKINT_Msk   |
	                SysTick_CTRL_ENABLE_Msk;
}

uint32_t millis(void)
{
	return miliseconds;
}

uint32_t can_timestamp(void)
{
	return can_rx_ts;
}
