/**
 * @file uio.c
 * @author Salvatore Barone <salvator.barone@gmail.com>
 * @date 13 06 2017
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
 * @example uio.c
 * Il file uio.c è un programma di esempio per l'interfacciamento con una periferica myGPIO. L'esempio
 * mostra come possa, un programma userspace in esecuzione su sistema operativo Linux, interfacciarsi
 * con un device myGPIO, interagendo con esso attraverso il driver UIO.
 *
 * @warning Se nel device tree source non viene indicato
 * <center>compatible = "generic-uio";</center>
 * tra i driver compatibili con il device, il driver UIO non viene correttamente istanziato ed il
 * programma non funzionerà.
 */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "myGPIO.h"

/**
 * @brief Stampa un messaggio che fornisce indicazioni sull'utilizzo del programma
 */
void howto(void) {
	printf("Uso:\n");
	printf("uio -d /dev/uioX -w|m <hex-value> -r\n");
	printf("\t-m <hex-value>: scrive nel registro \"mode\"\n");
	printf("\t-w <hex-value>: scrive nel registro \"write\"\n");
	printf("\t-r: legge il valore del registro \"read\"\n");
	printf("I parametri possono anche essere usati assieme.\n");
}

/**
 * @brief Effettua il parsing dei parametri passati al programma
 * @param [in] 	argc
 * @param [in] 	argv
 * @param [out] uio_file		file uio da usare
 * @param [out] op_mode			sarà impostato ad 1 se l'utente intende effettuare scrittuara su mode
 * @param [out] mode_value		conterrà il valore che l'utente intende scrivere nel registro mode
 * @param [out] op_write		sarà impostato ad 1 se l'utente intende effettuare scrittuara su write
 * @param [out] write_value		conterrà il valore che l'utente intende scrivere nel registro write
 * @param [out] op_read			sarà impostato ad 1 se l'utente intende effettuare lettura da read
 *
 * @retval 0 se il parsing ha successo
 * @retval -1 se si verifica un errore
 *
 * @details
 */
int parse_args(	int 		argc,
				char		**argv,
				char		**uio,			// file uio da usare
				uint8_t		*op_mode,		// impostato ad 1 se l'utente intende effettuare scrittuara su mode
				uint32_t	*mode_value,	// valore che l'utente intende scrivere nel registro mode
				uint8_t		*op_write,		// impostato ad 1 se l'utente intende effettuare scrittuara su write
				uint32_t	*write_value,	// valore che l'utente intende scrivere nel registro write
				uint8_t		*op_read)		// impostato ad 1 se l'utente intende effettuare lettura da read
{
	int par;
/** <h4>Parsing dei parametri del programma.</h4>
 * Il parsing viene effettuato usando la funzione getopt().
 * @code
 * #include <unistd.h>
 * int getopt(int argc, char * const argv[], const char *optstring);
 * @endcode
 * Essa prende in input i parametri argc ed argv passati alla funzione main() quando il programma viene invocato.
 * Quando una delle stringhe che compongono argv comincia con il carattere '-', getopt() la considera una opzione.
 * Il carattere immediatamente successivo il '-' identifica la particolare opzione.
 * La funzione può essere chiamata ripetutamente, fino a quando non restituisce -1, ad indicare che sono stati
 * analizzati tutti i parametri passati al programma.
 * Quando getopt() trova un'opzione, restituisce quel carattere ed aggiorna la variabile globale optind, che punta
 * al prossimo parametro contenuto in argv.
 * La stringa optstring indica quali sono le opzioni considerate. Se una opzione è seguita da ':' vuol dire che
 * essa è seguita da un argomento. Tale argomento può essere ottenuto mediante la variabile globale optarg.
 *
 * <h4>Parametri riconosciuti</h4>
 * La funzione riconosce i parametri:
 *  - 'd' : seguito dal percordo del device /dev/uioX col quale interagire
 *  - 'w' : operazione di scrittura, seguito dal valore che si intende scrivere, in esadecimale; la scrittura verrà
 *          effettuata sul registro WRITE;
 *  - 'm' : impostazione modalità, seguito dalla modalità col quale impostare il device; la scrittura verrà
 *          effettuata sul registro MODE;
 *  - 'r' : operazione di lettura, primo di argomento; la lettura viene effettuata dal registro READ ed è non
 *          bloccante, nel senso che viene semplicemente letto il contenuto del registro.
 */
	while((par = getopt(argc, argv, "d:w:m:r")) != -1) {
		switch (par) {
		case 'd' :
			*uio = optarg;
			break;
		case 'w' :
			*write_value = strtoul(optarg, NULL, 0);
			*op_write = 1;
			break;
		case 'm' :
			*mode_value = strtoul(optarg, NULL, 0);
			*op_mode = 1;
			break;
		case 'r' :
			*op_read = 1;
			break;
		default :
			printf("%c: parametro sconosciuto.\n", par);
			howto();
			return -1;
		}
	}
	return 0;
}


/**
 * @brief Effettua operazioni su un device
 *
 * @param [in] vrt_gpio_addr	indirizzo di memoria del device gpio
 * @param [in] op_mode			sarà impostato ad 1 se l'utente intende effettuare scrittuara su mode
 * @param [in] mode_value		conterrà il valore che l'utente intende scrivere nel registro mode
 * @param [in] op_write			sarà impostato ad 1 se l'utente intende effettuare scrittuara su write
 * @param [in] write_value		conterrà il valore che l'utente intende scrivere nel registro write
 * @param [in] op_read			sarà impostato ad 1 se l'utente intende effettuare lettura da read
 *
 * @details
 * La funzione viene invocata dopo che sia stato eseguito il parsing dei parametri passati al programma quando
 * esso viene invocato. è stata scritta per funzionare sia con il GPIO Xilinx che con il GPIO custom myGPIO.
 * è possibile utilizzare il primo definendo la macro __XIL_GPIO__. Effettua, sul device, le operazioni
 * impostate, in accordo con i parametri passati al programma alla sua invocazione.
 */
void gpio_op (	void* 		vrt_gpio_addr,
				uint8_t 	op_mode,
				uint32_t	mode_value,
				uint8_t		op_write,
				uint32_t	write_value,
				uint8_t		op_read)
{
	printf("Indirizzo gpio: %08x\n", (uint32_t)vrt_gpio_addr);

	#ifdef __XIL_GPIO__
#define MODE_OFFSET		4U
#define WRITE_OFFSET	0U
#define READ_OFFSET		8U
#else
	myGPIO_t gpio;
	myGPIO_Init(&gpio, (uint32_t)vrt_gpio_addr);
#endif
/** <h4>Impostazione della modalità di funzionamento</h4>
 * Nel caso in cui si stia operando su un device GPIO Xilinx, le operazioni di impostazione della modalità di
 * funzionamento del GPIO vengono effettuate scrivendo direttamente sul registro MODE del device. In caso contrario
 * si è preferito utilizzare la funzioni myGPIO_setMode() (Si veda il modulo myGPIO). Funzionalmente non c'è
 * differenza.
 */
	if (op_mode == 1) {
#ifdef __XIL_GPIO__
		*((uint32_t*)(vrt_gpio_addr+MODE_OFFSET)) = mode_value;
		mode_value = *((uint32_t*)(vrt_gpio_addr+MODE_OFFSET));
#else
		myGPIO_SetMode(&gpio, mode_value, myGPIO_write);
		myGPIO_SetMode(&gpio, ~mode_value, myGPIO_reset);
#endif
		printf("Scrittura sul registro mode: %08x\n", mode_value);
	}
/** <h4>Operazione di scrittura</h4>
 * Nel caso in cui si stia operando su un device GPIO Xilinx, le operazioni di scrittura del valore dei pin
 * del device GPIO vengono effettuate scrivendo direttamente sul registro WRITE del device. In caso contrario
 * si è preferito utilizzare la funzioni myGPIO_setValue() (Si veda il modulo myGPIO). Funzionalmente non c'è
 * differenza.
 */
	if (op_write == 1) {
#ifdef __XIL_GPIO__
		*((uint32_t*)(vrt_gpio_addr+WRITE_OFFSET)) = write_value;
		write_value = *((uint32_t*)(vrt_gpio_addr+WRITE_OFFSET));
#else
		myGPIO_SetValue(&gpio, write_value, myGPIO_set);
		myGPIO_SetValue(&gpio, ~write_value, myGPIO_reset);
#endif
		printf("Scrittura sul registro write: %08x\n", write_value);
	}
/** <h4>Operazione di lettura</h4>
 * Nel caso in cui si stia operando su un device GPIO Xilinx, le operazioni di lettura del valore dei pin
 * del device GPIO vengono effettuate leggendo direttamente dal registro READ del device. In caso contrario
 * si è preferito utilizzare la funzioni myGPIO_getRead() (Si veda il modulo myGPIO). Funzionalmente non c'è
 * differenza. La lettura è non bloccante: viene semplicemente letto il valore contenuto nel registro.
 * UIO permette l'implementazione di meccanismi di lettura basati su interruzione, ma in questo caso, per
 * semplicità, tale meccanismo è stato omesso. Si veda il modulo UIO-interrupt.
 */
	if (op_read == 1) {
		uint32_t read_value = 0;
#ifdef __XIL_GPIO__
		read_value = *((uint32_t*)(vrt_gpio_addr+READ_OFFSET));
#else
		read_value = myGPIO_GetRead(&gpio);
#endif
		printf("Lettura dat registro read: %08x\n", read_value);
	}
}

/**
 * @brief funzione main().
 *
 * @details
 */
int main(int argc, char** argv) {
	char* uio_file = 0;			// nome del file uio
	uint8_t op_mode = 0;		// impostato ad 1 se l'utente intende effettuare scrittuara su mode
	uint32_t mode_value;		// valore che l'utente intende scrivere nel registro mode
	uint8_t op_write = 0;		// impostato ad 1 se l'utente intende effettuare scrittuara su write
	uint32_t write_value;		// valore che l'utente intende scrivere nel registro write
	uint8_t op_read = 0;		// impostato ad 1 se l'utente intende effettuare lettura da read

	printf("%s build %d\n", argv[0], BUILD); // BUILD viene definita in compilazione

/** <h4>Parsing dei parametri di invocazione</h4>
 * Il parsing dei parametri passati al programma all'atto della sua invocazione viene effettuato dalla funzione
 * parse_args(). Si rimanda alla sua documentazione per i dettagli sui parametri riconosciuti.
 */
	if (parse_args(argc, argv, &uio_file, &op_mode, &mode_value, &op_write, &write_value, &op_read) == -1)
		return -1;
/**
 * Se non viene specificato il device UIO col quale interagire è impossibile continuare.
 * Per questo motivo, in questo caso, il programma viene terminato.
 */
	if (uio_file == 0) {
		printf("è necessario specificare il device UIO col quale interagire.\n");
		howto();
		return -1;
	}
/** <h4>Accesso ad un device /dev/uioX</h4>
 * Il driver generic-UIO è il driver generico per eccellenza. Ad ogni periferica compatibile con
 * UIO è associato un file diverso in /dev/uioX attraverso il quale è possibile raggiungere il device.
 * Tale file sarà /dev/uio0 per il primo device, /dev/uio1 per il secondo, /dev/uio2 per il terzo e così via.
 * on for subsequent devices. Tale file può essere usato per accedere allo spazio degli indirizzi
 * del device usando mmap().
 *
 * In questo caso, rispetto all'esempio noDriver, accedere al device è estremamente più semplice.
 * Se il device è compatibile con il driver UIO, è possibile "aprire" un file in /dev/uioX,
 * effettuare il mapping, connettendo il device allo spazio di indirizzamento del processo, senza la
 * necessità di conoscere l'indirizzo fisico della periferica col quale di intende comunicare.
 *
 * L'accesso al device /dev/uioX viene ottenuto mediante la system-call open():
 * @code
 * #include <sys/stat.h>
 * #include <fcntl.h>
 * int open(const char *path, int oflag, ...  );
 * @endcode
 * la quale restituisce il descrittore del file /dev/uioX, usato nel seguito per effettuare le operazioni
 * di I/O. I valori del parametro oflag specificano il modo in cui il file /dev/uioX viene aperto. In questo
 * caso viene usato O_RDWR, il quale garantisce accesso in lettura ed in scrittura. Altri valori sono O_RDONLY,
 * il quale garantisce accesso in sola lettura, ed O_WRONLY, che, invece, garantisce accesso in sola scrittura.
 */
	int descriptor = open (uio_file, O_RDWR);
	if (descriptor < 1) {
		perror(argv[0]);
		return -1;
	}
/** <h4>Mapping un device /dev/uioX</h4>
 * La "conversione" dell'indirizzo fisico del device in indirizzo virtuale appartenente allo spazio di
 * indirizzamento del processo viene effettuato tramite la chiamata alla funzione mmap(), la quale stabilisce
 * un mapping tra lo spazio di indirizzamento di un processo ed un file, una porzione di memoria condivisa o
 * un qualsiasi altro memory-object, restituendo un indirizzo virtuale valido, attraverso il quale è possibile
 * accedere al blocco di memoria fisico.
 * @code
 *		#include <sys/mman.h>
 *		void *mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off);
 * @endcode
 * Per semplicità supponiamo che la chiamata alla funzione sia la seguente:
 *               <center>pa=mmap(addr, len, prot, flags, fildes, off);</center>
 * la semantica dei diversi parametri è:
 * 	- pa: indirizzo virtuale dell'address-space locale del processo, a cui viene eseguito il map; se il mapping
 * 	  ha successo viene restituito qualcosa di diverso da MAP_FAILED;
 * 	- addr:
 * 	- len: lunghezza, in byte, del blocco mappato; in questo caso viene usato il valore restituito da
 * 	  sysconf(_SC_PAGESIZE);
 * 	- prot: specifica i permessi di accesso al blocco di memoria del quale si sta facendo il mapping;
 * 		- PROT_READ indica che il blocco può essere letto;
 * 		- PROT_WRITE indica che il blocco può essere scritto;
 * 		- PROT_NONE sta ad indicare che il blocco non può essere acceduto;
 * 	    .
 * 	- flags:fornisce informazioni aggiuntive circa la gestione del blocco di dati di cui si sta facendo il
 * 	  mapping; il valore del flag può essere uno dei seguenti:
 * 	  	- MAP_SHARED: modifiche al blocco sono condivise con chiunque altri lo stia usando;
 * 	  	- MAP_PRIVATE: le modifiche sono primate;
 *		.
 * 	- filedes: descrittore del file /dev/mem
 * 	- off: indirizzo fisico del blocco che si intente mappare; è necessario che sia allineato alla dimensione
 * 	  della pagina di memoria, così come restituito dalla funzione sysconf(_SC_PAGESIZE);
 *
 * In questo caso la chiamata a mmap avviene con i seguenti parametri:
 * @code
 * uint32_t page_size = sysconf(_SC_PAGESIZE);		// dimensione della pagina
 * void* vrt_gpio_addr = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, descriptor, 0);
 * @endcode
 *
 * Rispetto al "driver" nodriver, la chiamata differisce per un solo perticolare: essendo descriptor il
 * descrittore di uioX, e l'offset specificato nullo, la funzione restituisce direttamente l'indirizzo virtuale
 * del device nello spazio di indirizzamento del processo.
 */
	uint32_t page_size = sysconf(_SC_PAGESIZE);		// dimensione della pagina
	void* vrt_gpio_addr = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, descriptor, 0);
	if (vrt_gpio_addr == MAP_FAILED) {
		printf("Mapping indirizzo fisico - indirizzo virtuale FALLITO!\n");
		return -1;
	}
/** <h4>Operazioni sul device</h4>
 * Una volta effettuato il mapping, le operazioni preventivate con l'invocazione del programma vengono effettuate
 * dalla funzione gpio_op(). Si rimanda alla sua documentazione per i dettagli sulle operazioni effettuate().
 */
	gpio_op(vrt_gpio_addr, op_mode, mode_value, op_write, write_value, op_read);

	munmap(vrt_gpio_addr, page_size);
	close(descriptor);

	return 0;
}
