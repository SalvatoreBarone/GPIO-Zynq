#include "gpio.h"
#include <inttypes.h>
#include <stdio.h>

int main() {
	uint32_t GPIO[4] = {
		0x00000000,		// enable register
		0x00000000,		// write register
		0x00000000,		// read register
		0x00000000		// unused
	};

	GPIO_t gpio;

//	gpio_init(0, GPIO, 32, 0, 4, 8);
//	gpio_init(&gpio, 0, 32, 0, 4, 8);
//	gpio_init(&gpio, GPIO, 64, 0, 4, 8);
//	gpio_init(&gpio, GPIO, 32, 16, 4, 8);
//	gpio_init(&gpio, GPIO, 32, 0, 16, 8);
//	gpio_init(&gpio, GPIO, 32, 0, 4, 16);
//	gpio_init(&gpio, GPIO, 32, 0, 0, 8);

	gpio_init(&gpio, GPIO, 32, 0, 4, 8);

	gpio_setMode(&gpio, GPIO_pin0 | GPIO_pin4 | GPIO_pin8, GPIO_write);
	printf("%08X\t%08X\t%08X\t%08X\n", GPIO[0], GPIO[1], GPIO[2], GPIO[3]);

	gpio_setMode(&gpio, GPIO_pin4 | GPIO_pin8, GPIO_read);
	printf("%08X\t%08X\t%08X\t%08X\n", GPIO[0], GPIO[1], GPIO[2], GPIO[3]);

	gpio_setValue(&gpio, GPIO_byte3 | GPIO_byte0, GPIO_set);
	printf("%08X\t%08X\t%08X\t%08X\n", GPIO[0], GPIO[1], GPIO[2], GPIO[3]);

	gpio_setValue(&gpio, GPIO_pin0 | GPIO_pin1, GPIO_reset);
	printf("%08X\t%08X\t%08X\t%08X\n", GPIO[0], GPIO[1], GPIO[2], GPIO[3]);

	GPIO_value value = gpio_getValue(&gpio, GPIO_pin0);
	printf((value == GPIO_set ? "GPIO_set\n" : "GPIO_reset\n"));
	GPIO[2] = 1;
	value = gpio_getValue(&gpio, GPIO_pin0 | GPIO_pin1);
	printf((value == GPIO_set ? "GPIO_set\n" : "GPIO_reset\n"));
	GPIO[2] = 0x80000001;
	value = gpio_getValue(&gpio, GPIO_pin0 | GPIO_pin31);
	printf((value == GPIO_set ? "GPIO_set\n" : "GPIO_reset\n"));
	value = gpio_getValue(&gpio, GPIO_pin0);
	printf((value == GPIO_set ? "GPIO_set\n" : "GPIO_reset\n"));
	value = gpio_getValue(&gpio, GPIO_pin3);
	printf((value == GPIO_set ? "GPIO_set\n" : "GPIO_reset\n"));

	return 0;
}
