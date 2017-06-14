/**
 * @file uio-int.c
 * @author Salvatore Barone <salvator.barone@gmail.com>
 * @date 14 06 2017
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
 * @brief Questo e' un programma di esempio per l'interfacciamento con una periferica myGPIO.
 *
 * In questo specifico esempio l'interfacciamento avviene da user-space, interagendo attraverso il
 * driver uio. Utilizza gli interrupt per la lettura.
 */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "myGPIO.h"
#include "xil_gpio.h"

void howto(void);

int parse_args(	int 		argc,
				char		**argv,
				char		**uio_file,		// file uio da usare
				uint8_t		*op_mode,		// impostato ad 1 se l'utente intende effettuare scrittuara su mode
				uint32_t	*mode_value,	// valore che l'utente intende scrivere nel registro mode
				uint8_t		*op_write,		// impostato ad 1 se l'utente intende effettuare scrittuara su write
				uint32_t	*write_value,	// valore che l'utente intende scrivere nel registro write
				uint8_t		*op_read);		// impostato ad 1 se l'utente intende effettuare lettura da read

void gpio_op (	void* 		vrt_gpio_addr,	// indirizzo di memoria del device gpio
				int			uio_descriptor,	// descrittore del file /dev/uioX usato
				uint8_t 	op_mode,		// impostato ad 1 se l'utente intende effettuare scrittuara su mode
				uint32_t	mode_value,		// valore che l'utente intende scrivere nel registro mode
				uint8_t		op_write,		// impostato ad 1 se l'utente intende effettuare scrittuara su write
				uint32_t	write_value,	// valore che l'utente intende scrivere nel registro write
				uint8_t		op_read);		// impostato ad 1 se l'utente intende effettuare lettura da read



int main(int argc, char** argv) {
	char* uio_file = 0;			// nome del file uio
	uint8_t op_mode = 0;		// impostato ad 1 se l'utente intende effettuare scrittuara su mode
	uint32_t mode_value;		// valore che l'utente intende scrivere nel registro mode
	uint8_t op_write = 0;		// impostato ad 1 se l'utente intende effettuare scrittuara su write
	uint32_t write_value;		// valore che l'utente intende scrivere nel registro write
	uint8_t op_read = 0;		// impostato ad 1 se l'utente intende effettuare lettura da read

	if (parse_args(argc, argv, &uio_file, &op_mode, &mode_value, &op_write, &write_value, &op_read) == -1)
		return -1;

	if (uio_file == 0) {
		printf("E' necessario specificare l'indirizzo di memoria del device.\n");
		howto();
		return -1;
	}

	/* In questo caso accedere al device e' estremamente piu' semplice.
	 * Se il device e' compatibile con il driver UIO, e' possibile "aprire" un file in /dev/uioX,
	 * effettuare il mapping connettendo l'indirizzo fisico del device allo spazio di indirizzamento
	 * del processo, senza la necessita' di conoscere l'indirizzo della periferica col quale di intende
	 * comunicare.
	 * Ad ogni periferica compatibile con UIO e' associato un file diverso in /dev/uioX attraverso il
	 * quale e' possibile raggiungere il device.
	 *
	 */
	int descriptor = open (uio_file, O_RDWR);
	if (descriptor < 1) {
		perror(argv[0]);
		return -1;
	}

	/* Dopodiche' e' possibile connettere l'indirizzo fisico del device allo spazio di indirizzamento
	 * del processo.
	 *
	 *		#include <sys/mman.h>
	 *		void *mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off);
	 *
	 * The mmap() function shall establish a mapping between a process' address space and a file, shared
	 * memory object, or  typed memory object.  The format of the call is as follows:
	 *               pa=mmap(addr, len, prot, flags, fildes, off);
	 *
	 * 	- pa: 		indirizzo virtuale dell'address-space locale del processo, a cui viene eseguito il map.
	 * 		  		Se il mapping ha successo viene restituito qualcosa di diverso da MAP_FAILED
	 * 	- addr:
	 *
	 * 	- len:		lunghezza del blocco mappato
	 *
	 * 	- prot:		The  parameter  prot determines whether read, write, execute, or some combination of
	 * 				accesses are permitted to the data being mapped.  The prot shall be either PROT_NONE
	 * 				or the bit‐wise-inclusive OR of one or more of the other flags in the following table,
	 * 				defined in the <sys/mman.h> header.
	 * 				   Symbolic Constant   Description
	 * 				   PROT_READ           Data can be read.
	 * 				   PROT_WRITE          Data can be written.
	 * 				   PROT_EXEC           Data can be executed.
	 * 				   PROT_NONE           Data cannot be accessed.
	 *
	 * 	- flags:	The parameter flags provides other information about the handling of the mapped data.
	 * 				The value of flags is the bitwise-inclusive OR of these options, defined in <sys/mman.h>:
	 * 				   Symbolic Constant   Description
	 * 				   MAP_SHARED          Changes are shared.
	 * 				   MAP_PRIVATE         Changes are private.
	 * 				   MAP_FIXED           Interpret addr exactly.
	 *
	 * 	- filedes:	descrittore del file uio
	 *
	 * 	- off:		indirizzo fisico del blocco che si intente mappare
	 * 				The  off  argument  is  constrained to be aligned and sized according to the value returned
	 * 				by sysconf() when passed _SC_PAGESIZE or _SC_PAGE_SIZE.
	 *
	 *
	 * Rispetto al "driver" nodriver, la chiamata differisce per un solo perticolare: essendo descriptor il
	 * descrittore di uioX, e l'offset specificato nullo, la funzione restituisce direttamente l'indirizzo
	 * virtuale del device nello spazio di indirizzamento del processo.
	 */
	uint32_t page_size = sysconf(_SC_PAGESIZE);		// dimensione della pagina
	void* vrt_gpio_addr = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, descriptor, 0);
	if (vrt_gpio_addr == MAP_FAILED) {
		printf("Mapping indirizzo fisico - indirizzo virtuale FALLITO!\n");
		return -1;
	}

	gpio_op(vrt_gpio_addr, descriptor, op_mode, mode_value, op_write, write_value, op_read);

	munmap(vrt_gpio_addr, page_size);
	close(descriptor);

	return 0;
}


void howto(void) {
	printf("Uso:\n");
	printf("uio -d /dev/uioX -w|m <hex-value> -r\n");
	printf("\t-m <hex-value>: scrive nel registro \"mode\"\n");
	printf("\t-w <hex-value>: scrive nel registro \"write\"\n");
	printf("\t-r: legge il valore del registro \"read\"\n");
	printf("I parametri possono anche essere usati assieme.\n");
}

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
	/* Parsing dei parametri del programma.
	 * 	#include <unistd.h>
	 *
     *  int getopt(int argc, char * const argv[], const char *optstring);
     *
     * The  getopt()  function parses the command-line arguments.  Its arguments argc and argv are
     * the argument count and array as passed to the main() function on program invocation.  An
     * element of argv that starts with '-' (and is not exactly "-" or "--") is an option element.
     * The characters of this element (aside from the initial '-') are option characters.   If
     * getopt() is called repeatedly, it returns successively each of the option characters from each
     * of the option elements.
     *
     * If getopt() finds another option character, it returns that character, updating the external
     * variable optind and a static variable nextchar so that the next call to getopt() can resume
     * the scan with the following option character or argv-element. If there are no more option
     * characters, getopt() returns -1.
     *
     * optstring is a string containing the legitimate option characters.  If such a character is
     * followed by a colon, the option requires an argument, so getopt() places a pointer to the
     * following text in the same argv-element, or the text of the following argv-element, in optarg.
     * Two colons mean an option takes an optional arg;
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

void gpio_op (	void* 		vrt_gpio_addr,
				int			uio_descriptor,
				uint8_t 	op_mode,
				uint32_t	mode_value,
				uint8_t		op_write,
				uint32_t	write_value,
				uint8_t		op_read)
{
#ifndef __XIL_GPIO__
	myGPIO_t gpio;
	myGPIO_init(&gpio, vrt_gpio_addr);
#endif


	printf("Indirizzo gpio: %08x\n", (uint32_t)vrt_gpio_addr);
	if (op_mode == 1) {
#ifdef __XIL_GPIO__
		*((uint32_t*)(vrt_gpio_addr+GPIO_TRI_OFFSET)) = mode_value;
		mode_value = *((uint32_t*)(vrt_gpio_addr+GPIO_TRI_OFFSET));
#else
		myGPIO_setMode(&gpio, mode_value, myGPIO_write);
		myGPIO_setMode(&gpio, ~mode_value, myGPIO_reset);
#endif
		printf("Scrittura sul registro mode: %08x\n", mode_value);
	}

	if (op_write == 1) {
#ifdef __XIL_GPIO__
		*((uint32_t*)(vrt_gpio_addr+GPIO_DATA_OFFSET)) = write_value;
		write_value = *((uint32_t*)(vrt_gpio_addr+GPIO_DATA_OFFSET));
#else
		myGPIO_setValue(&gpio, write_value, myGPIO_set);
		myGPIO_setValue(&gpio, ~write_value, myGPIO_reset);
#endif
		printf("Scrittura sul registro write: %08x\n", write_value);
	}

	/* E' possibile accedere ad ognuno dei device attraverso un file diverso. Tale file sara' /dev/uio0
	 * per il primo device, /dev/uio1 per il secondo, /dev/uio2 per il terzo e cosi' via.
	 * on for subsequent devices. Tale file puo' essere usato per accedere allo spazio degli indirizzi
	 * del device usando mmap().
	 *
	 * Gli interrupt sono gestiti effettuando una lettura bloccante su /dev/uioX. Una read() su /dev/uioX
	 * fa in modo che il processo venga sospeso ed inserito nella cosa dei processi in attesa di un evento
	 * su quel file. Appena l'interrupt si manifesta, il processo viene posto nella cosa dei processi
	 * pronti. La funzione read() consente di ottenere anche il numero totale di interrupt manifestatisi
	 * su quella particolare periferica.
	 * Quando un device possiede piu' di una sorgente di interrupt interna, ma non possiede maschere IRQ
	 * differenti o registri di stato differenti, potrebbe essere impossibile, per un programma in
	 * userspace, determinare quale sia la sorgente di interrupt se l'handler implementato nel kernel
	 * le disabilita scrivendo nei registri.
	 * Per lasciare inalterati i registri della periferica il kernel deve disabilitare completamente le
	 * interruzioni, in modo che il programma userspace possa determinare la causa scatenante l'interruzione.
	 * Una volta terminate le operazioni, però, il programma userspace non puo' riabilitare le interruzioni,
	 * motivo per cui il driver implementa anche una funzione write().
	 * La funzione write(), chiamata su /dev/uioX, consente di riabilitare le interruzioni per quella
	 * specifica periferica, scrivendo 1.
	 *
	 * NOTA: la parte di codice per il GPIO Xilinx e' stata scritta per hardware configurato in
	 * modo che il channel 1 del gpio fosse connessi esclusivamente ai led,mentre switch e button
	 * fossero connessi al channel 2 dello stesso GPIO.
	 * Il channel 1 ha dimensione 4 bit, mentre il channel 2 e' da 8 bit.
	 *
	 */
	if (op_read == 1) {
		uint32_t read_value = 0;
		// interrupt enable (interni alla periferica)
		#ifdef __XIL_GPIO__
		// (globale + canale 2)
		XilGpio_Global_Interrupt((uint32_t*)vrt_gpio_addr, GLOBAL_INTR_ENABLE);
		XilGpio_Channel_Interrupt((uint32_t*)vrt_gpio_addr, CHANNEL2_INTR_ENABLE);
		#else
		myGPIO_interruptEnable(&gpio);
		#endif

		// attesa dell'arrivo dell'interruzione
		printf("Attesa dell'interruzione\n");
		uint32_t interrupt_count = 1;
		if (read(uio_descriptor, &interrupt_count, sizeof(uint32_t)) != sizeof(uint32_t)) {
			printf("Read error!\n");
			return;
		}

		printf("Interrupt count: %08x\n", interrupt_count);
		// disabilitazione interrupt (interni alla periferica)
		#ifdef __XIL_GPIO__
		XilGpio_Global_Interrupt((uint32_t*)vrt_gpio_addr, GLOBAL_INTR_DISABLE);
		XilGpio_Channel_Interrupt((uint32_t*)vrt_gpio_addr, CHANNEL2_INTR_DISABLE);
		#else
		myGPIO_interruptDisable(&gpio);
		#endif

		// "servizio" dell'interruzione.
		// lettura del registro
		#ifdef __XIL_GPIO__
		read_value = *((uint32_t*)(vrt_gpio_addr+GPIO_READ_OFFSET));
		#else
		read_value = myGPIO_getRead(&gpio);
		#endif
		printf("Lettura dat registro read: %08x\n", read_value);

		// attesa del rilascio del pusante
		// questo serve, in modo particolare, per il GPIO Xilinx, per il quale,
		// stranamente, viene generata una interruzione sia alla pressione che al
		// rilascio di uno dei button
		#ifdef __XIL_GPIO__
        while(*((uint32_t*)(vrt_gpio_addr+GPIO_READ_OFFSET))!=0);
		#else
        while(myGPIO_getRead(&gpio) != 0);
		#endif

		// invio dell'ack alla periferica
		#ifdef __XIL_GPIO__
		XilGpio_Ack_Interrupt((uint32_t*)vrt_gpio_addr, CHANNEL2_ACK);
		#else
		myGPIO_interruptAck(&gpio);
		#endif

		// riabilito le interruzioni "uio", scrivendo 1
		uint32_t reenable = 1;
		if (write(uio_descriptor, (void*)&reenable, sizeof(uint32_t)) != sizeof(uint32_t)) {
			printf("Write error!\n");
			return;
		}

	}
}
