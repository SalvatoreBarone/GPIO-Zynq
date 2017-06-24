/**
 * @file myGPIOK_dev_int.c
 * @author Salvatore Barone <salvator.barone@gmail.com>
 * @date 24 06 2017
 *
 * @copyright
 * Copyright 2017 Salvatore Barone <salvator.barone@gmail.com>
 *
 * This file is part of Zynq7000DriverPack
 *
 * Zynq7000DriverPack is free software; you can redistribute it and/or modify it under the terms of
 * the GNU General Public License as published by the Free Software Foundation; either version 3 of
 * the License, or any later version.
 *
 * Zynq7000DriverPack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if not,
 * write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
 * USA.
 */
#include "myGPIOK_dev_int.h"
#include <asm/io.h>

void myGPIOK_GlobalInterruptEnable(void* baseAddress) {
	iowrite32(1, (baseAddress + myGPIOK_GIES_OFFSET));
}

void myGPIOK_GlobalInterruptDisable(void* baseAddress) {
	iowrite32(0, (baseAddress + myGPIOK_GIES_OFFSET));
}

void myGPIOK_PinInterruptEnable(void* baseAddress, unsigned mask) {
	unsigned reg_value = ioread32((baseAddress + myGPIOK_PIE_OFFSET));
	reg_value |= mask;
	iowrite32(reg_value, (baseAddress + myGPIOK_PIE_OFFSET));
}

void myGPIOK_PinInterruptDisable(void* baseAddress, unsigned mask) {
	unsigned reg_value = ioread32((baseAddress + myGPIOK_PIE_OFFSET));
	reg_value &= ~mask;
	iowrite32(reg_value, (baseAddress + myGPIOK_PIE_OFFSET));
}

unsigned myGPIOK_PendingPinInterrupt(void* baseAddress) {
	return ioread32((baseAddress + myGPIOK_IRQ_OFFSET));
}

void myGPIOK_PinInterruptAck(void* baseAddress, unsigned mask) {
	iowrite32(mask, (baseAddress + myGPIOK_IACK_OFFSET));
}

#ifdef __XGPIO__
void XGpio_Global_Interrupt(void* baseAddress, unsigned mask) {
	iowrite32(mask, (baseAddress + XGPIO_GIE_OFFSET));
}

void XGpio_Channel_Interrupt(void* baseAddress, unsigned mask) {
	iowrite32(mask, (baseAddress + XGPIO_IER_OFFSET));
}

void XGpio_Ack_Interrupt(void* baseAddress, unsigned channel) {
	iowrite32(channel, (baseAddress + XGPIO_ISR_OFFSET));
}
#endif

