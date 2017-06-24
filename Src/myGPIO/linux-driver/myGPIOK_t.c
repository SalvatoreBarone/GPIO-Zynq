/**
 * @file myGPIOK_t.c
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
#include "myGPIOK_t.h"
#include "myGPIOK_dev_int.h"



int myGPIOK_t_Init(	myGPIOK_t* device,
					uint32_t id,
					uint32_t irq_mask) {
	int error = 0;

	device->id = id;
	device->irq_mask = irq_mask;


/** <h5>Inizializzazione della wait-queue per la system-call read() e poll()</h5>
 * In linux una wait queue viene implementata da una struttura dati wait_queue_head_t, definita in
 * <linux/wait.h>.
 * Il driver in questione prevede due wait-queue differenti: una per la system-call read() ed una per la
 * system-call poll(). Entrambe le code vengono inizializzate dalla funzione myGPIOK_probe().
 * @code
 * init_waitqueue_head(&my_queue);
 * @endcode
 * Si veda la documentazione della funzione myGPIOK_read() per dettagli ulteriori.
 */
	init_waitqueue_head(&device->read_queue);
	init_waitqueue_head(&device->poll_queue);
/** <h5>Inizializzazione degli spinlock</h5>
 * I semafori sono uno strumento potentissimo per per l'implementazione di sezioni critiche, ma non possono
 * essere usati in codice non interrompibile. Gli spilock sono come i semafori, ma possono essere usati
 * anche in codice non interrompibile, come puo' esserlo un modulo kernel.
 * Sostanzialmente se uno spinlock e' gia' stato acquisito da qualcun altro, si entra in un hot-loop dal
 * quale si esce solo quando chi possiede lo spinlock lo rilascia. Trattandosi di moduli kernel, e' di
 * vitale importanza che la sezione critica sia quanto piu' piccola possibile. Ovviamente l'implementazione
 * e' "un po'" piu' complessa di come e' stata descritta, ma il concetto e' questo. Gli spinlock sono
 * definiti in <linux/spinlock.h>.
 * L'inizializzazione di uno spinlock avviene usando la funzione
 * @code
 * void spin_lock_init(spinlock_t *lock);
 * @endcode
 */
	spin_lock_init(&device->slock_int);
	spin_lock_init(&device->sl_total_irq);
	device->can_read = 0;
	device->total_irq = 0;
/** <h5>Abilitazione degli interrupt del device</h5>
 * A seconda del valore CFLAGS_myGPIOK.o (si veda il Makefile a corredo), vengono abilitati gli interrupt della
 * periferica. Se si tratta del GPIO Xilinx vengono abilitati gli interrupt globali e gli interrupt sul canale
 * due. Se si tratta del device GPIO custom, essendo esso parecchio piu' semplice, e' necessario abilitare solo
 * gli interrupt globali.
 */
#ifdef __XGPIO__
	XGpio_Global_Interrupt(device->vrtl_addr, XGPIO_GIE);
	XGpio_Channel_Interrupt(device->vrtl_addr, device->irq_mask);
#else
	myGPIOK_GlobalInterruptEnable(device->vrtl_addr);
	myGPIOK_PinInterruptEnable(device->vrtl_addr, device->irq_mask);
#endif

	return error;
}

void myGPIOK_t_Destroy(myGPIOK_t* device) {

}

/**
 * @}
 * @}
 */
