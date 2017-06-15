/**
 * @file clientTCP.c
 * @author: Salvatore Barone <salvator.barone@gmail.com>
 * @date: 14 06 2017
 *
 * Client TCP.
 * Invia file testuali attraverso una socket TCP.
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
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

void help(void) {
	printf("Simple TCP client:\n");
	printf("Invia file testuali attraverso una socket TCP.\n");
	printf("\n");
	printf("\t\tclientTCP -a <address> -p <port> -f <file>\n");
	printf("\n");
	printf("\t-a <address>: indirizzo IP del server\n");
	printf("\t-p <port>: porta sul quale il server e' in ascolto\n");
	printf("\t-f <file>: file che si desidera inviare\n");
}

int parse_args(int argc, char **argv, char **host, int *port, char **file) {
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

int create_client_socket (char *host, int port) {

	int sock_descriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock_descriptor == -1) {
		fprintf(stderr, "Errore nella creazione del socket!\n");
		return -1;
	}

	struct sockaddr_in server_address;
	memset((char*)&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(port);
	server_address.sin_addr.s_addr = inet_addr(host);

	if (connect(sock_descriptor, (struct sockaddr *)&server_address, sizeof(server_address)) != 0) {
		fprintf(stderr, "connect fallita\n");
		return -1;
	}
	return sock_descriptor;
}

int main (int argc, char **argv) {

	char *host = NULL;
	int	port = 0;
	char *file = NULL;

	// parsing argomenti
	if (parse_args(argc, argv, &host, &port, &file) != 0)
		return -1;

	// apertura del file
	int file_descriptor = open(file, O_RDONLY);
	if (file_descriptor < 0) {
        printf("Impossibile aprire il file\n");
        return -1;
    }

	// apertura della socket
	int sock_descriptor;
	if ((sock_descriptor = create_client_socket(host, port)) == -1)
		return -1;

	// send lunghezza nome file
	int name_length = strlen(file);
	name_length = htonl(name_length);
	send(sock_descriptor, &name_length, sizeof(int), 0);
	// send nome file
	send(sock_descriptor, file, strlen(file), 0);

	// sending del file per intero
	int size = lseek(file_descriptor, 0, SEEK_END);
	off_t offset = 0;
	do
		sendfile(sock_descriptor, file_descriptor, &offset, size);
	while (offset != size);

	// chiusura della socket
	close(sock_descriptor);
	close(file_descriptor);
	return 0;
}

