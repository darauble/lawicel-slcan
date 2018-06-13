/*
 * The source implementing Lawicel SLCAN (CAN232) device.
 *
 * Adds some blinky indication of what's going on
 *
 * led.h
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

#ifndef LED_H_
#define LED_H_

#include "stm32f10x.h"

#define LED_PORT GPIOA
#define LED_RED GPIO_BRR_BR15
#define LED_GREEN GPIO_BRR_BR12
#define LED_BLUE GPIO_BRR_BR11

#define LED_ON(led) LED_PORT->BSRR |= led
#define LED_OFF(led) LED_PORT->BRR |= led

#define LED_PORT_INIT \
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; \
	LED_PORT->CRH &= ~(GPIO_CRH_CNF11 | GPIO_CRH_MODE11 | GPIO_CRH_CNF12 | GPIO_CRH_MODE12 | GPIO_CRH_CNF15 | GPIO_CRH_MODE15); \
	LED_PORT->CRH |= (GPIO_CRH_MODE11 | GPIO_CRH_MODE12 | GPIO_CRH_MODE15);

void blink_green();

void blink_blue();

void handle_red_blink();

void handle_green_blink();

void handle_blue_blink();


#endif /* LED_H_ */
