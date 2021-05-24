/**
 * @file myGPIO.c
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
#include "myGPIO.h"
#include <stdlib.h>
#include <assert.h>

#define  MODE_REG   0   /**< indice del registro "mode" */
#define  WRITE_REG  1   /**< indice del registro "write" */
#define  READ_REG   2   /**< indice del registro "read" */
#define  GIES_REG   3   /**< indice del registro "gies" */
#define  PIE_REG    4   /**< indice del registro "pie" */
#define  IRQ_REG    5   /**< indice del registro "irq" */
#define  IACK_REG   6   /**< indice del registro "iack" */

/**
 * @brief Inizializza un device myGPIO.
 *
 * @param[inout]  gpio      istanza myGPIO, che astrae un device myGPIO;
 * @param[in]    base_address  indirizzo di memoria a cui è mappato il device myGPIO;
 */
void myGPIO_Init(myGPIO_t *gpio, uint32_t base_address) {
	assert(base_address != 0);
	*gpio = (volatile uint32_t*)base_address;
}

/**
 * @brief Permette di settare la modalità lettura/scrittura dei pin di un device myGPIO;
 *
 * @param[in]  gpio  istanza myGPIO, che astrae un device myGPIO;
 * @param[in]  mask  maschera dei pin su cui agire;
 * @param[in]  mode  modalità di funzionamento dei pin;
 */
void myGPIO_SetMode(myGPIO_t gpio, uint32_t mask, uint32_t mode) {
	assert(gpio != NULL);
	uint32_t value = gpio[MODE_REG];
	gpio[MODE_REG] = (MYGPIO_MODE_WRITE == mode ?  value | mask : value &(~mask));
}

/**
 * @brief Permette di settare il valore dei pin di un device myGPIO, se configurati come output
 *
 * @param[in]  gpio   istanza myGPIO, che astrae un device myGPIO;
 * @param[in]  mask   maschera dei pin su cui agire;
 * @param[in]  value  valore dei pin
 */
void myGPIO_SetValue(myGPIO_t gpio, uint32_t mask, uint32_t value) {
	assert(gpio != NULL);
	uint32_t actual_value = gpio[WRITE_REG];
	gpio[WRITE_REG] = (MYGPIO_PIN_SET == value ?  actual_value|mask : actual_value&(~mask));
}

/**
 * @brief Permette di invertire il valore dei pin di un device myGPIO, se configurati come output
 *
 * @param[in]  gpio  istanza myGPIO, che astrae un device myGPIO;
 * @param[in]  mask  maschera dei pin su cui agire;
 *
 * @warning Usa la macro assert per verificare che gpio non sia un puntatore nullo
 */
void myGPIO_Toggle(myGPIO_t gpio, uint32_t mask) {
	assert(gpio != NULL);
	uint32_t actual_value = gpio[WRITE_REG];
	gpio[WRITE_REG] = actual_value ^ mask;
}

/**
 * @brief Permette di leggere il valore dei pin di un device myGPIO;
 *
 * @param[in] gpio  istanza myGPIO, che astrae un device myGPIO;
 * @param[in] mask  maschera dei pin su cui agire;
 *
 * @return  restituisce la OR dei pin letti
 * @retval MYGPIO_PIN_SET se uno dei pin letti è MYGPIO_PIN_SET,
 * @retval MYGPIO_PIN_RESET se TUTTI i pin sono MYGPIO_PIN_RESET
 *
 * @warning Usa la macro assert per verificare che gpio non sia un puntatore nullo
 */
uint32_t myGPIO_GetValue(myGPIO_t gpio, uint32_t mask) {
	assert(gpio != NULL);
	return ((gpio[READ_REG] & mask) == 0 ? MYGPIO_PIN_RESET : MYGPIO_PIN_SET);
}

/**
 * @brief Restituisce la maschera dei pin settati di un device myGPIO
 *
 * @param[in] gpio istanza myGPIO, che astrae un device myGPIO;
 *
 * @return maschera dei pin settati di un device myGPIO
 */
uint32_t myGPIO_GetRead(myGPIO_t gpio) {
	assert(gpio != NULL);
	return gpio[READ_REG];
}

/**
 * @brief Abilita gli interrupt globali;
 * 
 * @param [in] gpio istanza myGPIO, che astrae un device myGPIO;
 */
void myGPIO_GlobalInterruptEnable(myGPIO_t gpio) {
	assert(gpio != NULL);
	gpio[GIES_REG] = 1;
}

/**
 * @brief Disabilita gli interrupt globali;
 *
 * @param [in] gpio istanza myGPIO, che astrae un device myGPIO;
 */
void myGPIO_GlobalInterruptDisable(myGPIO_t gpio) {
	assert(gpio != NULL);
	gpio[GIES_REG] = 0;
}

/**
 * @brief Consente di verificare se gli interrupt globali siano abilitati.
 *
 * @param [in] gpio istanza myGPIO, che astrae un device myGPIO;

 * @retval MYGPIO_PIN_SET se il bit 0 del registro GIES è settato, ad indicare che gli interrupt sono abilitati
 * @retval MYGPIO_PIN_RESET se il bit 0 del registro GIES è resettato, ad indicare che gli interrupt non sono abilitati
 */
uint32_t myGPIO_IsGlobalInterruptEnabled(myGPIO_t gpio) {
	assert(gpio != NULL);
	return ((gpio[GIES_REG] & 1) == 0 ? MYGPIO_PIN_RESET : MYGPIO_PIN_SET);
}

/**
 * @brief Consente di verificare se esistano interrupt non ancora serviti.
 *
 * @param [in] gpio istanza myGPIO, che astrae un device myGPIO;
 * 
 * @retval MYGPIO_PIN_SET se il bit 1 del registro GIES è settato, ad indicare che esistono interrupt pending
 * @retval MYGPIO_PIN_RESET se il bit 1 del registro GIES è resettato, ad indicare che non esistono interrupt pending
 */
uint32_t myGPIO_PendingInterrupt(myGPIO_t gpio) {
	assert(gpio != NULL);
	return ((gpio[GIES_REG] & 2) == 0 ? MYGPIO_PIN_RESET : MYGPIO_PIN_SET);
}

/**
 * @brief Abilita gli interrupt per i singoli pin del device.
 * 
 * @param [in] gpio istanza myGPIO, che astrae un device myGPIO;
 * @param [in] mask maschera di selezione degli interrupt da abilitare;quelli non selezionati non
 * vengono abilitati;
 */
void myGPIO_PinInterruptEnable(myGPIO_t gpio, uint32_t mask) {
	assert(gpio != NULL);
	gpio[PIE_REG] |= mask;
}

/**
 * @brief Disabilita gli interrupt per i singoli pin del device.
 * 
 * @param [in] gpio istanza myGPIO, che astrae un device myGPIO;
 * @param [in] mask maschera di selezione degli interrupt da disabilitare, quelli non selezionati non
 * vengono disabilitati;
 */
void myGPIO_PinInterruptDisable(myGPIO_t gpio, uint32_t mask) {
	assert(gpio != NULL);
	gpio[PIE_REG] &= ~mask;
}

/**
 * @brief Consente di ottenere una maschera che indichi quali pin abbiano interrupt abilitati
 * 
 * @param [in] gpio istanza myGPIO, che astrae un device myGPIO;
 * 
 * @return maschera che riporta i pin per i quali gli interrupt sono stati abilitati;
 */
uint32_t myGPIO_EnabledPinInterrupt(myGPIO_t gpio) {
	assert(gpio != NULL);
	return gpio[PIE_REG];
}

/**
 * @brief Consente di ottenere una maschera che indichi quali interrupt non siano stati ancora serviti;
 * 
 * @param [in] gpio istanza myGPIO, che astrae un device myGPIO;
 * 
 * @return maschera che riporta i pin per i quali gli interrupt non sono stati ancora serviti;
 */
uint32_t myGPIO_PendingPinInterrupt(myGPIO_t gpio) {
	assert(gpio != NULL);
	return gpio[IRQ_REG];
}

/**
 * @brief Invia al device notifica di servizio di un interrupt;
 * 
 * @param [in] gpio istanza myGPIO, che astrae un device myGPIO;
 * @param [in] mask maschera di selezione dei bit;
 */
void myGPIO_PinInterruptAck(myGPIO_t gpio, uint32_t mask) {
	assert(gpio != NULL);
	gpio[IACK_REG] = mask;
}
