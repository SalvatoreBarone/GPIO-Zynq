/*
 * @file gpio.c
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
#include "gpio.h"
#include <stdlib.h>
#include <assert.h>

void GPIO_init(	GPIO_t* 	gpio,
				uint32_t	*base_address,
				uint8_t		width,
				uint8_t		enable_offset,
				uint8_t		write_offset,
				uint8_t		read_offset) {

	assert(gpio != NULL);
	assert(base_address != NULL);
	assert(enable_offset != write_offset && enable_offset != read_offset && write_offset != read_offset);
	gpio->base_address = base_address;
	gpio->width = width;
	gpio->enable_offset = enable_offset;
	gpio->write_offset = write_offset;
	gpio->read_offset = read_offset;
}

void GPIO_setMode(GPIO_t *gpio, GPIO_mask mask, GPIO_mode mode) {
	assert(gpio != NULL);
	assert(gpio->base_address != NULL);
	uint32_t value = gpio->base_address[gpio->enable_offset>>2];
	gpio->base_address[gpio->enable_offset>>2] = (mode == GPIO_write ?  value|mask : value&(~mask));
}

void GPIO_setValue(GPIO_t *gpio, GPIO_mask mask, GPIO_value value) {
	assert(gpio != NULL);
	assert(gpio->base_address != NULL);
	uint32_t actual_value = gpio->base_address[gpio->write_offset>>2];
	gpio->base_address[gpio->write_offset>>2] = (value == GPIO_set ?  actual_value|mask : actual_value&(~mask));
}

void GPIO_toggle(GPIO_t* gpio, GPIO_mask mask) {
	assert(gpio != NULL);
	assert(gpio->base_address != NULL);
	uint32_t actual_value = gpio->base_address[gpio->write_offset>>2];
	gpio->base_address[gpio->write_offset>>2] = actual_value ^ mask;
}

GPIO_value GPIO_getValue(GPIO_t *gpio, GPIO_mask mask) {
	assert(gpio != NULL);
	assert(gpio->base_address != NULL);
	return ((gpio->base_address[gpio->read_offset>>2] & mask) == 0 ? GPIO_reset : GPIO_set);
}
