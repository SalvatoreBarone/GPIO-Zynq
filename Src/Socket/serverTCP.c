/**
 * @file serverTCP.c
 * @author: Salvatore Barone <salvator.barone@gmail.com>
 * @date: 14 06 2017
 *
 * Server TCP.
 * Riceve file testuali da una socket TCP e li memorizza sul filesystem, in una directory specifica.
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
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <fcntl.h>

int lsock_descr;
void help() {
	printf("Simple TCP server:\n");
	printf("Riceve file testuali da una socket TCP e li memorizza sul filesystem, in una directory specifica.\n");
	printf("\n");
	printf("\t\tserverTCP -p <port> -m <connection> -d <destination>\n");
	printf("\n");
	printf("\t-p <port>: porta sul quale il server si mettera' in ascolto (default 8000)\n");
	printf("\t-m <connection>: numero massimo di connessioni simultanee (default 5)\n");
	printf("\t-d <destination>: directory nella quale verranno salvati i file spediti al server (default .)\n");
}

int parse_args(int argc, char **argv, int *port, int *max_connection, char **dest_dir) {
	int par;
	while ((par = getopt(argc, argv, "p:d:m:h")) != -1) {
		switch (par) {
		case 'd' :
			*dest_dir = optarg;
			break;
		case 'p' :
			*port = atoi(optarg);
			break;
		case 'm' :
			*max_connection = atoi(optarg);
			break;
		case 'h' :
			help();
			return -1;
		default :
			printf("%c: parametro sconosciuto.\n", par);
			break;
		}
	}
	return 0;
}

int create_listen_socket (int port, int max_connection) {

	int lsock_descr = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (lsock_descr == -1) {
		fprintf(stderr, "Errore nella creazione del socket!\n");
		return -1;
	}

	struct sockaddr_in server_addr;
	memset((char*)&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);

	if (bind(lsock_descr, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
		fprintf(stderr, "Bind fallito\n");
		return -1;
	}

	if (listen(lsock_descr, max_connection) < 0) {
		fprintf(stderr, "listen fallito\n");
		return -1;
	}
	return lsock_descr;
}

int accept_client_connection(int lsock_descr) {
	int client_sockd;
	if ((client_sockd = accept(lsock_descr, NULL, NULL)) < 0) {
		fprintf(stderr, "accept fallito\n");
		return -1;
	}
	return client_sockd;
}

typedef struct {
	int client_sockd;
	char* dest_dir;
} client_data_t;

void* serve_client_connection_thread(void* data) {
	client_data_t* client_data = (client_data_t*) data;
	char file_name[100];
	memset(file_name, 0, sizeof(file_name));
	// ricezione lunghezza nome file
	int name_length = 0;
	recv(client_data->client_sockd, &name_length, sizeof(int), 0);
	name_length = ntohl(name_length);

	recv(client_data->client_sockd, &file_name, name_length, 0);

	char* file_path = (char*)malloc(strlen(client_data->dest_dir) + strlen(file_name) + 2);
	strcpy(file_path, "");
	strcat(file_path, client_data->dest_dir);
	if (client_data->dest_dir[strlen(client_data->dest_dir)-1] != '/')
		strcat(file_path, "/");
	strcat(file_path, file_name);

	FILE* file_ptr = fopen(file_path, "w");
	char buffer[1000];
	int byte_received = 0;
	do {
		memset(buffer, 0, sizeof(buffer));
		byte_received = recv(client_data->client_sockd, &buffer, sizeof(buffer), 0);
		fprintf(file_ptr, "%s", buffer);
	} while (byte_received != 0);
	fclose(file_ptr);

	close(client_data->client_sockd);
	free(client_data);
	return 0;
}

void signal_handler(int sig) {
	printf("Ricevuto %d.\n", sig);
	printf("Chiusura del socket di ascolto.\n");
	//chiusura della socket di ascolto
	close(lsock_descr);
	abort();
}

int main (int argc, char** argv) {
	int port = 8000;
	int max_connection = 5;
	char *dest_dir = ".";

	if (parse_args(argc, argv, &port, &max_connection, &dest_dir) == -1)
		return -1;

	mkdir(dest_dir, S_IRWXU);

	if ((lsock_descr = create_listen_socket(port, max_connection)) == -1)
		return -1;

	signal(SIGTERM, signal_handler);
	signal(SIGINT, signal_handler);

	for (;;) {
		client_data_t* client_data = (client_data_t*) malloc(sizeof(client_data_t));
		client_data->dest_dir = dest_dir;
		int* descr = (int*) malloc(sizeof(int));
		if ((client_data->client_sockd = accept_client_connection(lsock_descr)) != -1) {
			pthread_t thread_id;
			pthread_create(&thread_id, NULL, serve_client_connection_thread, (void*)client_data);
		}
		else
			free(descr);
	}

	//chiusura della socket di ascolto
	close(lsock_descr);
	return 0;
}

