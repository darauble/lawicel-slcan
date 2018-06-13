/*
 * The source implementing Lawicel SLCAN (CAN232) device.
 *
 * Adds some blinky indication of what's going on
 *
 * led.c
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

#include "led.h"
#include "systick-utils.h"


static volatile int32_t green_on = 0;
static volatile int32_t blue_on = 0;


void blink_green()
{
	green_on = 3;
}

void blink_blue()
{
	blue_on = 3;
}

void handle_red_blink()
{
	static uint32_t cm = 0;
	static int32_t on = 0;

	if (millis() - cm > 500) {
		cm = millis();
		if (on) {
			LED_ON(LED_RED);
		} else {
			LED_OFF(LED_RED);
		}
		on = !on;
	}
}

void handle_green_blink()
{
	static int32_t on = 0;
	static uint32_t cm = 0;

	if (millis() - cm > 50) {
		cm = millis();
		if (green_on) {
			if (on) {
				LED_ON(LED_GREEN);
			} else {
				LED_OFF(LED_GREEN);
				green_on--;
			}
			on = !on;
		}
	}
}

void handle_blue_blink() // Not used (yet)
{

}
