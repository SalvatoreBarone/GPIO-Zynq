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
 *
 */
typedef struct {
	uint32_t*	base_address;
	uint8_t		width;
	uint8_t		enable_offset;
	uint8_t		write_offset;
	uint8_t		read_offset;
} GPIO_t;

/**
 *
 */
typedef enum {
	GPIO_pin0 = 0x1,        //!< GPIO_pin0
	GPIO_pin1 = 0x2,        //!< GPIO_pin1
	GPIO_pin2 = 0x4,        //!< GPIO_pin2
	GPIO_pin3 = 0x8,        //!< GPIO_pin3
	GPIO_pin4 = 0x10,       //!< GPIO_pin4
	GPIO_pin5 = 0x20,       //!< GPIO_pin5
	GPIO_pin6 = 0x40,       //!< GPIO_pin6
	GPIO_pin7 = 0x80,       //!< GPIO_pin7
	GPIO_pin8 = 0x100,      //!< GPIO_pin8
	GPIO_pin9 = 0x200,      //!< GPIO_pin9
	GPIO_pin10 = 0x400,     //!< GPIO_pin10
	GPIO_pin11 = 0x800,     //!< GPIO_pin11
	GPIO_pin12 = 0x1000,    //!< GPIO_pin12
	GPIO_pin13 = 0x2000,    //!< GPIO_pin13
	GPIO_pin14 = 0x4000,    //!< GPIO_pin14
	GPIO_pin15 = 0x8000,    //!< GPIO_pin15
	GPIO_pin16 = 0x10000,   //!< GPIO_pin16
	GPIO_pin17 = 0x20000,   //!< GPIO_pin17
	GPIO_pin18 = 0x40000,   //!< GPIO_pin18
	GPIO_pin19 = 0x80000,   //!< GPIO_pin19
	GPIO_pin20 = 0x100000,  //!< GPIO_pin20
	GPIO_pin21 = 0x200000,  //!< GPIO_pin21
	GPIO_pin22 = 0x400000,  //!< GPIO_pin22
	GPIO_pin23 = 0x800000,  //!< GPIO_pin23
	GPIO_pin24 = 0x1000000, //!< GPIO_pin24
	GPIO_pin25 = 0x2000000, //!< GPIO_pin25
	GPIO_pin26 = 0x4000000, //!< GPIO_pin26
	GPIO_pin27 = 0x8000000, //!< GPIO_pin27
	GPIO_pin28 = 0x10000000,//!< GPIO_pin28
	GPIO_pin29 = 0x20000000,//!< GPIO_pin29
	GPIO_pin30 = 0x40000000,//!< GPIO_pin30
	GPIO_pin31 = 0x80000000,//!< GPIO_pin31
	GPIO_byte0 = 0x000000ff,//!< GPIO_byte0
	GPIO_byte1 = 0x0000ff00,//!< GPIO_byte1
	GPIO_byte2 = 0x00ff0000,//!< GPIO_byte2
	GPIO_byte3 = 0xff000000 //!< GPIO_byte3
} GPIO_mask;

/**
 * @brief GPIO_mode
 */
typedef enum {
	GPIO_read,//!< GPIO_read
	GPIO_write//!< GPIO_write
} GPIO_mode;

/**
 * @brief GPIO_value
 */
typedef enum {
	GPIO_reset,//!< GPIO_reset
	GPIO_set   //!< GPIO_set
} GPIO_value;

/**
 * @brief Inizializza un device GPIO.
 *
 * Inizializza una struttura di tipo GPIO_t, che astrae u device GPIO, controllando che l'inizializzazione vada a buon fine,
 * effettuando diversi test sui parametri di inizializzazione e restituendo un codice di errore.
 *
 * @param gpio			puntatore a GPIO_t, che astrae un device GPIO;
 * @param base_address	indirizzo di memoria a cui e' mappato il device GPIO;
 * @param width			numero di pin di ingresso/uscita del device GPIO; Deve essere un numero compreso tra 1 e 32;
 * @param enable_offset offset in byte del registro "enable" del device GPIO;
 * 						il registro permette di impostare la modalita' di funzionamento del device GPIO;
 * @param write_offset	offset in byte del registro "write" del device GPIO;
 * 						il registro permette di scrivere sui pin del device GPIO;
 * @param read_offset	offset in byte del registro "read" del device GPIO;
 * 						il registro permette di leggere dai pin del device GPIO;
 */

void gpio_init(	GPIO_t* 	gpio,
				uint32_t	*base_address,
				uint8_t		width,
				uint8_t		enable_offset,
				uint8_t		write_offset,
				uint8_t		read_offset);

/**
 * @brief Permette di settare la modalita' lettura/scrittura dei pin di un device GPIO;
 *
 * // setta i pin 0 ed 1 di un device GPIO come pin di uscita, gli altri restano invariati
 * gpio_setMode(gpio, GPIO_pin0 | GPIO_pin1, GPIO_write);
 *
 * // setta i pin 19, 20 e 21 di un device GPIO come pin di ingresso, gli altri restano invariati
 * gpio_setMode(gpio, GPIO_pin19 | GPIO_pin20 | GPIO_pin21, GPIO_read);
 *
 *
 * @param gpio	puntatore a GPIO_t, che astrae un device GPIO;
 * @param mask	maschera dei pin su cui agire;
 * @param mode	modalita' di funzionamento dei pin;
 */
void gpio_setMode(GPIO_t* gpio, GPIO_mask mask, GPIO_mode mode);

/**
 * @brief Permette di settare il valore dei pin di un device GPIO;
 *
 * // setta i pin 0 ed 1 di un device GPIO a livello logico '1', gli altri restano invariati
 * gpio_setValue(gpio, GPIO_pin0 | GPIO_pin1, GPIO_set);
 *
 * // setta i pin 19, 20 e 21 di un device GPIO a livello logico '0', gli altri restano invariati
 * gpio_setValue(gpio, GPIO_pin19 | GPIO_pin20 | GPIO_pin21, GPIO_reset);
 *
 * @param gpio	puntatore a GPIO_t, che astrae un device GPIO;
 * @param mask	maschera dei pin su cui agire;
 * @param value valore dei pin
 */
void gpio_setValue(GPIO_t* gpio, GPIO_mask mask, GPIO_value value);

/**
 * @brief Permette di leggere il valore dei pin di un device GPIO;
 *
 * // legge il valore del pin 0 di un device GPIO.
 * GPIO_value value = gpio_getValue(gpio, GPIO_pin0);
 *
 * // legge il valore del pin 0, 3 e 5 di un device GPIO.
 * // Verra' restituita la OR tra i valori dei pin
 * GPIO_value value = gpio_getValue(gpio, GPIO_pin0 | GPIO_pin3 | GPIO_pin5);
 *
 * @param gpio	puntatore a GPIO_t, che astrae un device GPIO;
 * @param mask	maschera dei pin su cui agire;
 * @return
 */
GPIO_value gpio_getValue(GPIO_t* gpio, GPIO_mask mask);

#endif
