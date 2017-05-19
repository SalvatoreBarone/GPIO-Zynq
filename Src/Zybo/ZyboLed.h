/*
 * @file ZyboLed.h
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

#ifndef __ZYBO_LED_HEADER_H__
#define __ZYBO_LED_HEADER_H__

#include "gpio.h"

/**
 * @addtogroup Zybo
 * @{
 *
 * @defgroup Led
 * @{
 */

/**
 * @brief Maschere di selezione dei led
 */
typedef enum {
	ZyboLed3 = 0x8, //!< ZyboLed3, seleziona il led 3 sulla board Digilent Zybo;
	ZyboLed2 = 0x4, //!< ZyboLed2, seleziona il led 2 sulla board Digilent Zybo;
	ZyboLed1 = 0x2, //!< ZyboLed1, seleziona il led 1 sulla board Digilent Zybo;
	ZyboLed0 = 0x1 //!< ZyboLed0, seleziona il led 0 sulla board Digilent Zybo;
} ZyboLed_mask_t;

/**
 * @brief  Metodo alternativo per la specifica di uno dei led presenti sulla board Digilent Zybo
 * @param i indice del led da selezionare, da 0 a 3
 * @return  maschera di selezione del led i-esimo
 */
#define ZyboLed(i) ((uint32_t)(1<<i))

/**
 * @brief Status di accensione/spegnimento dei led
 */
typedef enum {
	ZyboLed_off,	//!< ZyboLed_off, corrisponde al valore logico '0', per lo spegnimento dei Led
	ZyboLed_on		//!< ZyboLed_on, corrisponde al valore logico '1', per l'accensione dei Led
} ZyboLed_status_t;

/**
 * @brief Struttura opaca che astrae l'insieme dei Led presenti sulla board Digilent Zybo;
 */
typedef struct {
	GPIO_t		*gpio;
	GPIO_mask 	Led3_pin;
	GPIO_mask 	Led2_pin;
	GPIO_mask 	Led1_pin;
	GPIO_mask 	Led0_pin;
} ZyboLed_t;

/**
 * @brief Inizializza un oggetto di tipo ZyboLed_t.
 * @param leds     puntatore a struttura ZyboLed_t, che astrae l'insieme dei Led presenti sulla board Digilent Zybo;
 * @param gpio     puntatore a struttura GPIO_t, che astrae un device GPIO; non viene inizializzato dalla funziona, sara'
 *                 necessario inizializzarlo preventivamente; si faccia riferimento all'esempio riportato di seguito
 * @param Led3_pin pin del device GPIO a cui e' associato il Led3 della board Digilent Zybo;
 * @param Led2_pin pin del device GPIO a cui e' associato il Led2 della board Digilent Zybo;
 * @param Led1_pin pin del device GPIO a cui e' associato il Led1 della board Digilent Zybo;
 * @param Led0_pin pin del device GPIO a cui e' associato il Led0 della board Digilent Zybo;
 *
 * @code
 * GPIO_t gpioLed;
 * GPIO_init(&gpioLed, XPAR_MYGPIO_0_S00_AXI_BASEADDR, 4, 0, 4, 8);				// inizializzazione del device GPIO
 * ZyboLed_t leds;
 * ZyboLed_init(&leds, &gpioLed, GPIO_pin3, GPIO_pin2, GPIO_pin1, GPIO_pin0);	// inzializzazione della struttura ZyboLed_t
 * @endcode
 *
 * @warning Usa la macro assert per verificare che:
 * - leds non sia un puntatore nullo;
 * - gpio non sia un puntatore nullo
 * - LedN_pin siano tutti pin differenti
 */
void ZyboLed_init(	ZyboLed_t	*leds,
					GPIO_t		*gpio,
					GPIO_mask 	Led3_pin,
					GPIO_mask 	Led2_pin,
					GPIO_mask 	Led1_pin,
					GPIO_mask 	Led0_pin);

/**
 * @brief Permette di accendere/spegnere i Led sulla board
 * @param leds   puntatore a struttura ZyboLed_t, che astrae l'insieme dei Led presenti sulla board Digilent Zybo;
 * @param mask   maschera di selezione dei led, quelli non selezionati vengono lasciati inalterati
 * @param status status dei led, ZyboLed_on per accendere, ZyboLed_off per spegnere
 *
 * @code
 * ZyboLed_setStatus(&leds, ZyboLed3 | ZyboLed1, ZyboLed_on);	// accensione dei Led 3 ed 1
 * ZyboLed_setStatus(&leds, ZyboLed3 | ZyboLed1, ZyboLed_off);	// spegnimento dei Led 3 ed 1
 * ZyboLed_setStatus(&leds, ZyboLed2 | ZyboLed0, ZyboLed_on);	// accendsione dei Led 2 ed 0
 * ZyboLed_setStatus(&leds, ZyboLed2 | ZyboLed0, ZyboLed_off);	// spegnimento dei Led 2 ed 0
 * ZyboLed_setStatus(&leds, ZyboLed3 | ZyboLed1 | ZyboLed2 | ZyboLed0, ZyboLed_on);		// accensone di tutti i led
 * ZyboLed_setStatus(&leds, ZyboLed3 | ZyboLed1 | ZyboLed2 | ZyboLed0, ZyboLed_off);	// spegnimento di tutti i led
 * @endcode
 *
 * @warning Usa la macro assert per verificare che:
 * - leds non sia un puntatore nullo;
 * - leds->gpio non sia un puntatore nullo
 */
void ZyboLed_setStatus(ZyboLed_t *leds, ZyboLed_mask_t mask, ZyboLed_status_t status);

/**
 * @brief Permette di accendere/spegnere i Led sulla board, invertendone il valore
 * @param leds   puntatore a struttura ZyboLed_t, che astrae l'insieme dei Led presenti sulla board Digilent Zybo;
 * @param mask   maschera di selezione dei led, quelli non selezionati vengono lasciati inalterati
 *
 * @code
 * ZyboLed_toggle(&leds, ZyboLed3 | ZyboLed1);	// accensione/spegnimento dei Led 3 ed 1
 * @endcode
 *
 * @warning Usa la macro assert per verificare che:
 * - leds non sia un puntatore nullo;
 * - leds->gpio non sia un puntatore nullo
 */
void ZyboLed_toggle(ZyboLed_t *leds, ZyboLed_mask_t mask);

/**
 * @}
 * @}
 */

#endif
