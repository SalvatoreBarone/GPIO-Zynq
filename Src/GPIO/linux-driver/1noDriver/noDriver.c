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

void howto(void);

int parse_args(	int argc,
				char** argv,
				uint32_t *gpio_address,		// indirizzo di memoria del device gpio
				uint8_t *op_mode,			// impostato ad 1 se l'utente intende effettuare scrittuara su mode
				uint32_t *mode,				// valore che l'utente intende scrivere nel registro mode
				uint8_t *op_write,			// impostato ad 1 se l'utente intende effettuare scrittuara su write
				uint32_t *write,			// valore che l'utente intende scrivere nel registro write
				uint8_t *op_read);			// impostato ad 1 se l'utente intende effettuare lettura da read)



int main(int argc, char** argv) {
	uint32_t gpio_address = 0;	// indirizzo di memoria del device gpio
	uint8_t op_mode = 0;		// impostato ad 1 se l'utente intende effettuare scrittuara su mode
	uint32_t mode;				// valore che l'utente intende scrivere nel registro mode
	uint8_t op_write = 0;		// impostato ad 1 se l'utente intende effettuare scrittuara su write
	uint32_t write;				// valore che l'utente intende scrivere nel registro write
	uint8_t op_read = 0;		// impostato ad 1 se l'utente intende effettuare lettura da read
	uint32_t read = 0;			// valore letto dal registro read del device

	if (parse_args(argc, argv, &gpio_address, &op_mode, &mode, &op_write, &write, &op_read) == -1)
		return -1;

	if (gpio_address == 0) {
		printf("E' necessario specificare l'indirizzo di memoria del device.\n");
		howto();
	}
	else {
		printf("Indirizzo gpio: %08x\n", gpio_address);
		if (op_mode == 1) {
			printf("Scrittura sul registro mode: %08x\n", mode);
		}

		if (op_write == 1) {
			printf("Scrittura sul registro write: %08x\n", write);
		}

		if (op_read == 1) {
			printf("Lettura dat registro read: %08x\n", read);
		}
	}

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

int parse_args(	int argc,
				char** argv,
				uint32_t *gpio_address,		// indirizzo di memoria del device gpio
				uint8_t *op_mode,			// impostato ad 1 se l'utente intende effettuare scrittuara su mode
				uint32_t *mode,				// valore che l'utente intende scrivere nel registro mode
				uint8_t *op_write,			// impostato ad 1 se l'utente intende effettuare scrittuara su write
				uint32_t *write,			// valore che l'utente intende scrivere nel registro write
				uint8_t *op_read)			// impostato ad 1 se l'utente intende effettuare lettura da read)
{
	char par;
	// parsing dei parametri del programma
	while((par = getopt(argc, argv, "a:w:m:rh")) != -1) {
		switch (par) {
		case 'a' :
			*gpio_address = strtoul(optarg, NULL, 0);
			break;
		case 'w' :
			*write = strtoul(optarg, NULL, 0);
			*op_write = 1;
			break;
		case 'm' :
			*mode = strtoul(optarg, NULL, 0);
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
