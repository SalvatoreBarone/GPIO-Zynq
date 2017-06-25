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

int myGPIOK_Init(	myGPIOK_t* myGPIOK_device,
					struct device *dev,
					uint32_t id,
					dev_t Mm,
					const char* name,
					irq_handler_t irq_handler,
					uint32_t irq_mask) {
	int error = 0;

	myGPIOK_device->id = id;
	myGPIOK_device->Mm_number = Mm;
	myGPIOK_device->irq_mask = irq_mask;

/** <h5>Accedere al segmento di memoria a cui la periferica e' mappata</h5>
 * Un driver, tipicamente, prende possesso del segmento di memoria cui e' mappato il device con la funzione
 * di probe. Il problema e' che il device e' mappato ad un indirizzo di memoria fisico ed il Kernel, così
 * come qualsiasi altro programma, lavora su indirizzi di memoria virtuali. La funzione
 *
 * @code
 * int of_address_to_resource(struct device_node *node, int index, struct resource *r);
 * @endcode
 *
 * popola una struttura resource con l'indirizzo di memoria cui e' mapato il device usando le informazioni
 * contenute all'interno del device tree. Ad esempio, se il device tree contiene
 * @code
 * reg = <0x41200000 0x10000>;
 * @endcode
 * signidifa che l'indirizzo fisico associato al device e' l'indirizzo 0x41200000, che al device sono riservati
 * 0x10000 bytes. of_address_to_resource() settera' res.start = 0x41200000 e res.end = 0x4120ffff.
 */
	if ((error = of_address_to_resource(dev->of_node, 0, &myGPIOK_device->rsrc)) != 0) {
		printk(KERN_ERR "%s: request_irq() ha restituito %d\n", __func__, error);
		return error;
	}
	myGPIOK_device->rsrc_size = myGPIOK_device->rsrc.end - myGPIOK_device->rsrc.start + 1;
/** <h5>Allocazione della memoria del device</h5>
 * Le regioni di memoria per di I/O vanno allocate prima di poter essere usate.
 * @code
 * struct resource *request_mem_region(unsigned long start, unsigned long len, char *name);
 * @endcode
 * Questa funzione alloca una regione di memoria di len byte a partire da start restituendone l'indirizzo,
 * mentre nel caso in cui si verifichi un errore viene restituito NULL. La funzione viene chiamata per ottenere
 * l'accesso esclusivo della regione di memoria, per evitare che driver diversi tentino di accedere allo stesso
 * spazio di memoria.
 */
	if ((myGPIOK_device->mreg = request_mem_region(myGPIOK_device->rsrc.start, myGPIOK_device->rsrc_size, name)) == NULL) {
		printk(KERN_ERR "%s: request_mem_region() ha restituito NULL\n", __func__);
		return -ENOMEM;
	}
/** <h5>Remapping</h5>
 * L'allocazione dello spazio di memoria non e' l'unico step da eseguire prima che tale memoria possa essere
 * usata. E' necessario fare in modo che sia resa accessibile al kernel attraverso un mapping, usando la
 * funzione.
 * @code
 * void *ioremap(unsigned long phys_addr, unsigned long size);
 * @endcode
 *
 */
	if ((myGPIOK_device->vrtl_addr = ioremap(myGPIOK_device->rsrc.start, myGPIOK_device->rsrc_size))==NULL) {
		printk(KERN_ERR "%s: ioremap() ha restituito NULL\n", __func__);
		release_mem_region(myGPIOK_device->rsrc.start, myGPIOK_device->rsrc_size);
		return -ENOMEM;
	}
/** <h5>Registrazione di un interrupt-handler</h5>
 * Il modulo deve registrare un handler per gli interrupt.
 * L'handler deve essere compatibile con il tipo puntatore a funzione irq_handler_t, così definito.
 * @code
 * struct irqreturn_t (*irq_handler_t)(int irq, struct pt_regs * regs);
 * @endcode
 * Il modulo definisce la funzione myGPIOK_irq_handler(). L'handler puo' essere registrato usando
 * @code
 * int request_irq(	unsigned int irqNumber,
 * 					irqreturn_t (*handler)(int, void *, struct pt_regs *),
 * 					unsigned long irqflags,
 * 					const char *devname,
 * 					void *dev_id);
 * @endcode
 * IL parametro irqNumber puo' essere determinato automaticamente usando la funzione
 * @code
 * unsigned int irq_of_parse_and_map(struct device_node *node, int index);
 * @endcode
 * La funzione irq_of_parse_and_map() effettua un looks-up nella specifica degli interrupt all'interno del
 * device tree e restituisce un irq number cosi' come de lo aspetta request_irq() (cioe' compaci con
 * l'enumerazione in /proc/interrupts). Il secondo argomento della funzione e', tipicamente, zero, ad
 * indicare che, all'interno del device tree, verra' preso in considerazione soltanto il primo degli
 * interrupt specificate.
 * Il device tree, nella sezione dedicata al gpio,reca
 * @code
 * interrupts = <0 29 4>;
 * @endcode
 * Il primo numero (0) e' un flag che indica se l'interrupt sia connesso ad una line SPI (shared peripheral
 * interrupt). Un valore diverso da zero indica che la linea e' SPI.
 * Il secondo numero si riferisce all'interrupt number. Per farla breve, quando si definisce la parte hardware,
 * in questo specifico esempio il device GPIO e' connesso alla linea 61 del GIC. Sottraendo 32 si orriene 29.
 * Il terzo numero si riferisce alla tipologia dell'interrupt. Sono possibili tre valori:
 *  - 0 : power-up default
 *  - 1 : rising-edge
 *  - 4 : a livelli, active alto
 */
	myGPIOK_device->irqNumber = irq_of_parse_and_map(dev->of_node, 0);
	if ((error = request_irq(myGPIOK_device->irqNumber , irq_handler, 0, name, NULL)) != 0) {
		printk(KERN_ERR "%s: request_irq() ha restituito %d\n", __func__, error);
		iounmap(myGPIOK_device->vrtl_addr);
		release_mem_region(myGPIOK_device->rsrc.start, myGPIOK_device->rsrc_size);
		return error;
	}
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
	init_waitqueue_head(&myGPIOK_device->read_queue);
	init_waitqueue_head(&myGPIOK_device->poll_queue);
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
	spin_lock_init(&myGPIOK_device->slock_int);
	spin_lock_init(&myGPIOK_device->sl_total_irq);
	myGPIOK_device->can_read = 0;
	myGPIOK_device->total_irq = 0;
/** <h5>Abilitazione degli interrupt del device</h5>
 * A seconda del valore CFLAGS_myGPIOK.o (si veda il Makefile a corredo), vengono abilitati gli interrupt della
 * periferica. Se si tratta del GPIO Xilinx vengono abilitati gli interrupt globali e gli interrupt sul canale
 * due. Se si tratta del device GPIO custom, essendo esso parecchio piu' semplice, e' necessario abilitare solo
 * gli interrupt globali.
 */
#ifdef __XGPIO__
	XGpio_Global_Interrupt(myGPIOK_device, XGPIO_GIE);
	XGpio_Channel_Interrupt(myGPIOK_device, myGPIOK_device->irq_mask);
#else
	myGPIOK_GlobalInterruptEnable(myGPIOK_device);
	myGPIOK_PinInterruptEnable(myGPIOK_device, myGPIOK_device->irq_mask);
#endif

	return error;
}

void myGPIOK_Destroy(myGPIOK_t* device) {
	#ifdef __XGPIO__
	XGpio_Global_Interrupt(device, XGPIO_GIDS);
	XGpio_Channel_Interrupt(device, device->irq_mask);
#else
	myGPIOK_GlobalInterruptDisable(device);
	myGPIOK_PinInterruptDisable(device, device->irq_mask);
#endif
	free_irq(device->irqNumber, NULL);
	iounmap(device->vrtl_addr);
	release_mem_region(device->rsrc.start, device->rsrc_size);
}





void myGPIOK_GlobalInterruptEnable(myGPIOK_t* device) {
	iowrite32(1, (device->vrtl_addr + myGPIOK_GIES_OFFSET));
}

void myGPIOK_GlobalInterruptDisable(myGPIOK_t* device) {
	iowrite32(0, (device->vrtl_addr + myGPIOK_GIES_OFFSET));
}

void myGPIOK_PinInterruptEnable(myGPIOK_t* device, unsigned mask) {
	unsigned reg_value = ioread32((device->vrtl_addr + myGPIOK_PIE_OFFSET));
	reg_value |= mask;
	iowrite32(reg_value, (device->vrtl_addr + myGPIOK_PIE_OFFSET));
}

void myGPIOK_PinInterruptDisable(myGPIOK_t* device, unsigned mask) {
	unsigned reg_value = ioread32((device->vrtl_addr + myGPIOK_PIE_OFFSET));
	reg_value &= ~mask;
	iowrite32(reg_value, (device->vrtl_addr + myGPIOK_PIE_OFFSET));
}

unsigned myGPIOK_PendingPinInterrupt(myGPIOK_t* device) {
	return ioread32((device->vrtl_addr + myGPIOK_IRQ_OFFSET));
}

void myGPIOK_PinInterruptAck(myGPIOK_t* device, unsigned mask) {
	iowrite32(mask, (device->vrtl_addr + myGPIOK_IACK_OFFSET));
}

#ifdef __XGPIO__
void XGpio_Global_Interrupt(myGPIOK_t* device, unsigned mask) {
	iowrite32(mask, (device->vrtl_addr + XGPIO_GIE_OFFSET));
}

void XGpio_Channel_Interrupt(myGPIOK_t* device, unsigned mask) {
	iowrite32(mask, (device->vrtl_addr + XGPIO_IER_OFFSET));
}

void XGpio_Ack_Interrupt(myGPIOK_t* device, unsigned channel) {
	iowrite32(channel, (device->vrtl_addr + XGPIO_ISR_OFFSET));
}
#endif

/**
 * @}
 * @}
 */
