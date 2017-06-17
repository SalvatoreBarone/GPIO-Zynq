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
 * <h2>Inizializzazione del driver - probe()</h2>
 *
 *
 * <h3>Corrispondenza tra funzioni kernel e funzioni driver.</h3>
 *
 * Essendo un device "visto" come un file, ogni device driver deve implementare tutte le
 * system-call previste per l'interfacciamento con un file. La corrispondenza tra la
 * system-call e la funzione fornita dal driver viene stabilita attraverso la struttura
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
 *
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
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/types.h>

#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/mod_devicetable.h>

#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>

/*================================ Sezione di inizializzazione del modulo ===================================*/

/**
 * @brief Viene chiamata quando il modulo viene inserito.
 *
 * Inizializza il driver e la periferica.
 *
 * @retval 0 nel caso in cui non si sia verificato nessun errore;
 * @retval -1 nel caso
 */
static int myGPIOK_probe(struct platform_device *op) {
	printk(KERN_INFO "Chiamata myGPIOK_probe\n");



	return 0;
}

/**
 * @breif Viene chiamata automaticamente alla rimozione del mosulo.
 *
 * Dealloca tutte la memoria utilizzata dal driver.
 */
static int myGPIOK_remove(struct platform_device *op) {
	printk(KERN_INFO "Chiamata myGPIOK_remove\n");
	return 0;
}

/**
 * @brief Nome identificativo del device-driver.
 * DEVE corrispondere al valore del campo "compatible" nel device tree source.
 */
#define DRIVER_NAME "myGPIOK"

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
 * @Inserisce una nuova entry nella tabella delle corrispondenze device - driver.
 * @param [inout]	of 				riferimento alla tabella
 * @param [in]		myGPIOk_match	struttura of_device_id
 */
MODULE_DEVICE_TABLE(of, myGPIOk_match);

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
 * la macro module_platform_driver() prende in input la struttura platform_driver ed implementa le
 * funzioni module_init() e module_close() standard, chiamate quando il modulo viene caricato o
 * rimosso dal kernel.
 *
 * @param myGPIOK_driver struttura platform_driver associata al driver
 */
module_platform_driver(myGPIOK_driver);

/*================================ Sezione di implementazione del modulo ===================================*/


/**
 * @brief definisce la corrispondenza tra funzioni kernel e funzioni driver.
 *
 * Essendo un device "visto" come un file, ogni device driver deve implementare tutte le
 * system-call previste per l'interfacciamento con un file. La corrispondenza tra la
 * system-call e la funzione fornita dal driver viene stabilita attraverso tale struttura.
 */
static struct file_operations myGPIO_fops = {
		.owner		= THIS_MODULE,
		.llseek		= myGPIOK_seek,
		.read		= myGPIOK_read,
		.write		= myGPIOK_write,
		.poll		= myGPIOK_poll,
		.open		= myGPIOK_open,
		.release	= myGPIOK_release
};

/**
 * @brief usato per cambiare la posizione della "testina" di lettura/scrittura in un file.
 *
 * @param file
 * @param off
 * @param whence
 *
 * @return La funzione restituisce la nuova posizione della testina o -1 nel caso in cui si
 * verifichi un errore.
 */
static loff_t myGPIOK_seek (struct file *file, loff_t off, int whence) {
	printk(KERN_INFO "Chiamata myGPIOK_seek\n");
	return 0;
}

/**
 * @brief legge dati dal device.
 *
 * @param file
 * @param buf
 * @param count
 * @param ppos
 *
 * @return La funzione restituisce il numero di byte letti con successo ma, nel caso si
 * verifichi un errore, restituisce un numero intero negativo.
 */
static ssize_t myGPIOK_read (struct file *file, char *buf, size_t count, loff_t *ppos) {
	printk(KERN_INFO "Chiamata myGPIOK_read\n");
	return 0;
}

/**
 * @brief Invia dati al device
 *
 * @param file
 * @param buf
 * @param size
 * @param off
 *
 * @return Il valore di ritorno, se non negativo, rappresenta il numero di byte
 * correttamente scritti.
 */
static ssize_t myGPIOK_write (struct file *file, const char *buf, size_t size, loff_t *off) {
	printk(KERN_INFO "Chiamata myGPIOK_write\n");
	return 0;
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
static unsigned int myGPIOK_poll (struct file *file, struct poll_table_struct *wait) {
	printk(KERN_INFO "Chiamata myGPIOK_poll\n");
	return 0;
}

/**
 * @brief Invocata all'apertura del file corrispondente al device.
 *
 * @param inode
 * @param file
 *
 * @return
 */
static int myGPIOK_open(struct inode *inode, struct file *file) {
	printk(KERN_INFO "Chiamata myGPIOK_open\n");
	return 0;
}

/**
 * @brief Invocata alla chiusura del file corrispondente al device.
 *
 * @param inode
 * @param file
 *
 * @return
 */
static int myGPIOK_release(struct inode *inode, struct file *file) {
	printk(KERN_INFO "Chiamata myGPIOK_release\n");
	return 0;
}

/**
 * @}
 * @}
 * @}
 * @}
 */


/*=========================================================================================================*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Salvatore Barone <salvator.barone@gmail.com>");
MODULE_DESCRIPTION("myGPIO device-driver in kernel mode");
MODULE_VERSION("0.1");
MODULE_ALIAS(DRIVER_NAME);

