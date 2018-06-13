/*
 * The source implementing Lawicel SLCAN (CAN232) device.
 *
 * Implements CAN bus initialization and reading via interrupt.
 *
 * can.h
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

#ifndef CAN_H_
#define CAN_H_

#define CAN_RX_DMA DMA1
#define CAN_RX_DMA_CH DMA1_Channel1
#define CAN_RX_DMA_CLK RCC_AHBENR_DMA1EN
#define CAN_RX_DMA_IFCR_CLEAR (DMA_ISR_GIF1 | DMA_ISR_TCIF1 | DMA_ISR_HTIF1 | DMA_ISR_TEIF1)
#define CAN_RX_DMA_TX_FLAG DMA_ISR_TCIF1
#define CAN_RX_DMA_IRQ_H DMA1_Channel1_IRQHandler
#define CAN_RX_DMA_IRQ DMA1_Channel1_IRQn

int open_can();

void close_can();

#endif /* CAN_H_ */
