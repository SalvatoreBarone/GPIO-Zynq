/**
 * @file ZyboLed.c
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

#include "ZyboLed.h"
#include <assert.h>
#include <stdlib.h>

static int validatePair(ZyboLed_t* leds)
{
	int array_dim = 4;
	int i, j;
	const myGPIO_mask pair[] = {leds->Led3_pin, leds->Led2_pin, leds->Led1_pin, leds->Led0_pin};
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

void ZyboLed_init(	ZyboLed_t	*leds,
					myGPIO_t		*gpio,
					myGPIO_mask 	Led3_pin,
					myGPIO_mask 	Led2_pin,
					myGPIO_mask 	Led1_pin,
					myGPIO_mask 	Led0_pin) {
	assert(leds);
	assert(gpio);
	leds->gpio = gpio;
	leds->Led3_pin = Led3_pin;
	leds->Led2_pin = Led2_pin;
	leds->Led1_pin = Led1_pin;
	leds->Led0_pin = Led0_pin;
	assert(validatePair(leds));
	myGPIO_setMode(leds->gpio, leds->Led3_pin | leds->Led2_pin | leds->Led1_pin | leds->Led0_pin, myGPIO_write);
	myGPIO_setValue(leds->gpio, leds->Led3_pin| leds->Led2_pin | leds->Led1_pin | leds->Led0_pin, myGPIO_reset);

}

void ZyboLed_setStatus(ZyboLed_t *leds, ZyboLed_mask_t mask, ZyboLed_status_t status) {
	assert(leds);
	assert(leds->gpio);
	myGPIO_mask gpio_mask = 0;
	gpio_mask |= ((mask & ZyboLed3) != 0 ? leds->Led3_pin : 0);
	gpio_mask |= ((mask & ZyboLed2) != 0 ? leds->Led2_pin : 0);
	gpio_mask |= ((mask & ZyboLed1) != 0 ? leds->Led1_pin : 0);
	gpio_mask |= ((mask & ZyboLed0) != 0 ? leds->Led0_pin : 0);
	myGPIO_setValue(leds->gpio, gpio_mask, (status == ZyboLed_off ? myGPIO_reset : myGPIO_set));
}


void ZyboLed_toggle(ZyboLed_t *leds, ZyboLed_mask_t mask) {
	assert(leds);
	assert(leds->gpio);
	myGPIO_mask gpio_mask = 0;
	gpio_mask |= ((mask & ZyboLed3) != 0 ? leds->Led3_pin : 0);
	gpio_mask |= ((mask & ZyboLed2) != 0 ? leds->Led2_pin : 0);
	gpio_mask |= ((mask & ZyboLed1) != 0 ? leds->Led1_pin : 0);
	gpio_mask |= ((mask & ZyboLed0) != 0 ? leds->Led0_pin : 0);
	myGPIO_toggle(leds->gpio, gpio_mask);
}
