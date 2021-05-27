/**
 * @file mygpiok.c
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
 * @example mygpiok.c
 * Il file mygpiok.c contiene un programma di esempio che usa, in modo totalmente trasparente, il
 * modulo kernel myGPIOK, che implementa un driver l'interfacciamento con una periferica myGPIO.
 * Il programma contenuto in mygpiok.c è un programma userspace che mostra come possa, un programma
 * userspace in esecuzione su sistema operativo Linux, interagire con un device myGPIO attraverso i
 * modulo kernel myGPIOK, che deve essere compilato e inserito nel kernel. Si veda la documentazione
 * interna al file mygpiok.c per ulteriori dettagli.
 *
 * @warning Se nel device tree source non viene indicato
 * <center>compatible = "myGPIOK";</center>
 * tra i driver compatibili con il device, il driver myGPIOK non viene correttamente istanziato ed il
 * programma userspace non funzionerà.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <inttypes.h>

#define MODE_OFFSET	  0U
#define WRITE_OFFSET	4U
#define READ_OFFSET		8U

/**
 * @brief Stampa un messaggio che fornisce indicazioni sull'utilizzo del programma
 */
void howto(void) {
	printf("Uso:\n");
	printf("gpio -d /dev/device -w|m <hex-value> -r\n");
	printf("\t-m <hex-value>: scrive nel registro \"mode\"\n");
	printf("\t-w <hex-value>: scrive nel registro \"write\"\n");
	printf("\t-r: legge il valore del registro \"read\"\n");
	printf("I parametri possono anche essere usati assieme.\n");
}

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
 * @param [out] param	puntatore a struttura param_t, conterrà i vari parametri di esecuzione
 * 						del programma.
 *
 * @retval 0 se il parsing ha successo
 * @retval -1 se si verifica un errore
 *
 * @details
 */
int parse_args(	int argc, char **argv, param_t	*param) {
	int par;
	char* devfile = NULL;
/**
 * <h4>Parsing dei parametri del programma.</h4>
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
 * Quando getopt() trova un'opzione, restituisce quel carattere ed aggiorna la variabile globale optid, che punta
 * al prossimo parametro contenuto in argv.
 * La stringa optstring indica quali sono le opzioni considerate. Se una opzione è seguita da ':' vuol dire che
 * essa è seguita da un argomento. Tale argomento può essere ottenuto mediante la variabile globale optarg.
 *
 * <h4>Parametri riconosciuti</h4>
 * La funzione riconosce i parametri:
 *  - 'd' : seguito dal percorso del device /dev/myGPIOK col quale interagire
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
/**
 * Se non viene specificato il device myGPIOK col quale interagire è impossibile continuare.
 * Per questo motivo, in questo caso, la funzione restituisce -1, per cui il programma viene terminato.
 */
	if (devfile == NULL) {
		printf ("è necessario specificare il device col quale interagire!\n");
		howto();
		return -1;
	}
/** <h4>Accesso ad un device tramite il driver myGPIOK</h4>
 * Ad ogni periferica compatibile con il driver myGPIOK è associato un file diverso in /dev/ attraverso il
 * quale è possibile interagire con il device. Rispetto al driver UIO, non è necessario effettuare nessuna
 * operazione di mapping, in quanto il driver myGPIOK tradurrà le sistem-call che normalmente verrevvero
 * usate su un file, in operazioni specifiche per il device.
 * L'accesso al device viene ottenuto mediante la system-call open():
 * @code
 * #include <sys/stat.h>
 * #include <fcntl.h>
 * int open(const char *path, int oflag, ...  );
 * @endcode
 * la quale restituisce il descrittore del file /dev/myGPIOKx, usato nel seguito per effettuare le operazioni
 * di I/O. I valori del parametro oflag specificano il modo in cui il file viene aperto. In questo
 * caso viene usato O_RDWR, il quale garantisce accesso in lettura ed in scrittura. Altri valori sono O_RDONLY,
 * il quale garantisce accesso in sola lettura, ed O_WRONLY, che, invece, garantisce accesso in sola scrittura.
 * Se l'apertura del file non riesce è impossibile continuare.
 * Per questo motivo, in questo caso, la funzione restituisce -1, per cui il programma viene terminato.
 */
	param->dev_descr = open(devfile, O_RDWR);
	if (param->dev_descr < 1) {
		perror(devfile);
		return -1;
	}
	return 0;
}

/**
 * @brief Effettua operazioni su un device
 *
 * @param [in] param	puntatore a struttura param_t, contiene i vari parametri di esecuzione
 * 						del programma.
 *
 * @details
 * La funzione viene invocata dopo che sia stato eseguito il parsing dei parametri passati al programma quando
 * esso viene invocato. è stata scritta per funzionare sia con il GPIO Xilinx che con il GPIO custom myGPIO.
 * è possibile utilizzare il primo definendo la macro __XIL_GPIO__. Effettua, sul device, le operazioni
 * impostate, in accordo con i parametri passati al programma alla sua invocazione.
 * <br>
 * Nel caso un cui si usi un driver ad-hoc, è possibile usare le funzioni read() e write() per interagire
 * con il device, leggendo il valore dei registri o scrivendolo. Il driver myGPIOK mette a disposizione anche
 * la funzione seek() che permette di scegliere quale registro leggere o scrivere.
 */
void gpio_op (param_t *param) {

/**
 * <h4>Impostazione della modalità di funzionamento</h4>
 * Per impostare la modalità di funzionamento è necessario scrivere sul registro MODE. L'offset di tale
 * registro è determinato in base al particolare device che si stà utilizzando.
 * Dopo aver spostato la "testina di scrittura" sul registro MODE usando la funzione seek(), viene effettuata
 * la scrittura su di esso usando la funzione write().
 * è possibile, definendo la macro __USE_PWRITE__, usare la funzione pwrite(), che combina le due operazioni.
 */
	if (param->op_mode == 1) {
		printf("Scrittura sul registro mode: %08x\n", param->mode_value);
#ifndef __USE_PWRITE__
		lseek(param->dev_descr, MODE_OFFSET, SEEK_SET);
		write(param->dev_descr, &(param->mode_value), sizeof(uint32_t));
#else
		pwrite(param->dev_descr, &(param->mode_value), sizeof(uint32_t), MODE_OFFSET);
#endif
	}
/**
 * <h4>Operazione di scrittura</h4>
 * Per impostare il valore dei pin del device è necessario scrivere sul registro WRITE. L'offset di tale
 * registro è determinato in base al particolare device che si stà utilizzando.
 * Dopo aver spostato la "testina di scrittura" sul registro WRITE usando la funzione seek(), viene effettuata
 * la scrittura su di esso usando la funzione write().
 * è possibile, definendo la macro __USE_PWRITE__, usare la funzione pwrite(), che combina le due operazioni.
 */
	if (param->op_write == 1) {
		printf("Scrittura sul registro write: %08x\n", param->write_value);
#ifndef __USE_PWRITE__
		lseek(param->dev_descr, WRITE_OFFSET, SEEK_SET);
		write(param->dev_descr, &(param->write_value), sizeof(uint32_t));
#else
		pwrite(param->dev_descr, &(param->mode_value), sizeof(uint32_t), WRITE_OFFSET);
#endif
	}
/**
 * <h4>Operazione di lettura con interrupt</h4>
 * La lettura dei pin del device avviene mediante la chiamata alla funzione read(), dopo aver spostato la
 * "testina di lettura" sul registro READ. L'offset di tale registro, come nei due casi precedenti, viene
 * determinato in base al particolare device che si sta usando.
 * è possibile, definendo la macro __USE_PREAD__, usare la funzione pread(), che combina le operazioni di
 * seek() e read().
 * Il driver myGPIOK implementa un meccanismo di lettura bloccante: qualora non ci siano dati disponibili,
 * il processo che chiama read() viene sospeso e messo in attesa che i dati siano disponibili. Quando arriva
 * una interruzione dal device, il driver myGPIOK lo gestisce e risveglia i processi che erano stati messi
 * precedentemente in attesa.
 * Si legga la documentazione del driver myGPIOK per i dettagli.
 */
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


int main (int argc, char **argv) {
	param_t param;

	if (parse_args(argc, argv, &param) == -1)
		return -1;

	gpio_op(&param);

	close(param.dev_descr);

	return 0;
}


