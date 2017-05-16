/*
 * @file hd44780.h
 * @author Salvatore Barone
 * @email salvator.barone@gmail.com
 * @copyright
 * This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the License, or any later version.
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * Un oggetto di tipo HD44780_LCD_t rappresenta un device lcd HD44780. Il modulo e' pensato per
 * permettere la gestione di piu' display da parte dello stesso processore, agendo su oggetti
 * HD44780_LCD_t diversi.
 */

#ifndef __HD44780_LCD__
#define __HD44780_LCD__

/**
 * @addtogroup LCD
 * @{
 */


#include "gpio.h"

typedef enum {
	HD44780_INTERFACE_4bit,
	HD44780_INTERFACE_8bit
} HD44780_InterfaceMode_t;

/**
 * @brief Struttura opaca che astrae un device Display LCD con cntroller Hitachi HD44780, o compatibile.
 * Un oggetto di tipo HD44780_LCD_t rappresenta un device lcd HD44780. Il modulo e' pensato per permettere la gestione di
 * piu' display da parte dello stesso processore, agendo su oggetti HD44780_LCD_t diversi.
 * Il modulo permette di utilizzare sia l'interfacciamento ad otto bit che quello a quattro bit, inizializzando il device
 * opportunamente, attraverso l'uso delle funzioni HD44780_Init8 eHD44780_Init4.
 * Il modulo fornisce anche semplici funzioni per la stampa di un carattere o di una stringa null-terminated di caratteri.
 * Si veda la documentazione delle funzioni HD44780_Printc e HD44780_Print.
 * Inoltre sono presenti diverse funzioni di utilita' generica, come quelle per la pulizia del display, per lo spostamento
 * del cursore di un posto in avanti o indietro, alla riga in basso o in alto.
 */
typedef struct {
	GPIO_t 		*gpio;
	GPIO_mask 	RS;
	GPIO_mask 	RW;
	GPIO_mask 	E;
	GPIO_mask 	Data7;
	GPIO_mask 	Data6;
	GPIO_mask 	Data5;
	GPIO_mask 	Data4;
	GPIO_mask 	Data3;
	GPIO_mask 	Data2;
	GPIO_mask 	Data1;
	GPIO_mask 	Data0;
	HD44780_InterfaceMode_t iface_mode;
} HD44780_LCD_t;

/**
 * @brief Inizializza un display lcd HD44780 con interfacciamento ad 8 bit.
 *
 * @warning Se i pin associati ai segnali di pilotaggio del device non sono correttamente
 * configurati come pin di output, il dispositivo non funzionera' correttamente.
 *
 * @warning Non modificare i campi della struttura dopo che essa sia stata inizializzata.
 *
 * @warning La struttura GPIO_t, a cui fa riferimento il parametro gpio, va inizializzata a parte.
 *
 * @param lcd 	puntatore a struttura di tipo HD44780_LCD_t che descrive un display HD44780 da inizializzare;
 * @param gpio	puntatore alla struttura GPIO_t che astrae il device GPIO a cui il display e' connesso. Non viene inizializzato
 *             	dalla funziona, sara' necessario inizializzarlo preventivamente;
 * @param RS 	pin del device GPIO a cui e' associato il segnale RS (data/command) del display LCD;
 * @param RW	pin del device GPIO a cui e' associato il segnale RW (read/write) del display LCD;
 * @param E 	pin del device GPIO a cui e' associato il segnale E (Enable) del display LCD;
 * @param Data7 pin del device GPIO a cui e' associato il segnale Data7 del display LCD;
 * @param Data6 pin del device GPIO a cui e' associato il segnale Data6 del display LCD;
 * @param Data5 pin del device GPIO a cui e' associato il segnale Data5 del display LCD;
 * @param Data4 pin del device GPIO a cui e' associato il segnale Data4 del display LCD;
 * @param Data3 pin del device GPIO a cui e' associato il segnale Data3 del display LCD;
 * @param Data2 pin del device GPIO a cui e' associato il segnale Data2 del display LCD;
 * @param Data1 pin del device GPIO a cui e' associato il segnale Data1 del display LCD;
 * @param Data0 pin del device GPIO a cui e' associato il segnale Data0 del display LCD;
 *
 * @code
 * GPIO_t gpioDisplay;
 * GPIO_init(&gpioDisplay, XPAR_MYGPIO_3_S00_AXI_BASEADDR, 11, 0, 4, 8);
 * HD44780_LCD_t lcd;
 * HD44780_Init8(&lcd, &gpioDisplay, 	GPIO_pin10, GPIO_pin9, GPIO_pin8,
 * 										GPIO_pin0, GPIO_pin1, GPIO_pin2, GPIO_pin3,
 *           							GPIO_pin4, GPIO_pin5, GPIO_pin6, GPIO_pin7);
 * HD44780_Print(&lcd, "Ciao! Come va");
 * HD44780_MoveToRow2(&lcd);
 * HD44780_Print(&lcd, "lo studio?");
 * @endcode
 */
void HD44780_Init8(	HD44780_LCD_t	*lcd,
					GPIO_t			*gpio,
					GPIO_mask		RS,
					GPIO_mask		RW,
					GPIO_mask		E,
					GPIO_mask		Data7,
					GPIO_mask 		Data6,
					GPIO_mask 		Data5,
					GPIO_mask 		Data4,
					GPIO_mask 		Data3,
					GPIO_mask 		Data2,
					GPIO_mask 		Data1,
					GPIO_mask 		Data0);

/**
 * @brief Inizializza un oggetto display lcd HD44780 affinche' si utilizzi l'interfaccia a 4 bit.
 *
 * Inizializza un oggetto HD44780_LCD_t verificando la validita' delle coppie porta-pin per l'
 * interfacciamento, configurando i pin GPIO e inizializzando il device.
 *
 * @warning Se i pin associati ai segnali di pilotaggio del device non sono correttamente
 * configurati come pin di output, il dispositivo non funzionera' correttamente.
 *
 * @warning Non modificare i campi della struttura HD44780_LCD_t dopo che essa sia stata
 * inizializzata.
 *
 * @warning La struttura GPIO_t, a cui fa riferimento il parametro gpio, va inizializzata a parte.
 *
 * @param lcd 	puntatore a struttura di tipo HD44780_LCD_t che descrive un display HD44780 da inizializzare;
 * @param gpio	puntatore alla struttura GPIO_t che astrae il device GPIO a cui il display e' connesso. Non viene inizializzato
 *             	dalla funziona, sara' necessario inizializzarlo preventivamente;
 * @param RS 	pin del device GPIO a cui e' associato il segnale RS (data/command) del display LCD;
 * @param RW	pin del device GPIO a cui e' associato il segnale RW (read/write) del display LCD;
 * @param E 	pin del device GPIO a cui e' associato il segnale E (Enable) del display LCD;
 * @param Data7 pin del device GPIO a cui e' associato il segnale Data7 del display LCD;
 * @param Data6 pin del device GPIO a cui e' associato il segnale Data6 del display LCD;
 * @param Data5 pin del device GPIO a cui e' associato il segnale Data5 del display LCD;
 * @param Data4 pin del device GPIO a cui e' associato il segnale Data4 del display LCD;
 *
 * @code
 * GPIO_t gpioDisplay;
 * GPIO_init(&gpioDisplay, XPAR_MYGPIO_3_S00_AXI_BASEADDR, 11, 0, 4, 8);
 * HD44780_LCD_t lcd;
 * HD44780_Init4(&lcd, &gpioDisplay, 	GPIO_pin10, GPIO_pin9, GPIO_pin8,
 * 										GPIO_pin0, GPIO_pin1, GPIO_pin2, GPIO_pin3);
 * HD44780_Print(&lcd, "Ciao! Come va");
 * HD44780_MoveToRow2(&lcd);
 * HD44780_Print(&lcd, "lo studio?");
 * @endcode
 */
void HD44780_Init4(	HD44780_LCD_t	*lcd,
					GPIO_t			*gpio,
					GPIO_mask		RS,
					GPIO_mask		RW,
					GPIO_mask		E,
					GPIO_mask		Data7,
					GPIO_mask 		Data6,
					GPIO_mask 		Data5,
					GPIO_mask 		Data4);

/**
 * @brief Stampa un carattere
 * @param lcd display da pilotare;
 * @param c carattere da stampare sul display;
 */
void HD44780_Printc(HD44780_LCD_t* lcd, char c);

/**
 * @brief Stampa una stringa null-terminated di caratteri
 *
 * La funzione può essere utilizzata per stampare anche numeri interi e floating point. Si veda
 * gli esempi di cui sotto.
 *
 * @param lcd display da pilotare;
 * @param s puntatore alla stringa null-terminated da stampare sul display;
 *
 * @code
 * // stampa di un intero
 * #include <stdlib.h>
 * 	...
 * char str[10];	// assicurarsi di allocare sufficiente spazio per la stampa del numero
 * sprintf(str,"%d", integer_number);
 * error = HD44780_Print(lcd, str);
 * // stampa di un intero
 * #include <stdlib.h>
 * ...
 * char str[10];
 * snprintf(str, 10,"%d", integer_number);
 * error = HD44780_Print(lcd, str);
 * // stampa di un float
 * #include <stdlib.h>
 * ...
 * char str[20];	// assicurarsi di allocare sufficiente spazio per la stampa del numero
 * sprintf(str,"%f", float_number);
 * error = HD44780_Print(lcd, str);
 * // stampa di un float, nel caso in cui la soluzione precedente dovesse non funzionare
 * #include <stdlib.h>
 * ...
 * char str[20];
 * int parte_intera, parte_decimale, moltiplicatore = 1000;
 * // se si desiderano piu' di tre cifre decimali basta aumentare la potenza del
 * // moltiplicatore
 * // es. cinque cifre decimali ==> moltiplicatore = 100000
 * // si sconsiglia di stampare piu' di quattro cifre decimali per non causare overflow
 * // nelle istruzioni che seguono
 * parte_intera = (int) float_number;
 * parte_decimale = (int)(float_number * moltiplicatore) - (parte_intera * moltiplicatore);
 * snprintf(str, 20,"%d.%d", parte_intera, parte_decimale);
 * error = HD44780_Print(lcd, str);
 * @endcode
 */
void HD44780_Print(HD44780_LCD_t* lcd, const char *s);

/**
 * @brief Pulisce il display e sposta il cursore all'inizio della prima riga
 * @param lcd display da pilotare;
 */
void HD44780_Clear(HD44780_LCD_t* lcd);

/**
 * @brief Sposta il cursore all'inizio della prima riga
 * @param lcd display da pilotare;
 */
void HD44780_Home(HD44780_LCD_t* lcd);

/**
 * @brief Sposta il cursore all'inizio della prima riga
 * @param lcd display da pilotare;
 */
void HD44780_MoveToRow1(HD44780_LCD_t* lcd);

/**
 * @brief Sposta il cursore all'inizio della seconda riga
 * @param lcd display da pilotare;
 */
void HD44780_MoveToRow2(HD44780_LCD_t* lcd);

/**
 * @brief Direzioni di spostamento del cursore
 */
typedef enum {
	HD44780_CursorLeft,//!< left sposta il cursore a sinistra
	HD44780_CursorRight//!< right sposta il cursore a destra
} HD44780_Direction_t;

/**
 * @brief Sposta il cursore di una posizione a destra o sinistra
 * @param lcd display da pilotare;
 * @param dir direzione in cui spostare il cursore, @see direction_t;
 */
void HD44780_MoveCursor(HD44780_LCD_t* lcd, HD44780_Direction_t dir);

/**
 * @brief Disattiva il display
 * @param lcd display da pilotare;
 */
void HD44780_DisplayOff(HD44780_LCD_t* lcd);

/**
 * @brief Disattiva la visualizzazione del cursore
 * @param lcd display da pilotare;
 */
void HD44780_CursorOff(HD44780_LCD_t* lcd);

/**
 * @brief Attiva la visualizzazione del cursore
 * @param lcd display da pilotare;
 */
void HD44780_CursorOn(HD44780_LCD_t* lcd);

/**
 * @brief Attiva il cursore lampeggiante
 * @param lcd display da pilotare;
 */
void HD44780_CursorBlink(HD44780_LCD_t* lcd);

/** @} */

#endif