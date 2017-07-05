/**
 * @example interrupt_bare.c
 * @author Salvatore Barone <salvator.barone@gmail.com>
 * @date 23 06 2017
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
 * @brief Uso del driver myGPIO con interruzioni bare-metal su Zynq-7000
 *
 * @details
 * <h3>Configurazione hardware</h3>
 * L'esempio fa riferimento ad una configurazione hardware in cui, oltre alla ip-core Zynq7000 processing
 * sysyem, sono presenti tre diversi device myGPIO, uno connesso ai led (base address 0x43c00000), uno
 * connesso ai button (base address 0x43c10000) ed uno connesso agli switch (base address 0x43c20000).
 * Lo schema viene riportato di seguito:
 * @htmlonly
 * <div align='center'>
 * <img src="../../schemes/interrupt_bare.png"/>
 * </div>
 * @endhtmlonly
 *
 * <h4>ISR per la gestione di interrupt provenienti dal gpio connesso agli switch</h4>
 * @code
void swc_isr(void* data) {
	myGPIO_GlobalInterruptDisable(&swc_gpio);
	myGPIO_mask enabledInterrut = myGPIO_EnabledPinInterrupt(&swc_gpio);
	myGPIO_PinInterruptDisable(&swc_gpio, enabledInterrut);

	myGPIO_mask pendingInterrupt = myGPIO_PendingPinInterrupt(&swc_gpio);
	myGPIO_PinInterruptAck(&swc_gpio, pendingInterrupt);

	myGPIO_mask value = myGPIO_GetRead(&swc_gpio);
	myGPIO_SetValue(&led_gpio, myGPIO_pin0 | myGPIO_pin1 | myGPIO_pin2 | myGPIO_pin3, myGPIO_reset);
	myGPIO_SetValue(&led_gpio, value, myGPIO_set);

	myGPIO_PinInterruptEnable(&swc_gpio, enabledInterrut);
	myGPIO_GlobalInterruptEnable(&swc_gpio);
}
 * @endcode
 * La funzione di cui sopra non fa altro che disabilitare momentaneamente le interruzioni della periferica,
 * leggere lo stato del registro “read”, resettare i led, per poi accendere solo quello corrispondente allo
 * switch arrivo e riabilitare l'interrupt della periferica.
 *
 * <h4>ISR per la gestione di interrupt provenienti dal gpio connesso ai button</h4>
 * @code
void btn_isr(void* data) {
	myGPIO_GlobalInterruptDisable(&btn_gpio);
	myGPIO_mask enabledInterrut = myGPIO_EnabledPinInterrupt(&btn_gpio);
	myGPIO_PinInterruptDisable(&btn_gpio, enabledInterrut);

	myGPIO_mask pendingInterrupt = myGPIO_PendingPinInterrupt(&btn_gpio);
	myGPIO_PinInterruptAck(&btn_gpio, pendingInterrupt);

	myGPIO_mask value = myGPIO_GetRead(&btn_gpio);
	myGPIO_SetValue(&led_gpio, myGPIO_pin0 | myGPIO_pin1 | myGPIO_pin2 | myGPIO_pin3, myGPIO_reset);
	myGPIO_SetValue(&led_gpio, value, myGPIO_set);

	myGPIO_PinInterruptEnable(&btn_gpio, enabledInterrut);
	myGPIO_GlobalInterruptEnable(&btn_gpio);
}
 * @endcode
 * La funzione di cui sopra non fa altro che disabilitare momentaneamente le interruzioni della periferica,
 * leggere lo stato del registro “read”, resettare i led, per poi accendere solo quello corrispondente al
 * button premuto e riabilitare l'interrupt della periferica.
 *
 * <h4>Configurazione del GIC e registrazione degli interrupt handler</h4>
 * @code
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
	if (XScuGic_Connect(&GIC, btn_irq_line, (Xil_InterruptHandler)btn_isr, (void*)NULL) != XST_SUCCESS)
		return -1;
	if (XScuGic_Connect(&GIC, swc_irq_line, (Xil_InterruptHandler)swc_isr, (void*)NULL) != XST_SUCCESS)
			return -1;

	// abilitazione degli interrupt sulle linee connesse alle periferiche
	// sintassi: XScuGic_Enable(GIC,irq_line);
	XScuGic_Enable(&GIC, btn_irq_line);
	XScuGic_Enable(&GIC, swc_irq_line);

	// abilitazione degli interrupt delle periferiche
	myGPIO_GlobalInterruptEnable(&btn_gpio);
	myGPIO_PinInterruptEnable(&btn_gpio, myGPIO_pin0 | myGPIO_pin1 | myGPIO_pin2 | myGPIO_pin3);
	myGPIO_GlobalInterruptEnable(&swc_gpio);
	myGPIO_PinInterruptEnable(&swc_gpio, myGPIO_pin0 | myGPIO_pin1 | myGPIO_pin2 | myGPIO_pin3);

	// abilitazione degli interrupt del processing-system
	Xil_ExceptionEnable();
	return 0;
}
 * @endcode
 * La funzione di configurazione delle interrupt e del GIC fa uso di alcune funzioni di libreria definite nell'header
 * file "xscugic.h", il quale implementa il driver della periferica GIC, e di alcune macro definite nel file
 * "xparameters.h". Solo per comodità le macro definite in xparameters.h sono state ridefinite come segue.
 * @code
#define led_base_addr XPAR_MYGPIO_0_S00_AXI_BASEADDR
#define btn_base_addr XPAR_MYGPIO_1_S00_AXI_BASEADDR
#define swc_base_addr XPAR_MYGPIO_2_S00_AXI_BASEADDR
#define led_irq_line XPAR_FABRIC_MYGPIO_0_INTERRUPT_INTR
#define btn_irq_line XPAR_FABRIC_MYGPIO_1_INTERRUPT_INTR
#define swc_irq_line XPAR_FABRIC_MYGPIO_2_INTERRUPT_INTR
#define gic_id		XPAR_SCUGIC_0_DEVICE_ID
 * @endcode
 * Le funzioni di libreria usate sono riportate di seguito, assieme ad una breve descrizione tratta dalla documentazione
 * interna.
 * - Xil_ExceptionInit(): inizializza gli exception-handlers di tutti i processori. Per ARM Cortex A53, R5 ed A9 gli
 *   exception-handlers sono sono inizializzati staticamente, per cui questa funzione non fa niente. Viene mantenutaper
 *   prevenire errori in fase di compilazione e per garantire backward-compatibility.
 * - XScuGic_LookupConfig(): looksup della configurazione di un device, basandosi sull'identificativo univoco dello
 *   stesso, dalla tabella contenente le configurazioni di tutti i device. Prende in ingresso un parametro DeviceId
 *   e restituisce un puntatore a XScuGic, contenente la configurazione, o NULL se il device non viene trovato.
 * - XScuGic_CfgInitialize(): inizializza e configura un interrupt-controller instance/driver. La procedura di
 *   inizializzazione prevede:
 *   	- l'inizializzazione dei campi di una struttura XScuGic;
 *   	- la configurazione della Initial vector-table, con funzioni stub;
 *   	- disabilitazione di tutte le sorgenti di interruzione
 *   	.
 *	Parametri:
 *	- InstancePtr: puntatore a struttura XScuGic;
 *	- ConfigPtr: puntatore alla configurazione del device, restituito dalla funzione XScuGic_LookupConfig();
 *	- EffectiveAddr: indirizzo base del device;
 *	Restituisce XST_SUCCESS se l'inizializzazione viene completata con successo.
 * - Xil_ExceptionRegisterHandler(): crea una connessione tra l'identificativo di una sorgente di eccezioni e
 *	  l'handler associato, in modo che l'handler venga eseguito qualora si manifestasse una eccezione. Prende i
 *	  seguenti parametri:
 *	  	- exception_id: ID della sorgente di eccezioni;
 *	  	- Handler: puntatore alla funzione di servizio;
 *	  	- Data: puntatore ai dati da passare all'handler;
 *	    .
 * - XScuGic_Connect(): crea una connessione tra l'identificativo di una sorgente di interruzioni e l'handler
 *	  associato, in modo che l'handler venga eseguito qualora si manifestasse una interruzione. Prende i
 *	  seguenti parametri:
 *	  	- InstancePtr: puntatore ad una istanza XScuGic;
 *	  	- Int_Id: ID della sorgente di interruzioni;
 *	  	- Handler: puntatore alla funzione di servizio;
 *	  	- CallBackRef: puntatore ai dati da passare alla isr;
 *	  	.
 *	  	Restituisce XST_SUCCESS se l'handler è stato connesso correttamente.
 * - XScuGic_Enable(): abilita la sorgente di interruzioni Int_Id. Se ci sono pending interrupt per tale linea,
 *   scateneranno una interruzione dopo la chiamata a questa funzione. Parametri:
 *    	- InstancePtr: puntatore ad una istanza XScuGic;
 *    	- Int_Id: ID della sorgente di interruzioni;
 *      .
 * - Xil_ExceptionEnable(): abilita le interruzioni.
 */
#include "xparameters.h"
#include "xscugic.h"
#include "myGPIO.h"

myGPIO_t led_gpio;
myGPIO_t btn_gpio;
myGPIO_t swc_gpio;
XScuGic GIC;

#define led_base_addr XPAR_MYGPIO_0_S00_AXI_BASEADDR
#define btn_base_addr XPAR_MYGPIO_1_S00_AXI_BASEADDR
#define swc_base_addr XPAR_MYGPIO_2_S00_AXI_BASEADDR

#define led_irq_line XPAR_FABRIC_MYGPIO_0_INTERRUPT_INTR
#define btn_irq_line XPAR_FABRIC_MYGPIO_1_INTERRUPT_INTR
#define swc_irq_line XPAR_FABRIC_MYGPIO_2_INTERRUPT_INTR

#define gic_id		XPAR_SCUGIC_0_DEVICE_ID

// funzione di inizializzazione dei device gpio
void gpio_init(void);

// isr per button e switch
// devono necessariamente avere questa firma: restituire void e possedere un solo parametro puntatore
// a void. In questo caso non viene utilizzato (tutte le variabili sono globali), ma tale puntatore
// può essere usato per scambiare dati di ingresso/uscita alle isr
void btn_isr(void*); // isr per i button
void swc_isr(void*); // isr per gli switch

// funzione di configurazione del device gic e delle interruzioni
int int_config(void);

int main() {
	gpio_init();
	int_config();
	for (;;);
    return 0;
}

void gpio_init(void) {
	uint8_t i;

	myGPIO_Init(&led_gpio, led_base_addr);
	myGPIO_Init(&btn_gpio, btn_base_addr);
	myGPIO_Init(&swc_gpio, swc_base_addr);
	for (i=0; i<4; i++) {
		myGPIO_SetMode(&led_gpio, myGPIO_pin(i), myGPIO_write);
		myGPIO_SetValue(&led_gpio, myGPIO_pin(i), myGPIO_reset);
		myGPIO_SetMode(&btn_gpio, myGPIO_pin(i), myGPIO_read);
		myGPIO_SetValue(&btn_gpio, myGPIO_pin(i), myGPIO_reset);
		myGPIO_SetMode(&swc_gpio, myGPIO_pin(i), myGPIO_read);
		myGPIO_SetValue(&swc_gpio, myGPIO_pin(i), myGPIO_reset);
	}
}

void btn_isr(void* data) {
	myGPIO_GlobalInterruptDisable(&btn_gpio);
	myGPIO_mask enabledInterrut = myGPIO_EnabledPinInterrupt(&btn_gpio);
	myGPIO_PinInterruptDisable(&btn_gpio, enabledInterrut);

	myGPIO_mask pendingInterrupt = myGPIO_PendingPinInterrupt(&btn_gpio);
	myGPIO_PinInterruptAck(&btn_gpio, pendingInterrupt);

	myGPIO_mask value = myGPIO_GetRead(&btn_gpio);
	myGPIO_SetValue(&led_gpio, myGPIO_pin0 | myGPIO_pin1 | myGPIO_pin2 | myGPIO_pin3, myGPIO_reset);
	myGPIO_SetValue(&led_gpio, value, myGPIO_set);

	myGPIO_PinInterruptEnable(&btn_gpio, enabledInterrut);
	myGPIO_GlobalInterruptEnable(&btn_gpio);
}

void swc_isr(void* data) {
	myGPIO_GlobalInterruptDisable(&swc_gpio);
	myGPIO_mask enabledInterrut = myGPIO_EnabledPinInterrupt(&swc_gpio);
	myGPIO_PinInterruptDisable(&swc_gpio, enabledInterrut);

	myGPIO_mask pendingInterrupt = myGPIO_PendingPinInterrupt(&swc_gpio);
	myGPIO_PinInterruptAck(&swc_gpio, pendingInterrupt);

	myGPIO_mask value = myGPIO_GetRead(&swc_gpio);
	myGPIO_SetValue(&led_gpio, myGPIO_pin0 | myGPIO_pin1 | myGPIO_pin2 | myGPIO_pin3, myGPIO_reset);
	myGPIO_SetValue(&led_gpio, value, myGPIO_set);

	myGPIO_PinInterruptEnable(&swc_gpio, enabledInterrut);
	myGPIO_GlobalInterruptEnable(&swc_gpio);
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
	if (XScuGic_Connect(&GIC, btn_irq_line, (Xil_InterruptHandler)btn_isr, (void*)NULL) != XST_SUCCESS)
		return -1;
	if (XScuGic_Connect(&GIC, swc_irq_line, (Xil_InterruptHandler)swc_isr, (void*)NULL) != XST_SUCCESS)
			return -1;

	// abilitazione degli interrupt sulle linee connesse alle periferiche
	// sintassi: XScuGic_Enable(GIC,irq_line);
	XScuGic_Enable(&GIC, btn_irq_line);
	XScuGic_Enable(&GIC, swc_irq_line);

	// abilitazione degli interrupt delle periferiche
	myGPIO_GlobalInterruptEnable(&btn_gpio);
	myGPIO_PinInterruptEnable(&btn_gpio, myGPIO_pin0 | myGPIO_pin1 | myGPIO_pin2 | myGPIO_pin3);
	myGPIO_GlobalInterruptEnable(&swc_gpio);
	myGPIO_PinInterruptEnable(&swc_gpio, myGPIO_pin0 | myGPIO_pin1 | myGPIO_pin2 | myGPIO_pin3);

	// abilitazione degli interrupt del processing-system
	Xil_ExceptionEnable();
	return 0;
}

