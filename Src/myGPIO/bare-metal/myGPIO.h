/**
 * @file myGPIO.h
 * @author: Salvatore Barone <salvator.barone@gmail.com>
 * @date: 12 05 2017
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
 */

#ifndef __myGPIO_DRIVER_HEADER_H__
#define __myGPIO_DRIVER_HEADER_H__

#include <inttypes.h>

/**
 * @addtogroup myGPIO
 * @{
 * @addtogroup bare-metal
 * @{
 *
 * @brief device-driver OO-like bare-metal per device myGPIO
 */

/**
 * @brief Struttura che astrae un device myGPIO.
 * 
 * Comprende l'indirizzo di memoria a cui il device e' mappato e gli offset dei registri attraverso
 * i quali e' possibile interagire con il device stesso. La struttura e' pensata, com'e' ovvio, per consentire
 * l'uso di più device GPIO nello stesso programma, identificando ciascuno di essi attraverso l'indirizzo
 * di memoria al quale sono mappati. La struttura di cui sopra, nel resto dell'implementazione del driver,
 * e' totalmente trasparente a chi la utilizza, nel senso che non e' strettamente necessario conoscerne i
 * dettagli per poter utilizzare il driver. 
 */
typedef struct {
	uint32_t*	base_address;	/**< indirizzo base */
	uint8_t		mode_offset;	/**< offset, rispetto all'indirizzo base, del registro "mode" */
	uint8_t		write_offset;	/**< offset, rispetto all'indirizzo base, del registro "write" */
	uint8_t		read_offset;	/**< offset, rispetto all'indirizzo base, del registro "read" */
	uint8_t		gies_offset;	/**< offset, rispetto all'indirizzo base, del registro "gies" */
	uint8_t		pie_offset;		/**< offset, rispetto all'indirizzo base, del registro "pie" */
	uint8_t		irq_offset;		/**< offset, rispetto all'indirizzo base, del registro "irq" */
	uint8_t		iack_offset;	/**< offset, rispetto all'indirizzo base, del registro "iack" */
} myGPIO_t;

#define myGPIO_MODE_OFFSET		0x00U	//!< @brief Offset, rispetto all'indirizzo base, del registro "mode" per il device myGPIO
#define myGPIO_WRITE_OFFSET		0x04U	//!< @brief Offset, rispetto all'indirizzo base, del registro "write" per il device myGPIO
#define myGPIO_READ_OFFSET		0x08U	//!< @brief Offset, rispetto all'indirizzo base, del registro "read" per il device myGPIO
#define myGPIO_GIES_OFFSET		0x0CU	//!< @brief Offset, rispetto all'indirizzo base, del registro "gies" per il device myGPIO
#define myGPIO_PIE_OFFSET		0x10U	//!< @brief Offset, rispetto all'indirizzo base, del registro "pie" per il device myGPIO
#define myGPIO_IRQ_OFFSET		0x14U	//!< @brief Offset, rispetto all'indirizzo base, del registro "irq" per il device myGPIO
#define myGPIO_IACK_OFFSET		0x18U	//!< @brief Offset, rispetto all'indirizzo base, del registro "iack" per il device myGPIO

//! @brief Maschere di selezione dei pin di un device myGPIO
typedef enum {
	myGPIO_pin0 = 0x1U,        //!< myGPIO pin0 maschera di selezione del pin 0 di un device myGPIO
	myGPIO_pin1 = 0x2U,        //!< myGPIO pin1 maschera di selezione del pin 1 di un device myGPIO
	myGPIO_pin2 = 0x4U,        //!< myGPIO pin2 maschera di selezione del pin 2 di un device myGPIO
	myGPIO_pin3 = 0x8U,        //!< myGPIO pin3 maschera di selezione del pin 3 di un device myGPIO
	myGPIO_pin4 = 0x10U,       //!< myGPIO pin4 maschera di selezione del pin 4 di un device myGPIO
	myGPIO_pin5 = 0x20U,       //!< myGPIO pin5 maschera di selezione del pin 5 di un device myGPIO
	myGPIO_pin6 = 0x40U,       //!< myGPIO pin6 maschera di selezione del pin 6 di un device myGPIO
	myGPIO_pin7 = 0x80U,       //!< myGPIO pin7 maschera di selezione del pin 7 di un device myGPIO
	myGPIO_pin8 = 0x100U,      //!< myGPIO pin8 maschera di selezione del pin 8 di un device myGPIO
	myGPIO_pin9 = 0x200U,      //!< myGPIO pin9 maschera di selezione del pin 9 di un device myGPIO
	myGPIO_pin10 = 0x400U,     //!< myGPIO pin10 maschera di selezione del pin 10 di un device myGPIO
	myGPIO_pin11 = 0x800U,     //!< myGPIO pin11 maschera di selezione del pin 11 di un device myGPIO
	myGPIO_pin12 = 0x1000U,    //!< myGPIO pin12 maschera di selezione del pin 12 di un device myGPIO
	myGPIO_pin13 = 0x2000U,    //!< myGPIO pin13 maschera di selezione del pin 13 di un device myGPIO
	myGPIO_pin14 = 0x4000U,    //!< myGPIO pin14 maschera di selezione del pin 14 di un device myGPIO
	myGPIO_pin15 = 0x8000U,    //!< myGPIO pin15 maschera di selezione del pin 15 di un device myGPIO
	myGPIO_pin16 = 0x10000U,   //!< myGPIO pin16 maschera di selezione del pin 16 di un device myGPIO
	myGPIO_pin17 = 0x20000U,   //!< myGPIO pin17 maschera di selezione del pin 17 di un device myGPIO
	myGPIO_pin18 = 0x40000U,   //!< myGPIO pin18 maschera di selezione del pin 18 di un device myGPIO
	myGPIO_pin19 = 0x80000U,   //!< myGPIO pin19 maschera di selezione del pin 19 di un device myGPIO
	myGPIO_pin20 = 0x100000U,  //!< myGPIO pin20 maschera di selezione del pin 20 di un device myGPIO
	myGPIO_pin21 = 0x200000U,  //!< myGPIO pin21 maschera di selezione del pin 21 di un device myGPIO
	myGPIO_pin22 = 0x400000U,  //!< myGPIO pin22 maschera di selezione del pin 22 di un device myGPIO
	myGPIO_pin23 = 0x800000U,  //!< myGPIO pin23 maschera di selezione del pin 23 di un device myGPIO
	myGPIO_pin24 = 0x1000000U, //!< myGPIO pin24 maschera di selezione del pin 24 di un device myGPIO
	myGPIO_pin25 = 0x2000000U, //!< myGPIO pin25 maschera di selezione del pin 25 di un device myGPIO
	myGPIO_pin26 = 0x4000000U, //!< myGPIO pin26 maschera di selezione del pin 26 di un device myGPIO
	myGPIO_pin27 = 0x8000000U, //!< myGPIO pin27 maschera di selezione del pin 27 di un device myGPIO
	myGPIO_pin28 = 0x10000000U,//!< myGPIO pin28 maschera di selezione del pin 28 di un device myGPIO
	myGPIO_pin29 = 0x20000000U,//!< myGPIO pin29 maschera di selezione del pin 29 di un device myGPIO
	myGPIO_pin30 = 0x40000000U,//!< myGPIO pin30 maschera di selezione del pin 30 di un device myGPIO
	myGPIO_pin31 = 0x80000000U,//!< myGPIO pin31 maschera di selezione del pin 31 di un device myGPIO
	myGPIO_byte0 = 0x000000ffU,//!< myGPIO byte0 maschera di selezione deI pin 0-7 di un device myGPIO
	myGPIO_byte1 = 0x0000ff00U,//!< myGPIO byte1 maschera di selezione deI pin 8-15 di un device myGPIO
	myGPIO_byte2 = 0x00ff0000U,//!< myGPIO byte2 maschera di selezione deI pin 16-23 di un device myGPIO
	myGPIO_byte3 = 0xff000000U //!< myGPIO byte3 maschera di selezione deI pin 24-31 di un device myGPIO
} myGPIO_mask;

/**
 * @brief Metodo alternativo per la specifica di uno dei pin di un device myGPIO
 * @param[in] i indice del bit da selezionare, da 0 (bit meno significativo) a 31 (bit piu' significativo)
 * @return maschera di selezione del pin i-esimo
 */
#define myGPIO_pin(i) ((uint32_t)(1<<(i)))

//! @brief myGPIO_mode, modalita' di funzionamento (lettura/scrittura) di un device myGPIO
typedef enum {
	myGPIO_read, //!< myGPIO_read  modalita' lettura
	myGPIO_write //!< myGPIO_write modalita' scrittura
} myGPIO_mode;

//! @brief myGPIO_value, valore di un myGPIO
typedef enum {
	myGPIO_reset,//!< myGPIO_reset, corrisponde al valore logico '0'
	myGPIO_set   //!< myGPIO_set, corrisponde al valore logico '1'
} myGPIO_value;

/**
 * @brief Inizializza un device myGPIO.
 *
 * Inizializza una struttura di tipo myGPIO_t, che astrae u device myGPIO, controllando che l'inizializzazione vada a buon fine,
 * effettuando diversi test sui parametri di inizializzazione e restituendo un codice di errore.
 *
 * @param[inout]	gpio			puntatore a myGPIO_t, che astrae un device myGPIO;
 * @param[in]		base_address	indirizzo di memoria a cui e' mappato il device myGPIO;
 * @code
 * myGPIO_t gpio;
 * myGPIO_init(&gpio, BASE_ADDRESS);
 * @endcode
 *
 * @warning Usa la macro assert per verificare che gpio e base_address non siano nulli.
 */
void myGPIO_Init(myGPIO_t* gpio, uint32_t base_address);
				
/**
 * @brief Permette di settare la modalita' lettura/scrittura dei pin di un device myGPIO;
 *
 * @code
 * // setta i pin 0 ed 1 di un device myGPIO come pin di uscita, gli altri restano invariati
 * myGPIO_setMode(gpio, myGPIO_pin0 | myGPIO_pin1, myGPIO_write);
 *
 * // setta i pin 19, 20 e 21 di un device myGPIO come pin di ingresso, gli altri restano invariati
 * myGPIO_setMode(gpio, myGPIO_pin19 | myGPIO_pin20 | myGPIO_pin21, myGPIO_read);
 * @endcode
 *
 * @param[in]	gpio	puntatore a myGPIO_t, che astrae un device myGPIO;
 * @param[in]	mask	maschera dei pin su cui agire;
 * @param[in]	mode	modalita' di funzionamento dei pin;
 *
 * @warning Usa la macro assert per verificare che gpio non sia un puntatore nullo
 */
void myGPIO_SetMode(myGPIO_t* gpio, myGPIO_mask mask, myGPIO_mode mode);

/**
 * @brief Permette di settare il valore dei pin di un device myGPIO, se configurati come output
 *
 * @code
 * // setta i pin 0 ed 1 di un device myGPIO a livello logico '1', gli altri restano invariati
 * myGPIO_setValue(gpio, myGPIO_pin0 | myGPIO_pin1, myGPIO_set);
 *
 * // setta i pin 19, 20 e 21 di un device myGPIO a livello logico '0', gli altri restano invariati
 * myGPIO_setValue(gpio, myGPIO_pin19 | myGPIO_pin20 | myGPIO_pin21, myGPIO_reset);
 * @endcode
 *
 * @param[in]	gpio	puntatore a myGPIO_t, che astrae un device myGPIO;
 * @param[in]	mask	maschera dei pin su cui agire;
 * @param[in]	value	valore dei pin
 *
 * @warning Usa la macro assert per verificare che gpio non sia un puntatore nullo
 */
void myGPIO_SetValue(myGPIO_t* gpio, myGPIO_mask mask, myGPIO_value value);

/**
 * @brief Permette di invertire il valore dei pin di un device myGPIO, se configurati come output
 *
 * @code
 * // inverte i pin 0 ed 1 di un device myGPIO a livello logico '1', gli altri restano invariati
 * myGPIO_toggle(gpio, myGPIO_pin0 | myGPIO_pin1);
 * @endcode
 *
 * @param[in]	gpio	puntatore a myGPIO_t, che astrae un device myGPIO;
 * @param[in]	mask	maschera dei pin su cui agire;
 *
 * @warning Usa la macro assert per verificare che gpio non sia un puntatore nullo
 */
void myGPIO_Toggle(myGPIO_t* gpio, myGPIO_mask mask);

/**
 * @brief Permette di leggere il valore dei pin di un device myGPIO;
 *
 * @code
 * // legge il valore del pin 0 di un device myGPIO.
 * myGPIO_value value = gpio_getValue(gpio, myGPIO_pin0);
 *
 * // legge il valore del pin 0, 3 e 5 di un device myGPIO.
 * // Verra' restituita la OR tra i valori dei pin
 * myGPIO_value value = gpio_getValue(gpio, myGPIO_pin0 | myGPIO_pin3 | myGPIO_pin5);
 * @endcode
 *
 * @param[in] gpio	puntatore a myGPIO_t, che astrae un device myGPIO;
 * @param[in] mask	maschera dei pin su cui agire;
 *
 * @return	restituisce la OR dei pin letti
 * @retval myGPIO_set se uno dei pin letti e' myGPIO_set,
 * @retval myGPIO_reset se TUTTI i pin sono myGPIO_reset
 *
 * @warning Usa la macro assert per verificare che gpio non sia un puntatore nullo
 */
myGPIO_value myGPIO_GetValue(myGPIO_t* gpio, myGPIO_mask mask);

/**
 * @brief Restituisce la maschera dei pin settati di un device myGPIO
 *
 * @param[in] gpio puntatore a myGPIO_t, che astrae un device myGPIO;
 *
 * @return maschera dei pin settati di un device myGPIO
 *
 * @code
 * myGPIO_mask mask = myGPIO_read(&gpio);
 * if (mask & myGPIO_pin3) {
 * 		...
 * }
 * else {
 * 		...
 * }
 * @endcode
 */
myGPIO_mask myGPIO_GetRead(myGPIO_t *gpio);

/**
 * @brief Abilita gli interrupt globali;
 * 
 * @param [in] gpio puntatore a myGPIO_t, che astrae un device myGPIO;
 * 
 */
void myGPIO_GlobalInterruptEnable(myGPIO_t *gpio);

/**
 * @brief Disabilita gli interrupt globali;
 * @param [in] gpio puntatore a myGPIO_t, che astrae un device myGPIO;
 */
void myGPIO_GlobalInterruptDisable(myGPIO_t *gpio);

/**
 * @brief Consente di verificare se gli interrupt globali siano abilitati.
 * @param [in] gpio puntatore a myGPIO_t, che astrae un device myGPIO;
 * @retval myGPIO_set se il bit 0 del registro GIES e' settato, ad indicare che gli interrupt sono abilitati
 * @retval myGPIO_reset se il bit 0 del registro GIES e' resettato, ad indicare che gli interrupt non sono abilitati
 */
myGPIO_value myGPIO_IsGlobalInterruptEnabled(myGPIO_t *gpio);

/**
 * @brief Consente di verificare se esistano interrupt non ancora serviti.
 * @param [in] gpio puntatore a myGPIO_t, che astrae un device myGPIO;
 * @retval myGPIO_set se il bit 1 del registro GIES e' settato, ad indicare che esistono interrupt pending
 * @retval myGPIO_reset se il bit 1 del registro GIES e' resettato, ad indicare che non esistono interrupt pending
 */
myGPIO_value myGPIO_PendingInterrupt(myGPIO_t *gpio);

/**
 * @brief Abilita gli interrupt per i singoli pin del device.
 * @param [in] gpio puntatore a myGPIO_t, che astrae un device myGPIO;
 * @param [in] mask maschera di selezione degli interrupt da abilitare;quelli non selezionati non
 * vengono abilitati;
 */
void myGPIO_PinInterruptEnable(myGPIO_t *gpio, myGPIO_mask mask);

/**
 * @brief Disabilita gli interrupt per i singoli pin del device.
 * @param [in] gpio puntatore a myGPIO_t, che astrae un device myGPIO;
 * @param [in] mask maschera di selezione degli interrupt da disabilitare, quelli non selezionati non
 * vengono disabilitati;
 */
void myGPIO_PinInterruptDisable(myGPIO_t *gpio, myGPIO_mask mask);

/**
 * @brief Consente di ottenere una maschera che indichi quali pin abbiano interrupt abilitati
 * @param [in] gpio puntatore a myGPIO_t, che astrae un device myGPIO;
 * @return maschera che riporta i pin per i quali gli interrupt sono stati abilitati;
 */
myGPIO_mask myGPIO_EnabledPinInterrupt(myGPIO_t *gpio);

/**
 * @brief Consente di ottenere una maschera che indichi quali interrupt non siano stati ancora serviti;
 * @param [in] gpio puntatore a myGPIO_t, che astrae un device myGPIO;
 * @return maschera che riporta i pin per i quali gli interrupt non sono stati ancora serviti;
 */
myGPIO_mask myGPIO_PendingPinInterrupt(myGPIO_t *gpio);

/**
 * @brief Invia al device notifica di servizio di un interrupt;
 * @param [in] gpio puntatore a myGPIO_t, che astrae un device myGPIO;
 * @param [in] mask maschera di selezione dei bit;
 */
void myGPIO_PinInterruptAck(myGPIO_t *gpio, myGPIO_mask mask);

/**
 * @}
 * @}
 */

#endif
