/**
 * @file ZyboSwitch.c
 * @author Salvatore Barone
 * @email salvator.barone@gmail.com
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

#include "ZyboSwitch.h"
#include <assert.h>
#include <stdlib.h>

static int validatePair(ZyboSwitch_t* switches) {
	int array_dim = 4;
	int i, j;
	const myGPIO_mask pair[] = {switches->Switch3_pin, switches->Switch2_pin, switches->Switch1_pin, switches->Switch0_pin};
	for (i = 0; i < array_dim; i++) {
		if (pair[i] == 0)
			return 0;
		for (j = i+1; j < array_dim; j++) {
			if (pair[j] == 0)
			return 0;
			if (pair[i] == pair[j])
			return 0;
		}
	}
	return 1;
}

void ZyboSwitch_init(	ZyboSwitch_t	*switches,
						myGPIO_t			*gpio,
						myGPIO_mask 		Switch3_pin,
						myGPIO_mask 		Switch2_pin,
						myGPIO_mask 		Switch1_pin,
						myGPIO_mask 		Switch0_pin) {
	assert(switches);
	assert(gpio);
	switches->gpio = gpio;
	switches->Switch3_pin = Switch3_pin;
	switches->Switch2_pin = Switch2_pin;
	switches->Switch1_pin = Switch1_pin;
	switches->Switch0_pin = Switch0_pin;
	assert(validatePair(switches));
	myGPIO_setMode(switches->gpio, switches->Switch3_pin | switches->Switch2_pin | switches->Switch1_pin | switches->Switch0_pin, myGPIO_read);
	myGPIO_setValue(switches->gpio, switches->Switch3_pin| switches->Switch2_pin | switches->Switch1_pin | switches->Switch0_pin, myGPIO_reset);
}

ZyboSwitch_status_t ZyboSwitch_getStatus(ZyboSwitch_t *switches, ZyboSwitch_mask_t mask) {
	assert(switches);
	assert(switches->gpio);
	myGPIO_mask gpio_mask = 0;
	gpio_mask |= ((mask & ZyboSwitch3) != 0 ? switches->Switch3_pin : 0);
	gpio_mask |= ((mask & ZyboSwitch2) != 0 ? switches->Switch2_pin : 0);
	gpio_mask |= ((mask & ZyboSwitch1) != 0 ? switches->Switch1_pin : 0);
	gpio_mask |= ((mask & ZyboSwitch0) != 0 ? switches->Switch0_pin : 0);
	return (myGPIO_getValue(switches->gpio, gpio_mask) == myGPIO_reset ? ZyboSwitch_off : ZyboSwitch_on);
}
