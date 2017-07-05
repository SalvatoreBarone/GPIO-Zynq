/**
 * @example bsp_example.c
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
 *
 * @brief Uso del driver myGPIO bare-metal per il pilotaggio dei Led, lettura di switch e button,
 * pilotaggio di un display Hitachihd44780.
 */
#include <stdio.h>
#include "xparameters.h"
#include "myGPIO.h"
#include "ZyboLed.h"
#include "ZyboSwitch.h"
#include "ZyboButton.h"
#include "hd44780.h"

int main()
{
    myGPIO_t gpioLed;
    myGPIO_Init(&gpioLed, XPAR_MYGPIO_0_S00_AXI_BASEADDR);
    ZyboLed_t boardLed;
    ZyboLed_init(&boardLed, &gpioLed, myGPIO_pin3, myGPIO_pin2, myGPIO_pin1, myGPIO_pin0);

    myGPIO_t gpioSwitch;
    myGPIO_Init(&gpioSwitch, XPAR_MYGPIO_1_S00_AXI_BASEADDR);
    ZyboSwitch_t boardSwitch;
    ZyboSwitch_init(&boardSwitch, &gpioSwitch, myGPIO_pin3, myGPIO_pin2, myGPIO_pin1, myGPIO_pin0);

    myGPIO_t gpioButton;
    myGPIO_Init(&gpioButton, XPAR_MYGPIO_2_S00_AXI_BASEADDR);
    ZyboButton_t boardButton;
    ZyboButton_init(&boardButton, &gpioButton, myGPIO_pin3, myGPIO_pin2, myGPIO_pin1, myGPIO_pin0);

    myGPIO_t gpioDisplay;
    myGPIO_Init(&gpioDisplay, XPAR_MYGPIO_3_S00_AXI_BASEADDR);

    HD44780_LCD_t lcd;
// Si decommenti le seguenti linee se si desidera interfacciarsi in modalità 8-bit    
//    HD44780_Init8(&lcd, &gpioDisplay, 	myGPIO_pin10, myGPIO_pin9, myGPIO_pin8
//    									myGPIO_pin0, myGPIO_pin1, myGPIO_pin2, myGPIO_pin3,
//										myGPIO_pin4, myGPIO_pin5, myGPIO_pin6, myGPIO_pin7);

    HD44780_Init4(&lcd, &gpioDisplay, 	myGPIO_pin10, myGPIO_pin9, myGPIO_pin8,
        								myGPIO_pin0, myGPIO_pin1, myGPIO_pin2, myGPIO_pin3);
    HD44780_Clear(&lcd);
    HD44780_Print(&lcd, "Hello world!");

    ZyboLed_mask_t led_mask[] = {ZyboLed0, ZyboLed1, ZyboLed2, ZyboLed3};
    ZyboSwitch_mask_t switch_mask[] = {ZyboSwitch0, ZyboSwitch1, ZyboSwitch2, ZyboSwitch3};
    ZyboButton_mask_t button_mask[] = {ZyboButton0, ZyboButton1, ZyboButton2, ZyboButton3};
    for (;;) {
    	int i;
    	for (i=0; i<4; i++) {
    		ZyboButton_status_t button_status = ZyboButton_getStatus(&boardButton, button_mask[i]);
    		ZyboSwitch_status_t switch_status = ZyboSwitch_getStatus(&boardSwitch, switch_mask[i]);
    		ZyboLed_status_t led_status = (button_status == ZyboButton_on || switch_status == ZyboSwitch_on ? ZyboLed_on : ZyboLed_off);
    		ZyboLed_setStatus(&boardLed, led_mask[i], led_status);
    	}
    }

    return 0;
}
