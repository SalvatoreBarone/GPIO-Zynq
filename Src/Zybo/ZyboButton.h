/*
 * @file ZyboButton.h
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

#ifndef __ZYBO_BUTTON_HEADER_H__
#define __ZYBO_BUTTON_HEADER_H__

#include "gpio.h"

/**
 * @addtogroup Zybo
 * @{
 */


/**
 * @brief Maschere di selezione dei PushButton
 */
typedef enum {
	ZyboButton3 = 0x8, //!< ZyboButton3, seleziona il button 3 sulla board Digilent Zybo;
	ZyboButton2 = 0x4, //!< ZyboButton2, seleziona il button 2 sulla board Digilent Zybo;
	ZyboButton1 = 0x2, //!< ZyboButton1, seleziona il button 1 sulla board Digilent Zybo;
	ZyboButton0 = 0x1  //!< ZyboButton0, seleziona il button 0 sulla board Digilent Zybo;
} ZyboButton_mask_t;

/**
 * @brief Status di attivo/inattivo dei PushButton
 */
typedef enum {
	ZyboButton_off,	//!< ZyboButton_off, corrisponde al valore logico '0', indica che un pushbutton e' inattivo;
	ZyboButton_on  	//!< ZyboButton_on, corrisponde al valore logico '1', indica che un pushbutton e' attivo;
} ZyboButton_status_t;

/**
 * @brief Struttura opaca che astrae l'insieme dei button presenti sulla board Digilent Zybo;
 */
typedef struct {
	GPIO_t		*gpio;
	GPIO_mask 	Button3_pin;
	GPIO_mask 	Button2_pin;
	GPIO_mask 	Button1_pin;
	GPIO_mask 	Button0_pin;
} ZyboButton_t;

/**
 * @brief Inizializza un oggetto di tipo ZyboButton_t.
 * @param buttons		puntatore a struttura ZyboButton_t, che astrae l'insieme dei button presenti sulla board Digilent Zybo;
 * @param gpio     		puntatore a struttura GPIO_t, che astrae un device GPIO; non viene inizializzato dalla funziona, sara'
 *                   	necessario inizializzarlo preventivamente; si faccia riferimento all'esempio riportato di seguito
 * @param Button3_pin	pin del device GPIO a cui e' associato il button 3 della board Digilent Zybo;
 * @param Button2_pin	pin del device GPIO a cui e' associato il button 2 della board Digilent Zybo;
 * @param Button1_pin	pin del device GPIO a cui e' associato il button 1 della board Digilent Zybo;
 * @param Button0_pin	pin del device GPIO a cui e' associato il button 0 della board Digilent Zybo;
 *
 * @code
 * GPIO_t gpioButton;
 * GPIO_init(&gpioButton, XPAR_MYGPIO_1_S00_AXI_BASEADDR, 4, 0, 4, 8);
 * ZyboButton_t buttons;
 * ZyboButton_init(&buttons, &gpioButton, GPIO_pin3, GPIO_pin2, GPIO_pin1, GPIO_pin0);
 * @endcode
 *
 * @warning Usa la macro assert per verificare che:
 * - buttons non sia un puntatore nullo;
 * - gpio non sia un puntatore nullo
 * - ButtonN_pin siano tutti pin differenti
 */
void ZyboButton_init(	ZyboButton_t	*buttons,
						GPIO_t			*gpio,
						GPIO_mask 		Button3_pin,
						GPIO_mask 		Button2_pin,
						GPIO_mask 		Button1_pin,
						GPIO_mask 		Button0_pin);

/**
 * @brief Tempo di attesa (in millisecondi) usato per prevenire il fenomeno del bouncing. Il valore di default è 50, determinato
 * empiricamente. Puo' essere modificato a piacimento cambiando il valore alla macro seguente.
 */
#define ZyboButton_DebounceWait 50

/**
 * @brief Permettere di mettere il programma in attesa attiva finche' i button restano inattivi;
 *
 * @warning La funzione integra le funzionalita' di debouncing. Il tempo di attesa e' determinato sulla base del valore della
 * macro ZyboButton_DebounceWait
 *
 * @param buttons puntatore a struttura ZyboButton_t, che astrae l'insieme dei button presenti sulla board Digilent Zybo;
 *
 * @code
 * ZyboButton_waitWhileIdle(&buttons);
 * ZyboButton_status_t button3_status = ZyboButton_getStatus(&buttons, ZyboButton3);				// leggo lo stato ddi button 3
 * ZyboButton_status_t button2_status = ZyboButton_getStatus(&buttons, ZyboButton2);				// leggo lo stato ddi button 2
 * ZyboButton_status_t button1_status = ZyboButton_getStatus(&buttons, ZyboButton1);				// leggo lo stato ddi button 1
 * ZyboButton_status_t button0_status = ZyboButton_getStatus(&buttons, ZyboButton0);				// leggo lo stato ddi button 0
 * ZyboButton_waitWhileBusy(&buttons);
 * @endcode
 *
 * @warning Usa la macro assert per verificare che:
 * - buttons non sia un puntatore nullo;
 * - buttons->gpio non sia un puntatore nullo
 */
void ZyboButton_waitWhileIdle(ZyboButton_t *buttons);

/**
 * @brief Permettere di mettere il programma in attesa attiva finche' i button restano attivi;
 *
 * @warning La funzione integra le funzionalita' di debouncing. Il tempo di attesa e' determinato sulla base del valore della
 * macro ZyboButton_DebounceWait
 *
 * @param buttons puntatore a struttura ZyboButton_t, che astrae l'insieme dei button presenti sulla board Digilent Zybo;
 *
 * @code
 * ZyboButton_waitWhileIdle(&buttons);
 * ZyboButton_status_t button3_status = ZyboButton_getStatus(&buttons, ZyboButton3);				// leggo lo stato ddi button 3
 * ZyboButton_status_t button2_status = ZyboButton_getStatus(&buttons, ZyboButton2);				// leggo lo stato ddi button 2
 * ZyboButton_status_t button1_status = ZyboButton_getStatus(&buttons, ZyboButton1);				// leggo lo stato ddi button 1
 * ZyboButton_status_t button0_status = ZyboButton_getStatus(&buttons, ZyboButton0);				// leggo lo stato ddi button 0
 * ZyboButton_waitWhileBusy(&buttons);
 * @endcode
 *
 * @warning Usa la macro assert per verificare che:
 * - buttons non sia un puntatore nullo;
 * - buttons->gpio non sia un puntatore nullo
 */
void ZyboButton_waitWhileBusy(ZyboButton_t *buttons);

/**
 * @brief Permette la lettura dello stato dei button presenti sulla board.
 * @param buttons		puntatore a struttura ZyboButton_t, che astrae l'insieme dei button presenti sulla board Digilent Zybo;
 * @param mask   		maschera di selezione dei button, quelli non selezionati non vengono tenuti in considerazione
 * @return				status status dei button
 * @retval 				ZyboButton_on se uno dei button selezionati e' attivo;
 * @retval				ZyboButton_off altrimenti
 *
 * @code
 * ZyboButton_status_t button_status = ZyboButton_getStatus(&buttons, ZyboButton3);				// leggo lo stato ddi button 3
 * ZyboLed_status_t led_status = (button_status == ZyboButton_on ? ZyboLed_on : ZyboLed_off);	// se lo stato e' attivo accendo il led 3
 * ZyboLed_setStatus(&leds, ZyboLed3, led_status);												// accendo/spengo led 3
 * @endcode
 *
 * @warning Usa la macro assert per verificare che:
 * - buttons non sia un puntatore nullo;
 * - buttons->gpio non sia un puntatore nullo
 */
ZyboButton_status_t ZyboButton_getStatus(ZyboButton_t *buttons, ZyboButton_mask_t mask);

/** @} */

#endif
