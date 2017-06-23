/**
 * @file ZyboButton.c
 * @author Salvatore Barone <salvator.barone@gmail.com>
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

#include "ZyboButton.h"
#include <assert.h>
#include <stdlib.h>

#define timer_wait_ms(ms)   usleep(ms<<10)

static int validatePair(ZyboButton_t* buttons) {
	int array_dim = 4;
	int i, j;
	const myGPIO_mask pair[] = {buttons->Button3_pin, buttons->Button2_pin, buttons->Button1_pin, buttons->Button0_pin};
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

void ZyboButton_init(	ZyboButton_t	*buttons,
						myGPIO_t			*gpio,
						myGPIO_mask 		Button3_pin,
						myGPIO_mask 		Button2_pin,
						myGPIO_mask 		Button1_pin,
						myGPIO_mask 		Button0_pin) {
	assert(buttons);
	assert(gpio);
	buttons->gpio = gpio;
	buttons->Button3_pin = Button3_pin;
	buttons->Button2_pin = Button2_pin;
	buttons->Button1_pin = Button1_pin;
	buttons->Button0_pin = Button0_pin;
	assert(validatePair(buttons));
	myGPIO_SetMode(buttons->gpio, buttons->Button3_pin | buttons->Button2_pin | buttons->Button1_pin | buttons->Button0_pin, myGPIO_read);
	myGPIO_SetValue(buttons->gpio, buttons->Button3_pin| buttons->Button2_pin | buttons->Button1_pin | buttons->Button0_pin, myGPIO_reset);
}

void ZyboButton_waitWhileIdle(ZyboButton_t *buttons) {
	while (ZyboButton_getStatus(buttons, ZyboButton3 | ZyboButton2 | ZyboButton1 | ZyboButton0) == ZyboButton_off);
	timer_wait_ms(ZyboButton_DebounceWait);
}

void ZyboButton_waitWhileBusy(ZyboButton_t *buttons) {
	while (ZyboButton_getStatus(buttons, ZyboButton3 | ZyboButton2 | ZyboButton1 | ZyboButton0) == ZyboButton_on);
	timer_wait_ms(ZyboButton_DebounceWait);
}

ZyboButton_status_t ZyboButton_getStatus(ZyboButton_t *buttons, ZyboButton_mask_t mask) {
	assert(buttons);
	assert(buttons->gpio);
	myGPIO_mask gpio_mask = 0;
	gpio_mask |= ((mask & ZyboButton3) != 0 ? buttons->Button3_pin : 0);
	gpio_mask |= ((mask & ZyboButton2) != 0 ? buttons->Button2_pin : 0);
	gpio_mask |= ((mask & ZyboButton1) != 0 ? buttons->Button1_pin : 0);
	gpio_mask |= ((mask & ZyboButton0) != 0 ? buttons->Button0_pin : 0);
	return (myGPIO_GetValue(buttons->gpio, gpio_mask) == myGPIO_reset ? ZyboButton_off : ZyboButton_on);
}