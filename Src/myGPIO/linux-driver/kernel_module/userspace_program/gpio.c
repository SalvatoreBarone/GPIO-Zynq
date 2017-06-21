/**
 * @file gpio.c
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
 * @defgroup Userspace-program
 * @{
 *
 * @brief Programma di esempio per l'interfacciamento con una periferica myGPIO attraverso un driver kernel.
 *
 * In questo specifico esempio l'interfacciamento avviene da user-space, interagendo attraverso il
 * driver myGPIOK.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "myGPIO.h"
#include "xil_gpio.h"

#ifdef __XIL_GPIO__
#define MODE_OFFSET		GPIO_TRI_OFFSET
#define WRITE_OFFSET	GPIO_DATA_OFFSET
#define READ_OFFSET		GPIO_READ_OFFSET
#else
#define MODE_OFFSET		myGPIO_MODE_OFFSET
#define WRITE_OFFSET	myGPIO_WRITE_OFFSET
#define READ_OFFSET		myGPIO_READ_OFFSET
#endif

/**
 * @brief Stampa un messaggio che fornisce indicazioni sull'utilizzo del programma
 */
void howto(void);

/**
 * @brief La struttura raccoglie tutti i parametri di esecuzione del programma.
 */
typedef struct {
	int 		dev_descr;		//!< device descriptor
	uint8_t		op_mode;		//!< impostato ad 1 se l'utente intende effettuare scrittuara su mode
	uint32_t	mode_value;		//!< valore che l'utente intende scrivere nel registro mode
	uint8_t		op_write;		//!< impostato ad 1 se l'utente intende effettuare scrittuara su write
	uint32_t	write_value;	//!< valore che l'utente intende scrivere nel registro write
	uint8_t		op_read;		//!< impostato ad 1 se l'utente intende effettuare lettura da read
} param_t;

/**
 * @brief Effettua il parsing dei parametri passati al programma
 *
 * @param [in] 	argc
 * @param [in] 	argv
 * @param [out] param	puntatore a struttura param_t, conterra' i vari parametri di esecuzione
 * 						del programma.
 *
 * @retval 0 se il parsing ha successo
 * @retval -1 se si verifica un errore
 */
int parse_args(	int argc, char **argv, param_t	*param);

/**
 * @brief Effettua operazioni su un device
 *
 * @param [in] param	puntatore a struttura param_t, contiene i vari parametri di esecuzione
 * 						del programma.
 */
void gpio_op (param_t *param);


int main (int argc, char **argv) {
	param_t param;

	if (parse_args(argc, argv, &param) == -1)
		return -1;

	gpio_op(&param);

	close(param.dev_descr);

	return 0;
}

/*============================================================================================================
 * Definizione funzioni
 */

void howto(void) {
	printf("Uso:\n");
	printf("gpio -d /dev/device -w|m <hex-value> -r\n");
	printf("\t-m <hex-value>: scrive nel registro \"mode\"\n");
	printf("\t-w <hex-value>: scrive nel registro \"write\"\n");
	printf("\t-r: legge il valore del registro \"read\"\n");
	printf("I parametri possono anche essere usati assieme.\n");
}

int parse_args(	int argc, char **argv, param_t	*param) {
	int par;
	char* devfile = NULL;
	while((par = getopt(argc, argv, "d:w:m:r")) != -1) {
		switch (par) {
		case 'd' :
			devfile = optarg;
			break;
		case 'w' :
			param->write_value = strtoul(optarg, NULL, 0);
			param->op_write = 1;
			break;
		case 'm' :
			param->mode_value = strtoul(optarg, NULL, 0);
			param->op_mode = 1;
			break;
		case 'r' :
			param->op_read = 1;
			break;
		default :
			printf("%c: parametro sconosciuto.\n", par);
			howto();
			return -1;
		}
	}

	if (devfile == NULL) {
		printf ("E' necessario specificare il device col quale interagire!\n");
		howto();
		return -1;
	}

	param->dev_descr = open(devfile, O_RDWR);
	if (param->dev_descr < 1) {
		perror(devfile);
		return -1;
	}
	return 0;
}

void gpio_op (param_t *param) {

	printf("MODE_OFFSET  : %X\n", MODE_OFFSET);
	printf("WRITE_OFFSET : %X\n", WRITE_OFFSET);
	printf("READ_OFFSET  : %X\n", READ_OFFSET);

	if (param->op_mode == 1) {
		printf("Scrittura sul registro mode: %08x\n", param->mode_value);
#ifndef __USE_PWRITE__
		lseek(param->dev_descr, MODE_OFFSET, SEEK_SET);
		write(param->dev_descr, &(param->mode_value), sizeof(uint32_t));
#else
		pwrite(param->dev_descr, &(param->mode_value), sizeof(uint32_t), MODE_OFFSET);
#endif
	}

	if (param->op_write == 1) {
		printf("Scrittura sul registro write: %08x\n", param->write_value);
#ifndef __USE_PWRITE__
		lseek(param->dev_descr, WRITE_OFFSET, SEEK_SET);
		write(param->dev_descr, &(param->write_value), sizeof(uint32_t));
#else
		pwrite(param->dev_descr, &(param->mode_value), sizeof(uint32_t), WRITE_OFFSET);
#endif
	}

	if (param->op_read == 1) {
		uint32_t read_value = 0;
#ifndef __USE_PREAD__
		lseek(param->dev_descr, READ_OFFSET, SEEK_SET);
		read(param->dev_descr, &read_value, sizeof(uint32_t));
#else
		pread(param->dev_descr, &read_value, sizeof(uint32_t), READ_OFFSET);
#endif
		printf("Lettura dal registro read: %08x\n", read_value);
	}
}

/**
 * @}
 * @}
 * @}
 * @}
 */

