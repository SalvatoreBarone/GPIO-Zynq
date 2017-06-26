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
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/interrupt.h>
#include <linux/irqreturn.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#include <linux/sched.h>
#include <linux/poll.h>

#include "myGPIOK_t.h"
#include "myGPIOK_list.h"

/**
 * @brief Nome identificativo del device-driver.
 * DEVE corrispondere al valore del campo "compatible" nel device tree source.
 */
#define DRIVER_NAME "myGPIOK"

/**
 * @brief Nome del file creato in /dev/ per ciascuno dei device
 */
#define DRIVER_FNAME "myGPIOK%d"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Salvatore Barone <salvator.barone@gmail.com>");
MODULE_DESCRIPTION("myGPIO device-driver in kernel mode");
MODULE_VERSION("3.2");
MODULE_ALIAS(DRIVER_NAME);

/**
 * @brief Numero massimo di device gestibili
 *
 * In realta', il numero di device gestibili e' virtualmente illimitato. Il fattore limitante e' la memoria
 * disponibile ed in numero di Major-minor number disponibili.
 * MAX_NUM_OF_DEVICES incide sulla dimensione della struttura dati myGPIOk_list_t che mantiene i riferimenti
 * alle strutture di controllo dei diversi device.
 */
#define MAX_NUM_OF_DEVICES 15

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
 * @brief Array di puntatori a struttura myGPIOK_t, contenente tutti i dati necessari al device driver.
 */
static myGPIOK_list_t *device_list = NULL;

/**
 * @brief Classe del device
 * Ai device-drivers viene associata una classe ed un device-name.
 * Per creare ed associare una classe ad un device driver si puo' usare la seguente.
 * @code
 * struct class * class_create(struct module * owner, const char * name);
 * @endcode
 * Parametri:
 *  - owner: puntatore al modulo che "possiede" la classe, THIS_MODULE
 *  - name: puntatore alla stringa identificativa (il nome) del device driver, DRIVER_NAME
 */
static struct class*  myGPIOK_class  = NULL;

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
 * @brief Inserisce una nuova entry nella tabella delle corrispondenze device - driver.
 * @param [inout]	of 				riferimento alla tabella
 * @param [in]		myGPIOK_match	struttura of_device_id
 */
MODULE_DEVICE_TABLE(of, myGPIOK_match);

/**
 * @brief la macro module_platform_driver() prende in input la struttura platform_driver ed implementa le
 * funzioni module_init() e module_close() standard, chiamate quando il modulo viene caricato o
 * rimosso dal kernel.
 *
 * @param [in] myGPIOK_driver struttura platform_driver associata al driver
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
static struct file_operations myGPIOK_fops = {
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
	myGPIOK_t *myGPIOK_ptr = NULL;
	printk(KERN_INFO "Chiamata %s\n", __func__);

	if (device_list == NULL) {

		if ((device_list = kmalloc(sizeof(myGPIOK_list_t), GFP_KERNEL)) == NULL ) {
			printk(KERN_ERR "%s: kmalloc ha restituito NULL\n", __func__);
			return -ENOMEM;
		}

		if ((error = myGPIOK_list_Init(device_list, MAX_NUM_OF_DEVICES)) != 0) {
			printk(KERN_ERR "%s: myGPIOK_list_Init() ha restituito %d\n", __func__, error);
			kfree(device_list);
			device_list = NULL;
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
 */
		if ((myGPIOK_class = class_create(THIS_MODULE, DRIVER_NAME) ) == NULL) {
			printk(KERN_ERR "%s: class_create() ha restituito NULL\n", __func__);
			kfree(device_list);
			device_list = NULL;
			return -ENOMEM;
		}

	}

	/* Allocazione dell'oggetto myGPIOK_t */
	if ((myGPIOK_ptr = kmalloc(sizeof(myGPIOK_t), GFP_KERNEL)) == NULL) {
		printk(KERN_ERR "%s: kmalloc ha restituito NULL\n", __func__);
		return -ENOMEM;
	}

	if ((error = myGPIOK_Init(	myGPIOK_ptr,
								THIS_MODULE,
								op,
								myGPIOK_class,
								DRIVER_NAME,
								DRIVER_FNAME,
								myGPIOK_list_device_count(device_list),
								&myGPIOK_fops,
								(irq_handler_t) myGPIOK_irq_handler,
								myGPIOK_USED_INT)) != 0) {
		printk(KERN_ERR "%s: myGPIOK_t_Init() ha restituito %d\n", __func__, error);
		kfree(myGPIOK_ptr);
		return error;
	}

	myGPIOK_list_add(device_list, myGPIOK_ptr);

	printk(KERN_INFO "\t%s => %s%d\n", op->name, DRIVER_NAME, myGPIOK_list_device_count(device_list)-1);

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
	myGPIOK_t *myGPIOK_ptr = NULL;

	printk(KERN_INFO "Chiamata %s\n\tptr: %08x\n\tname: %s\n\tid: %u\n", __func__, (uint32_t) op, op->name, op->id);

	myGPIOK_ptr = myGPIOK_list_find_by_op(device_list, op);
	if (myGPIOK_ptr != NULL) {
		myGPIOK_Destroy(myGPIOK_ptr);
		kfree(myGPIOK_ptr);
	}

	if (myGPIOK_list_device_count(device_list) == 0) {
		myGPIOK_list_Destroy(device_list);
		kfree(device_list);
		class_destroy(myGPIOK_class);
	}

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
	myGPIOK_t *myGPIOK_ptr;
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
 * <br>
 * Disponendo, in questo caso, della struttura myGPIOK_list_t, si fa uso di quest'ultima, e delle sue funzioni
 * membro, per risalire al device a partire dal suo major e minor number.
 * Il puntatore al particolare device myGPIOK_t sara' conservato all'interno del campo private_data della
 * struttura file.
 */
	printk(KERN_INFO "%s\n\tminor : %d", __func__, MINOR(inode->i_cdev->dev));

	if ((myGPIOK_ptr = myGPIOK_list_find_by_minor(device_list, inode->i_cdev->dev)) == NULL) {
		printk(KERN_INFO "%s: myGPIOK_list_find_by_minor() ha restituito NULL\n", __func__);
		return -1;
	}

	file_ptr->private_data = myGPIOK_ptr;
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
	printk(KERN_INFO "Chiamata %s\n", __func__);
	myGPIOK_dev = file_ptr->private_data;
	return myGPIOK_GetPollMask(myGPIOK_dev, file_ptr, wait);
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
	myGPIOK_t *myGPIOK_dev_ptr = NULL;
	printk(KERN_INFO "Chiamata %s\n\tline: %d\n", __func__, irq);

	if ((myGPIOK_dev_ptr = myGPIOK_list_find_irq_line(device_list, irq)) == NULL) {
		printk(KERN_INFO "%s\n\tmyGPIOK_list_find_irq_line() restituisce NULL:\n", __func__);
		return IRQ_NONE;
	}
/** <h5>Disabilitazione delle interruzioni della periferica</h5>
 * Prima di servire l'interruzione, gli interrupt della periferica vengono disabilitati.
 * Se si tratta di un GPIO Xilinx, vengono disabilitati sia gli interrupt globali che quelli generati dal
 * secondo canale. Se, invece, si tratta di un device GPIO custom myGPIO, vengono disabilitati solo gli interrupt
 * globali.
 */
#ifdef __XGPIO__
	XGpio_Global_Interrupt(myGPIOK_dev_ptr, XGPIO_GIDS);
	XGpio_Channel_Interrupt(myGPIOK_dev_ptr, myGPIOK_dev_ptr->irq_mask);
#else
	myGPIOK_GlobalInterruptDisable(myGPIOK_dev_ptr);
	myGPIOK_PinInterruptDisable(myGPIOK_dev_ptr, myGPIOK_dev_ptr->irq_mask);
#endif
/** <h5>Setting del valore del flag "interrupt occurred"</h5>
 * Dopo aver disabilitato gli interrupt della periferica, occorre settare in modo appropriato il flag
 * "interrupt occurred", in modo che i processi in attesa possano essere risvegliati in modo sicuro.
 * Per prevenire race condition, tale operazione viene effettuata mutua esclusione.
 */
	myGPIOK_SetCanRead(myGPIOK_dev_ptr);

/** <h5>Incremento del numero totale di interrupt</h5>
 * Dopo aver settato il flag, viene incrementato il valore degli interrupt totali.
 * Anche questa operazione viene effettuata in mutua esclusione.
 */
	myGPIOK_IncrementTotal(myGPIOK_dev_ptr);
/** <h5>Wakeup dei processi sleeping</h5>
 * La ISR deve chiamare esplicitamente wakeup() per risvegliare i processi messi in sleeping in attesa che
 * un particolare evento si manifestasse.
 * Se due processi vengono risvegliati contemporaneamente potrebbero originarsi race-condition.
 */
	myGPIOK_WakeUp(myGPIOK_dev_ptr);
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
		myGPIOK_TestCanReadAndSleep(myGPIOK_dev_ptr);
/**<h5>Reset del flag "interrupt occurred" per read() bloccanti</h5>
 * Nel momento in cui il processo viene risvegliato e la condizione della quale era in attesa e' tale che
 * esso puo' continuare la sua esecuzione, e' necessario resettare tale flag. Questa operazione va effettuata
 * per prevenire race-condition dovute al risveglio di piu' processi in attesa del manifestarsi dello stesso
 * evento. Il reset del flag va, pertanto, effettuato in mutua esclusione.
 */
		myGPIOK_ResetCanRead(myGPIOK_dev_ptr);
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
	read_addr = myGPIOK_GetDeviceAddress(myGPIOK_dev_ptr)+*off;
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
	XGpio_Ack_Interrupt(myGPIOK_dev_ptr->vrtl_addr, myGPIOK_dev_ptr->irq_mask);
#else
	myGPIOK_PinInterruptAck(myGPIOK_dev_ptr, myGPIOK_dev_ptr->irq_mask);
#endif
/** <h5>Abilitazione degli interrupt della periferica</h5>
 * Dopo aver inviato notifica di servizio dell'interruzione al device, vengono nuovamente abilitati gli interrupt.
 */
#ifdef __XGPIO__
	XGpio_Global_Interrupt(myGPIOK_dev_ptr, XGPIO_GIE);
	XGpio_Channel_Interrupt(myGPIOK_dev_ptr, myGPIOK_dev_ptr->irq_mask);
#else
	myGPIOK_GlobalInterruptEnable(myGPIOK_dev_ptr);
	myGPIOK_PinInterruptEnable(myGPIOK_dev_ptr, myGPIOK_dev_ptr->irq_mask);
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
	write_addr = myGPIOK_GetDeviceAddress(myGPIOK_dev_ptr)+*off;
	iowrite32(data_to_write, write_addr);
	return size;
}


/**
 * @}
 * @}
 */
