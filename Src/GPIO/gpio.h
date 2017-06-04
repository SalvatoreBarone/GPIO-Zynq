/*
 * @file gpio.h
 * @author: Salvatore Barone
 * @email salvator.barone@gmail.com
 * @date: 12 05 2017
 * @copyright
 * Copyright 2017 Salvatore Barone <salvator.barone@gmail.com>, <salvator.barone@studenti.unina.it>
 * This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or any later version.
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef __GPIO_DRIVER_HEADER_H__
#define __GPIO_DRIVER_HEADER_H__

#include <inttypes.h>

/**
 * @addtogroup GPIO
 * @{
 *
 * Il modulo definisce un driver OO-like per l'utilizzo di una periferica GPIO custom.
 */

/**
 * @brief Struttura che astrae un device GPIO.
 */
typedef struct {
	uint32_t*	base_address;	/**< indirizzo base */
	uint8_t		width;			/**< dimensione, in bit */
	uint8_t		enable_offset;	/**< offset, rispetto all'indirizzo base, del registro "enable" */
	uint8_t		write_offset;	/**< offset, rispetto all'indirizzo base, del registro "write" */
	uint8_t		read_offset;	/**< offset, rispetto all'indirizzo base, del registro "read" */
} GPIO_t;

/**
 * @brief Maschere di selezione dei pin di un device GPIO
 */
typedef enum {
	GPIO_pin0 = 0x1,        //!< GPIO pin0 maschera di selezione del pin 0 di un device GPIO
	GPIO_pin1 = 0x2,        //!< GPIO pin1 maschera di selezione del pin 1 di un device GPIO
	GPIO_pin2 = 0x4,        //!< GPIO pin2 maschera di selezione del pin 2 di un device GPIO
	GPIO_pin3 = 0x8,        //!< GPIO pin3
	GPIO_pin4 = 0x10,       //!< GPIO pin4
	GPIO_pin5 = 0x20,       //!< GPIO pin5
	GPIO_pin6 = 0x40,       //!< GPIO pin6
	GPIO_pin7 = 0x80,       //!< GPIO pin7
	GPIO_pin8 = 0x100,      //!< GPIO pin8
	GPIO_pin9 = 0x200,      //!< GPIO pin9
	GPIO_pin10 = 0x400,     //!< GPIO pin10
	GPIO_pin11 = 0x800,     //!< GPIO pin11
	GPIO_pin12 = 0x1000,    //!< GPIO pin12
	GPIO_pin13 = 0x2000,    //!< GPIO pin13
	GPIO_pin14 = 0x4000,    //!< GPIO pin14
	GPIO_pin15 = 0x8000,    //!< GPIO pin15
	GPIO_pin16 = 0x10000,   //!< GPIO pin16
	GPIO_pin17 = 0x20000,   //!< GPIO pin17
	GPIO_pin18 = 0x40000,   //!< GPIO pin18
	GPIO_pin19 = 0x80000,   //!< GPIO pin19
	GPIO_pin20 = 0x100000,  //!< GPIO pin20
	GPIO_pin21 = 0x200000,  //!< GPIO pin21
	GPIO_pin22 = 0x400000,  //!< GPIO pin22
	GPIO_pin23 = 0x800000,  //!< GPIO pin23
	GPIO_pin24 = 0x1000000, //!< GPIO pin24
	GPIO_pin25 = 0x2000000, //!< GPIO pin25
	GPIO_pin26 = 0x4000000, //!< GPIO pin26
	GPIO_pin27 = 0x8000000, //!< GPIO pin27
	GPIO_pin28 = 0x10000000,//!< GPIO pin28
	GPIO_pin29 = 0x20000000,//!< GPIO pin29
	GPIO_pin30 = 0x40000000,//!< GPIO pin30
	GPIO_pin31 = 0x80000000,//!< GPIO pin31	maschera di selezione del pin 31 di un device GPIO
	GPIO_byte0 = 0x000000ff,//!< GPIO byte0 maschera di selezione deI pin 0-7 di un device GPIO
	GPIO_byte1 = 0x0000ff00,//!< GPIO byte1 maschera di selezione deI pin 8-15 di un device GPIO
	GPIO_byte2 = 0x00ff0000,//!< GPIO byte2 maschera di selezione deI pin 16-23 di un device GPIO
	GPIO_byte3 = 0xff000000 //!< GPIO byte3 maschera di selezione deI pin 24-31 di un device GPIO
} GPIO_mask;

/**
 * @brief Metodo alternativo per la specifica di uno dei pin di un device GPIO
 * @param[in] i indice del bit da selezionare, da 0 (bit meno significativo) a 31 (bit piu' significativo)
 * @return maschera di selezione del pin i-esimo
 */
#define GPIO_pin(i) ((uint32_t)(1<<i))

/**
 * @brief GPIO_mode, modalita' di funzionamento (lettura/scrittura) di un device GPIO
 */
typedef enum {
	GPIO_read, //!< GPIO_read  modalita' lettura
	GPIO_write //!< GPIO_write modalita' scrittura
} GPIO_mode;

/**
 * @brief GPIO_value, valore di un GPIO
 */
typedef enum {
	GPIO_reset,//!< GPIO_reset, corrisponde al valore logico '0'
	GPIO_set   //!< GPIO_set, corrisponde al valore logico '1'
} GPIO_value;

/**
 * @brief Inizializza un device GPIO.
 *
 * Inizializza una struttura di tipo GPIO_t, che astrae u device GPIO, controllando che l'inizializzazione vada a buon fine,
 * effettuando diversi test sui parametri di inizializzazione e restituendo un codice di errore.
 *
 * @param[inout]	gpio			puntatore a GPIO_t, che astrae un device GPIO;
 * @param[in]		base_address	indirizzo di memoria a cui e' mappato il device GPIO;
 * @param[in]		width			numero di pin di ingresso/uscita del device GPIO; Deve essere un numero compreso tra 1 e 32;
 * @param[in] 		enable_offset	offset in byte del registro "enable" del device GPIO;
 * 									il registro permette di impostare la modalita' di funzionamento del device GPIO;
 * @param[in] 		write_offset	offset in byte del registro "write" del device GPIO;
 * 									il registro permette di scrivere sui pin del device GPIO;
 * @param[in] 		read_offset		offset in byte del registro "read" del device GPIO;
 * 									il registro permette di leggere dai pin del device GPIO;
 *
 * @code
 * GPIO_t gpio;
 * GPIO_init(&gpio, BASE_ADDRESS, WIDTH, EN_OFFSET; WR_OFFSET, RD_OFFSET);
 * @endcode
 *
 * @warning Usa la macro assert per verificare che gpio non sia un puntatore nullo e che gli offset siano tutti differenti
 */
void GPIO_init(	GPIO_t* 	gpio,
				uint32_t	*base_address,
				uint8_t		width,
				uint8_t		enable_offset,
				uint8_t		write_offset,
				uint8_t		read_offset);

/**
 * @brief Permette di settare la modalita' lettura/scrittura dei pin di un device GPIO;
 *
 * @code
 * // setta i pin 0 ed 1 di un device GPIO come pin di uscita, gli altri restano invariati
 * GPIO_setMode(gpio, GPIO_pin0 | GPIO_pin1, GPIO_write);
 *
 * // setta i pin 19, 20 e 21 di un device GPIO come pin di ingresso, gli altri restano invariati
 * GPIO_setMode(gpio, GPIO_pin19 | GPIO_pin20 | GPIO_pin21, GPIO_read);
 * @endcode
 *
 * @param[in]	gpio	puntatore a GPIO_t, che astrae un device GPIO;
 * @param[in]	mask	maschera dei pin su cui agire;
 * @param[in]	mode	modalita' di funzionamento dei pin;
 *
 * @warning Usa la macro assert per verificare che gpio non sia un puntatore nullo
 */
void GPIO_setMode(GPIO_t* gpio, GPIO_mask mask, GPIO_mode mode);

/**
 * @brief Permette di settare il valore dei pin di un device GPIO, se configurati come output
 *
 * @code
 * // setta i pin 0 ed 1 di un device GPIO a livello logico '1', gli altri restano invariati
 * GPIO_setValue(gpio, GPIO_pin0 | GPIO_pin1, GPIO_set);
 *
 * // setta i pin 19, 20 e 21 di un device GPIO a livello logico '0', gli altri restano invariati
 * GPIO_setValue(gpio, GPIO_pin19 | GPIO_pin20 | GPIO_pin21, GPIO_reset);
 * @endcode
 *
 * @param[in]	gpio	puntatore a GPIO_t, che astrae un device GPIO;
 * @param[in]	mask	maschera dei pin su cui agire;
 * @param[in]	value	valore dei pin
 *
 * @warning Usa la macro assert per verificare che gpio non sia un puntatore nullo
 */
void GPIO_setValue(GPIO_t* gpio, GPIO_mask mask, GPIO_value value);

/**
 * @brief Permette di invertire il valore dei pin di un device GPIO, se configurati come output
 *
 * @code
 * // inverte i pin 0 ed 1 di un device GPIO a livello logico '1', gli altri restano invariati
 * GPIO_toggle(gpio, GPIO_pin0 | GPIO_pin1);
 * @endcode
 *
 * @param[in]	gpio	puntatore a GPIO_t, che astrae un device GPIO;
 * @param[in]	mask	maschera dei pin su cui agire;
 *
 * @warning Usa la macro assert per verificare che gpio non sia un puntatore nullo
 */
void GPIO_toggle(GPIO_t* gpio, GPIO_mask mask);

/**
 * @brief Permette di leggere il valore dei pin di un device GPIO;
 *
 * @code
 * // legge il valore del pin 0 di un device GPIO.
 * GPIO_value value = gpio_getValue(gpio, GPIO_pin0);
 *
 * // legge il valore del pin 0, 3 e 5 di un device GPIO.
 * // Verra' restituita la OR tra i valori dei pin
 * GPIO_value value = gpio_getValue(gpio, GPIO_pin0 | GPIO_pin3 | GPIO_pin5);
 * @endcode
 *
 * @param[in] gpio	puntatore a GPIO_t, che astrae un device GPIO;
 * @param[in] mask	maschera dei pin su cui agire;
 *
 * @return	restituisce la OR dei pin letti
 * @retval GPIO_set se uno dei pin letti e' GPIO_set,
 * @retval GPIO_reset se TUTTI i pin sono GPIO_reset
 *
 * @warning Usa la macro assert per verificare che gpio non sia un puntatore nullo
 */
GPIO_value GPIO_getValue(GPIO_t* gpio, GPIO_mask mask);

/**
 * @}
 */

#endif
