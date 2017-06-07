#include "myGPIO.h"
#include <inttypes.h>
#include <stdio.h>

void print_config(myGPIO_t *gpio);
void print_status(uint32_t *gpio);

int main() {
	uint32_t myGPIO[4] = {
		0x00000000,		// enable register
		0x00000000,		// write register
		0x00000000,		// read register
		0x00000000		// interrupt register
	};

	myGPIO_t gpio;
	myGPIO_init(&gpio, myGPIO);
	print_config(&gpio);
	print_status(myGPIO);
	
	myGPIO_setMode(&gpio, myGPIO_byte2 | myGPIO_pin0 | myGPIO_pin3, myGPIO_write);
	myGPIO_setValue(&gpio, myGPIO_pin0 | myGPIO_pin3, myGPIO_set);
	print_status(myGPIO);
	
	
	myGPIO_interruptEnable(&gpio);
	print_status(myGPIO);
	
	myGPIO_interruptDisable(&gpio);
	print_status(myGPIO);

	myGPIO_interruptAck(&gpio);
	print_status(myGPIO);

	return 0;
}

void print_config(myGPIO_t *gpio) {
	printf("gpio@%08x\n", (uint32_t)gpio);
	printf("\tbase address=\t\t%08x\n", (uint32_t)(gpio->base_address));
	printf("\toffset:\n");
	printf("\t\tmode=\t\t%02x\n", gpio->mode_offset);
	printf("\t\twrite=\t\t%02x\n", gpio->write_offset);
	printf("\t\tread=\t\t%02x\n", gpio->read_offset);
	printf("\t\tinterrupt=\t%02x\n", gpio->int_offset);
}

void print_status(uint32_t *gpio) {
	printf("gpio@%08x\n", (uint32_t)gpio);
	printf("\tmode=\t\t%08x\n", gpio[0]);
	printf("\twrite=\t\t%08x\n", gpio[1]);
	printf("\tread=\t\t%08x\n", gpio[2]);
	printf("\tinterrupt=\t%08x\n", gpio[3]);
}
