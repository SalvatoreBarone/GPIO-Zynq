/**
 * @file myGPIOK.c
 * @author Salvatore Barone <salvator.barone@gmail.com>
 * @date 16 06 2017
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
 * @addtogroup Kernel-Driver
 * @{
 * @defgroup Kernel-Module
 * @{
 *
 * @brief Device-driver in kernel-mode per myGPIO
 *
 * <h2>Platform-device</h2>
 * I device driver, anche se sono moduli kernel, non si scrivono come normali moduli Kernel.
 *
 * I "platform-device" sono device che non possono annunciarsi al software (non possono dire "Hey,
 * sono qui'!" al sistema operativo), quindi sono intrinsecamente "non-scopribili", nel senso che
 * il sistema, al boot, deve sapere che ci sono, ma non e' in grado di scoprirli. A differenza dei
 * device PCI o USB, che non sono platform-device, un device I²C non viene enumerato a livello
 * hardware, per cui e' necessario che il sistema operativo sappia, a tempo di "compilazione", cioe'
 * prima del boot - quale device sia connesso al bus I²C.
 *
 * I non-discoverable devices stanno proliferando molto velocemente nel mondo embedded, per cui il
 * Kernel Linux offre ancora la possibilita' di specificare quale hardware sia presente nel sistema.
 * Bisogna distinguere in:
 *  - Platform Driver
 *  - Platform Device
 *
 * Per quanto riguarda la parte driver, il kernel Linux kernel definisce un insieme di operazioni
 * standard che possono essere effettuate su un platform-device.
 * Un riferimento pue' essere http://lxr.free-electrons.com/source/include/linux/platform_device.h#L173.
 *
 * Le callbacks probe() e remove() costituiscono l'insieme minimo di operazioni che devono essere
 * implementate. Tali funzioni devono avere gli stessi parametri delle due seguenti, ma possono avere
 * nome qualsiasi.
 *
 * @code
 * 		static int sample_drv_probe(struct platform_device *pdev) {
 * 				//Empty Probe function.
 * 		}
 *
 * 		static int sample_drv_remove(struct platform_device *pdev) {
 * 				//Empty remove function.
 * 		}
 * @endcode
 *
 * La definizione di quali funzioni probe() e remove() chiamare quando viene caricato un driver viene
 * effettuato attraverso la seguente struttura e la chiamata alla macro module_platform_driver(), la
 * quale prende in input la struttura seguente ed implementa, al posto nostro, le funzioni module_init()
 * e module_close() standard, chiamate quando il modulo viene caricato o rimosso dal kernel.
 *
 * @code
 * 		static struct platform_driver sample_pldriver = {
 * 			.probe  = sample_drv_probe,
 * 			.remove = sample_drv_remove,
 * 			.driver = {
 *					.name  = DRIVER_NAME,
 *				},
 * 		};
 *
 * 		module_platform_driver(sample_pldriver);
 * @endcode
 *
 * Si noti DRIVER_NAME: deve essere identica alla stringa indicata, nel device-tree, al campo "compatible".
 *
 * Affinche' il driver possa essere caricato a caldo, e' necessario aggiungere alla struttura di cui sopra
 * qualche informazione in piu'.
 * Tutti i device-driver devono esporre un ID. A tempo di compilazione, il processo di build estrae queste
 * informazioni dai driver per la preparazione di una tabella. Quando si "inserisce" il device, la tabella
 * viene riferita dal kernel e, se viene trovata una entry corrispondente al driver per quel device, il
 * driver viene caricato ed inizializzato.
 *
 * Usando la struttura
 *
 * @code
 * 		static struct of_device_id device_match[] = {
 * 				{.compatible = DRIVER_NAME},
 * 				{},
 * 		};
 *
 * 		MODULE_DEVICE_TABLE(of, device_match);
 * @endcode
 *
 * si identifica un particolare device. La macro MODULE_DEVICE_TABLE() viene usata per inserire una nuova
 * entry nella tabella accennata precedentemente. Alla struttura platform_driver possono essere aggiunte
 * anche queste informazioni, per cui essa si presentera' come riportato di seguito.
 *
 * @code
 * 		static struct platform_driver sample_pldriver = {
 * 			.probe  = sample_drv_probe,
 * 			.remove = sample_drv_remove,
 * 			.driver = {
 *					.name  = DRIVER_NAME,
 *					.owner = THIS_MODULE,
 *					.of_match_table = device_match,
 *   			},
 * 		};
 * @endcode
 *
 */


#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/fcntl.h>
#include <linux/unistd.h>

#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/irqreturn.h>
#include <linux/spinlock.h>

#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#include <linux/sched.h>
#include <linux/poll.h>

/*
 * Funzioni implementate dal modulo
 */
static int 			myGPIOK_probe			(struct platform_device *op);
static int 			myGPIOK_remove			(struct platform_device *op);

static int 			myGPIOK_open			(struct inode *inode, struct file *file_ptr);
static int 			myGPIOK_release			(struct inode *inode, struct file *file_ptr);

static loff_t		myGPIOK_llseek			(struct file *file_ptr, loff_t off, int whence);
static unsigned int myGPIOK_poll			(struct file *file_ptr, struct poll_table_struct *wait);
static ssize_t 		myGPIOK_read			(struct file *file_ptr, char *buf, size_t count, loff_t *ppos);
static ssize_t 		myGPIOK_write 			(struct file *file_ptr, const char *buf, size_t size, loff_t *off);

static irqreturn_t	myGPIOK_irq_handler		(int irq, struct pt_regs * regs);



/**
 * @brief Stuttura per l'astrazione di un device
 *
 * E' buona abitudine, se non quasi indispensabile, definire una struttura dati nella quale contenere tutto
 * cio' che e' legato al device o al driver. In questo modulo viene usata la struttura myGPIOK_t per contenere
 * tutto cio' che e' necessario al funzionamento del driver.
 *
 */
typedef struct {
	struct cdev cdev;	/**<	Stuttura per l'astrazione di un device a caratteri,
	 	 	 	 	 	 	 	Il kernel usa, internamente, una struttura cdev per rappresentare i device a
	 	 	 	 	 	 	 	caratteri. Prima che il kernel invochi le funzioni definite dal driver per il
	 	 	 	 	 	 	 	device, bisogna allocare e registrare uno, o piu', oggetti cdev.
	 	 	 	 	 	 	 	In questo caso e' sufficiente allocare uno solo di questi oggetti.*/

	uint32_t irqNumber; /**< 	interrupt-number a cui il device e' connesso. Restituito salla chiamata alla
								funzione irq_of_parse_and_map() */

	struct resource rsrc; /**<	Struttura che astrae una risorsa device, dal punto di vista della memoria alla
	 	 	 	 	 	 	 	quale la risorsa e' mappata. In particolare i campi "start" ed "end" contendono,
	 	 	 	 	 	 	 	rispettivamente, il primo e l'ultimo indirizzo fisico a cui il device e'
	 	 	 	 	 	 	 	mappato. */

	struct resource *mreg; /**<	puntatre alla regione di memoria cui il device e' mapapto */

	uint32_t rsrc_size; /**< 	rsrc.end - rsrc.start
	 	 	 	 	 	 	 	numero di indirizzi associati alla periferica.
	 	 	 	 	 	 	 	occorre per effettuare il mapping indirizzo fisico - indirizzo virtuale */

	uint32_t* vrtl_addr; /**<	indirizzo virtuale della periferica */


} myGPIOK_t;

/**
 * @brief Nome identificativo del device-driver.
 * DEVE corrispondere al valore del campo "compatible" nel device tree source.
 */
#define DRIVER_NAME "myGPIOK"

/**
 * @brief Puntatore a struttura myGPIOK_t, contenente tutti i dati necessari al device driver.
 */
myGPIOK_t *myGPIOK_dev_ptr = NULL;

/**
 * @brief Major e minor number per il device driver
 */
static dev_t myGPIOK_Mm_number;

/**
 * @brief Numero di minor-number richiesti da driver
 */
#define REQUIRED_MINOR_NUMBER 1

/**
 * @brief
 */
static struct class*  myGPIOK_class  = NULL;

/**
 * @brief
 */
static struct device* myGPIOK_device = NULL;



/**
 * @brief Identifica il device all'interno del device tree
 *
 * Tutti i device-driver devono esporre un ID. A tempo di compilazione, il processo di build estrae queste
 * informazioni dai driver per la preparazione di una tabella. Quando si "inserisce" il device, la tabella
 * viene riferita dal kernel e, se viene trovata una entry corrispondente al driver per quel device, il
 * driver viene caricato ed inizializzato.
 */
static struct of_device_id myGPIOk_match[] = {
		{.compatible = DRIVER_NAME},
		{},
};

/**
 * @brief Definisce quali funzioni probe() e remove() chiamare quando viene caricato un driver.
 */
static struct platform_driver myGPIOK_driver = {
		.probe = myGPIOK_probe,
		.remove = myGPIOK_remove,
		.driver = {
				.name = DRIVER_NAME,
				.owner = THIS_MODULE,
				.of_match_table = myGPIOk_match,
		},
};

/**
 * @brief mantiene puntatori a funzioni che definiscono il gli operatori che agiscono su un file/device.
 *
 * Essendo un device "visto" come un file, ogni device driver deve implementare tutte le
 * system-call previste per l'interfacciamento con un file. La corrispondenza tra la
 * system-call e la funzione fornita dal driver viene stabilita attraverso tale struttura.
 * La struttura dati file_operations, definita in <linux/fs.h> mantiene puntatori a funzioni definite dal
 * driver che consentono di definire il comportamento degli operatori che agiscono su un file.
 */
static struct file_operations myGPIO_fops = {
		.owner		= THIS_MODULE,
		.llseek		= myGPIOK_llseek,
		.read		= myGPIOK_read,
		.write		= myGPIOK_write,
		.poll		= myGPIOK_poll,
		.open		= myGPIOK_open,
		.release	= myGPIOK_release
};

/*================================ Implementazione delle funzioni del modulo ===================================*/

/**
 * @brief Viene chiamata quando il modulo viene inserito.
 *
 * Inizializza il driver e la periferica.
 * Alloca un oggetto myGPIOK_t, che astrae un device myGPIO, e lo inizializza, associandogli
 * gli operatori che permetteranno di interagire con tale device da user-space.
 *
 * <h5>Inizializzazione del driver</h5>
 *
 * Il device GPIO viene gestito come un character-device, ossia un device su cui e' possibile leggere e/o
 * scrivere byte. Il kernel usa, internamente, una struttura cdev per rappresentare i device a caratteri. Prima
 * che il kernel invochi le funzioni definite dal driver per il device, bisogna allocare e registrare uno, o
 * piu', oggetti cdev. Per farlo e' necessario includere <linux/cdev.h>, che definisce tale struttura e le
 * relative funzioni.
 *
 * <h5>Major-number e Minor-number</h5>
 * Ai device drivers sono associati un major-number ed un minor-number. Il major-number viene usato dal kernel
 * per identificare il driver corretto corrispondente ad uno specifico device, quando si effettuano operazioni
 * su di esso. Il ruolo del minor number dipende dal device e viene gestito internamente dal driver.
 * Il driver scritto per GPIO non usera' minor-number.
 * La registrazione di un device driver puo' essere effettuata chiamando <b>alloc_chrdev_region()</b>, la quale
 * alloca un char-device numbers. Il major number viene scelto dinamicamente e restituito dalla funzione
 * attraverso il parametro dev. La funzione restituisce un valore negativo nel caso in cui si verifichino errori,
 * 0 altrimenti.
 * @code
 * int alloc_chrdev_region (dev_t * dev, unsigned baseminor, unsigned count, const char *name);
 * @endcode
 *  - dev: major e minor number
 *  - baseminor: primo dei minor number richiesti
 *  - count: numero di minornumber richiesti
 *  - name: nome del device driver
 *
 * <h5>Device Class</h5>
 * Ai device-drivers viene associata una classe ed un device-name.
 * Per creare ed associare una classe ad un device driver si puo' usare la seguente.
 * @code
 * struct class * class_create(struct module * owner, const char * name);
 * @endcode
 *  - owner: puntatore al modulo che "possiede" la classe, THIS_MODULE
 *  - name: puntatore alla stringa identificativa (il nome) del device driver, DRIVER_NAME
 *
 * <h5>Operatori</h5>
 * Essendo un device "visto" come un file, ogni device driver deve implementare tutte le
 * system-call previste per l'interfacciamento con un file. La corrispondenza tra la
 * system-call e la funzione fornita dal driver viene stabilita attraverso la struttura
 * file_operations.
 * La struttura dati file_operations, definita in <linux/fs.h> mantiene puntatori a funzioni definite dal
 * driver che consentono di definire il comportamento degli operatori che agiscono su un file.
 *
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
 *
 * Ogni campo della struttura deve puntare ad una funzione del driver che implementa uno
 * specifico "operatore" su file, oppure impostata a NULL se l'operatore non e' supportato.
 * L'esatto comportamento del kernel, quando uno dei puntatori e' NULL, varia da funzione
 * a funzione.
 * La lista seguente introduce tutti gli operatori che un'applicazione puo' invocare
 * su un device. La lista e' stata mantenuta snella, includendo solo i campi strettamente
 * necessari.
 *
 * - <i>struct module *owner</i> :<br>
 * 		il primo campo della struttura non e' un operatore, ma un puntatore al modulo che
 * 		"possiede" la struttura. Il campo ha lo scopo di evitare che il modulo venga rimosso
 * 		dal kernel quando uno degli operatori e' in uso. Viene inizializzato usando la macro
 * 		THIS_MODULE, definita in <linux/module.h>.
 *
 * - <i>loff_t (*llseek) (struct file *, loff_t, int)</i> :
 * 		il campo llseek e' usato per cambiare la posizione della "testina" di lettura/
 * 		scrittura in un file. La funzione restituisce la nuova posizione della testina.
 * 		loff_t e' un intero a 64 bit (anche su architetture a 32 bit). Eventuali errori
 * 		vengono segnalati con un valore di ritorno negativo. Se questo campo e' posto a
 * 		NULL, eventuali chiamate a seek modifigheranno la posizione della testina in un
 * 		modo impredicibile.
 *
 * - <i>ssize_t (*read) (struct file *, char _ _user *, size_t, loff_t *)</i> :<br>
 * 		usata per leggere dati dal device. Se lasciato a NULL, ogni chiamata a read fallira'
 * 		e non sara' possibile leggere dal device. La funzione restituisce il numero di byte
 * 		letti con successo ma, nel caso si verifichi un errore, restituisce un numero intero
 * 		negativo.
 *
 * - <i>ssize_t (*write) (struct file *, const char _ _user *, size_t, loff_t *)</i> :<br>
 * 		invia dati al device. Se NULL ogni chiamata alla system-call write causera' un errore.
 * 		Il valore di ritorno, se non negativo, rappresenta il numero di byte correttamente
 * 		scritti.
 *
 * - <i>unsigned int (*poll) (struct file *, struct poll_table_struct *)</i> :<br>
 * 		questo metodo e' il back-end di tre diverse system-calls: poll, epoll e select, le quali
 * 		sono usate per capire se una operazione di lettura/scrittura du un device possano
 * 		risultare bloccanti o meno. La funzione dovrebbe restituire una maschera che indichi
 * 		se sia possibile effettuare operazioni di lettura/scrittura non bloccanti, in modo che
 * 		il kernel possa bloccare il processo e risvegliarlo solo quando tali operazioni diventino
 * 		possibili. Se viene lasciata NULL si intende che le operazioni di lettura/scrittura sul
 * 		device siano sempre non-bloccanti.
 *
 * - <i>int (*open) (struct inode *, struct file *)</i> :<br>
 * 		Anche se, di solito, e' la prima operazione che si effettua su un file, non e' strettamente
 * 		necessaria la sua implementazione. Se lasciata NULL, l'apertura del device andra' comunque
 * 		a buon fine, ma al driver non verra' inviata alcuna notifica.
 *
 * - <i>int (*release) (struct inode *, struct file *)</i> :<br>
 * 		questo operatore viene invocato quando il file viene rilasciato. Come open, puo' essere
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
 *
 * <h5>Aggiunta del device</h5>
 * Il driver, a questo punto, e' pronto per essere aggiunto. E' possibile aggiungere il driver usando
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
 * Il passo successivo e' la registrazione del device e la sua aggiunta al filesystem. Tale operazione
 * puo' essere effettuata chiamando
 * @code
 * struct device * device_create( struct class *class, struct device *parent, dev_t devt, const char *fmt, ...)
 * @endcode
 *  - class: puntatore alla struttura class alla quale il device deve essere registrato
 *  - parent: puntatore ad eventuale device parent
 *  - devt: tmajor number
 *  - fmt: nome del device.
 *
 * La funzione pu' essere usata solo sulla classe dei device a caratteri. Crea un device all'interno del
 * filesystem, associandogli il major number preventivamente inizializzato. La funzione restituisce il puntatore
 * alla struttura device creata all'interno del filesystem. Si noti che il puntatre alla struttura classes DEVE
 * essere stato precedentemente creato attraverso una chiamata alla funzione <i>class_create()</i>.
 *
 *
 * <h5>Accedere al segmento di memoria a cui la periferica e' mappata</h5>
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
 *
 * Le regioni di memoria per di I/O vanno allocate prima di poter essere usate.
 *
 * @code
 * struct resource *request_mem_region(unsigned long start, unsigned long len, char *name);
 * @endcode
 *
 * Questa funzione alloca una regione di memoria di len byte a partire da start restituendone l'indirizzo,
 * mentre nel caso in cui si verifichi un errore viene restituito NULL. La funzione viene chiamata per ottenere
 * l'accesso esclusivo della regione di memoria, per evitare che driver diversi tentino di accedere allo stesso
 * spazio di memoria.
 *
 * L'allocazione dello spazio di memoria non e' l'unico step da eseguire prima che tale memoria possa essere
 * usata. E' necessario fare in modo che sia resa accessibile al kernel attraverso un mapping, usando la
 * funzione.
 *
 * @code
 * void *ioremap(unsigned long phys_addr, unsigned long size);
 * @endcode
 *
 *
 *  * <h5>Interrupts</h5>
 * The LKM driver must register a handler function for the interrupt, which defines the actions that the interrupt
 * should perform.
 * L'handler deve essere compatibile con il tipo puntatore a funzione irq_handler_t, così definito.
 * @code
 * struct irqreturn_t (*irq_handler_t)(int irq, struct pt_regs * regs);
 * @endcode
 * Il modulo definisce la funzione myGPIOK_irq_handler(). L'handler puo' essere registrato usando
 *
 * @code
 * int request_irq(	unsigned int irqNumber,
 * 					irqreturn_t (*handler)(int, void *, struct pt_regs *),
 * 					unsigned long irqflags,
 * 					const char *devname,
 * 					void *dev_id);
 * @endcode
 *
 * IL parametro irqNumber puo' essere determinato automaticamente usando la funzione
 *
 * @code
 * unsigned int irq_of_parse_and_map(struct device_node *node, int index);
 * @endcode
 *
 * La funzione irq_of_parse_and_map() effettua un looks-up nella specifica degli interrupt all'interno del
 * device tree e restituisce un irq number cosi' come de lo aspetta request_irq() (cioe' compaci con
 * l'enumerazione in /proc/interrupts). Il secondo argomento della funzione e', tipicamente, zero, ad
 * indicare che, all'interno del device tree, verra' preso in considerazione soltanto il primo degli
 * interrupt specificate.
 * Il device tree, nella sezione dedicata al gpio,reca
 * @code
 * interrupts = <0 29 4>;
 * @endcode
 *
 * Il primo numero (0) e' un flag che indica se l'interrupt sia connesso ad una line SPI (shared peripheral
 * interrupt). Un valore diverso da zero indica che la linea e' SPI.
 *
 * Il secondo numero si riferisce all'interrupt number. Per farla breve, quando si definisce la parte hardware,
 * in questo specifico esempio il device GPIO e' connesso alla linea 61 del GIC. Sottraendo 32 si orriene 29.
 *
 * Il terzo numero si riferisce alla tipologia dell'interrupt. Sono possibili tre valori:
 *  - 0 : power-up default
 *  - 1 : rising-edge
 *  - 4 : a livelli, active alto
 *
 *
 *
 * @retval 0 nel caso in cui non si sia verificato nessun errore;
 * @retval -ENOMEM nel caso in cui non sia possibile allocare memoria;
 * @retval <0 per altri errori
 */
static int myGPIOK_probe(struct platform_device *op) {
	int error = 0;
	struct device *dev;

	printk(KERN_INFO "Chiamata %s\n", __func__);

	/* Allocazione dell'oggetto myGPIOK_t */
	if ((myGPIOK_dev_ptr = kmalloc(sizeof(myGPIOK_t), GFP_KERNEL)) == NULL) {
		printk(KERN_ERR "%s: kmalloc ha restituito NULL\n", __func__);
		return -ENOMEM;
	}

	/* Allocazione del device a caratteri */
	if ((error = alloc_chrdev_region(&myGPIOK_Mm_number, 0 , REQUIRED_MINOR_NUMBER, DRIVER_NAME)) != 0) {
		printk(KERN_ERR "%s: alloc_chrdev_region() ha restituito %d\n", __func__, error);
		kfree(myGPIOK_dev_ptr);
		return error;
	}

	/* Creazione della classe */
	if ((myGPIOK_class = class_create(THIS_MODULE, DRIVER_NAME) ) == NULL) {
		printk(KERN_ERR "%s: class_create() ha restituito NULL\n", __func__);
		unregister_chrdev_region(myGPIOK_Mm_number, REQUIRED_MINOR_NUMBER);
		kfree(myGPIOK_dev_ptr);
		return -ENOMEM;
	}

	/* Inizializzazione del device a caratteri */
	cdev_init (&(myGPIOK_dev_ptr->cdev), &myGPIO_fops);
	myGPIOK_dev_ptr->cdev.owner = THIS_MODULE;

	/* Aggiunta del device a caratteri al filesystem */
	if ((error = cdev_add(&myGPIOK_dev_ptr->cdev, myGPIOK_Mm_number, REQUIRED_MINOR_NUMBER)) != 0) {
		printk(KERN_ERR "%s: cdev_add() ha restituito %d\n", __func__, error);
		class_destroy(myGPIOK_class);
		unregister_chrdev_region(myGPIOK_Mm_number, REQUIRED_MINOR_NUMBER);
		kfree(myGPIOK_dev_ptr);
		return error;
	}

	/* Creazione del device nel filesystem */
	if ((myGPIOK_device = device_create(myGPIOK_class, NULL, myGPIOK_Mm_number, NULL, DRIVER_NAME)) == NULL) {
		printk(KERN_ERR "%s: device_create() ha restituito NULL\n", __func__);
		cdev_del(&myGPIOK_dev_ptr->cdev);
		class_destroy(myGPIOK_class);
		unregister_chrdev_region(myGPIOK_Mm_number, REQUIRED_MINOR_NUMBER);
		kfree(myGPIOK_dev_ptr);
		return -ENOMEM;
	}

	dev = &op->dev;

	/* costruzione del mapping in memoria della periferica */
	if ((error = of_address_to_resource(dev->of_node, 0, &myGPIOK_dev_ptr->rsrc)) != 0) {
		printk(KERN_ERR "%s: request_irq() ha restituito %d\n", __func__, error);
		device_destroy(myGPIOK_class, myGPIOK_Mm_number);
		cdev_del(&myGPIOK_dev_ptr->cdev);
		class_destroy(myGPIOK_class);
		unregister_chrdev_region(myGPIOK_Mm_number, REQUIRED_MINOR_NUMBER);
		kfree(myGPIOK_dev_ptr);
		return error;
	}
	myGPIOK_dev_ptr->rsrc_size = myGPIOK_dev_ptr->rsrc.end - myGPIOK_dev_ptr->rsrc.start + 1;

	/* richiesta di accesso esclusivo all'area di memoria a cui il device e' mappato */
	if ((myGPIOK_dev_ptr->mreg = request_mem_region(myGPIOK_dev_ptr->rsrc.start, myGPIOK_dev_ptr->rsrc_size, DRIVER_NAME)) == NULL) {
		printk(KERN_ERR "%s: request_mem_region() ha restituito NULL\n", __func__);
		device_destroy(myGPIOK_class, myGPIOK_Mm_number);
		cdev_del(&myGPIOK_dev_ptr->cdev);
		class_destroy(myGPIOK_class);
		unregister_chrdev_region(myGPIOK_Mm_number, REQUIRED_MINOR_NUMBER);
		kfree(myGPIOK_dev_ptr);
		return -ENOMEM;
	}

	/* remapping */
	if ((myGPIOK_dev_ptr->vrtl_addr = ioremap(myGPIOK_dev_ptr->rsrc.start, myGPIOK_dev_ptr->rsrc_size))==NULL) {
		printk(KERN_ERR "%s: ioremap() ha restituito NULL\n", __func__);
		release_mem_region(myGPIOK_dev_ptr->rsrc.start, myGPIOK_dev_ptr->rsrc_size);
		device_destroy(myGPIOK_class, myGPIOK_Mm_number);
		cdev_del(&myGPIOK_dev_ptr->cdev);
		class_destroy(myGPIOK_class);
		unregister_chrdev_region(myGPIOK_Mm_number, REQUIRED_MINOR_NUMBER);
		kfree(myGPIOK_dev_ptr);
		return -ENOMEM;
	}

	/* registrazione dell'interrupt handler */
	myGPIOK_dev_ptr->irqNumber = irq_of_parse_and_map(dev->of_node, 0);
	if ((error = request_irq(myGPIOK_dev_ptr->irqNumber ,(irq_handler_t) myGPIOK_irq_handler, 0, DRIVER_NAME, NULL)) != 0) {
		printk(KERN_ERR "%s: request_irq() ha restituito %d\n", __func__, error);
		iounmap(myGPIOK_dev_ptr->vrtl_addr);
		release_mem_region(myGPIOK_dev_ptr->rsrc.start, myGPIOK_dev_ptr->rsrc_size);
		device_destroy(myGPIOK_class, myGPIOK_Mm_number);
		cdev_del(&myGPIOK_dev_ptr->cdev);
		class_destroy(myGPIOK_class);
		unregister_chrdev_region(myGPIOK_Mm_number, REQUIRED_MINOR_NUMBER);
		kfree(myGPIOK_dev_ptr);
		return error;
	}



	return error;
}

/**
 * @breif Viene chiamata automaticamente alla rimozione del mosulo.
 *
 * Dealloca tutta la memoria utilizzata dal driver, de-inizializzando il device.
 */
static int myGPIOK_remove(struct platform_device *op) {
	printk(KERN_INFO "Chiamata %s\n", __func__);

	free_irq(myGPIOK_dev_ptr->irqNumber, NULL);
	iounmap(myGPIOK_dev_ptr->vrtl_addr);
	release_mem_region(myGPIOK_dev_ptr->rsrc.start, myGPIOK_dev_ptr->rsrc_size);
	device_destroy(myGPIOK_class, myGPIOK_Mm_number);
	cdev_del(&myGPIOK_dev_ptr->cdev);
	class_destroy(myGPIOK_class);
	unregister_chrdev_region(myGPIOK_Mm_number, REQUIRED_MINOR_NUMBER);
	kfree(myGPIOK_dev_ptr);

	return 0;
}

/**
 * @brief Invocata all'apertura del file corrispondente al device.
 *
 * <h3>Il metodo open()</h3>
 * Il metodo open di un device driver viene fornito per effettuare ogni inizializzazione necessaria ad
 * operazioni successive. Effettua le seguenti operazioni:
 *  - verifica che non si siano manifestati errori;
 *  - inizializza il device
 *  - aggiorna il puntatore f_op, se necessario;
 *  - alloca e popola ogni struttura dati necessaria, ponendola successivamente nel campo private_data
 *    della struttura dati file.
 *
 * In primo luogo e' necessario identificare il device che sta per essere aperto. Tenendo presente che
 * il prototipo di qualunque metodo open e'
 *
 * @code
 * int (*open)(struct inode *inode, struct file *filp);
 * @endcode
 *
 * il parametro inode contiene tutte le informazioni necessarie all'interno del campo i_cdev, il quale
 * contiene la struttura cdev inizializzata precedentemente dalla funzione di probe(). Il problema e'
 * che non abbiamo bisogno della sola struttura cdev, ma della struttura che la contiene, in questo
 * caso della struttura myGPIOK_t.
 * Fortunatamente i programmatori del kernel hanno reso la vita semplice agli altri, predisponendo la
 * macro container_if() definita in <linux/kernel.h>.
 *
 * @code
 * container_of(pointer, container_type, container_field);
 * @endcode
 *
 * La macro prende in ingresso un puntatore ad un campo di tipo container_field, di una struttura
 * container_type, restituendo il puntatore alla struttura che la contiene.
 *
 * @param inode
 * @param file
 *
 * @return
 */
static int myGPIOK_open(struct inode *inode, struct file *file_ptr) {
	myGPIOK_t *myGPIOK_dev_ptr;

	printk(KERN_INFO "Chiamata %s\n", __func__);

	myGPIOK_dev_ptr = container_of(inode->i_cdev, myGPIOK_t, cdev);
	file_ptr->private_data = myGPIOK_dev_ptr;

	return 0;
}

/**
 * @brief Invocata alla chiusura del file corrispondente al device.
 *
 *
 *
 * @param inode
 * @param file
 *
 * @return
 */
static int myGPIOK_release(struct inode *inode, struct file *file_ptr) {
	printk(KERN_INFO "Chiamata %s\n", __func__);

	return 0;
}

/**
 * @brief Implementa le system-call lseek() e llseek().
 *
 * @warning L'implementazione di read() e write() non sposta la testina di lettura/scrittura!
 *
 * @param file_ptr
 * @param off
 * @param whence
 * @return
 */
static loff_t myGPIOK_llseek (struct file *file_ptr, loff_t off, int whence) {

	myGPIOK_t *myGPIOK_dev_ptr;
    loff_t newpos;

	printk(KERN_INFO "Chiamata %s\n", __func__);

	myGPIOK_dev_ptr = file_ptr->private_data;

    switch(whence) {
      case 0: /* SEEK_SET */
        newpos = off;
        break;

      case 1: /* SEEK_CUR */
        newpos = file_ptr->f_pos + off;
        break;

      case 2: /* SEEK_END */
        newpos = myGPIOK_dev_ptr->rsrc_size + off;
        break;

      default: /* can't happen */
        return -EINVAL;
    }
    if (newpos < 0)
    	return -EINVAL;

    file_ptr->f_pos = newpos;

    return newpos;
}

/**
 * @brief Verifica che le operazioni di lettura/scrittura risultino non-bloccanti.
 *
 * Questo metodo e' il back-end di tre diverse system-calls: poll, epoll e select,
 * le quali	sono usate per capire se una operazione di lettura/scrittura du un device
 * possano risultare bloccanti o meno.
 *
 * @param file
 * @param wait
 *
 * @return restituisce una maschera di bit che indica se sia possibile effettuare
 * operazioni di lettura/scrittura non bloccanti, in modo che il kernel possa
 * bloccare il processo e risvegliarlo solo quando tali operazioni diventino possibili.
 */
static unsigned int myGPIOK_poll (struct file *file_ptr, struct poll_table_struct *wait) {
	printk(KERN_INFO "Chiamata %s\n", __func__);
	return 0;
}

/**
 *
 * @param irq
 * @param regs
 * @return
 */
static irqreturn_t myGPIOK_irq_handler(int irq, struct pt_regs * regs) {
	irqreturn_t irqreturn;
	printk(KERN_INFO "Chiamata %s\n", __func__);
	return  irqreturn;
}

/**
 * @brief Legge dati dal device.
 *
 * <h3>Il metodo read()</h3>
 *
 * <h5>Accesso ai registri del device</h5>
 * Si potrebbe senrire la tentazione di usare il puntatore restituito da ioremap() dereferenziandolo per
 * accedere alla memoria. Questo modo di procedere non e' portabile ed e' prono ad errori. Il modo corretto
 * di accedere alla memoria e' attraverso l'uso delle funzioni per il memory-mapped I/O, definite in <asm/io.h>.
 *
 * Per leggere dalla memoria vengono usate le seguenti:
 *
 * @code
 * unsigned int ioread8(void *addr);
 * unsigned int ioread16(void *addr);
 * unsigned int ioread32(void *addr);
 * @endcode
 *
 * addr e' l'indirizzo di memoria virtuale del device, ottenuto mediante chiamata a ioremap(), a cui viene,
 * eventualmente, aggiunto un offset. Il valore restituito dalle funzioni e' quello letto dalla particolare
 * locazione di memoria a cui viene effettuato accesso.
 *
 * Esiste un insieme di funzioni simili che, invece, scrivono dulla memoria:
 *
 * @code
 * void iowrite8(u8 value, void *addr);
 * void iowrite16(u16 value, void *addr);
 * void iowrite32(u32 value, void *addr);
 * @endcode
 *
 * @param file
 * @param buf
 * @param count
 * @param ppos
 *
 * @return La funzione restituisce il numero di byte letti con successo ma, nel caso si
 * verifichi un errore, restituisce un numero intero negativo.
 */
static ssize_t myGPIOK_read (struct file *file_ptr, char *buf, size_t count, loff_t *ppos) {
	printk(KERN_INFO "Chiamata %s\n", __func__);
	return 0;
}

/**
 * @brief Invia dati al device
 *
 * <h3>Operazioni di lettura e scrittura</h3>
 * The read and write methods both perform a similar task, that is, copying data from and to application code.
 * Therefore, their prototypes are pretty similar, and it's worth introducing them at the same time:
 *
 * @code
 * ssize_t read(struct file *filp, char __user *buff, size_t count, loff_t *offp);
 *
 * ssize_t write(struct file *filp, const char __user *buff, size_t count, loff_t *offp);
 * @endcode
 *
 * For both methods, filp is the file pointer and count is the size of the requested data transfer. The buff
 * argument points to the user buffer holding the data to be written or the empty buffer where the newly read
 * data should be placed. Finally, offp is a pointer to a "long offset type" object that indicates the file
 * position the user is accessing. The return value is a "signed size type"; its use is discussed later.
 *
 * Let us repeat that the buff argument to the read and write methods is a user-space pointer. Therefore, it
 * cannot be directly dereferenced by kernel code. There are a few reasons for this restriction:
 *  - Depending on which architecture your driver is running on, and how the kernel was configured, the
 *    user-space pointer may not be valid while running in kernel mode at all. There may be no mapping for
 *    that address, or it could point to some other, random data.
 *  - Even if the pointer does mean the same thing in kernel space, user-space memory is paged, and the
 *    memory in question might not be resident in RAM when the system call is made. Attempting to reference
 *    the user-space memory directly could generate a page fault, which is something that kernel code is not
 *    allowed to do. The result would be an "oops," which would result in the death of the process that made
 *    the system call.
 *  - The pointer in question has been supplied by a user program, which could be buggy or malicious. If your
 *    driver ever blindly dereferences a user-supplied pointer, it provides an open doorway allowing a
 *    user-space program to access or overwrite memory anywhere in the system. If you do not wish to be
 *    responsible for compromising the security of your users' systems, you cannot ever dereference a
 *    user-space pointer directly.
 *
 * Obviously, your driver must be able to access the user-space buffer in order to get its job done. This
 * access must always be performed by special, kernel-supplied functions, however, in order to be safe.
 * This functions are defined in <asm/uaccess.h>
 * We need to copy a whole segment of data to or from the user address space. This capability is offered by
 * the following kernel functions, which copy an arbitrary array of bytes and sit at the heart of most read
 * and write implementations:
 *
 * @code
 * unsigned long copy_to_user(void __user *to, const void *from, unsigned long count);
 *
 * unsigned long copy_from_user(void *to, const void __user *from, unsigned long count);
 * @endcode
 *
 * Although these functions behave like normal memcpy functions, a little extra care must be used when
 * accessing user space from kernel code. The user pages being addressed might not be currently present in
 * memory, and the virtual memory subsystem can put the process to sleep while the page is being transferred
 * into place. This happens, for example, when the page must be retrieved from swap space. The net result
 * for the driver writer is that any function that accesses user space must be reentrant, must be able to
 * execute concurrently with other driver functions, and, in particular, must be in a position where it can
 * legally sleep.
 * The role of the two functions is not limited to copying data to and from user-space: they also check
 * whether the user space pointer is valid. If the pointer is invalid, no copy is performed; if an invalid
 * address is encountered during the copy, on the other hand, only part of the data is copied. In both
 * cases, the return value is the amount of memory still to be copied.
 *
 * As far as the actual device methods are concerned, the task of the read method is to copy data from the
 * device to user space (using copy_to_user), while the write method must copy data from user space to the
 * device (using copy_from_user). Each read or write system call requests transfer of a specific number of
 * bytes, but the driver is free to transfer less data.
 *
 * Whatever the amount of data the methods transfer, they should generally update the file position at *offp
 * to represent the current file position after successful completion of the system call. The kernel then
 * propagates the file position change back into the file structure when appropriate. The pread and pwrite
 * system calls have different semantics, however; they operate from a given file offset and do not change
 * the file position as seen by any other system calls. These calls pass in a pointer to the user-supplied
 * position, and discard the changes that your driver makes.
 *
 * Both the read and write methods return a negative value if an error occurs. A return value greater than
 * or equal to 0, instead, tells the calling program how many bytes have been successfully transferred. If
 * some data is transferred correctly and then an error happens, the return value must be the count of
 * bytes successfully transferred, and the error does not get reported until the next time the function
 * is called. Implementing this convention requires, of course, that your driver remember that the error
 * has occurred so that it can return the error status in the future.
 *
 * <h5>Il metodo read()</h5>
 * The return value for read is interpreted by the calling application program:
 *  - If the value equals the count argument passed to the read system call, the requested number of bytes
 *    has been transferred. This is the optimal case.
 *  - If the value is positive, but smaller than count, only part of the data has been transferred. This
 *    may happen for a number of reasons, depending on the device. Most often, the application program
 *    retries the read. For instance, if you read using the fread function, the library function reissues
 *    the system call until completion of the requested data transfer.
 *  - If the value is 0, end-of-file was reached (and no data was read).
 *
 *  A negative value means there was an error. The value specifies what the error was, according to
 *  <linux/errno.h>. Typical values returned on error include -EINTR (interrupted system call) or -EFAULT
 *  (bad address).
 *  What is missing from the preceding list is the case of "there is no data, but it may arrive later."
 *  In this case, the read system call should block.
 *
 * <h5>Il metodo write()</h5>
 * write, like read, can transfer less data than was requested, according to the following rules for the
 * return value:
 *  - If the value equals count, the requested number of bytes has been transferred.
 *  - If the value is positive, but smaller than count, only part of the data has been transferred. The
 *    program will most likely retry writing the rest of the data.
 *  - If the value is 0, nothing was written. This result is not an error, and there is no reason to return
 *    an error code. Once again, the standard library retries the call to write.
 *  - A negative value means an error occurred; as for read, valid error values are those defined in
 *    <linux/errno.h>.
 *
 * <h5>Accesso ai registri del device</h5>
 * Si potrebbe senrire la tentazione di usare il puntatore restituito da ioremap() dereferenziandolo per
 * accedere alla memoria. Questo modo di procedere non e' portabile ed e' prono ad errori. Il modo corretto
 * di accedere alla memoria e' attraverso l'uso delle funzioni per il memory-mapped I/O, definite in <asm/io.h>.
 *
 * Per leggere dalla memoria vengono usate le seguenti:
 *
 * @code
 * unsigned int ioread8(void *addr);
 * unsigned int ioread16(void *addr);
 * unsigned int ioread32(void *addr);
 * @endcode
 *
 * addr e' l'indirizzo di memoria virtuale del device, ottenuto mediante chiamata a ioremap(), a cui viene,
 * eventualmente, aggiunto un offset. Il valore restituito dalle funzioni e' quello letto dalla particolare
 * locazione di memoria a cui viene effettuato accesso.
 *
 * Esiste un insieme di funzioni simili che, invece, scrivono dulla memoria:
 *
 * @code
 * void iowrite8(u8 value, void *addr);
 * void iowrite16(u16 value, void *addr);
 * void iowrite32(u32 value, void *addr);
 * @endcode
 *
 * @param file
 * @param buf
 * @param size
 * @param off
 *
 * @return Il valore di ritorno, se non negativo, rappresenta il numero di byte
 * correttamente scritti.
 */
static ssize_t myGPIOK_write (struct file *file_ptr, const char *buf, size_t size, loff_t *off) {

	myGPIOK_t *myGPIOK_dev_ptr;
	uint32_t data_to_write;

	printk(KERN_INFO "Chiamata %s\n", __func__);

	myGPIOK_dev_ptr = file_ptr->private_data;

	if (*off > myGPIOK_dev_ptr->rsrc_size)
		return -EFAULT;

	/* Il processore Zynq a bordo della Zybo e' little endian. Per questo motivo e'
	 * possibile convertire char* in uint32_t* mediante un semplice casting, senza
	 * invertire manualmente l'ordine dei byte.
	 */
	if (copy_from_user(&data_to_write, buf, size))
		return -EFAULT;

	iowrite32(data_to_write, (myGPIOK_dev_ptr->vrtl_addr+*off));

	printk(KERN_INFO "%s : offset %08X\n", __func__, *off);
	printk(KERN_INFO "%s : write %08X\n", __func__, data_to_write);

	return size;
}


/**
 * @}
 * @}
 * @}
 * @}
 */


/*=====================================Sezione non documentata ============================================*/

/*
 * @Inserisce una nuova entry nella tabella delle corrispondenze device - driver.
 * @param [inout]	of 				riferimento alla tabella
 * @param [in]		myGPIOk_match	struttura of_device_id
 */
MODULE_DEVICE_TABLE(of, myGPIOk_match);

/*
 * la macro module_platform_driver() prende in input la struttura platform_driver ed implementa le
 * funzioni module_init() e module_close() standard, chiamate quando il modulo viene caricato o
 * rimosso dal kernel.
 *
 * @param myGPIOK_driver struttura platform_driver associata al driver
 */
module_platform_driver(myGPIOK_driver);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Salvatore Barone <salvator.barone@gmail.com>");
MODULE_DESCRIPTION("myGPIO device-driver in kernel mode");
MODULE_VERSION("0.1");
MODULE_ALIAS(DRIVER_NAME);

