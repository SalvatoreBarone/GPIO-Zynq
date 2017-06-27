/**
 * @file myGPIOK_t.h
 * @author Salvatore Barone <salvator.barone@gmail.com>
 * @date 24 06 2017
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
 * @addtogroup myGPIO
 * @{
 * @addtogroup Linux-Driver
 * @{
 * @addtogroup myGPIOK_t
 * @{
 * @brief Definisce l'oggetto myGPIO_t, che rappresenta un device myGPIO a livello kernel
 */
#ifndef __MYGPIOK_T__
#define __MYGPIOK_T__

#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/fs.h>

#include <linux/interrupt.h>
#include <linux/irqreturn.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>

#include <linux/wait.h>
#include <linux/spinlock.h>

#include <asm/uaccess.h>
#include <asm/io.h>

/**
 * @brief Stuttura per l'astrazione di un device myGPIO in kernel-mode
 *
 * E' buona abitudine, se non quasi indispensabile, definire una struttura dati nella quale contenere tutto
 * cio' che e' legato al device o al driver. In questo modulo viene usata la struttura myGPIOK_t per contenere
 * tutto cio' che e' necessario al funzionamento del driver.
 */
typedef struct {
	dev_t Mm;					/**<	Major e minor number associati al device */
	struct platform_device *op; /**<	Puntatore a struttura platform_device cui l'oggetto myGPIOK_t si riferisce */
	struct cdev cdev;			/**<	Stuttura per l'astrazione di un device a caratteri
										Il kernel usa, internamente, una struttura cdev per rappresentare i device a
										caratteri. Prima che il kernel invochi le funzioni definite dal driver per il
										device, bisogna allocare e registrare uno, o piu', oggetti cdev. In questo
										caso e' sufficiente allocare uno solo di questi oggetti. */
	struct device* dev;			/**< */
	struct class*  class;		/**< */
	uint32_t irqNumber; 		/**< 	interrupt-number a cui il device e' connesso. Restituito dalla
										chiamata alla funzione irq_of_parse_and_map() */
	uint32_t irq_mask;			/**<	maschera delle interruzioni interne per il device */
	struct resource rsrc; 		/**<	Struttura che astrae una risorsa device, dal punto di vista della
										memoria alla quale la risorsa e' mappata. In particolare i campi
										"start" ed "end" contengono, rispettivamente, il primo e l'ultimo
										indirizzo fisico a cui il device e' mappato. */
	struct resource *mreg;		/**<	puntatre alla regione di memoria cui il device e' mapapto */
	uint32_t rsrc_size; 		/**<	rsrc.end - rsrc.start
	 	 	 	 	 	 	 			numero di indirizzi associati alla periferica.
	 	 	 	 	 	 	 			occorre per effettuare il mapping indirizzo fisico - indirizzo
	 	 	 	 	 	 	 			virtuale */
	void *vrtl_addr; 			/**<	indirizzo virtuale della periferica */
	wait_queue_head_t read_queue; /**<  wait queue per la system-call read()
										Una chiamata a read() potrebbe arrivare quando i dati non sono
										disponibili, ma potrebbero esserlo in futuro, oppure, una chiamata a
										write() potrebbe avvenire quando il device non e' in grado di accettare
										altri dati (perche' il suo buffer di ingresso potrebbe essere pieno).
										Il processo chiamante non ha la minima conoscenza delle dinamiche
										interne del device, per cui, nell'impossibilita' di servire la
										richiesta, il driver deve bloccare il processo e metterlo tra i
										processi "sleeping", fin quando la richiesta non puo' essere servita.
										Tutti i processi in attesa di un particolare evento vengono posti
										all'interno della stessa wait queue. In linux una wait queue viene
										implementata da una struttura dati wait_queue_head_t, definita in
										<linux/wait.h>. */
	wait_queue_head_t poll_queue; /**< 	wait queue per la system-call poll() */
	uint32_t can_read; 			/**< 	Flag "puoi leggere"
	 	 	 	 	 	 				Il valore viene settato dalla funzione myGPIOK_irq_handler() al manifestarsi
	 	 	 	 	 	 				di un interrupt, prima di risvegliare i processi in attesa di un interrupt.
										I processi che effettuano read() bloccante restano bloccati finoche'
										int_occurred = 0 */
	spinlock_t slock_int; /**<			Spinlock usato per garantire l'accesso in mutua esclusione alla variabile
										int_occurred da parte delle funzioni del modulo.
										I semafori sono uno strumento potentissimo per per l'implementazione di
										sezioni	critiche, ma non possono essere usati in codice non interrompibile.
										Gli spilock sono come i semafori, ma possono essere usati anche in codice
										non interrompibile,	come puo' esserlo un modulo kernel.
										Sostanzialmente se uno spinlock e' gia' stato acquisito da qualcun altro, si
										entra in un hot-loop dal quale si esce solo quando chi possiede lo spinlock
										lo rilascia. Trattandosi di moduli kernel, e' di vitale importanza che la
										sezione critica sia quanto piu' piccola possibile. Ovviamente
										l'implementazione e' "un po'" piu' complessa di come e' stata descritta,
										ma il concetto e' questo. Gli spinlock sono definiti in <linux/spinlock.h>. */
	uint32_t total_irq;			/**< 	numero totale di interrupt manifestatesi */
	spinlock_t sl_total_irq; 	/**<	Spinlock usato per garantire l'accesso in mutua esclusione alla variabile
								 		total_irq da parte delle funzioni del modulo */

} myGPIOK_t;

extern int myGPIOK_Init(	myGPIOK_t* myGPIOK_device,
							struct module *owner,
							struct platform_device *op,
							struct class*  class,
							const char* driver_name,
							const char* device_name,
							uint32_t serial,
							struct file_operations *f_ops,
							irq_handler_t irq_handler,
							uint32_t irq_mask);

extern void myGPIOK_Destroy(myGPIOK_t* device);

extern void myGPIOK_SetCanRead(myGPIOK_t* device);

extern void myGPIOK_ResetCanRead(myGPIOK_t* device);

extern void myGPIOK_TestCanReadAndSleep(myGPIOK_t* device);

extern unsigned myGPIOK_GetPollMask(myGPIOK_t *device, struct file *file_ptr, struct poll_table_struct *wait);

extern void myGPIOK_IncrementTotal(myGPIOK_t* device);

extern void myGPIOK_WakeUp(myGPIOK_t* device);

extern void* myGPIOK_GetDeviceAddress(myGPIOK_t* device);


#define myGPIOK_GIES_OFFSET		0x0CU	//!< @brief Offset, rispetto all'indirizzo base, del registro "GIES"
#define myGPIOK_PIE_OFFSET		0x10U	//!< @brief Offset, rispetto all'indirizzo base, del registro "PIE"
#define myGPIOK_IRQ_OFFSET		0x14U	//!< @brief Offset, rispetto all'indirizzo base, del registro "IRQ"
#define myGPIOK_IACK_OFFSET		0x18U	//!< @brief Offset, rispetto all'indirizzo base, del registro "IACK"

extern void myGPIOK_GlobalInterruptEnable(myGPIOK_t* myGPIOK_device);

extern void myGPIOK_GlobalInterruptDisable(myGPIOK_t* myGPIOK_device);

extern void myGPIOK_PinInterruptEnable(myGPIOK_t* myGPIOK_device, unsigned mask);

extern void myGPIOK_PinInterruptDisable(myGPIOK_t* myGPIOK_device, unsigned mask);

extern unsigned myGPIOK_PendingPinInterrupt(myGPIOK_t* myGPIOK_device);

extern void myGPIOK_PinInterruptAck(myGPIOK_t* myGPIOK_device, unsigned mask);

/**
 * @cond
 * Funzioni e definizioni di servizio per GPIO Xilinx
 * Non verranno documentate.
 */
#ifdef __XGPIO__
#define XGPIO_GIE_OFFSET	0x11C
#define XGPIO_ISR_OFFSET	0x120
#define XGPIO_IER_OFFSET	0x128
#define XGPIO_GIE			0x80000000
#define XGPIO_CH1_IE		0x00000001
#define XGPIO_CH2_IE		0x00000002
#define XGPIO_GIDS			0x00000000
#define XGPIO_CH1_IDS		0x00000000
#define XGPIO_CH2_IDS		0x00000000
#define XGPIO_CH1_ACK		0x01
#define XGPIO_CH2_ACK		0x02

#define XGPIO_USED_INT		XGPIO_CH2_IE

extern void XGpio_Global_Interrupt(myGPIOK_t* myGPIOK_device, unsigned mask);

extern void XGpio_Channel_Interrupt(myGPIOK_t* myGPIOK_device, unsigned mask);

extern void XGpio_Ack_Interrupt(myGPIOK_t* myGPIOK_device, unsigned channel);

#endif
/**
 * @endcond
 */


#endif

/**
 * @}
 * @}
 * @}
 */
