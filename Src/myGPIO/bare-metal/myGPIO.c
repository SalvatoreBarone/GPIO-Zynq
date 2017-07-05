/**
 * @file myGPIO.c
 * @author Salvatore Barone <salvator.barone@gmail.com>
 * @date 12 05 2017
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
#include "myGPIO.h"
#include <stdlib.h>
#include <assert.h>

void myGPIO_Init(myGPIO_t* gpio, uint32_t base_address) {
	assert(gpio != NULL);
	assert(base_address != 0);
	
	gpio->base_address = (uint32_t *)base_address;
	gpio->mode_offset = myGPIO_MODE_OFFSET;
	gpio->write_offset = myGPIO_WRITE_OFFSET;
	gpio->read_offset = myGPIO_READ_OFFSET;
	gpio->gies_offset = myGPIO_GIES_OFFSET;
	gpio->pie_offset = myGPIO_PIE_OFFSET;
	gpio->irq_offset = myGPIO_IRQ_OFFSET;
	gpio->iack_offset = myGPIO_IACK_OFFSET;
}

void myGPIO_SetMode(myGPIO_t *gpio, myGPIO_mask mask, myGPIO_mode mode) {
	assert(gpio != NULL);
	assert(gpio->base_address != NULL);
	uint32_t value = gpio->base_address[gpio->mode_offset>>2];
	gpio->base_address[gpio->mode_offset>>2] = (mode == myGPIO_write ?  value|mask : value&(~mask));
}

void myGPIO_SetValue(myGPIO_t *gpio, myGPIO_mask mask, myGPIO_value value) {
	assert(gpio != NULL);
	assert(gpio->base_address != NULL);
	uint32_t actual_value = gpio->base_address[gpio->write_offset>>2];
	gpio->base_address[gpio->write_offset>>2] = (value == myGPIO_set ?  actual_value|mask : actual_value&(~mask));
}

void myGPIO_Toggle(myGPIO_t* gpio, myGPIO_mask mask) {
	assert(gpio != NULL);
	assert(gpio->base_address != NULL);
	uint32_t actual_value = gpio->base_address[gpio->write_offset>>2];
	gpio->base_address[gpio->write_offset>>2] = actual_value ^ mask;
}

myGPIO_value myGPIO_GetValue(myGPIO_t *gpio, myGPIO_mask mask) {
	assert(gpio != NULL);
	assert(gpio->base_address != NULL);
	return ((gpio->base_address[gpio->read_offset>>2] & mask) == 0 ? myGPIO_reset : myGPIO_set);
}

myGPIO_mask myGPIO_GetRead(myGPIO_t *gpio) {
	assert(gpio != NULL);
	assert(gpio->base_address != NULL);
	return gpio->base_address[gpio->read_offset>>2];
}

void myGPIO_GlobalInterruptEnable(myGPIO_t *gpio) {
	assert(gpio != NULL);
	assert(gpio->base_address != NULL);
	gpio->base_address[gpio->gies_offset>>2] = 1;
}

void myGPIO_GlobalInterruptDisable(myGPIO_t *gpio) {
	assert(gpio != NULL);
	assert(gpio->base_address != NULL);
	gpio->base_address[gpio->gies_offset>>2] = 0;
}

myGPIO_value myGPIO_IsGlobalInterruptEnabled(myGPIO_t *gpio) {
	assert(gpio != NULL);
	assert(gpio->base_address != NULL);
	return ((gpio->base_address[gpio->gies_offset>>2] & 1) == 0 ? myGPIO_reset : myGPIO_set);
}

myGPIO_value myGPIO_PendingInterrupt(myGPIO_t *gpio) {
	assert(gpio != NULL);
	assert(gpio->base_address != NULL);
	return ((gpio->base_address[gpio->gies_offset>>2] & 2) == 0 ? myGPIO_reset : myGPIO_set);
}

void myGPIO_PinInterruptEnable(myGPIO_t *gpio, myGPIO_mask mask) {
	assert(gpio != NULL);
	assert(gpio->base_address != NULL);
	gpio->base_address[gpio->pie_offset>>2] |= mask;
}

void myGPIO_PinInterruptDisable(myGPIO_t *gpio, myGPIO_mask mask) {
	assert(gpio != NULL);
	assert(gpio->base_address != NULL);
	gpio->base_address[gpio->pie_offset>>2] &= ~mask;
}

myGPIO_mask myGPIO_EnabledPinInterrupt(myGPIO_t *gpio) {
	assert(gpio != NULL);
	assert(gpio->base_address != NULL);
	return gpio->base_address[gpio->pie_offset>>2];
}

myGPIO_mask myGPIO_PendingPinInterrupt(myGPIO_t *gpio) {
	assert(gpio != NULL);
	assert(gpio->base_address != NULL);
	return gpio->base_address[gpio->irq_offset>>2];
}

void myGPIO_PinInterruptAck(myGPIO_t *gpio, myGPIO_mask mask) {
	assert(gpio != NULL);
	assert(gpio->base_address != NULL);
	gpio->base_address[gpio->iack_offset>>2] = mask;
}
