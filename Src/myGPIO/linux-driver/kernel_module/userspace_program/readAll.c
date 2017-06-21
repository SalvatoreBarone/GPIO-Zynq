/**
 * @file readAll.c
 * @author Salvatore Barone <salvator.barone@gmail.com>
 * @date 19 06 2017
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
 *
 * @addtogroup myGPIO
 * @{
 * @addtogroup Linux-Driver
 * @{
 * @addtogroup Kernel-Driver
 * @{
 * @addtogroup Userspace-program
 * @{
 * @defgroup userspace-readall
 * @{
 *
 * @brief Programma di test/debug. Legge tutti i registri della periferica direttamente dal file /dev/mem
 */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

void howto(void) {
	printf("Uso:\n");
	printf("noDriver -a <gpio_phisycal_address> -o <max-offset>\n");
	printf("-a <gpio_phisycal_address>: indirizzo fisico del device GPIO\n");
	printf("\t-o <max-offset>: offsett dell'ultimo registro letto\n");
}

int parse_args(int argc, char **argv, uint32_t	*gpio_address, uint32_t	*max_offset) {
	int par;

	while((par = getopt(argc, argv, "a:o:")) != -1) {
		switch (par) {
		case 'a' :
			*gpio_address = strtoul(optarg, NULL, 0);
			break;
		case 'o' :
			*max_offset = strtoul(optarg, NULL, 0);
			break;
		default :
			printf("%c: parametro sconosciuto.\n", par);
			howto();
			return -1;
		}
	}
	return 0;
}

int main(int argc, char** argv) {
	uint32_t gpio_addr = 0;
	uint32_t max_offset = 16;

	if (parse_args(argc, argv, &gpio_addr, &max_offset) == -1)
		return -1;

	if (gpio_addr == 0) {
		printf("E' necessario specificare l'indirizzo di memoria del device.\n");
		howto();
		return -1;
	}

	int descriptor = open ("/dev/mem", O_RDWR);
	if (descriptor < 1) {
		perror(argv[0]);
		return -1;
	}

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

	printf("base address : %08X\n", (uint32_t) vrt_gpio_addr);
	uint32_t read_value = 0;
	uint32_t i;
	for (i=0; i<=max_offset; i+=4) {
		read_value = *((uint32_t*)(vrt_gpio_addr+i));
		printf("\toffset : %08X => %08X\n", i, read_value);
	}

	munmap(vrt_page_addr, page_size);
	close(descriptor);

	return 0;
}


/**
 * @}
 * @}
 * @}
 * @}
 * @}
 */
