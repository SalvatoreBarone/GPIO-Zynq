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
 */

#ifndef __myGPIO_DRIVER_HEADER_H__
#define __myGPIO_DRIVER_HEADER_H__

#include <inttypes.h>

/**
 * @addtogroup myGPIO
 * @{
 *
 * Il modulo definisce un driver OO-like per l'utilizzo di una periferica myGPIO custom.
 */

/**
 * @brief Struttura che astrae un device myGPIO.
 * 
 * Essa comprende l'indirizzo di memoria a cui il device e' mappato e gli offset dei registri attraverso
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
	uint8_t		int_offset;		/**< offset, rispetto all'indirizzo base, del registro "interrupt" */
} myGPIO_t;

/**
 * @brief Offset, rispetto all'indirizzo base, del registro "mode" per il device myGPIO
 */
#define myGPIO_MODE_OFFSET		0U

/**
 * @brief Offset, rispetto all'indirizzo base, del registro "write" per il device myGPIO
 */
#define myGPIO_WRITE_OFFSET		4U

/**
 * @brief Offset, rispetto all'indirizzo base, del registro "read" per il device myGPIO
 */
#define myGPIO_READ_OFFSET		8U

/**
 * @brief Offset, rispetto all'indirizzo base, del registro "interrupt" per il device myGPIO
 */
#define myGPIO_INTR_OFFSET		12U

/**
 * @brief maschera del bit del registro "interrupt" che funge da interrupt-enable, per il device myGPIO
 */
#define myGPIO_INTR_IntEn_mask	0x1U

/**
 * @brief maschera del bit del registro "interrupt" che funge da interrupt-request, per il device myGPIO
 */
#define myGPIO_INTR_Irq_mask	0x2U

/**
 * @brief maschera del bit del registro "interrupt" che funge da interrupt-ack, per il device myGPIO
 */
#define myGPIO_INTR_IntAck_mask 0x4U

/**
 * @brief Maschere di selezione dei pin di un device myGPIO
 */
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

/**
 * @brief myGPIO_mode, modalita' di funzionamento (lettura/scrittura) di un device myGPIO
 */
typedef enum {
	myGPIO_read, //!< myGPIO_read  modalita' lettura
	myGPIO_write //!< myGPIO_write modalita' scrittura
} myGPIO_mode;

/**
 * @brief myGPIO_value, valore di un myGPIO
 */
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
void myGPIO_init(myGPIO_t* gpio, uint32_t base_address);
				
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
void myGPIO_setMode(myGPIO_t* gpio, myGPIO_mask mask, myGPIO_mode mode);

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
void myGPIO_setValue(myGPIO_t* gpio, myGPIO_mask mask, myGPIO_value value);

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
void myGPIO_toggle(myGPIO_t* gpio, myGPIO_mask mask);

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
myGPIO_value myGPIO_getValue(myGPIO_t* gpio, myGPIO_mask mask);

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
myGPIO_mask myGPIO_getRead(myGPIO_t *gpio);

/**
 * @brief Abilita gli interrupt per un device myGPIO
 * 
 * @param [in] gpio puntatore a myGPIO_t, che astrae un device myGPIO;
 * 
 * @code
//L'esempio seguente fa riferimento ad un sistema con un device myGPIO di 8 bit ai quali sono connessi
//led e button presenti sulla board Zybo.
//I led sono connessi ai bit 0-3 del device myGPIO, mentre i button sono connessi ai bit 4-7.
#include "xscugic.h"
#include "xparameters.h"
#include "myGPIO.h"

#define myGPIO_id 			XPAR_MYGPIO_0_DEVICE_ID			// ID del device MYGPIO0
#define myGPIO_addr			XPAR_MYGPIO_0_S00_AXI_BASEADDR	// Indirizzo base del device MYGPIO0
#define myGPIO_irq_line		XPAR_FABRIC_MYGPIO_0_IRQ_INTR	// linea interrupt a cui e' connesso il device MYGPIO0
#define GIC_ID				XPAR_SCUGIC_0_DEVICE_ID			// ID del device GIC
#define GIC_baddr			XPAR_SCUGIC_0_CPU_BASEADDR		// Indirizzo base del device GIC

// funzione di configurazione delle interrupt
int INT_config(XScuGic *GIC, myGPIO_t* gpio);

// prototipo della isr per la gestione della pressione di un button
// deve necessariamente essere una funzione che restituisce void e prende in ingresso un
// puntatore a void per i dati
void button_isr(void* data);

int main()
{
	XScuGic GIC;
	myGPIO_t gpio;

	myGPIO_init(&gpio, myGPIO_addr);
    myGPIO_setMode(&gpio, myGPIO_pin0 | myGPIO_pin1 | myGPIO_pin2 | myGPIO_pin3, myGPIO_write);
    myGPIO_setMode(&gpio, myGPIO_pin4 | myGPIO_pin5 | myGPIO_pin6 | myGPIO_pin7, myGPIO_read);
    myGPIO_setValue(&gpio, myGPIO_pin0 | myGPIO_pin1 | myGPIO_pin2 | myGPIO_pin3, myGPIO_reset);

    INT_config(&GIC, &gpio);

    for(;;);

    return 0;
}

int INT_config(XScuGic *GIC, myGPIO_t* gpio) {

	// inizializza il driver del GIC
	Xil_ExceptionInit();

	// ottiene i parametri di configurazione del GIC, lo configura ed inizializza
	// sintassi : XScuGic_LookupConfig(GIC_id)
	// sintassi : XScuGic_CfgInitialize(GIC, config, address)
	XScuGic_Config *IntcConfig = XScuGic_LookupConfig(GIC_ID);
	if (IntcConfig == NULL)
		return -1;
	if (XScuGic_CfgInitialize(GIC, IntcConfig, IntcConfig->CpuBaseAddress) != XST_SUCCESS)
		return -1;

	// registra l'interrupt handler del GIC alla logica di gestione del processing-system
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,(Xil_ExceptionHandler)XScuGic_InterruptHandler, GIC);

	// registrazione degli handler
	// le due righe seguenti stabiliscono quale sia l'handler da chiamare e quali dati bisogna passargli
	// qualora si manifesti una interruzione su una line di irq.
	// sintassi : XScuGic_Connect(GIC, irq_line, handler, data)
	if (XScuGic_Connect(GIC, myGPIO_irq_line, (Xil_InterruptHandler)button_isr, (void*)gpio) != XST_SUCCESS)
		return -1;

	// abilitazione degli interrupt sulle linee connesse alle periferiche
	// sintassi: XScuGic_Enable(GIC,irq_line);
	XScuGic_Enable(GIC, myGPIO_irq_line);
	// abilitazione degli interrupt delle periferiche
	myGPIO_interruptEnable(gpio);

	// abilitazione degli interrupt del processing-system
	Xil_ExceptionEnable();

	return 0;
}

void button_isr(void* data) {
	myGPIO_t* gpio = (myGPIO_t*)data;

	myGPIO_interruptDisable(gpio);	// disabilito gli interrupt
	myGPIO_interruptAck(gpio);		// clear del flag irq

	myGPIO_mask mask = myGPIO_getRead(gpio);
	myGPIO_setValue(gpio, myGPIO_pin0 | myGPIO_pin1 | myGPIO_pin2 | myGPIO_pin3, myGPIO_reset);
	myGPIO_setValue(gpio, mask>>4, myGPIO_set);

	myGPIO_value value = myGPIO_getIrq(gpio);
	myGPIO_interruptAck(gpio);
	value = myGPIO_getIrq(gpio);
	myGPIO_setValue(gpio, myGPIO_pin0 | myGPIO_pin1 | myGPIO_pin2 | myGPIO_pin3, value);
	myGPIO_setValue(gpio, myGPIO_pin0 | myGPIO_pin1 | myGPIO_pin2 | myGPIO_pin3, value);

	myGPIO_interruptEnable(gpio);	// ri-abilito gli interrupt
}
 * @endcode
 * @code
#include "xscugic.h"
#include "xparameters.h"
#include "myGPIO.h"

#define btn_addr	XPAR_MYGPIO_0_S00_AXI_BASEADDR	// base-address del gpio connesso ai button
#define btn_line	XPAR_FABRIC_MYGPIO_0_IRQ_INTR	// linea di interrupt del gpio connesso ai button
#define swc_addr	XPAR_MYGPIO_1_S00_AXI_BASEADDR	// base-address del gpio connesso agli switch
#define swc_line	XPAR_FABRIC_MYGPIO_1_IRQ_INTR	// linea di interrupt del gpio connesso agli switch
#define led_addr	XPAR_MYGPIO_2_S00_AXI_BASEADDR	// base-address del gpio connesso ai led

#define gic_id		XPAR_SCUGIC_0_DEVICE_ID			// id del device gic

myGPIO_t led_gpio;
myGPIO_t btn_gpio;
myGPIO_t swc_gpio;
XScuGic GIC;

// funzione di inizializzazione dei device gpio
void gpio_init(void);


// isr per button e switch
// devono necessariamente avere questa firma: restituire void e possedere un solo parametro puntatore
// a void. In questo caso non viene utilizzato (tutte le variabili sono globali), ma tale puntatore
// puo' essere usato per scambiare dati di ingresso/uscita alle isr
void btn_isr(void*); // isr per i button
void swc_isr(void*); // isr per gli switch

// funzione di configurazione del device gic e delle interruzioni
int int_config(void);

int main()
{
	gpio_init();
	int_config();

	for (;;);

    return 0;
}


void gpio_init(void) {
	uint8_t i;
	myGPIO_init(&led_gpio, led_addr);
	myGPIO_init(&btn_gpio, btn_addr);
	myGPIO_init(&swc_gpio, swc_addr);
	for (i=0; i<myGPIO_pin4; i++) {
		myGPIO_setMode(&led_gpio, myGPIO_pin(i), myGPIO_write);
		myGPIO_setValue(&led_gpio, myGPIO_pin(i), myGPIO_reset);
		myGPIO_setMode(&btn_gpio, myGPIO_pin(i), myGPIO_read);
		myGPIO_setValue(&btn_gpio, myGPIO_pin(i), myGPIO_reset);
		myGPIO_setMode(&swc_gpio, myGPIO_pin(i), myGPIO_read);
		myGPIO_setValue(&swc_gpio, myGPIO_pin(i), myGPIO_reset);
	}
}

int int_config(void) {
	// inizializza il driver del GIC
	Xil_ExceptionInit();

	// ottiene i parametri di configurazione del GIC, lo configura ed inizializza
	// sintassi : XScuGic_LookupConfig(GIC_id)
	// sintassi : XScuGic_CfgInitialize(GIC_ptr, config, cpu_address)
	XScuGic_Config *IntcConfig = XScuGic_LookupConfig(gic_id);
	if (IntcConfig == NULL)
		return -1;
	if (XScuGic_CfgInitialize(&GIC, IntcConfig, IntcConfig->CpuBaseAddress) != XST_SUCCESS)
		return -1;

	// registra l'interrupt handler del GIC alla logica di gestione del processing-system
	// sintassi : Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, handler, gic_ptr)
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,(Xil_ExceptionHandler)XScuGic_InterruptHandler, &GIC);

	// registrazione degli handler
	// le righe seguenti stabiliscono quale sia l'handler da chiamare e quali dati bisogna passargli
	// qualora si manifesti una interruzione su una line di irq.
	// sintassi : XScuGic_Connect(GIC, irq_line, handler, data)
	if (XScuGic_Connect(&GIC, btn_line, (Xil_InterruptHandler)btn_isr, (void*)NULL) != XST_SUCCESS)
		return -1;
	if (XScuGic_Connect(&GIC, swc_line, (Xil_InterruptHandler)swc_isr, (void*)NULL) != XST_SUCCESS)
			return -1;

	// abilitazione degli interrupt sulle linee connesse alle periferiche
	// sintassi: XScuGic_Enable(GIC,irq_line);
	XScuGic_Enable(&GIC, btn_line);
	XScuGic_Enable(&GIC, swc_line);

	// abilitazione degli interrupt delle periferiche
	myGPIO_interruptEnable(&btn_gpio);
	myGPIO_interruptEnable(&swc_gpio);

	// abilitazione degli interrupt del processing-system
	Xil_ExceptionEnable();
	return 0;
}

void btn_isr(void* data) {
	myGPIO_interruptDisable(&btn_gpio);	// disabilito gli interrupt
	myGPIO_interruptAck(&btn_gpio);		// clear del flag irq
	myGPIO_mask mask = myGPIO_getRead(&btn_gpio);
	myGPIO_setValue(&led_gpio, myGPIO_pin0 | myGPIO_pin1 | myGPIO_pin2 | myGPIO_pin3, myGPIO_reset);
	myGPIO_setValue(&led_gpio, mask, myGPIO_set);
	myGPIO_interruptEnable(&btn_gpio);	// ri-abilito gli interrupt
}

void swc_isr(void* data) {
	myGPIO_interruptDisable(&swc_gpio);	// disabilito gli interrupt
	myGPIO_interruptAck(&swc_gpio);		// clear del flag irq
	myGPIO_mask mask = myGPIO_getRead(&swc_gpio);
	myGPIO_setValue(&led_gpio, myGPIO_pin0 | myGPIO_pin1 | myGPIO_pin2 | myGPIO_pin3, myGPIO_reset);
	myGPIO_setValue(&led_gpio, mask, myGPIO_set);
	myGPIO_interruptEnable(&swc_gpio);	// ri-abilito gli interrupt
}
 * @endcode
 */
void myGPIO_interruptEnable(myGPIO_t *gpio);

/**
 * @brief Disabilita gli interrupt per un device myGPIO
 * 
 * @param [in] gpio puntatore a myGPIO_t, che astrae un device myGPIO;
 * 
 * @code
 * @endcode
 */
void myGPIO_interruptDisable(myGPIO_t *gpio);

/**
 * @brief Segnala al device che l'interruzione da lui sollevata e' stata servita.
 * 
 * @param [in] gpio puntatore a myGPIO_t, che astrae un device myGPIO;
 * 
 * @code
 * @endcode
 */
void myGPIO_interruptAck(myGPIO_t *gpio);

/**
 * @brief Consente di verificare se un device myGPIO abbia generato un'interruzione
 * 
 * @param [in] gpio puntatore a myGPIO_t, che astrae un device myGPIO;
 * 
 * @retval myGPIO_set se il device ha lanciato un'interruzione non ancora servita
 * @retval myGPIO_reset se il device non ha interruzioni pendenti
 * 
 * @code
 * @endcode
 */
myGPIO_value myGPIO_getIrq(myGPIO_t *gpio);

/**
 * @}
 * @}
 */

#endif
