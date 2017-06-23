/**
 * @file ZyboSwitch.h
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

#ifndef __ZYBO_SWITCH_HEADER_H__
#define __ZYBO_SWITCH_HEADER_H__

#include "myGPIO.h"

/**
 * @addtogroup myGPIO
 * @{
 * @addtogroup bare-metal
 * @{
 * @addtogroup Zybo
 * @{
 * @defgroup Switch
 * @{
 *
 * @brief supporto per gestione degli switch su board Digilent Zybo
 */


/**
 * @brief Maschere di selezione degli switch
 */
typedef enum {
	ZyboSwitch3 = 0x8U, //!< ZyboSwitch3, seleziona lo switch 3 sulla board Digilent Zybo;
	ZyboSwitch2 = 0x4U, //!< ZyboSwitch2, seleziona lo switch 2 sulla board Digilent Zybo;
	ZyboSwitch1 = 0x2U, //!< ZyboSwitch1, seleziona lo switch 1 sulla board Digilent Zybo;
	ZyboSwitch0 = 0x1U //!< ZyboSwitch0, seleziona lo switch 0 sulla board Digilent Zybo;
} ZyboSwitch_mask_t;

/**
 * @brief  Metodo alternativo per la specifica di uno degli switch presenti sulla board Digilent Zybo
 * @param[in] i indice dello switch da selezionare, da 0 a 3
 * @return maschera di selezione dello switch i-esimo
 */
#define ZyboSwitch(i) ((uint32_t)(1<<(i)))

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
	myGPIO_t		*gpio;			/**< puntatore a struttura myGPIO_t, che astrae il particolare GPIO usato per la lettura dello
									stato degli switch presenti sulla board */
	myGPIO_mask 	Switch3_pin;	/**< maschera di selezione per il particolare bit del device GPIO connesso allo switch numero 3
									 della board Zybo*/
	myGPIO_mask 	Switch2_pin;	/**< maschera di selezione per il particolare bit del device GPIO connesso allo switch numero 2
									 della board Zybo*/
	myGPIO_mask 	Switch1_pin;	/**< maschera di selezione per il particolare bit del device GPIO connesso allo switch numero 1
									 della board Zybo*/
	myGPIO_mask 	Switch0_pin;	/**< maschera di selezione per il particolare bit del device GPIO connesso allo switch numero 0
									 della board Zybo*/
} ZyboSwitch_t;

/**
 * @brief Inizializza un oggetto di tipo ZyboSwitch_t.
 * @param[inout]	switches		puntatore a struttura ZyboSwitch_t, che astrae l'insieme degli switch presenti sulla board
 * 									Digilent Zybo;
 * @param[in]		gpio     		puntatore a struttura myGPIO_t, che astrae un device GPIO; non viene inizializzato dalla
 * 									funziona, sara' necessario inizializzarlo preventivamente; si faccia riferimento all'esempio
 * 									riportato di seguito
 * @param[in] 		Switch3_pin		pin del device GPIO a cui e' associato lo switch 3 della board Digilent Zybo;
 * @param[in] 		Switch2_pin		pin del device GPIO a cui e' associato lo switch 2 della board Digilent Zybo;
 * @param[in] 		Switch1_pin		pin del device GPIO a cui e' associato lo switch 1 della board Digilent Zybo;
 * @param[in] 		Switch0_pin		pin del device GPIO a cui e' associato lo switch 0 della board Digilent Zybo;
 *
 * @code
 * myGPIO_t gpioSwitch;
 * GPIO_init(&gpioSwitch, XPAR_MYGPIO_1_S00_AXI_BASEADDR, 4, 0, 4, 8);
 * ZyboSwitch_t switches;
 * ZyboSwitch_init(&switches, &gpioSwitch, GPIO_pin3, GPIO_pin2, GPIO_pin1, GPIO_pin0);
 * @endcode
 *
 * @warning Usa la macro assert per verificare che:
 * - switches non sia un puntatore nullo;
 * - gpio non sia un puntatore nullo
 * - SwitchN_pin siano tutti pin differenti
 */
void ZyboSwitch_init(	ZyboSwitch_t	*switches,
						myGPIO_t		*gpio,
						myGPIO_mask 	Switch3_pin,
						myGPIO_mask 	Switch2_pin,
						myGPIO_mask 	Switch1_pin,
						myGPIO_mask 	Switch0_pin);

/**
 * @brief Permette la lettura dello stato degli switch presenti sulla board.
 *
 * @param[in]	switches	puntatore a struttura ZyboSwitch_t, che astrae l'insieme degli switch presenti sulla board
 * 							Digilent Zybo;
 * @param[in]	mask		maschera di selezione degli switch, quelli non selezionati non vengono tenuti in considerazione
 *
 * @return status status degli switch
 * @retval ZyboSwitch_on se uno degli switch selezionati e' attivo;
 * @retval ZyboSwitch_off altrimenti
 *
 * @code
 * ZyboSwitch_status_t switch_status = ZyboSwitch_getStatus(&switches, ZyboSwitch3);			// leggo lo stato dello switch 3
 * ZyboLed_status_t led_status = (switch_status == ZyboSwitch_on ? ZyboLed_on : ZyboLed_off);	// se lo switch 3 e' attivo accendo il led 3
 * ZyboLed_setStatus(&leds, ZyboLed3, led_status);												// accendo/spengo led 3
 * @endcode
 *
 * @warning Usa la macro assert per verificare che:
 * - switches non sia un puntatore nullo;
 * - switches->gpio non sia un puntatore nullo
 */
ZyboSwitch_status_t ZyboSwitch_getStatus(ZyboSwitch_t *switches, ZyboSwitch_mask_t mask);

/**
 * @}
 * @}
 * @}
 * @}
 */

#endif
