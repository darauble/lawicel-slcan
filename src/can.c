/*
 * The source implementing Lawicel SLCAN (CAN232) device.
 *
 * Implements CAN bus initialization and reading via interrupt.
 *
 * can.c
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

#include <string.h>

#include "stm32f10x.h"
#include "stm32f10x_can.h" // For CAN_Receive
#include "misc.h"
#include "can.h"
#include "buffers.h"
#include "led.h"
#include "slcan_config.h"

//#define CAN_MCR_RESET (uint32_t) 0x00010002
#define INAK_TIMEOUT (uint32_t) 0xFFFF;
#define CAN_CFG_FLAGS (CAN_MCR_ABOM)

#define AIRCR_VECTKEY_MASK ((uint32_t)0x05FA0000)
#define NVIC_PriorityGroup_1 ((uint32_t)0x600)

#ifndef STM32F10X_CL
#define CAN1_RX_IRQ_CH USB_LP_CAN1_RX0_IRQn
#else
#define CAN1_RX_IRQ_CH CAN1_RX0_IRQn
#endif

extern char buf[100];
extern volatile int len;

static CanRxMsg rx_msg;

int open_can()
{
	// Configure NVIC for CAN
	SCB->AIRCR = AIRCR_VECTKEY_MASK | NVIC_PriorityGroup_1;
	NVIC->IP[CAN1_RX_IRQ_CH] = 0x00;
	NVIC->ISER[CAN1_RX_IRQ_CH >> 0x05] = (uint32_t) 0x01 << (CAN1_RX_IRQ_CH & (uint8_t)0x1F);

	// Configure IOs for CAN
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN; // Enable Alternative IO
	RCC->APB1ENR |= RCC_APB1ENR_CAN1EN; // Enable CAN clock
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; // Enable GPIOB pins for CAN1

	GPIOB->CRH &= ~(GPIO_CRH_CNF8 | GPIO_CRH_MODE8 | GPIO_CRH_CNF9 | GPIO_CRH_MODE9); // Reset pin8 and 9

	GPIOB->CRH |= GPIO_CRH_CNF8_1; // CNF b10: input pull up/down, mode 00: input
	GPIOB->BSRR |= GPIO_BSRR_BS8; // Pull up

	GPIOB->CRH |= GPIO_CRH_CNF9_1| GPIO_CRH_MODE9; // CNF b10: alternative push-pull, mode 11: 50 MHz output

	//GPIO_PinRemapConfig(GPIO_Remapping_CAN , ENABLE);
	AFIO->MAPR &= ~(AFIO_MAPR_CAN_REMAP);
	AFIO->MAPR |= AFIO_MAPR_CAN_REMAP_1; // b10, CAN_RX PB8, CAN_TX PB9.

	// Configure CAN peripheral
	CAN1->MCR = CAN_MCR_RESET;
	CAN1->MCR &= ~CAN_MCR_SLEEP; // Exit from sleep
	CAN1->MCR |= CAN_MCR_INRQ; // Request initialisation

	uint32_t timeout = INAK_TIMEOUT;

	while (((CAN1->MSR & CAN_MSR_INAK) == 0) && (--timeout));
	if (timeout == 0) {
		CAN1->MCR &= ~(uint32_t)CAN_MCR_INRQ;// Leave initialisation
		return -1; // Initialisation failed
	}

	CAN1->MCR |= CAN_CFG_FLAGS;
	CAN1->BTR = cfg_can_mode
			| cfg_can_sjw_quanta
			| cfg_can_ts1_quanta
			| cfg_can_ts2_quanta
			| cfg_can_prescaler;

	CAN1->MCR &= ~(uint32_t)CAN_MCR_INRQ;// Leave initialisation

	timeout = INAK_TIMEOUT;
	while (((CAN1->MSR & CAN_MSR_INAK) == 0) && (--timeout));
	if (timeout == 0) {
		return -2; // Initialisation finalisation failed
	}
	//*/

	// Enable filter 0 to pass through all messages
	CAN1->FMR |= CAN_FMR_FINIT; // Enter initialisation

	CAN1->FA1R &= ~(uint32_t)CAN_FA1R_FACT0; // Deactivate filter
	CAN1->FS1R |= CAN_FS1R_FSC0; // Scale 32 bit
	CAN1->sFilterRegister[0].FR1 = 0x00; // No filter
	CAN1->sFilterRegister[0].FR2 = 0x00;
	CAN1->FM1R &= ~(uint32_t)CAN_FM1R_FBM0; // Mask mode
	CAN1->FFA1R &= (uint32_t)CAN_FFA1R_FFA0; // FIFO0 assignment
	CAN1->FA1R |= CAN_FA1R_FACT0; // Activate filter

	CAN1->FMR &= ~CAN_FMR_FINIT; // Exit initialisation

	// Enable CAN interrupt
	CAN1->IER |= CAN_IER_FMPIE0;


	// Configure DMA NVIC
	NVIC->IP[CAN_RX_DMA_IRQ] = 0x00;
	NVIC->ISER[CAN_RX_DMA_IRQ >> 0x05] = (uint32_t) 0x01 << (CAN_RX_DMA_IRQ & (uint8_t)0x1F);

	return 0;
}

void close_can()
{
	CAN1->IER &= ~(CAN_IER_FMPIE0);
	NVIC->ICER[CAN1_RX_IRQ_CH >> 0x05] = (uint32_t) 0x01 << (CAN1_RX_IRQ_CH & (uint8_t)0x1F);
	CAN1->MCR = CAN_MCR_RESET; // Reset Master Control Register
	RCC->APB1RSTR |= RCC_APB1RSTR_CAN1RST; // Disable CAN clock
}


#ifndef STM32F10X_CL
void USB_LP_CAN1_RX0_IRQHandler(void)
#else
void CAN1_RX0_IRQHandler(void)
#endif
{
	blink_green();
	can_rx_buf.rx[can_rx_buf.write].mailbox = CAN1->sFIFOMailBox[0]; // Copy FIFO0 mailbox to buffer
	CAN1->RF0R |= CAN_RF0R_RFOM0; // Release FIFO0
	can_rx_buf.write = ((can_rx_buf.write+1) & CANBUFLEN);
}
