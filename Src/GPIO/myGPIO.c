/*
 * @file myGPIO.c
 * @author: Salvatore Barone
 * @email salvator.barone@gmail.com
 * @date: 12 05 2017
 * @copyright
 * Copyright 2017 Salvatore Barone <salvator.barone@gmail.com>, <salvator.barone@studenti.unina.it>
 * This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or any later version.
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "myGPIO.h"
#include <stdlib.h>
#include <assert.h>

void myGPIO_init(	myGPIO_t* gpio, uint32_t	*base_address) {
	assert(gpio != NULL);
	assert(base_address != NULL);
	
	gpio->base_address = base_address;
	gpio->mode_offset = myGPIO_MODE_OFFSET;
	gpio->write_offset = myGPIO_WRITE_OFFSET;
	gpio->read_offset = myGPIO_READ_OFFSET;
	gpio->int_offset = myGPIO_INTR_OFFSET;
}

void myGPIO_setMode(myGPIO_t *gpio, myGPIO_mask mask, myGPIO_mode mode) {
	assert(gpio != NULL);
	assert(gpio->base_address != NULL);
	uint32_t value = gpio->base_address[gpio->mode_offset>>2];
	gpio->base_address[gpio->mode_offset>>2] = (mode == myGPIO_write ?  value|mask : value&(~mask));
}

void myGPIO_setValue(myGPIO_t *gpio, myGPIO_mask mask, myGPIO_value value) {
	assert(gpio != NULL);
	assert(gpio->base_address != NULL);
	uint32_t actual_value = gpio->base_address[gpio->write_offset>>2];
	gpio->base_address[gpio->write_offset>>2] = (value == myGPIO_set ?  actual_value|mask : actual_value&(~mask));
}

void myGPIO_toggle(myGPIO_t* gpio, myGPIO_mask mask) {
	assert(gpio != NULL);
	assert(gpio->base_address != NULL);
	uint32_t actual_value = gpio->base_address[gpio->write_offset>>2];
	gpio->base_address[gpio->write_offset>>2] = actual_value ^ mask;
}

myGPIO_value myGPIO_getValue(myGPIO_t *gpio, myGPIO_mask mask) {
	assert(gpio != NULL);
	assert(gpio->base_address != NULL);
	return ((gpio->base_address[gpio->read_offset>>2] & mask) == 0 ? myGPIO_reset : myGPIO_set);
}

myGPIO_mask myGPIO_getRead(myGPIO_t *gpio) {
	assert(gpio != NULL);
	assert(gpio->base_address != NULL);
	return gpio->base_address[gpio->read_offset>>2];
}

void myGPIO_interruptEnable(myGPIO_t *gpio) {
	assert(gpio != NULL);
	assert(gpio->base_address != NULL);
	gpio->base_address[gpio->int_offset>>2] |= myGPIO_INTR_IntEn_mask;	
}

void myGPIO_interruptDisable(myGPIO_t *gpio) {
	assert(gpio != NULL);
	assert(gpio->base_address != NULL);
	gpio->base_address[gpio->int_offset>>2] &= ~myGPIO_INTR_IntEn_mask;
}

void myGPIO_interruptAck(myGPIO_t *gpio) {
	assert(gpio != NULL);
	assert(gpio->base_address != NULL);
	gpio->base_address[gpio->int_offset>>2] |= myGPIO_INTR_IntAck_mask;
}

myGPIO_value myGPIO_getIrq(myGPIO_t *gpio) {
	assert(gpio != NULL);
	assert(gpio->base_address != NULL);
	return ((gpio->base_address[gpio->int_offset>>2]&myGPIO_INTR_Irq_mask) == 0 ? myGPIO_reset : myGPIO_set);
}
