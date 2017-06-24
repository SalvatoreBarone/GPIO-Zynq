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
 *
 * @brief Device-driver in kernel-mode per myGPIO
 *
 * @warning Se nel device tree source non viene indicato
 * <center>compatible = "myGPIOK";</center>
 * tra i driver compatibili con il device, il driver myGPIOK non viene correttamente istanziato ed il
 * programma userspace non funzionera'.
 *
 * @details
 *
 * <h2>Descrizione generale del driver</h2>
 * Il modulo driver implementa definisce il tipo myGPIOK_t ed implementa le seguenti funzioni:
 *  - myGPIOK_probe(): richiamata quando il modulo, o un device compatibile col modulo, viene inserito:
 *  - myGPIOK_remove(): richiamata quando il modulo, o un device compatibile, viene rimosso;
 *  - myGPIOK_open(): implementa la system call open();
 *  - myGPIOK_llseek(): implementa la system call seek();
 *  - myGPIOK_write(): implementa la system call seek();
 *  - myGPIOK_irq_handler(): implementa la ISR dedicata alla gestione delle interruzioni provenienti dal
 *    device;
 *  - myGPIOK_poll() : implementa il back-end di tre diverse system-calls (poll, epoll e select)
 *  - myGPIOK_read() : implementa la system call read.
 *
 * Nel seguito viene presentato un breve escursus su tutto cio' che c'e' da sapere per comprendere come
 * funziona un device-driver e come poterne scrivere uno.
 * Dopo aver letto il seguito, si consiglia, in ordine, di leggere, in ordine, la documentazione della
 * struttura myGPIOK-t, poi quella delle funzioni
 * - myGPIOK_probe();
 * - myGPIOK_open();
 * - myGPIOK_llseek();
 * - myGPIOK_write();
 * - myGPIOK_read();
 * - myGPIOK_irq_handler();
 *
 * <h3>Platform-device</h3>
 * I device driver, anche se sono moduli kernel, non si scrivono come normali moduli Kernel.
 * I "platform-device" sono device che non possono annunciarsi al software (non possono dire "Hey,
 * sono qui'!" al sistema operativo), quindi sono intrinsecamente "non-scopribili", nel senso che
 * il sistema, al boot, deve sapere che ci sono, ma non e' in grado di scoprirli. A differenza dei
 * device PCI o USB, che non sono platform-device, un device I²C non viene enumerato a livello
 * hardware, per cui e' necessario che il sistema operativo sappia, a tempo di "compilazione", cioe'
 * prima del boot - quale device sia connesso al bus I²C.
 * I non-discoverable devices stanno proliferando molto velocemente nel mondo embedded, per cui il
 * Kernel Linux offre ancora la possibilita' di specificare quale hardware sia presente nel sistema.
 * Bisogna distinguere in:
 *  - Platform Driver
 *  - Platform Device
 *
 * Per quanto riguarda la parte driver, il kernel Linux kernel definisce un insieme di operazioni
 * standard che possono essere effettuate su un platform-device.
 * Un riferimento pue' essere http://lxr.free-electrons.com/source/include/linux/platform_device.h#L173.
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

#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/irqreturn.h>

#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#include <linux/sched.h>
#include <linux/poll.h>

#include "myGPIOK_dev_int.h"
#include "myGPIOK_t.h"

/**
 * @brief Nome identificativo del device-driver.
 * DEVE corrispondere al valore del campo "compatible" nel device tree source.
 */
#define DRIVER_NAME "myGPIOK"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Salvatore Barone <salvator.barone@gmail.com>");
MODULE_DESCRIPTION("myGPIO device-driver in kernel mode");
MODULE_VERSION("3.2");
MODULE_ALIAS(DRIVER_NAME);




/**
 * @brief Numero di minor-number richiesti dal driver, corrispondera' al numero massimo di device gestibili
 */
#define MAX_NUM_OF_DEVICES 16

/**
 * @brief Major e minor number per il device driver
 */
static dev_t myGPIOK_Mm_number;


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

#define myGPIOK_USED_INT		0xFFFFFFFFU //!< @brief Maschea di abilitazione degli interrupt per i singoli pin

/**
 * @brief Puntatore a struttura myGPIOK_t, contenente tutti i dati necessari al device driver.
 */
myGPIOK_t *myGPIOK_dev_ptr = NULL;

/**
 * @brief Numero di device myGPIOK_t attivi
 */
uint32_t myGPIOK_on = 0;

/**
 * @brief
 */
static struct class*  myGPIOK_class  = NULL;

/**
 * @brief
 */
static struct device* myGPIOK_device = NULL;

/**
 * @brief Stuttura per l'astrazione di un device a caratteri,
 *
 * Il kernel usa, internamente, una struttura cdev per rappresentare i device a caratteri. Prima che il
 * kernel invochi le funzioni definite dal driver per il device, bisogna allocare e registrare uno, o piu',
 * oggetti cdev. In questo caso e' sufficiente allocare uno solo di questi oggetti.
 */
struct cdev myGPIOK_cdev;

/**
 * @brief Identifica il device all'interno del device tree
 *
 * Tutti i device-driver devono esporre un ID. A tempo di compilazione, il processo di build estrae queste
 * informazioni dai driver per la preparazione di una tabella. Quando si "inserisce" il device, la tabella
 * viene riferita dal kernel e, se viene trovata una entry corrispondente al driver per quel device, il
 * driver viene caricato ed inizializzato.
 */
static struct of_device_id myGPIOK_match[] = {
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
				.of_match_table = myGPIOK_match,
		},
};

/**
 * @brief  Inserisce una nuova entry nella tabella delle corrispondenze device - driver.
 * @param [inout]	of 				riferimento alla tabella
 * @param [in]		myGPIOK_match	struttura of_device_id
 */
MODULE_DEVICE_TABLE(of, myGPIOK_match);

/**
 * @brief la macro module_platform_driver() prende in input la struttura platform_driver ed implementa le
 * funzioni module_init() e module_close() standard, chiamate quando il modulo viene caricato o
 * rimosso dal kernel.
 *
 * @param myGPIOK_driver struttura platform_driver associata al driver
 */
module_platform_driver(myGPIOK_driver);

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

/**
 * @brief Viene chiamata quando il modulo viene inserito.
 *
 * @param [inout] op
 *
 * @retval 0 nel caso in cui non si sia verificato nessun errore;
 * @retval -ENOMEM nel caso in cui non sia possibile allocare memoria;
 * @retval <0 per altri errori
 *
 * @details
 * <h3>Inizializzazione del driver</h3>
 * Il device myGPIO viene gestito come un character-device, ossia un device su cui e' possibile leggere e/o
 * scrivere byte. Il kernel usa, internamente, una struttura cdev per rappresentare i device a caratteri. Prima
 * che il kernel invochi le funzioni definite dal driver per il device, bisogna allocare e registrare uno, o
 * piu', oggetti cdev. Per farlo e' necessario includere <linux/cdev.h>, che definisce tale struttura e le
 * relative funzioni.
 */
static int myGPIOK_probe(struct platform_device *op) {
	int error = 0;

	printk(KERN_INFO "Chiamata %s\n\tname: %s\n\tid: %u\n", __func__, op->name, op->id);


/** <h5>Major-number e Minor-number</h5>
 * Ai device drivers sono associati un major-number ed un minor-number. Il major-number viene usato dal kernel
 * per identificare il driver corretto corrispondente ad uno specifico device, quando si effettuano operazioni
 * su di esso. Il ruolo del minor number dipende dal device e viene gestito internamente dal driver.
 * Questo driver, cosi' come molti altri, usa il minor number per distinguere le diverse istanze di device
 * myGPIO che usano il device-driver myGPIOK.
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
 */
	if ((error = alloc_chrdev_region(&myGPIOK_Mm_number, 0 , MAX_NUM_OF_DEVICES, DRIVER_NAME)) != 0) {
		printk(KERN_ERR "%s: alloc_chrdev_region() ha restituito %d\n", __func__, error);
		return error;
	}
/** <h5>Device Class</h5>
 * Ai device-drivers viene associata una classe ed un device-name.
 * Per creare ed associare una classe ad un device driver si puo' usare la seguente.
 * @code
 * struct class * class_create(struct module * owner, const char * name);
 * @endcode
 *  - owner: puntatore al modulo che "possiede" la classe, THIS_MODULE
 *  - name: puntatore alla stringa identificativa (il nome) del device driver, DRIVER_NAME
 *
 */
	if ((myGPIOK_class = class_create(THIS_MODULE, DRIVER_NAME) ) == NULL) {
		printk(KERN_ERR "%s: class_create() ha restituito NULL\n", __func__);
		unregister_chrdev_region(myGPIOK_Mm_number, MAX_NUM_OF_DEVICES);
		return -ENOMEM;
	}
/** <h5>Operatori</h5>
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
 */
	cdev_init (&myGPIOK_cdev, &myGPIO_fops);
	myGPIOK_cdev.owner = THIS_MODULE;
/** <h5>Aggiunta del device</h5>
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
 */
	if ((error = cdev_add(&myGPIOK_cdev, myGPIOK_Mm_number, MAX_NUM_OF_DEVICES)) != 0) {
		printk(KERN_ERR "%s: cdev_add() ha restituito %d\n", __func__, error);
		class_destroy(myGPIOK_class);
		unregister_chrdev_region(myGPIOK_Mm_number, MAX_NUM_OF_DEVICES);
		return error;
	}
/** <h5>Creazione del device</h5>
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
 */
	if ((myGPIOK_device = device_create(myGPIOK_class, NULL, myGPIOK_Mm_number, NULL, DRIVER_NAME)) == NULL) {
		printk(KERN_ERR "%s: device_create() ha restituito NULL\n", __func__);
		cdev_del(&myGPIOK_cdev);
		class_destroy(myGPIOK_class);
		unregister_chrdev_region(myGPIOK_Mm_number, MAX_NUM_OF_DEVICES);
		return -ENOMEM;
	}

	/* Allocazione dell'oggetto myGPIOK_t */
	if ((myGPIOK_dev_ptr = kmalloc(sizeof(myGPIOK_t), GFP_KERNEL)) == NULL) {
		printk(KERN_ERR "%s: kmalloc ha restituito NULL\n", __func__);
		return -ENOMEM;
	}


	if ((error = myGPIOK_t_Init(myGPIOK_dev_ptr, &op->dev, op->id, DRIVER_NAME, (irq_handler_t)myGPIOK_irq_handler, myGPIOK_USED_INT)) != 0) {
		printk(KERN_ERR "%s: myGPIOK_t_Init() ha restituito %d\n", __func__, error);
		device_destroy(myGPIOK_class, myGPIOK_Mm_number);
		cdev_del(&myGPIOK_cdev);
		class_destroy(myGPIOK_class);
		unregister_chrdev_region(myGPIOK_Mm_number, MAX_NUM_OF_DEVICES);
		return error;
	}

	return error;
}

/**
 * @breif Viene chiamata automaticamente alla rimozione del mosulo.
 *
 * @param [inout] op
 *
 * @retval 0 se non si verifica nessun errore
 *
 * @details
 * Dealloca tutta la memoria utilizzata dal driver, de-inizializzando il device e disattivando gli interrupt per il
 * device, effettuando tutte le operazioni inverse della funzione myGPIOK_probe().
 */
static int myGPIOK_remove(struct platform_device *op) {
	printk(KERN_INFO "Chiamata %s\n", __func__);
	myGPIOK_t_Destroy(myGPIOK_dev_ptr);
	device_destroy(myGPIOK_class, myGPIOK_Mm_number);
	cdev_del(&myGPIOK_cdev);
	class_destroy(myGPIOK_class);
	unregister_chrdev_region(myGPIOK_Mm_number, MAX_NUM_OF_DEVICES);
	//kfree(myGPIOK_dev_ptr);
	return 0;
}

/**
 * @brief Invocata all'apertura del file corrispondente al device.
 *
 * @param [in]		inode
 * @param [inout]	file
 *
 * @retval 0 se non si verifica nessun errore
 *
 * @details
 * <h5>Il metodo open()</h5>
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
 */
static int myGPIOK_open(struct inode *inode, struct file *file_ptr) {
	//myGPIOK_t *myGPIOK_dev_ptr;
	printk(KERN_INFO "Chiamata %s\n", __func__);
/**
 * <h3>Identificare il particolare device associato al file</h3>
 * Il parametro inode contiene tutte le informazioni necessarie all'interno del campo i_cdev, il quale
 * contiene la struttura cdev inizializzata precedentemente dalla funzione di probe(). Il problema e'
 * che non abbiamo bisogno della sola struttura cdev, ma della struttura che la contiene, in questo
 * caso della struttura myGPIOK_t.
 * Fortunatamente i programmatori del kernel hanno reso la vita semplice agli altri, predisponendo la
 * macro container_if() definita in <linux/kernel.h>.
 * @code
 * container_of(pointer, container_type, container_field);
 * @endcode
 * La macro prende in ingresso un puntatore ad un campo di tipo container_field, di una struttura
 * container_type, restituendo il puntatore alla struttura che la contiene.
 */
	printk(KERN_INFO "%s\n\tminor : %d", __func__, MINOR(inode->i_cdev->dev));



	//myGPIOK_dev_ptr = container_of(inode->i_cdev, myGPIOK_t, cdev);
	file_ptr->private_data = myGPIOK_dev_ptr;
	return 0;
}

/**
 * @brief Invocata alla chiusura del file corrispondente al device.
 *
 * @param [in]	inode
 * @param [in]	file
 *
 * @retval 0 se non si verifica nessun errore
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
 * @param [inout]	file_ptr
 * @param [in]		off
 * @param [in]		whence
 * @return Nuova posizione della "testina" di lettura/scrittura
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
 * @param[inout]	file
 * @param[inout]	wait
 *
 * @return restituisce una maschera di bit che indica se sia possibile effettuare
 * operazioni di lettura/scrittura non bloccanti, in modo che il kernel possa
 * bloccare il processo e risvegliarlo solo quando tali operazioni diventino possibili.
 *
 * @details
 * Questo metodo e' il back-end di tre diverse system-calls: poll, epoll e select,
 * le quali	sono usate per capire se una operazione di lettura/scrittura du un device
 * possano risultare bloccanti o meno.
 */
static unsigned int myGPIOK_poll (struct file *file_ptr, struct poll_table_struct *wait) {
	myGPIOK_t *myGPIOK_dev;
	unsigned int mask = 0;
	printk(KERN_INFO "Chiamata %s\n", __func__);
	myGPIOK_dev = file_ptr->private_data;
	poll_wait(file_ptr, &myGPIOK_dev->poll_queue,  wait);
	spin_lock(&myGPIOK_dev->slock_int);
	if(myGPIOK_dev->can_read)
		mask = POLLIN | POLLRDNORM;
	spin_unlock(&myGPIOK_dev->slock_int);
	return mask;
}

/**
 * @brief Interrupt-handler
 * @param irq
 * @param regs
 * @retval IRQ_HANDLED dopo aver servito l'interruzione
 *
 * @details
 * Gestisce il manifestarsi di un evento interrompente proveniente dalla periferica.
 * Viene registrata dalla funzione myGPIOK_probe() affinche' venga richiamata al manifestarsi di un interrupt
 * sulla linea cui e' connesso il device
 */
static irqreturn_t myGPIOK_irq_handler(int irq, struct pt_regs * regs) {
	unsigned long flags;
	printk(KERN_INFO "Chiamata %s\n", __func__);
/** <h5>Disabilitazione delle interruzioni della periferica</h5>
 * Prima di servire l'interruzione, gli interrupt della periferica vengono disabilitati.
 * Se si tratta di un GPIO Xilinx, vengono disabilitati sia gli interrupt globali che quelli generati dal
 * secondo canale. Se, invece, si tratta di un device GPIO custom myGPIO, vengono disabilitati solo gli interrupt
 * globali.
 */
#ifdef __XGPIO__
	XGpio_Global_Interrupt(myGPIOK_dev_ptr->vrtl_addr, XGPIO_GIDS);
	XGpio_Channel_Interrupt(myGPIOK_dev_ptr->vrtl_addr, XGPIO_CH2_IDS);
#else
	myGPIOK_GlobalInterruptDisable(myGPIOK_dev_ptr->vrtl_addr);
	myGPIOK_PinInterruptDisable(myGPIOK_dev_ptr->vrtl_addr, myGPIOK_USED_INT);
#endif
/** <h5>Setting del valore del flag "interrupt occurred"</h5>
 * Dopo aver disabilitato gli interrupt della periferica, occorre settare in modo appropriato il flag
 * "interrupt occurred", in modo che i processi in attesa possano essere risvegliati in modo sicuro.
 * Per prevenire race condition, tale operazione viene effettuata mutua esclusione.
 * I semafori sono uno strumento potentissimo per per l'implementazione di sezioni critiche, ma non possono
 * essere usati in codice non interrompibile. Gli spilock sono come i semafori, ma possono essere usati
 * anche in codice non interrompibile, come puo' esserlo un modulo kernel.
 * Sostanzialmente se uno spinlock e' gia' stato acquisito da qualcun altro, si entra in un hot-loop dal
 * quale si esce solo quando chi possiede lo spinlock lo rilascia. Trattandosi di moduli kernel, e' di
 * vitale importanza che la sezione critica sia quanto piu' piccola possibile. Ovviamente l'implementazione
 * e' "un po'" piu' complessa di come e' stata descritta, ma il concetto e' questo. Gli spinlock sono
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
	spin_lock_irqsave(&myGPIOK_dev_ptr->slock_int, flags);
	myGPIOK_dev_ptr-> can_read = 1;
	spin_unlock_irqrestore(&myGPIOK_dev_ptr->slock_int, flags);
/** <h5>Incremento del numero totale di interrupt</h5>
 * Dopo aver settato il flag, viene incrementato il valore degli interrupt totali.
 * Anche questa operazione viene effettuata in mutua esclusione.
 */
	spin_lock_irqsave(&myGPIOK_dev_ptr->sl_total_irq, flags);
	myGPIOK_dev_ptr->total_irq++;
	spin_unlock_irqrestore(&myGPIOK_dev_ptr->sl_total_irq, flags);
/** <h5>Wakeup dei processi sleeping</h5>
 * La ISR deve chiamare esplicitamente wakeup() per risvegliare i processi messi in sleeping in attesa che
 * un particolare evento si manifestasse. La funzione
 * @code
 * void wake_up_interruptible(wait_queue_head_t *queue);
 * @endcode
 * risveglia tutti i processi posti in una determinata coda (risvegliando solo quelli che, in precedenza, hanno
 * effettuato una chiamata a wait_event_interruptible()).
 * Se due processi vengono risvegliati contemporaneamente potrebbero originarsi race-condition.
 */
	wake_up_interruptible(&myGPIOK_dev_ptr->read_queue);
	wake_up_interruptible(&myGPIOK_dev_ptr->poll_queue);
	return IRQ_HANDLED;
}

/**
 * @brief Legge dati dal device.
 *
 * @param [in]  file
 * @param [out] buf
 * @param [in]  count
 * @param [in]  off
 *
 * @warning L'offset viene diviso per quattro prima di essere aggiunto all'indirizzo base del device.
 *
 * @return restituisce un valore negativo nel caso in cui si sia verificato un errore. Un valore maggiore
 * o uguale a zero indica il numero di byte scritti con successo.
 *
 * @details
 * <h3>Operazioni di lettura e scrittura</h3>
 * I metodi read() e write() effettuano operazioni simili, ossia copiare dati da/verso il device. Il loro
 * prototipo e' molto simile.
 * @code
 * ssize_t read(struct file *filp, char __user *buff, size_t count, loff_t *offp);
 * ssize_t write(struct file *filp, const char __user *buff, size_t count, loff_t *offp);
 * @endcode
 * Per entrambi i metodi filep e' il puntatore al file che rapresenta il device, count e' la dimensione dei
 * dati da trasferire, buff e' il puntatore al buffer contenente i dati (da scrivere per la write() o letti
 * per la read()). Infine offp e' il puntatore ad un oggetto "long offset type" che indica la posizione
 * alla quale si sta effettuando l'accesso.
 *
 * <h5>I/O bloccante</h5>
 * Nel paragrafo precedente e' stato ignorato un problema importante: come comportarsi quando il driver
 * non e' in grado di servire immegiatamente una richiesta? Una chiamata a read() potrebbe arrivare quando
 * i dati non sono disponibili, ma potrebbero esserlo in futuro, oppure, una chiamata a write() potrebbe
 * avvenire quando il device non e' in grado di accettare altri dati (perche' il suo buffer di ingresso
 * potrebbe essere pieno). Il processo chiamante e' totalmente all'oscuro di queste dinamiche, anzi potrebbe
 * non avere la minima conoscenza delle dinamiche interne del device: chiama le funzioni read() o write()
 * e si aspetta che facciano cio' che devono fare, per cui, nell'impossibilita' di servire la richiesta, il
 * driver bloccare il processo e metterlo tra i processi "sleeping", fin quando la richiesta non puo' essere
 * servita.
 * Il codice (la ISR) che dovra' risvegliare il processo quado potra' servire la sua richiesta, deve essere
 * a conoscenza dell'evento "risvegliante" e deve essere in grado di "trovare" i processi in attesa per
 * quel particolare evento. Per questo motivo, tutti i processi in attesa di un particolare evento vengono
 * posti all'interno della stessa wait queue.
 * Il codice della ISR deve effettuare una chiamata a wakeup() per risvegliare i processi in attesa di un
 * evento quando questo si e' manifestato. Si veda la documentazione della funzione myGPIOK_irq_handler()
 * per dettagli ulteriori.
 */
static ssize_t myGPIOK_read (struct file *file_ptr, char *buf, size_t count, loff_t *off) {
	myGPIOK_t *myGPIOK_dev_ptr;
	void* read_addr;
	uint32_t data_readed;
	printk(KERN_INFO "Chiamata %s\n", __func__);
	myGPIOK_dev_ptr = file_ptr->private_data;
	if (*off > myGPIOK_dev_ptr->rsrc_size)
		return -EFAULT;
/** <h5>I/O non-bloccante</h5>
 * Esistono casi in cui il processo chiamante non vuole essere bloccato in attesa di un evento. Questa evenienza
 * viene esplicitamente indicata attraverso il flag O_NONBLOCK flag in filp->f_flags. Il flag viene definito in
 * <linux/fcntl.h> il quale e' incluso in<linux/fs.h>.
 */
	if ((file_ptr->f_flags & O_NONBLOCK) == 0) {
		printk(KERN_INFO "%s e' bloccante\n", __func__);
/** <h5>Porre un processo nello stato sleeping</h5>
 * Quando un processo viene messo nello stato sleep, lo si fa aspettandosi che una condizione diventi vera in
 * futuro. Al risveglio, pero', non c'e' nessuna garanzia che quella particolare condizione sia ancora vera,
 * per cui essa va nuovamente testata.
 * Il modo piu' semplice per potte un processo nello stato sleeping e' chiamare la macro wait_event(), o una
 * delle sue varianti: essa combina la gestione della messa in sleeping del processo ed il check della
 * condizione che il processo si aspetta diventi vera.
 * @code
 * wait_event_interruptible(queue, condition);
 * @endcode
 * Il parametro queue e' la coda di attesa mentre condition e' la condizione che, valutata true, causa la
 * messa in sleep del processo. La condizione viene valutata sia prima che il processo sia messo in sleeping
 * che al suo risveglio. Lo sleep in cui il processo viene messo chiamando wait_event_interruptible() puo'
 * essere interrotto anche da un segnale, per cui la macro restituisce un intero che, se diverso da zero,
 * indica che il processo e' stato risvegliato da un segnale.
 *
 * La condizione sulla quale i processi vengono bloccati riguarda il flag "interrupt occurred". Fin quando
 * questo flag, posto in and con la maschera MYGPIOK_SREAD, e' zero, il processo deve restare bloccato, per
 * cui i processi che effettuano read() bloccante restano bloccati finche' int_occurred & MYGPIO_SREAD == 0.
 * Quando tale uguaglianza non sara' piu' valida, perche' il valore di int_occurred viene settato dalla
 * funzione myGPIOK_irq_handler(), allora il processo verra' risvegliato.
 */
		printk(KERN_INFO "%s : can_read %08X\n", __func__, myGPIOK_dev_ptr-> can_read);
		wait_event_interruptible(myGPIOK_dev_ptr->read_queue, (myGPIOK_dev_ptr->can_read != 0));
		printk(KERN_INFO "%s : can_read %08X\n", __func__, myGPIOK_dev_ptr-> can_read);
/**<h5>Reset del flag "interrupt occurred" per read() bloccanti</h5>
 * Nel momento in cui il processo viene risvegliato e la condizione della quale era in attesa e' tale che
 * esso puo' continuare la sua esecuzione, e' necessario resettare tale flag. Questa operazione va effettuata
 * per prevenire race-condition dovute al risveglio di piu' processi in attesa del manifestarsi dello stesso
 * evento. Il reset del flag va, pertanto, effettuato in mutua esclusione.
 *
 * I semafori sono uno strumento potentissimo per per l'implementazione di sezioni critiche, ma non possono
 * essere usati in codice non interrompibile. Gli spilock sono come i semafori, ma possono essere usati
 * anche in codice non interrompibile, come puo' esserlo un modulo kernel.
 * Sostanzialmente se uno spinlock e' gia' stato acquisito da qualcun altro, si entra in un hot-loop dal
 * quale si esce solo quando chi possiede lo spinlock lo rilascia. Trattandosi di moduli kernel, e' di
 * vitale importanza che la sezione critica sia quanto piu' piccola possibile. Ovviamente l'implementazione
 * e' "un po'" piu' complessa di come e' stata descritta, ma il concetto e' questo. Gli spinlock sono
 * definiti in <linux/spinlock.h>.
 * Esistono diversi modi per acquisire uno spinlock. Nel seguito viene usata la funzione
 * @code
 * void spin_lock(spinlock_t *lock);
 * @endcode
 * per rilasciare uno spinlock, invece, verra' usata
 * @code
 * void spin_unlock(spinlock_t *lock);
 * @endcode
 */
		spin_lock(&myGPIOK_dev_ptr->slock_int);
		myGPIOK_dev_ptr-> can_read = 0;
		spin_unlock(&myGPIOK_dev_ptr->slock_int);
		printk(KERN_INFO "%s : can_read %08X\n", __func__, myGPIOK_dev_ptr-> can_read);
	}
	else {
		printk(KERN_INFO "%s non e' bloccante\n", __func__);
	}
/** <h5>Accesso ai registri del device</h5>
 * Si potrebbe senrire la tentazione di usare il puntatore restituito da ioremap() dereferenziandolo per
 * accedere alla memoria. Questo modo di procedere non e' portabile ed e' prono ad errori. Il modo corretto
 * di accedere alla memoria e' attraverso l'uso delle funzioni per il memory-mapped I/O, definite in <asm/io.h>.
 * Per leggere dalla memoria vengono usate le seguenti:
 * @code
 * unsigned int ioread8(void *addr);
 * unsigned int ioread16(void *addr);
 * unsigned int ioread32(void *addr);
 * @endcode
 * addr e' l'indirizzo di memoria virtuale del device, ottenuto mediante chiamata a ioremap(), a cui viene,
 * eventualmente, aggiunto un offset. Il valore restituito dalle funzioni e' quello letto dalla particolare
 * locazione di memoria a cui viene effettuato accesso.
 * Per scrivere nella memoria vengono usate le seguenti:
 * @code
 * void iowrite8(u8 value, void *addr);
 * void iowrite16(u16 value, void *addr);
 * void iowrite32(u32 value, void *addr);
 * @endcode
 */
	read_addr = myGPIOK_dev_ptr->vrtl_addr+*off;
	data_readed = ioread32(read_addr);
/** <h5>Accesso alla memoria userspace</h5>
 * Buff e' un puntatore appartenente allo spazio di indirizzamento del programma user-space che utilizza
 * il modulo kernel. Il modulo, quindi, non puo' accedere direttamente ad esso, dereferenziandolo, per
 * diverse ragioni, tra le quali:
 *  - a seconda dell'architettura sulla quale il driver e' in esecuzione e di come il kernel e' stato
 *    configurato, il puntatore userspace potrebbe non essere valido mentre il modulo kernel viene eseguito;
 *  - la memoria user-space e' paginata e potrebbe non essere presente in RAM quando la system-call viene
 *    effettuata, per cui dereferenziando il puntatore potrebbe originarsi un page-fault con conseguente
 *    terminazione del processo che ha effettuato la system-call;
 *  - il puntatore in questione potrebbe essere stato fornito da un programma user-space buggato o malizioso,
 *    motivo per cui dereferenziandolo verrebbe a crearsi un punto di accesso attraverso il quale il
 *    programma userspace puo' modificare la memoria senza costrizioni.
 *
 * Ovviamente il driver deve essere in grado di poter accedere al buffer userspace, per cui tale accesso
 * va fatto solo ed esclusivamente attraverso delle funzioni fornite dal kernel stesso, e definite in
 * <asm/uaccess.h>
 * @code
 * unsigned long copy_to_user(void __user *to, const void *from, unsigned long count);
 * unsigned long copy_from_user(void *to, const void __user *from, unsigned long count);
 * @endcode
 * Queste due funzioni non si limitano a copiare dati da/verso userspacem: verificano, infatti, anche che
 * il puntatore al buffer userspace sia valido. Se il puntatore non risultasse valido la copia non viene
 * effettuata.
 * Sia il metodo read() che il metodo write() restituiscono un valore negativo nel caso in cui si sia
 * verificato un errore. Un valore maggiore o uguale a zero indica il numero di byte trasferiti con
 * successo.
 *
 * <h5>Piccola nota sull'endianess</h5>
 * Il processore Zynq e' little endian. Per questo motivo e' possibile convertire char* in uint32_t* mediante
 * un semplice casting, senza invertire manualmente l'ordine dei byte.
 */
	if (copy_to_user(buf, &data_readed, count))
		return -EFAULT;

	printk(KERN_INFO "%s : address %08X\n", __func__, (uint32_t) read_addr);
	printk(KERN_INFO "%s : read %08X\n", __func__, data_readed);
	printk(KERN_INFO "%s : can_read %08X\n", __func__, myGPIOK_dev_ptr-> can_read);

/**<h5>Debouncing</h5>
 * Sebbene normalmente non necessario, in questo caso si e' preferito inserire un hot-loop, in modo da attendere
 * che il device di input venga riportato allo stato di riposo prima di continuare l'esecuzione della funzione.
 * Questo piccolo espediente serve a fare in modo che, nel nostro caso, non vengano generate interruzioni spurie.
 * Il ciclo va rimosso in qualsiasi applicazione che non riguardi la pressione di un tasto.
 */
	while (ioread32(read_addr) != 0);
/** <h5>Ack degli interrupt della periferica</h5>
 * Viene inviato l'Ack alla periferica, per segnalargli che l'interrupt e' stato servito, solo dopo che la lettura
 * sia stata effettuata.
 */
#ifdef __XGPIO__
	XGpio_Ack_Interrupt(myGPIOK_dev_ptr->vrtl_addr, XGPIO_CH2_ACK);
#else
		myGPIOK_PinInterruptAck(myGPIOK_dev_ptr->vrtl_addr, myGPIOK_USED_INT);
#endif
/** <h5>Abilitazione degli interrupt della periferica</h5>
 * Dopo aver inviato notifica di servizio dell'interruzione al device, vengono nuovamente abilitati gli interrupt.
 */
#ifdef __XGPIO__
	XGpio_Global_Interrupt(myGPIOK_dev_ptr->vrtl_addr, XGPIO_GIE);
	XGpio_Channel_Interrupt(myGPIOK_dev_ptr->vrtl_addr, XGPIO_CH2_IE);
#else
	myGPIOK_GlobalInterruptEnable(myGPIOK_dev_ptr->vrtl_addr);
	myGPIOK_PinInterruptEnable(myGPIOK_dev_ptr->vrtl_addr, myGPIOK_USED_INT);
#endif

	return count;
}

/**
 * @brief Invia dati al device
 *
 * @param [in] file
 * @param [in] buf
 * @param [in] size
 * @param [in] off
 *
 * @warning L'offset viene diviso per quattro prima di essere aggiunto all'indirizzo base del device.
 *
 * @return restituisce un valore negativo nel caso in cui si sia verificato un errore. Un valore maggiore
 * o uguale a zero indica il numero di byte scritti con successo.
 *
 * @details
 * <h3>Operazioni di lettura e scrittura</h3>
 * I metodi read() e write() effettuano operazioni simili, ossia copiare dati da/verso il device. Il loro
 * prototipo e' molto simile.
 *
 * @code
 * ssize_t read(struct file *filp, char __user *buff, size_t count, loff_t *offp);
 * ssize_t write(struct file *filp, const char __user *buff, size_t count, loff_t *offp);
 * @endcode
 *
 * Per entrambi i metodi filep e' il puntatore al file che rapresenta il device, count e' la dimensione dei
 * dati da trasferire, buff e' il puntatore al buffer contenente i dati (da scrivere per la write() o letti
 * per la read()). Infine offp e' il puntatore ad un oggetto "long offset type" che indica la posizione
 * alla quale si sta effettuando l'accesso.
 *
 */
static ssize_t myGPIOK_write (struct file *file_ptr, const char *buf, size_t size, loff_t *off) {
	myGPIOK_t *myGPIOK_dev_ptr;
	uint32_t data_to_write;
	void* write_addr;
	printk(KERN_INFO "Chiamata %s\n", __func__);
	myGPIOK_dev_ptr = file_ptr->private_data;
	if (*off > myGPIOK_dev_ptr->rsrc_size)
		return -EFAULT;
/** <h5>Accesso alla memoria userspace</h5>
 * Buff e' un puntatore appartenente allo spazio di indirizzamento del programma user-space che utilizza
 * il modulo kernel. Il modulo, quindi, non puo' accedere direttamente ad esso, dereferenziandolo, per
 * diverse ragioni, tra le quali:
 *  - a seconda dell'architettura sulla quale il driver e' in esecuzione e di come il kernel e' stato
 *    configurato, il puntatore userspace potrebbe non essere valido mentre il modulo kernel viene eseguito;
 *  - la memoria user-space e' paginata e potrebbe non essere presente in RAM quando la system-call viene
 *    effettuata, per cui dereferenziando il puntatore potrebbe originarsi un page-fault con conseguente
 *    terminazione del processo che ha effettuato la system-call;
 *  - il puntatore in questione potrebbe essere stato fornito da un programma user-space buggato o malizioso,
 *    motivo per cui dereferenziandolo verrebbe a crearsi un punto di accesso attraverso il quale il
 *    programma userspace puo' modificare la memoria senza costrizioni.
 *
 * Ovviamente il driver deve essere in grado di poter accedere al buffer userspace, per cui tale accesso
 * va fatto solo ed esclusivamente attraverso delle funzioni fornite dal kernel stesso, e definite in
 * <asm/uaccess.h>
 * @code
 * unsigned long copy_to_user(void __user *to, const void *from, unsigned long count);
 * unsigned long copy_from_user(void *to, const void __user *from, unsigned long count);
 * @endcode
 * Queste due funzioni non si limitano a copiare dati da/verso userspacem: verificano, infatti, anche che
 * il puntatore al buffer userspace sia valido. Se il puntatore non risultasse valido la copia non viene
 * effettuata.
 * Sia il metodo read() che il metodo write() restituiscono un valore negativo nel caso in cui si sia
 * verificato un errore. Un valore maggiore o uguale a zero indica il numero di byte trasferiti con
 * successo.
 *
 * <h5>Piccola nota sull'endianess</h5>
 * Il processore Zynq e' little endian. Per questo motivo e' possibile convertire char* in uint32_t* mediante
 * un semplice casting, senza invertire manualmente l'ordine dei byte.
 */
	if (copy_from_user(&data_to_write, buf, size))
		return -EFAULT;
/** <h5>Accesso ai registri del device</h5>
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
 * Per scrivere nella memoria vengono usate le seguenti:
 *
 * @code
 * void iowrite8(u8 value, void *addr);
 * void iowrite16(u16 value, void *addr);
 * void iowrite32(u32 value, void *addr);
 * @endcode
 */
	write_addr = myGPIOK_dev_ptr->vrtl_addr+*off;
	iowrite32(data_to_write, write_addr);
	printk(KERN_INFO "%s : address %08X\n", __func__, (uint32_t) write_addr);
	printk(KERN_INFO "%s : write %08X\n", __func__, data_to_write);
	return size;
}


/**
 * @}
 * @}
 */
