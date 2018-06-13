/*
 * The source implementing Lawicel SLCAN (CAN232) device.
 *
 * Adds configurable values.
 *
 * slcan_config.h
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

#ifndef SLCAN_CONFIG_H_
#define SLCAN_CONFIG_H_

extern uint32_t cfg_can_mode;
extern uint32_t cfg_can_sjw_quanta;
extern uint32_t cfg_can_ts1_quanta;
extern uint32_t cfg_can_ts2_quanta;
extern uint32_t cfg_can_prescaler;

#endif /* SLCAN_CONFIG_H_ */
