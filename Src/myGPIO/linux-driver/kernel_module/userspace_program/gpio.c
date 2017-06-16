#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "myGPIO.h"

/*============================================================================================================
 * Dichiarazione funzioni
 */

void howto(void);

typedef struct {
	int 		dev_descr;		// device descriptor
	uint8_t		op_mode;		// impostato ad 1 se l'utente intende effettuare scrittuara su mode
	uint32_t	mode_value;		// valore che l'utente intende scrivere nel registro mode
	uint8_t		op_write;		// impostato ad 1 se l'utente intende effettuare scrittuara su write
	uint32_t	write_value;	// valore che l'utente intende scrivere nel registro write
	uint8_t		op_read;		// impostato ad 1 se l'utente intende effettuare lettura da read

} param_t;

int parse_args(	int argc, char **argv, param_t	*param);

void gpio_op (param_t *param);

/*============================================================================================================
 * Main
 */

int main (int argc, char **argv) {
	param_t param;

	if (parse_args(argc, argv, &param) == -1)
		return -1;

	gpio_op(&param);

	close(param.dev_descr);
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
	if (param->op_mode == 1) {
		printf("Scrittura sul registro mode: %08x\n", param->mode_value);
	}

	if (param->op_write) {
		printf("Scrittura sul registro write: %08x\n", param->write_value);
	}

		if (param->op_mode == 1) {
		printf("Scrittura sul registro mode: %08x\n", param->mode_value);
	}

	if (param->op_read) {
		uint32_t read_value = 0;
		printf("Scrittura sul registro write: %08x\n", read_value);
	}
}


