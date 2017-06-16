/**
 * @file myGPIOK.c
 * @author Salvatore Barone <salvator.barone@gmail.com>
 * @date 16 06 2017
 *
 * @brief device-driver in kernel-mode per myGPIO
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
 * @brief Questo e' un programma di esempio per l'interfacciamento con una periferica myGPIO.
 *
 * In questo specifico esempio l'interfacciamento avviene da user-space, interagendo attraverso il
 * driver uio.
 */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kmalloc.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Salvatore Barone <salvator.barone@gmail.com>");
MODULE_DESCRIPTION("myGPIO device-driver in kernel mode");
MODULE_VERSION("0.1");


static int myGPIO_module_init(void) {
	printk(KERN_INFO "Chiamata myGPIO_module_init\n");
	return 0;
}

static void myGPIO_module_exit(void) {
	printk(KERN_INFO "Chiamata myGPIO_module_exit\n");

}

module_init(myGPIO_module_init);
module_exit(myGPIO_module_exit);
