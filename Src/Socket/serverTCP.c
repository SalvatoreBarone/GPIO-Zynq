/**
 * @file serverTCP.c
 * @author: Salvatore Barone <salvator.barone@gmail.com>
 * @date: 14 06 2017
 *
 *  Server TCP.
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main (int argc, char** argv) {
	if (argc != 2) {
		fprintf(stderr, "usage: server port\n");
		return 1;
	}
	int port = atoi(argv[1]);
	/* crerazione del socket: 
	 * della quintupla {protocol, local_addr, local_process, foreign_addr, foreign_process} viene specificato
	 * solo il primo parametro, ossia il protocollo. In questo caso il socket è creato della famiglia AF_INET
	 * per la comunicazione attraverso il protocollo IPv4, di tipo SOCK_STREAM, ossia sequenza di byte trasfe-
	 * rita in maniera ordinata ed affidabile attraverso il protocollo TCP, come specificato dal parametro
	 * IPPROTO_TCP.
	 */ 
	int lsock_descr = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (lsock_descr == -1) {
		fprintf(stderr, "Errore nella creazione del socket!\n");
		return -1;
	}
	/* struttura dati per l'indirizzo locale del server:
	 * la struttura dati per la memorizzazione dell'indirizzo locale del server è server_addr, di tipo
	 * struct sockaddr_in. Essendo il socket di tipo AF_INET, il membro sin_family deve essere proprio AF_INET
	 * mentre il membro sin_addr.s_addr viene impostato tramite la macro INADDR_ANY.
	 * La conversione del parametro port da "host byte order" a "TCP/IP network order" avviene per mezzo della
	 * chiamata alla funzione htons()-
	 */
	struct sockaddr_in server_addr;
	memset((char*)&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);
	/* bind:
	 * l'indirizzo locale del server server_addr viene utilizzato nella funzione bind() per fare in modo che
	 * richieste TCP in arrivo sulla porta specificata, siano servite dal server. Dopo la chiamata a bind,
	 * della quintupla rimangono specificati solo i primi tre parametri.
	 */
	if (bind(lsock_descr, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
		fprintf(stderr, "Dind fallito\n");
		return -1;
	}
	/* listen:
	 * il server rende noto che è disposto ad accettare connessioni TCP sulla porta precedentemente specifica-
	 * ta. Viene specificato, attraverso il secondo parametro, anche il numero massimo di connessioni che
	 * possono trovarsi in coda, in attesa di essere servite.
	 */
	if (listen(lsock_descr, 10) < 0) {
		fprintf(stderr, "listen fallito\n");
		return -1;
	}
	printf("Server in ascolto sul port TCP %d\n", port);
	int execute = 1;
	while (execute) {
		// socket descriptor del socket client
		int client_sockd;							
		/* accept:
		 * attraverso la chiamata alla funzione accept(), il server si mette materialmente in attesa di
		 * connessioni. La chiamata è bloccante se nella coda non ci sono connessioni da poter essere
		 * servite. Accept, chiamata su un socket, di cui si specifica il descrittore, a cui da adesso ci
		 * riferiremo come socket di ascolto, restituisce un socket identico a quello di ascolto riservato
		 * alla comunicazione con il client con il quale è stata instaurata una connessione. Per il socket
		 * di ascolto, continuano ad essere specificati soltanto i primi tre valori della quintupla mentre
		 * per il socket del client, tutti i valori della quintupla sono specificati ed attraverso di esso
		 * il server ed il client possono comunicare tra loro.
		 * Il secondo ed il terzo paramentro, ossia indirizzo del client e la sua lunghezza, non sono di
		 * particolare interesse, in questo caso in quanto il socket è riservato esclusivamente alla
		 * comunicazione con quello specifico client.
		 */
		if ((client_sockd = accept(lsock_descr, NULL, NULL)) < 0) {
			fprintf(stderr, "accept fallito\n");
			return 1;
		}
		while (execute) {
			char buffer[1000];
			memset(buffer, 0, sizeof(buffer));
			/* recv:
			 * attraverso la chiamata a recv, il server è in grado di attendere l'invio, da parte del
			 * client, di una sequenza di byte. La sequenza viene vista come una sequenza di caratteri
			 * per cui è del tutto simile ad una operazione di read su un descrittore di file! Infatti
			 * read(client_sockd, &buffer, sizeof(buffer)) funzionerebbe ugualmente ed indistintamente
			 * da recv. Recv non prevede di speficicare l'indirizzo del client in quanto, come già
			 * detto, il socket che si usa è utilizzato esclusivamente per la comunicazione con uno 
			 * specifico client.
			 */
			recv(client_sockd, &buffer, sizeof(buffer), 0);
			if (!strncmp(buffer,"quit",5))
				execute = 0;
			printf("Ricevuto dal client: %s\n", buffer);
			/* send:
			 * attraverso la chiamata a send, il server è in grado di inviare dei dati, sottoforma di
			 * sequenza di byte, al client. La sequenza viene vista come una sequenza di caratteri
			 * per cui è del tutto simile ad una operazione di write su un descrittore di file!
			 * Infatti write(client_sockd, &buffer, strlen(buffer)) funzionerebbe ugualmente ed in
			 * modo indistinguibile da send.
			 * Send non prevede di speficicare l'indirizzo del client in quanto, come già detto, il
			 * socket che si usa è utilizzato esclusivamente per la comuni cazione con uno specifico
			 * client.
			 */
			send(client_sockd, &buffer, strlen(buffer), 0);
			printf("Inviata al client: %s\n", buffer);
		}
		printf("Closing connection with client.\n");
		/* chiusura del socket client:
		 * nel momento in cui la comunicazione con il client è terminata, il socket utilizzato per quella
		 * comunicazione può essere chiuso, liberando le risorse che occupava. Il socket di ascolto rimane
		 * aperto e va chiuso quando non più necessario.
		 */
		close(client_sockd);
	}
	printf("Chiusura del socket di ascolto.\n");
	//chiusura della socket di ascolto
	close(lsock_descr);
	return 0;
}
