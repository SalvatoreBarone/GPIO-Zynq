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
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/poll.h>

/**
 * @brief Inizializza una struttura myGPIOK_t e configura il device corrispondente
 *
 * @param [in]	myGPIOK_device puntatore a struttura myGPIOK_t, che si riferisce al device su cui operare
 * @param [in]	owner puntatore a struttura struct module, proprietario del device (THIS_MODULE)
 * @param [in]	op puntatore a struct platform_device, costituito dal parametro "op" con cui viene invocata probe() o la remove()
 * @param [in]	class puntatore a struct class, classe del device, deve essere stata precedentemente creata con class_create()
 * @param [in]	driver_name nome del driver
 * @param [in]	device_name nome del device
 * @param [in]	serial numero seriale del device
 * @param [in]	f_ops puntatore a struttura struct file_operations, specifica le funzioni che agiscono sul device
 * @param [in]	irq_handler puntatore irq_handler_t alla funzione che gestirà gli interrupt generati dal device
 * @param [in]	irq_mask maschera delle interruzioni del device
 *
 * @retval "0" se non si è verificato nessun errore
 *
 * @details
 */
int myGPIOK_Init(	myGPIOK_t* myGPIOK_device,
					struct module *owner,
					struct platform_device *op,
					struct class*  class,
					const char* driver_name,
					const char* device_name,
					uint32_t serial,
					struct file_operations *f_ops,
					irq_handler_t irq_handler,
					uint32_t irq_mask) {
	int error = 0;
	struct device *dev = NULL;
	char *file_name = kmalloc(strlen(driver_name) + 5, GFP_KERNEL);
	sprintf(file_name, device_name, serial);
	myGPIOK_device->op = op;
	myGPIOK_device->class = class;
/** <h5>Major-number e Minor-number</h5>
 * Ai device drivers sono associati un major-number ed un minor-number. Il major-number viene usato dal kernel
 * per identificare il driver corretto corrispondente ad uno specifico device, quando si effettuano operazioni
 * su di esso. Il ruolo del minor number dipende dal device e viene gestito internamente dal driver.
 * Questo driver, così come molti altri, usa il Major ed il minor number per distinguere le diverse istanze di
 * device myGPIO che usano il device-driver myGPIOK.
 * La registrazione di un device driver può essere effettuata chiamando <b>alloc_chrdev_region()</b>, la quale
 * alloca un char-device numbers. Il major number viene scelto dinamicamente e restituito dalla funzione
 * attraverso il parametro dev. La funzione restituisce un valore negativo nel caso in cui si verifichino errori,
 * 0 altrimenti.
 * @code
 * int alloc_chrdev_region (dev_t * dev, unsigned baseminor, unsigned count, const char *name);
 * @endcode
 *  - dev: major e minor number
 *  - baseminor: primo dei minor number richiesti
 *  - count: numero di minornumber richiesti
 *  - name: nome del device
 */
	if ((error = alloc_chrdev_region(&myGPIOK_device->Mm, 0 , 1, file_name)) != 0) {
		printk(KERN_ERR "%s: alloc_chrdev_region() ha restituito %d\n", __func__, error);
		return error;
	}
/** <h5>Operatori</h5>
 * Essendo un device "visto" come un file, ogni device driver deve implementare tutte le
 * system-call previste per l'interfacciamento con un file. La corrispondenza tra la
 * system-call e la funzione fornita dal driver viene stabilita attraverso la struttura
 * file_operations.
 * La struttura dati file_operations, definita in <linux/fs.h> mantiene puntatori a funzioni definite dal
 * driver che consentono di definire il comportamento degli operatori che agiscono su un file.
 * @code
 * 				static struct file_operations myGPIO_fops = {
 * 					.owner		= THIS_MODULE,
 * 					.llseek		= driver_seek,
 * 					.read		= driver_read,
 * 					.write		= driver_write,
 * 					.poll		= driver_poll,
 * 					.open		= driver_open,
 * 					.release	= driver_release
 * 				};
 * @endcode
 * Ogni campo della struttura deve puntare ad una funzione del driver che implementa uno
 * specifico "operatore" su file, oppure impostata a NULL se l'operatore non è supportato.
 * L'esatto comportamento del kernel, quando uno dei puntatori è NULL, varia da funzione
 * a funzione.
 * La lista seguente introduce tutti gli operatori che un'applicazione può invocare
 * su un device. La lista è stata mantenuta snella, includendo solo i campi strettamente
 * necessari.
 *
 * - <i>struct module *owner</i> :<br>
 * 		il primo campo della struttura non è un operatore, ma un puntatore al modulo che
 * 		"possiede" la struttura. Il campo ha lo scopo di evitare che il modulo venga rimosso
 * 		dal kernel quando uno degli operatori è in uso. Viene inizializzato usando la macro
 * 		THIS_MODULE, definita in <linux/module.h>.
 *
 * - <i>loff_t (*llseek) (struct file *, loff_t, int)</i> :
 * 		il campo llseek è usato per cambiare la posizione della "testina" di lettura/
 * 		scrittura in un file. La funzione restituisce la nuova posizione della testina.
 * 		loff_t è un intero a 64 bit (anche su architetture a 32 bit). Eventuali errori
 * 		vengono segnalati con un valore di ritorno negativo. Se questo campo è posto a
 * 		NULL, eventuali chiamate a seek modifigheranno la posizione della testina in un
 * 		modo impredicibile.
 *
 * - <i>ssize_t (*read) (struct file *, char _ _user *, size_t, loff_t *)</i> :<br>
 * 		usata per leggere dati dal device. Se lasciato a NULL, ogni chiamata a read fallirà
 * 		e non sarà possibile leggere dal device. La funzione restituisce il numero di byte
 * 		letti con successo ma, nel caso si verifichi un errore, restituisce un numero intero
 * 		negativo.
 *
 * - <i>ssize_t (*write) (struct file *, const char _ _user *, size_t, loff_t *)</i> :<br>
 * 		invia dati al device. Se NULL ogni chiamata alla system-call write causerà un errore.
 * 		Il valore di ritorno, se non negativo, rappresenta il numero di byte correttamente
 * 		scritti.
 *
 * - <i>unsigned int (*poll) (struct file *, struct poll_table_struct *)</i> :<br>
 * 		questo metodo è il back-end di tre diverse system-calls: poll, epoll e select, le quali
 * 		sono usate per capire se una operazione di lettura/scrittura du un device possano
 * 		risultare bloccanti o meno. La funzione dovrebbe restituire una maschera che indichi
 * 		se sia possibile effettuare operazioni di lettura/scrittura non bloccanti, in modo che
 * 		il kernel possa bloccare il processo e risvegliarlo solo quando tali operazioni diventino
 * 		possibili. Se viene lasciata NULL si intende che le operazioni di lettura/scrittura sul
 * 		device siano sempre non-bloccanti.
 *
 * - <i>int (*open) (struct inode *, struct file *)</i> :<br>
 * 		Anche se, di solito, è la prima operazione che si effettua su un file, non è strettamente
 * 		necessaria la sua implementazione. Se lasciata NULL, l'apertura del device andrà comunque
 * 		a buon fine, ma al driver non verrà inviata alcuna notifica.
 *
 * - <i>int (*release) (struct inode *, struct file *)</i> :<br>
 * 		questo operatore viene invocato quando il file viene rilasciato. Come open, può essere
 * 		lasciato NULL.
 *
 * L'inizializzazione di un device a caratteri passa anche attraverso la definizione di questo tipo di
 * operatori. Essi possono essere impostati attraverso l'uso della funzione
 * @code
 *	void cdev_init (struct cdev *cdev, const struct file_operations *fops);
 * @endcode
 * la quale prende, come parametri
 *  - cdev: puntatore a struttura cdev da inizializzare;
 *  - fops: puntatore a struttura file_operation con cui inizializzare il device.
 */
	cdev_init (&myGPIOK_device->cdev, f_ops);
	myGPIOK_device->cdev.owner = owner;
/** <h5>Creazione del device</h5>
 * Il passo successivo è la registrazione del device e la sua aggiunta al filesystem. Tale operazione
 * può essere effettuata chiamando
 * @code
 * struct device * device_create( struct class *class, struct device *parent, dev_t devt, const char *fmt, ...)
 * @endcode
 *  - class: puntatore alla struttura class alla quale il device deve essere registrato
 *  - parent: puntatore ad eventuale device parent
 *  - devt: tmajor number
 *  - fmt: nome del device.
 *
 * La funzione pù essere usata solo sulla classe dei device a caratteri. Crea un device all'interno del
 * filesystem, associandogli il major number preventivamente inizializzato. La funzione restituisce il puntatore
 * alla struttura device creata all'interno del filesystem. Si noti che il puntatre alla struttura classes DEVE
 * essere stato precedentemente creato attraverso una chiamata alla funzione <i>class_create()</i>.
 */
	if ((myGPIOK_device->dev = device_create(class, NULL, myGPIOK_device->Mm, NULL, file_name)) == NULL) {
		printk(KERN_ERR "%s: device_create() ha restituito NULL\n", __func__);
		error = -ENOMEM;
		goto device_create_error;
	}
/** <h5>Aggiunta del device</h5>
 * Il driver, a questo punto, è pronto per essere aggiunto. è possibile aggiungere il driver usando
 * @code
 * int cdev_add (struct cdev *p, dev_t dev, unsigned count);
 * @endcode
 * La quale accetta come parametri
 *  - p: puntatore a struttura cdev structure per il device
 *  - dev: device number (precedentemente inizializzato usando la funzione <i>alloc_chrdev_region()</i>)
 *  - count: numero di minor-numbers richiesti per il device
 *
 * La funzione restituisce un numero negativo in caso di errore.
 *
 */
	if ((error = cdev_add(&myGPIOK_device->cdev, myGPIOK_device->Mm, 1)) != 0) {
		printk(KERN_ERR "%s: cdev_add() ha restituito %d\n", __func__, error);
		goto cdev_add_error;
	}
/** <h5>Accedere al segmento di memoria a cui la periferica è mappata</h5>
 * Un driver, tipicamente, prende possesso del segmento di memoria cui è mappato il device con la funzione
 * di probe. Il problema è che il device è mappato ad un indirizzo di memoria fisico ed il Kernel, così
 * come qualsiasi altro programma, lavora su indirizzi di memoria virtuali. La funzione
 *
 * @code
 * int of_address_to_resource(struct device_node *node, int index, struct resource *r);
 * @endcode
 *
 * popola una struttura resource con l'indirizzo di memoria cui è mapato il device usando le informazioni
 * contenute all'interno del device tree. Ad esempio, se il device tree contiene
 * @code
 * reg = <0x41200000 0x10000>;
 * @endcode
 * signidifa che l'indirizzo fisico associato al device è l'indirizzo 0x41200000, che al device sono riservati
 * 0x10000 bytes. of_address_to_resource() setterà res.start = 0x41200000 e res.end = 0x4120ffff.
 */
	dev = &op->dev;
	if ((error = of_address_to_resource(dev->of_node, 0, &myGPIOK_device->rsrc)) != 0) {
		printk(KERN_ERR "%s: request_irq() ha restituito %d\n", __func__, error);
		goto of_address_to_resource_error;
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
	if ((myGPIOK_device->mreg = request_mem_region(myGPIOK_device->rsrc.start, myGPIOK_device->rsrc_size, file_name)) == NULL) {
		printk(KERN_ERR "%s: request_mem_region() ha restituito NULL\n", __func__);
		error = -ENOMEM;
		goto request_mem_region_error;
	}
/** <h5>Remapping</h5>
 * L'allocazione dello spazio di memoria non è l'unico step da eseguire prima che tale memoria possa essere
 * usata. è necessario fare in modo che sia resa accessibile al kernel attraverso un mapping, usando la
 * funzione.
 * @code
 * void *ioremap(unsigned long phys_addr, unsigned long size);
 * @endcode
 *
 */
	if ((myGPIOK_device->vrtl_addr = ioremap(myGPIOK_device->rsrc.start, myGPIOK_device->rsrc_size))==NULL) {
		printk(KERN_ERR "%s: ioremap() ha restituito NULL\n", __func__);
		error = -ENOMEM;
		goto ioremap_error;
	}
/** <h5>Registrazione di un interrupt-handler</h5>
 * Il modulo deve registrare un handler per gli interrupt.
 * L'handler deve essere compatibile con il tipo puntatore a funzione irq_handler_t, così definito.
 * @code
 * struct irqreturn_t (*irq_handler_t)(int irq, struct pt_regs * regs);
 * @endcode
 * Il modulo definisce la funzione myGPIOK_irq_handler(). L'handler può essere registrato usando
 * @code
 * int request_irq(	unsigned int irqNumber,
 * 					irqreturn_t (*handler)(int, void *, struct pt_regs *),
 * 					unsigned long irqflags,
 * 					const char *devname,
 * 					void *dev_id);
 * @endcode
 * IL parametro irqNumber può essere determinato automaticamente usando la funzione
 * @code
 * unsigned int irq_of_parse_and_map(struct device_node *node, int index);
 * @endcode
 * La funzione irq_of_parse_and_map() effettua un looks-up nella specifica degli interrupt all'interno del
 * device tree e restituisce un irq number così come de lo aspetta request_irq() (cioè compaci con
 * l'enumerazione in /proc/interrupts). Il secondo argomento della funzione è, tipicamente, zero, ad
 * indicare che, all'interno del device tree, verrà preso in considerazione soltanto il primo degli
 * interrupt specificate.
 * Il device tree, nella sezione dedicata al gpio,reca
 * @code
 * interrupts = <0 29 4>;
 * @endcode
 * Il primo numero (0) è un flag che indica se l'interrupt sia connesso ad una line SPI (shared peripheral
 * interrupt). Un valore diverso da zero indica che la linea è SPI.
 * Il secondo numero si riferisce all'interrupt number. Per farla breve, quando si definisce la parte hardware,
 * in questo specifico esempio il device GPIO è connesso alla linea 61 del GIC. Sottraendo 32 si orriene 29.
 * Il terzo numero si riferisce alla tipologia dell'interrupt. Sono possibili tre valori:
 *  - 0 : power-up default
 *  - 1 : rising-edge
 *  - 4 : a livelli, active alto
 */
	myGPIOK_device->irqNumber = irq_of_parse_and_map(dev->of_node, 0);
	if ((error = request_irq(myGPIOK_device->irqNumber , irq_handler, 0, file_name, NULL)) != 0) {
		printk(KERN_ERR "%s: request_irq() ha restituito %d\n", __func__, error);
		goto irq_of_parse_and_map_error;
	}
	myGPIOK_device->irq_mask = irq_mask;
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
 * anche in codice non interrompibile, come può esserlo un modulo kernel.
 * Sostanzialmente se uno spinlock è già stato acquisito da qualcun altro, si entra in un hot-loop dal
 * quale si esce solo quando chi possiede lo spinlock lo rilascia. Trattandosi di moduli kernel, è di
 * vitale importanza che la sezione critica sia quanto più piccola possibile. Ovviamente l'implementazione
 * è "un pò" più complessa di come è stata descritta, ma il concetto è questo. Gli spinlock sono
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
 * due. Se si tratta del device GPIO custom, essendo esso parecchio più semplice, è necessario abilitare solo
 * gli interrupt globali.
 */
#ifdef __XGPIO__
	XGpio_Global_Interrupt(myGPIOK_device, XGPIO_GIE);
	XGpio_Channel_Interrupt(myGPIOK_device, myGPIOK_device->irq_mask);
#else
	myGPIOK_GlobalInterruptEnable(myGPIOK_device);
	myGPIOK_PinInterruptEnable(myGPIOK_device, myGPIOK_device->irq_mask);
#endif
	goto no_error;

irq_of_parse_and_map_error:
	iounmap(myGPIOK_device->vrtl_addr);
ioremap_error:
	release_mem_region(myGPIOK_device->rsrc.start, myGPIOK_device->rsrc_size);
request_mem_region_error:
of_address_to_resource_error:
cdev_add_error:
	device_destroy(myGPIOK_device->class, myGPIOK_device->Mm);
device_create_error:
	cdev_del(&myGPIOK_device->cdev);
	unregister_chrdev_region(myGPIOK_device->Mm, 1);

no_error:
	return error;
}

/**
 * @brief Deinizializza un device, rimuovendo le strutture kernel allocate per il suo funzionamento
 *
 * @param [in] device puntatore a struttura myGPIOK_t, specifica il particolare device su cui agire
 */
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
	device_destroy(device->class, device->Mm);
	cdev_del(&device->cdev);
	unregister_chrdev_region(device->Mm, 1);
}

/**
 * @brief Set del flag "interrupt occurred" (canRead)
 * @param [in] device puntatore a struttura myGPIOK_t, che si riferisce al device su cui operare
 *
 * @details
 * <h5>Setting del valore del flag "interrupt occurred"</h5>
 * Dopo aver disabilitato gli interrupt della periferica, occorre settare in modo appropriato il flag
 * "interrupt occurred", in modo che i processi in attesa possano essere risvegliati in modo sicuro.
 * Per prevenire race condition, tale operazione viene effettuata mutua esclusione.
 * I semafori sono uno strumento potentissimo per per l'implementazione di sezioni critiche, ma non possono
 * essere usati in codice non interrompibile. Gli spilock sono come i semafori, ma possono essere usati
 * anche in codice non interrompibile, come può esserlo un modulo kernel.
 * Sostanzialmente se uno spinlock è già stato acquisito da qualcun altro, si entra in un hot-loop dal
 * quale si esce solo quando chi possiede lo spinlock lo rilascia. Trattandosi di moduli kernel, è di
 * vitale importanza che la sezione critica sia quanto più piccola possibile. Ovviamente l'implementazione
 * è "un pò" più complessa di come è stata descritta, ma il concetto è questo. Gli spinlock sono
 * definiti in <linux/spinlock.h>.
 * Esistono diversi modi per acquisire uno spinlock. Nel seguito viene usata la funzione
 * @code
 * void spin_lock_irqsave(spinlock_t *lock, unsigned long flags);
 * @endcode
 * la quale disabilita gli interrupt sul processore locale prima di acquisire lo spinlock, per poi
 * ripristinarlo quando lo spinlock viene rilasciato, usando
 * @code
 * void spin_unlock_irqrestore(spinlock_t *lock, unsigned long flags);
 * @endcode
 */
void myGPIOK_SetCanRead(myGPIOK_t* device) {
	unsigned long flags;
	spin_lock_irqsave(&device->slock_int, flags);
	device-> can_read = 1;
	spin_unlock_irqrestore(&device->slock_int, flags);
}

/**
 * @brief Reset del flag "interrupt occurred" (canRead)
 *
 * @param [in] device puntatore a struttura myGPIOK_t, che si riferisce al device su cui operare
 *
 * @details
 * <h5>Reset del flag "interrupt occurred" per read() bloccanti</h5>
 * Nel momento in cui il processo viene risvegliato e la condizione della quale era in attesa è tale che
 * esso può continuare la sua esecuzione, è necessario resettare tale flag. Questa operazione va effettuata
 * per prevenire race-condition dovute al risveglio di più processi in attesa del manifestarsi dello stesso
 * evento. Il reset del flag va, pertanto, effettuato in mutua esclusione.
 *
 * I semafori sono uno strumento potentissimo per per l'implementazione di sezioni critiche, ma non possono
 * essere usati in codice non interrompibile. Gli spilock sono come i semafori, ma possono essere usati
 * anche in codice non interrompibile, come può esserlo un modulo kernel.
 * Sostanzialmente se uno spinlock è già stato acquisito da qualcun altro, si entra in un hot-loop dal
 * quale si esce solo quando chi possiede lo spinlock lo rilascia. Trattandosi di moduli kernel, è di
 * vitale importanza che la sezione critica sia quanto più piccola possibile. Ovviamente l'implementazione
 * è "un pò" più complessa di come è stata descritta, ma il concetto è questo. Gli spinlock sono
 * definiti in <linux/spinlock.h>.
 * Esistono diversi modi per acquisire uno spinlock. Nel seguito viene usata la funzione
 * @code
 * void spin_lock(spinlock_t *lock);
 * @endcode
 * per rilasciare uno spinlock, invece, verrà usata
 * @code
 * void spin_unlock(spinlock_t *lock);
 * @endcode
 */
void myGPIOK_ResetCanRead(myGPIOK_t* device) {
	unsigned long flags;
	spin_lock_irqsave(&device->slock_int, flags);
	device-> can_read = 0;
	spin_unlock_irqrestore(&device->slock_int, flags);
}

/**
 * @brief Testa la condizione "interrupt occurred", mettendo in attesa il processo, se necessario
 *
 * @param [in] device puntatore a struttura myGPIOK_t, che si riferisce al device su cui operare
 *
 * @details
 * <h5>Porre un processo nello stato sleeping</h5>
 * Quando un processo viene messo nello stato sleep, lo si fa aspettandosi che una condizione diventi vera in
 * futuro. Al risveglio, però, non c'è nessuna garanzia che quella particolare condizione sia ancora vera,
 * per cui essa va nuovamente testata.
 * Il modo più semplice per potte un processo nello stato sleeping è chiamare la macro wait_event(), o una
 * delle sue varianti: essa combina la gestione della messa in sleeping del processo ed il check della
 * condizione che il processo si aspetta diventi vera.
 * @code
 * wait_event_interruptible(queue, condition);
 * @endcode
 * Il parametro queue è la coda di attesa mentre condition è la condizione che, valutata true, causa la
 * messa in sleep del processo. La condizione viene valutata sia prima che il processo sia messo in sleeping
 * che al suo risveglio. Lo sleep in cui il processo viene messo chiamando wait_event_interruptible() può
 * essere interrotto anche da un segnale, per cui la macro restituisce un intero che, se diverso da zero,
 * indica che il processo è stato risvegliato da un segnale.
 *
 * La condizione sulla quale i processi vengono bloccati riguarda il flag "interrupt occurred". Fin quando
 * questo flag, posto in and con la maschera MYGPIOK_SREAD, è zero, il processo deve restare bloccato, per
 * cui i processi che effettuano read() bloccante restano bloccati finché int_occurred & MYGPIO_SREAD == 0.
 * Quando tale uguaglianza non sarà più valida, perché il valore di int_occurred viene settato dalla
 * funzione myGPIOK_irq_handler(), allora il processo verrà risvegliato.
 */
void myGPIOK_TestCanReadAndSleep(myGPIOK_t* device) {
	wait_event_interruptible(device->read_queue, (device->can_read != 0));
}

/**
 * @brief Verifica che le operazioni di lettura/scrittura risultino non-bloccanti.
 *
 * @param [in] 		device puntatore a struttura myGPIOK_t, che si riferisce al device su cui operare
 * @param [inout]	file
 * @param [inout]	wait
 *
 * @return restituisce una maschera di bit che indica se sia possibile effettuare
 * operazioni di lettura/scrittura non bloccanti, in modo che il kernel possa
 * bloccare il processo e risvegliarlo solo quando tali operazioni diventino possibili.
 *
 * @details
 * Questo metodo è il back-end di tre diverse system-calls: poll, epoll e select,
 * le quali	sono usate per capire se una operazione di lettura/scrittura du un device
 * possano risultare bloccanti o meno.
 */
unsigned myGPIOK_GetPollMask(myGPIOK_t *device, struct file *file_ptr, struct poll_table_struct *wait) {
	unsigned mask;
	poll_wait(file_ptr, &device->poll_queue,  wait);
	spin_lock(&device->slock_int);
	if(device->can_read)
		mask = POLLIN | POLLRDNORM;
	spin_unlock(&device->slock_int);
	return mask;
}

/**
 * @brief Incrementa il contatore degli interrupt per un particolare device
 *
 * @param [in] device puntatore a struttura myGPIOK_t, che si riferisce al device su cui operare
 *
 * @details
 * <h5>Incremento del numero totale di interrupt</h5>
 * Dopo aver settato il flag, viene incrementato il valore degli interrupt totali.
 * Anche questa operazione viene effettuata in mutua esclusione.
 */
void myGPIOK_IncrementTotal(myGPIOK_t* device) {
	unsigned long flags;
	spin_lock_irqsave(&device->sl_total_irq, flags);
	device->total_irq++;
	spin_unlock_irqrestore(&device->sl_total_irq, flags);
}

/**
 * @brief Risveglia i process in attesa sulle code di read e poll.
 *
 * @param [in] device puntatore a struttura myGPIOK_t, che si riferisce al device su cui operare
 *
 * @details
 * <h5>Wakeup dei processi sleeping</h5>
 * La ISR deve chiamare esplicitamente wakeup() per risvegliare i processi messi in sleeping in attesa che
 * un particolare evento si manifestasse. La funzione
 * @code
 * void wake_up_interruptible(wait_queue_head_t *queue);
 * @endcode
 * risveglia tutti i processi posti in una determinata coda (risvegliando solo quelli che, in precedenza, hanno
 * effettuato una chiamata a wait_event_interruptible()).
 * Se due processi vengono risvegliati contemporaneamente potrebbero originarsi race-condition.
 */
void myGPIOK_WakeUp(myGPIOK_t* device) {
	wake_up_interruptible(&device->read_queue);
	wake_up_interruptible(&device->poll_queue);
}

/**
 * @brief Restituisce l'indirizzo virtuale di memoria cui è mappato un device
 *
 * @param [in] device puntatore a struttura myGPIOK_t, che si riferisce al device su cui operare
 */
void* myGPIOK_GetDeviceAddress(myGPIOK_t* device) {
	return device->vrtl_addr;
}

/**
 * @brief Abilita gli interrupt globali;
 *
 * @param [in] device puntatore a struttura myGPIOK_t, che si riferisce al device su cui operare
 */
void myGPIOK_GlobalInterruptEnable(myGPIOK_t* device) {
	iowrite32(1, (device->vrtl_addr + myGPIOK_GIES_OFFSET));
}

/**
 * @brief Disabilita gli interrupt globali;
 *
 * @param [in] device puntatore a struttura myGPIOK_t, che si riferisce al device su cui operare
 */
void myGPIOK_GlobalInterruptDisable(myGPIOK_t* device) {
	iowrite32(0, (device->vrtl_addr + myGPIOK_GIES_OFFSET));
}

/**
 * @brief Abilita gli interrupt per i singoli pin del device.
 *
 * @param [in] device puntatore a struttura myGPIOK_t, che si riferisce al device su cui operare
 * @param [in] mask maschera di selezione degli interrupt da abilitare; quelli non selezionati non vengono abilitati;
 */
void myGPIOK_PinInterruptEnable(myGPIOK_t* device, unsigned mask) {
	unsigned reg_value = ioread32((device->vrtl_addr + myGPIOK_PIE_OFFSET));
	reg_value |= mask;
	iowrite32(reg_value, (device->vrtl_addr + myGPIOK_PIE_OFFSET));
}

/**
 * @brief Disabilita gli interrupt per i singoli pin del device.
 *
 * @param [in] device puntatore a struttura myGPIOK_t, che si riferisce al device su cui operare
 *
 * @param [in] mask maschera di selezione degli interrupt da disabilitare; quelli non selezionati non vengono disabilitati;
 */
void myGPIOK_PinInterruptDisable(myGPIOK_t* device, unsigned mask) {
	unsigned reg_value = ioread32((device->vrtl_addr + myGPIOK_PIE_OFFSET));
	reg_value &= ~mask;
	iowrite32(reg_value, (device->vrtl_addr + myGPIOK_PIE_OFFSET));
}

/**
 * @brief Consente di ottenere una maschera che indichi quali interrupt non siano stati ancora serviti;
 *
 * @param [in] device puntatore a struttura myGPIOK_t, che si riferisce al device su cui operare
 *
 * @return maschera che riporta i pin per i quali gli interrupt non sono stati ancora serviti;
 */
unsigned myGPIOK_PendingPinInterrupt(myGPIOK_t* device) {
	return ioread32((device->vrtl_addr + myGPIOK_IRQ_OFFSET));
}

/**
 * @brief Invia al device notifica di servizio di un interrupt;
 *
 * @param [in] device puntatore a struttura myGPIOK_t, che si riferisce al device su cui operare
 *
 * @param [in] mask mask maschera di selezione degli interrupt da notificare; quelli non selezionati non vengono notificati;
 */
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
