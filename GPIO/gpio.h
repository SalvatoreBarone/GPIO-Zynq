/*
 * @file gpio.h
 *
 * @author: Salvatore Barone <salvator.barone@gmail.com>, <salvator.barone@studenti.unina.it>
 * @date: 12 maggio 2017
 * @copyright
 * Copyright 2017 Salvatore Barone <salvator.barone@gmail.com>, <salvator.barone@studenti.unina.it>
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef __GPIO_HEADER_H__
#define __GPIO_HEADER_H__

#include <inttypes.h>

/**
 *
 */
typedef struct {
	uint32_t*	base_address;
	uint8_t		width;
	uint8_t		enable_offset;
	uint8_t		write_offset;
	uint8_t		read_offset;
} GPIO_t;


typedef enum {
	GPIO_bit0 = 0x1,
	GPIO_bit1 = 0x2,
	GPIO_bit2 = 0x4,
	GPIO_bit3 = 0x8,
	GPIO_bit4 = 0x10,
	GPIO_bit5 = 0x20,
	GPIO_bit6 = 0x40,
	GPIO_bit7 = 0x80,
	GPIO_bit8 = 0x100,
	GPIO_bit9 = 0x200,
	GPIO_bit10 = 0x400,
	GPIO_bit11 = 0x800,
	GPIO_bit12 = 0x1000,
	GPIO_bit13 = 0x2000,
	GPIO_bit14 = 0x4000,
	GPIO_bit15 = 0x8000,
	GPIO_bit16 = 0x10000,
	GPIO_bit17 = 0x20000,
	GPIO_bit18 = 0x40000,
	GPIO_bit19 = 0x80000,
	GPIO_bit20 = 0x100000,
	GPIO_bit21 = 0x200000,
	GPIO_bit22 = 0x400000,
	GPIO_bit23 = 0x800000,
	GPIO_bit24 = 0x1000000,
	GPIO_bit25 = 0x2000000,
	GPIO_bit26 = 0x4000000,
	GPIO_bit27 = 0x8000000,
	GPIO_bit28 = 0x10000000,
	GPIO_bit29 = 0x20000000,
	GPIO_bit30 = 0x40000000,
	GPIO_bit31 = 0x80000000
} GPIO_mask;

/**
 *
 */
typedef enum {
	GPIO_read,//!< GPIO_read
	GPIO_write//!< GPIO_write
} GPIO_mode;

/**
 * @brief GPIO_value
 */
typedef enum {
	GPIO_reset,//!< GPIO_reset
	GPIO_set   //!< GPIO_set
} GPIO_value;

typedef enum {
	GPIO_init_null_pointer,
	GPIO_init_invalid_width,
	GPIO_init_invalid_offset
} GPIO_init_error;

GPIO_init_error gpio_init(	GPIO_t* 	gpio,
							uint32_t	*base_address,
							uint8_t		width,
							uint8_t		enable_offset,
							uint8_t		write_offset,
							uint8_t		read_offset);

void gpio_setMode(GPIO_t* gpio, GPIO_mask mask, GPIO_mode mode);

void gpio_setValue(GPIO_t* gpio, GPIO_mask mask, GPIO_value value);

#endif
