/*
 * @file ZyboSwitch.h
 * @author Salvatore Barone
 * @email salvator.barone@gmail.com
 * @copyright
 * This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the License, or any later version.
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef __ZYBO_SWITCH_HEADER_H__
#define __ZYBO_SWITCH_HEADER_H__

#include "gpio.h"

/**
 * @brief Maschere di selezione degli switch
 */
typedef enum {
	ZyboSwitch3 = 0x8, //!< ZyboSwitch3, seleziona lo switch 3 sulla board Digilent Zybo;
	ZyboSwitch2 = 0x4, //!< ZyboSwitch2, seleziona lo switch 2 sulla board Digilent Zybo;
	ZyboSwitch1 = 0x2, //!< ZyboSwitch1, seleziona lo switch 1 sulla board Digilent Zybo;
	ZyboSwitch0 = 0x1 //!< ZyboSwitch0, seleziona lo switch 0 sulla board Digilent Zybo;
} ZyboSwitch_mask_t;

/**
 * @brief Status di attivo/inattivo degli switch
 */
typedef enum {
	ZyboSwitch_off,	//!< ZyboSwitch_off, corrisponde al valore logico '0', indica che lo switch e' inattivo;
	ZyboSwitch_on	//!< ZyboSwitch_on, corrisponde al valore logico '1', indica che lo switch e' attivo;
} ZyboSwitch_status_t;

/**
 * @brief Struttura opaca che astrae l'insieme degli switch presenti sulla board Digilent Zybo;
 */
typedef struct {
	GPIO_t		*gpio;
	GPIO_mask 	Switch3_pin;
	GPIO_mask 	Switch2_pin;
	GPIO_mask 	Switch1_pin;
	GPIO_mask 	Switch0_pin;
} ZyboSwitch_t;

/**
 * @brief Inizializza un oggetto di tipo ZyboSwitch_t.
 * @param switches		puntatore a struttura ZyboSwitch_t, che astrae l'insieme degli switch presenti sulla board Digilent Zybo;
 * @param gpio     		puntatore a struttura GPIO_t, che astrae un device GPIO; non viene inizializzato dalla funziona, sara'
 *                   	necessario inizializzarlo preventivamente; si faccia riferimento all'esempio riportato di seguito
 * @param Switch3_pin	pin del device GPIO a cui e' associato lo switch 3 della board Digilent Zybo;
 * @param Switch2_pin	pin del device GPIO a cui e' associato lo switch 2 della board Digilent Zybo;
 * @param Switch1_pin	pin del device GPIO a cui e' associato lo switch 1 della board Digilent Zybo;
 * @param Switch0_pin	pin del device GPIO a cui e' associato lo switch 0 della board Digilent Zybo;
 *
 * GPIO_t gpioSwitch;
 * GPIO_init(&gpioSwitch, XPAR_MYGPIO_1_S00_AXI_BASEADDR, 4, 0, 4, 8);
 * ZyboSwitch_t switches;
 * ZyboSwitch_init(&switches, &gpioSwitch, GPIO_pin3, GPIO_pin2, GPIO_pin1, GPIO_pin0);
 */
void ZyboSwitch_init(	ZyboSwitch_t	*switches,
						GPIO_t			*gpio,
						GPIO_mask 		Switch3_pin,
						GPIO_mask 		Switch2_pin,
						GPIO_mask 		Switch1_pin,
						GPIO_mask 		Switch0_pin);

/**
 * @brief Permette la lettura dello stato degli switch presenti sulla board.
 * @param switches		puntatore a struttura ZyboSwitch_t, che astrae l'insieme degli switch presenti sulla board Digilent Zybo;
 * @param mask			maschera di selezione degli switch, quelli non selezionati non vengono tenuti in considerazione
 * @return				status status degli switch
 *                   	- ZyboSwitch_on se uno degli switch selezionati e' attivo;
 *                    	- ZyboSwitch_off altrimenti
 *
 * ZyboSwitch_status_t switch_status = ZyboSwitch_getStatus(&switches, ZyboSwitch3);			// leggo lo stato dello switch 3
 * ZyboLed_status_t led_status = (switch_status == ZyboSwitch_on ? ZyboLed_on : ZyboLed_off);	// se lo switch 3 e' attivo accendo il led 3
 * ZyboLed_setStatus(&leds, ZyboLed3, led_status);												// accendo/spengo led 3
 */
ZyboSwitch_status_t ZyboSwitch_getStatus(ZyboSwitch_t *switches, ZyboSwitch_mask_t mask);

#endif
