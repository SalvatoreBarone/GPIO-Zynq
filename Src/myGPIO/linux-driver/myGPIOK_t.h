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
 */
#ifndef __MYGPIOK_T__
#define __MYGPIOK_T__

#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/wait.h>
#include <linux/spinlock.h>

/**
 * @brief Stuttura per l'astrazione di un device myGPIO in kernel-mode
 *
 * E' buona abitudine, se non quasi indispensabile, definire una struttura dati nella quale contenere tutto
 * cio' che e' legato al device o al driver. In questo modulo viene usata la struttura myGPIOK_t per contenere
 * tutto cio' che e' necessario al funzionamento del driver.
 */
typedef struct {
	uint32_t id;				/**<	Identificativo del device */
	struct cdev cdev;			/**<	Stuttura per l'astrazione di un device a caratteri,
	 	 	 	 	 	 	 			Il kernel usa, internamente, una struttura cdev per rappresentare i
	 	 	 	 	 	 	 			device a caratteri. Prima che il kernel invochi le funzioni definite
	 	 	 	 	 	 	 			dal driver per il device, bisogna allocare e registrare uno, o piu',
	 	 	 	 	 	 	 			oggetti cdev. In questo caso e' sufficiente allocare uno solo di
	 	 	 	 	 	 	 			questi oggetti.*/
	uint32_t irqNumber; 		/**< 	interrupt-number a cui il device e' connesso. Restituito dalla
										chiamata alla funzione irq_of_parse_and_map() */
	uint32_t irq_line;			/**<	linea interrupt alla quale il device e' connesso */
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


int myGPIOK_t_Init(	myGPIOK_t* device,
					uint32_t id,
					uint32_t irq_mask);

void myGPIOK_t_Destroy(myGPIOK_t* device);

#endif

/**
 * @}
 * @}
 */
