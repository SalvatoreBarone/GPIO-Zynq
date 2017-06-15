/**
 * @file clientTCP.c
 * @author: Salvatore Barone <salvator.barone@gmail.com>
 * @date: 14 06 2017
 *
 * Client TCP.
 * Invia pacchetti TCP su una socket.
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
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void help(void);

int parse_args(	int 	argc,
				char	**argv,
				char	**host,
				int		*port,
				char	**file);

int create_socket (char *host, int port);

int main (int argc, char **argv) {
	
	char *host = NULL;
	int	port = 0;
	char *file = NULL;

	printf("Parsing parametri\n");
	if (parse_args(argc, argv, &host, &port, &file) != 0)
		return -1;

	printf("Creazione del socket\n");
	int sock_descriptor;
	if ((sock_descriptor = create_socket(host, port)) == -1)
		return -1;

	int execute = 1;
	while (execute) {
		char buffer [1000];
		memset(buffer, 0, sizeof(buffer));
		printf("Scrivi una stringa da spedire al server: ");
		gets(buffer);
		/* send:
		 * attraverso la chiamata a send, il server è in grado di inviare dei dati, sottoforma di sequenza
		 * di byte, al client. La sequenza viene vista come una sequenza di caratteri per cui è del tutto
		 * simile ad una operazione di write su un descrittore di file!* Infatti write(client_sockd, 
		 * &buffer, strlen(buffer)) funzionerebbe ugualmente ed in modo indistinguibile da send.
		 * Send non prevede di speficicare l'indirizzo del client in quanto, come già detto, il socket che
		 * si usa è utilizzato esclusivamente per la comuni cazione con uno specifico client.
		 */
		send(sock_descriptor, &buffer, strlen(buffer), 0);
		if (!strncmp(buffer, "quit", 4))
			execute = 0;
		/* recv:
		 * attraverso la chiamata a recv, il server è in grado di attendere l'invio, da parte del client,
		 * di una sequenza di byte. La sequenza viene vista come una sequenza di caratteri per cui è del
		 * tutto simile ad una operazione di read su un descrittore di file! Infatti read(client_sockd,
		 * &buffer, sizeof(buffer)) funzionerebbe ugualmente ed in modo indistinguibile da recv.
		 * Recv non prevede di speficicare l'indirizzo del client in quanto, come già detto, il socket che
		 * si usa è utilizzato esclusivamente per la comunicazione con uno  specifico client.
		 */
		memset(buffer, 0, sizeof(buffer));
		recv(sock_descriptor, &buffer, sizeof(buffer), 0);
		printf("Ricevuto dal server: %s\n", buffer);
	}
	/* chiusura del socket client:
	 * nel momento in cui la comunicazione con il server è terminata, il socket utilizzato per quella
	 * comunicazione può essere chiuso, liberando le risorse che occupata.
	 */
	close(sock_descriptor);
	return 0;
}

void help(void) {
	printf("Simple TCP client:\n");
	printf("\n");
	printf("\t\tclientTCP -a <address> -p <port> -f <file>\n");
	printf("\n");
	printf("\t-a <address>: indirizzo IP del server\n");
	printf("\t-p <port>: porta sul quale il server e' in ascolto\n");
	printf("\t-f <file>: file che si desidera inviare\n");
}


int parse_args(	int 	argc,
				char	**argv,
				char	**host,
				int		*port,
				char	**file)
{
	int par;
	while ((par = getopt(argc, argv, "a:p:f:h")) != -1) {
		switch (par) {
		case 'a' :
			*host = optarg;
			break;
		case 'p' :
			*port = atoi(optarg);
			break;
		case 'f' :
			*file = optarg;
			break;
		case 'h' :
			help();
			return -1;
		default :
			printf("%c: parametro sconosciuto.\n", par);
			break;
		}
	}

	if (*host == NULL) {
		printf("E' necessario specificare l'indirizzo dell'host al quale connettersi\n");
		return -1;
	}

	if (*port == 0) {
		printf("E' necessario specificare la porta sulla quale il server e' in ascolto\n");
		return -1;
	}

	if (*file == NULL) {
		printf("E' necessario specificare un file da trasferire\n");
		return -1;
	}

	return 0;
}

int create_socket (	char *host, int port) {
	/* crerazione del socket:
	 * della quintupla {protocol, local_addr, local_process, foreign_addr, foreign_process} viene specificato
	 * solo il primo parametro, ossia il protocollo. In questo caso il socket è creato della famiglia AF_INET
	 * per la comunicazione attraverso il protocollo IPv4, di tipo SOCK_STREAM, ossia sequenza di byte trasfe-
	 * rita in maniera ordinata ed affidabile attraverso il protocollo TCP, come specificato dal parametro
	 * IPPROTO_TCP.
	 */
	int sock_descriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock_descriptor == -1) {
		fprintf(stderr, "Errore nella creazione del socket!\n");
		return -1;
	}
	/* struttura dati per l'indirizzo locale del server:
	 * la struttura dati per la memorizzazione dell'indirizzo del server è server_address, di tipo
	 * struct sockaddr_in. Essendo il socket di tipo AF_INET, il membro sin_family deve essere proprio AF_INET
	 * mentre il membro sin_addr.s_addr viene impostato tramite la chiamata alla funzione inet_addr() che
	 * provvede alla conversione dell'indirizzo dell'host, specificato nella notazione dotted-decimal, nel
	 * formato TCP/IP network order, ossia un intero a 32 bit.
	 * La conversione del parametro port da "host byte order" a "TCP/IP network order" (intero a 16 bit)
	 * avviene per mezzo della chiamata alla funzione htons()-
	 */
	struct sockaddr_in server_address;
	memset((char*)&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(port);
	server_address.sin_addr.s_addr = inet_addr(host);

	/* connect:
	 * attraverso la chiamata alla funzione connect(), il client è in grado di effettuare una connessione con
	 * il server. Per le comunicazioni connection-oriented, come in questo caso, la chiamata scatena un mecca-
	 * nismo inteso a stabilire una connessione stabile e affidabile tra client e server affinchè possano
	 * comunicare tra di loro atraverso un flusso ordinato ed affidabile di byte. Il client riceverà dal
	 * sistema operativo, un indirizzo locale dove poter essere contattato dal server, in maniera totalmente
	 * automatica: non è necessario, quindi, che il client chiami la funzione bind(). Dopo questa chiamata,
	 * dunque, tutti e cinque i parametri della quintupla sono definiti ed identificano univocamente un canale
	 * di comunicazione tra client e server.
	 */
	if (connect(sock_descriptor, (struct sockaddr *)&server_address, sizeof(server_address)) != 0) {
		fprintf(stderr, "connect fallita\n");
		return -1;
	}
	return sock_descriptor;
}
