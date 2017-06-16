/**
 * @file noDriver.c
 * @author Salvatore Barone <salvator.barone@gmail.com>
 * @date 12 06 2017
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
 * In questo specifico esempio l'interfacciamento avviene da user-space, agendo direttamente sui registri
 * di memoria, senza mediazione di altri driver.
 */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "myGPIO.h"

void howto(void);

int parse_args(	int 		argc,
				char		**argv,
				uint32_t	*gpio_address,	// indirizzo di memoria del device gpio
				uint8_t		*op_mode,		// impostato ad 1 se l'utente intende effettuare scrittuara su mode
				uint32_t	*mode_value,	// valore che l'utente intende scrivere nel registro mode
				uint8_t		*op_write,		// impostato ad 1 se l'utente intende effettuare scrittuara su write
				uint32_t	*write_value,	// valore che l'utente intende scrivere nel registro write
				uint8_t		*op_read);		// impostato ad 1 se l'utente intende effettuare lettura da read

void gpio_op (	void* 		vrt_gpio_addr,	// indirizzo di memoria del device gpio
				uint8_t 	op_mode,		// impostato ad 1 se l'utente intende effettuare scrittuara su mode
				uint32_t	mode_value,		// valore che l'utente intende scrivere nel registro mode
				uint8_t		op_write,		// impostato ad 1 se l'utente intende effettuare scrittuara su write
				uint32_t	write_value,	// valore che l'utente intende scrivere nel registro write
				uint8_t		op_read);		// impostato ad 1 se l'utente intende effettuare lettura da read



int main(int argc, char** argv) {
	uint32_t gpio_addr = 0;		// indirizzo di memoria del device gpio
	uint8_t op_mode = 0;		// impostato ad 1 se l'utente intende effettuare scrittuara su mode
	uint32_t mode_value;		// valore che l'utente intende scrivere nel registro mode
	uint8_t op_write = 0;		// impostato ad 1 se l'utente intende effettuare scrittuara su write
	uint32_t write_value;		// valore che l'utente intende scrivere nel registro write
	uint8_t op_read = 0;		// impostato ad 1 se l'utente intende effettuare lettura da read

	if (parse_args(argc, argv, &gpio_addr, &op_mode, &mode_value, &op_write, &write_value, &op_read) == -1)
		return -1;

	if (gpio_addr == 0) {
		printf("E' necessario specificare l'indirizzo di memoria del device.\n");
		howto();
		return -1;
	}

	/* Apertura di /dev/mem
	 *
	 * 		#include <sys/stat.h>
     * 		#include <fcntl.h>
     * 		int open(const char *path, int oflag, ...  );
	 *
	 * The  open()  function shall establish the connection between a file and a file descriptor.
	 * It shall create an open file description that refers to a file and a file descriptor thatrefers
	 * to that open file description. The file descriptor is used by other I/O functions to refer to
	 * that file. The path argument points to a pathname naming the file.
	 *
	 * Values  for oflag are constructed by a bitwise-inclusive OR of flags from the following list,
	 * defined in <fcntl.h>. Applications shall specify exactly one of the first three values (file
	 * access modes) below in the value of oflag:
	 * 		O_RDONLY Open for reading only.
	 * 		O_WRONLY Open for writing only.
	 * 		O_RDWR Open for reading and writing. The result is undefined if this flag is applied to a FIFO.
	 *
	 *
	 *
	 * 		#include <stdio.h>
	 * 		void perror(const char *s);
	 *
	 * The  routine perror() produces a message on the standard error output, describing the last error
	 * encountered during a call to a system or library function.  First (if s is not NULL and *s is not
	 * a null byte ('\0')) the argument string s is printed, followed by a colon and a blank. Then the
	 * message and a new-line.
	 */
	int descriptor = open ("/dev/mem", O_RDWR);
	if (descriptor < 1) {
		perror(argv[0]);
		return -1;
	}

	/* Calcolo dell'indirizzo virtuale del device.
	 * Linux implementa la segregazione della memoria. Vale a dire che un processo puo' accedere solo
	 * agli indirizzo di memoria (virtuali) appartenenti al suo address-space.
	 * Se e' necessario effettuare un accesso ad un indirizzo specifico, bisogna effettuare il mapping
	 * di quell'indirizzo nell'address space del processo.
	 * Linux implementa la paginazione della memoria, quindi l'indirizzo del quale si desidera effettuare
	 * il mapping, apparterra' ad una specifica pagina di memoria. Per sapere a quale pagina  appartenga
	 * l'idirizzo, e' necessario conoscere quale sia la dimensione delle pagine di memoria. Tipicamente
	 * la dimensione delle pagine e' una potenza del due.
	 * Si supponga che l'indirizzo di cui si vuole fare il mapping e' 0x43C002F0 e che la dimensione delle
	 * pagine sia 16KB.
	 * Scrivendo la dimensione delle pagine in esadecimale
	 * 												0x00002000
	 * sottraendo 1
	 * 												0x00001FFF
	 * negando
	 * 												0xFFFFE000
	 * si ottiene una maschera che, posta in and con un indirizzo, restituisce l'indirizzo della pagina di
	 * memoria a cui l'indirizzo appartiene. In questo caso
	 * 										0x43C002F0 & 0xFFFFE000 = 0x43C00000
	 * L'indirizzo della pagina potra' essere usato per il mapping, ma per accedere allo specifico indirizzo
	 * e' necessario calcolarne l'offset, sottraengogli l'indirizzo della pagina. In questo modo, dopo aver
	 * effettuato il mapping, si potra' accedere allo stesso a partire dall'indirizzo virtuale della pagina
	 * stessa.
	 *
	 *
	 *		#include <unistd.h>
	 *		long sysconf(int name);
	 *
	 * POSIX allows an application to test at compile or run time whether certain options are supported,
	 * or what the value is of certain configurable constants or limits.
	 * At run time, one can ask for numerical values using the present function sysconf().
	 * We give the name of the variable, the name of the sysconf() argument used to inquire about its
	 * value, and a short description.
	 * 		PAGESIZE or _SC_PAGESIZE Size of a page in bytes.
	 *
	 *
	 *
	 *
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
	 * 	- filedes:	descrittore del file /dev/mem
	 *
	 * 	- off:		indirizzo fisico del blocco che si intente mappare
	 * 				The  off  argument  is  constrained to be aligned and sized according to the value returned
	 * 				by sysconf() when passed _SC_PAGESIZE or _SC_PAGE_SIZE.
	 *
	 */
	uint32_t page_size = sysconf(_SC_PAGESIZE);		// dimensione della pagina
	uint32_t page_mask = ~(page_size-1);			// maschera di conversione indirizzo -> indirizzo pagina
	uint32_t page_addr = gpio_addr & page_mask;		// indirizzo della "pagina fisica" a cui e' mappato il device
	uint32_t offset = gpio_addr - page_addr;		// offset del device rispetto all'indirizzo della pagina
	// conversione dell'indirizzo fisico in indirizzo virtuale
	void* vrt_page_addr = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, descriptor, page_addr);

	if (vrt_page_addr == MAP_FAILED) {
		printf("Mapping indirizzo fisico - indirizzo virtuale FALLITO!\n");
		return -1;
	}

	void* vrt_gpio_addr = vrt_page_addr + offset;	// indirizzo virtuale del device gpio

	gpio_op(vrt_gpio_addr, op_mode, mode_value, op_write, write_value, op_read);

	munmap(vrt_page_addr, page_size);
	close(descriptor);

	return 0;
}


void howto(void) {
	printf("Uso:\n");
	printf("noDriver -a gpio_phisycal_address -w|m <hex-value> -r\n");
	printf("\t-m <hex-value>: scrive nel registro \"mode\"\n");
	printf("\t-w <hex-value>: scrive nel registro \"write\"\n");
	printf("\t-r: legge il valore del registro \"read\"\n");
	printf("I parametri possono anche essere usati assieme.\n");
}

int parse_args(	int 		argc,
				char		**argv,
				uint32_t	*gpio_address,	// indirizzo di memoria del device gpio
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
	while((par = getopt(argc, argv, "a:w:m:r")) != -1) {
		switch (par) {
		case 'a' :
			*gpio_address = strtoul(optarg, NULL, 0);
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
				uint8_t 	op_mode,
				uint32_t	mode_value,
				uint8_t		op_write,
				uint32_t	write_value,
				uint8_t		op_read)
{
#ifdef __XIL_GPIO__
#define MODE_OFFSET		4U
#define WRITE_OFFSET	0U
#define READ_OFFSET		8U
#else
	myGPIO_t gpio;
	myGPIO_init(&gpio, (uint32_t)vrt_gpio_addr);
#endif



	printf("Indirizzo gpio: %08x\n", (uint32_t)vrt_gpio_addr);
	if (op_mode == 1) {
#ifdef __XIL_GPIO__
		*((uint32_t*)(vrt_gpio_addr+MODE_OFFSET)) = mode_value;
		mode_value = *((uint32_t*)(vrt_gpio_addr+MODE_OFFSET));
#else
		myGPIO_setMode(&gpio, mode_value, myGPIO_write);
		myGPIO_setMode(&gpio, ~mode_value, myGPIO_reset);
#endif
		printf("Scrittura sul registro mode: %08x\n", mode_value);
	}

	if (op_write == 1) {
#ifdef __XIL_GPIO__
		*((uint32_t*)(vrt_gpio_addr+WRITE_OFFSET)) = write_value;
		write_value = *((uint32_t*)(vrt_gpio_addr+WRITE_OFFSET));
#else
		myGPIO_setValue(&gpio, write_value, myGPIO_set);
		myGPIO_setValue(&gpio, ~write_value, myGPIO_reset);
#endif
		printf("Scrittura sul registro write: %08x\n", write_value);
	}

	if (op_read == 1) {
		uint32_t read_value = 0;
#ifdef __XIL_GPIO__
		read_value = *((uint32_t*)(vrt_gpio_addr+READ_OFFSET));
#else
		read_value = myGPIO_getRead(&gpio);
#endif
		printf("Lettura dat registro read: %08x\n", read_value);
	}
}
