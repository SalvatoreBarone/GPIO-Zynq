/**
 * @file myGPIOK_list.h
 * @author Salvatore Barone <salvator.barone@gmail.com>
 * @date 24 06 2017
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
#include "myGPIOK_list.h"
#include <linux/slab.h>


int myGPIOK_list_Init(myGPIOK_list_t *list, uint32_t list_size) {
	uint32_t i;
	list->list_size = list_size;
	list->device_count = 0;
	list->device_list = kmalloc(list->list_size * sizeof(myGPIOK_t*), GFP_KERNEL);

	if (list->device_list == NULL)
		return -ENOMEM;

	list->op_list = kmalloc(list->list_size * sizeof(struct platform_device*), GFP_KERNEL);

	for (i=0; i<list->list_size; i++) {
		list->device_list[i] = NULL;
		list->op_list[i] = NULL;
	}

	return 0;
}

void myGPIOK_list_Destroy(myGPIOK_list_t* list) {
	kfree(list->device_list);
	kfree(list->op_list);
}

int myGPIOK_list_add(myGPIOK_list_t *list, struct platform_device* op, myGPIOK_t *device) {
	if (list->device_count >= list->list_size)
		return -1;

	list->op_list[list->device_count] = op;
	list->device_list[list->device_count] = device;
	list->device_count++;
	return 0;
}

myGPIOK_t* myGPIOK_list_find_by_op(myGPIOK_list_t *list, struct platform_device *op) {
	uint32_t i = 0;
	do {
		if (list->op_list[i] == op)
			return list->device_list[i];
		i++;
	} while (i < list->device_count);
	return NULL;
}

myGPIOK_t* myGPIOK_list_find_by_minor(myGPIOK_list_t *list, dev_t dev) {
	uint32_t i = 0;
	do {
		if (list->device_list[i]->Mm_number == dev)
			return list->device_list[i];
		i++;
	} while (i < list->device_count);
	return NULL;
}

myGPIOK_t* myGPIOK_list_find_irq_line(myGPIOK_list_t *list, int irq_line) {
	uint32_t i = 0;
	do {
		if (list->device_list[i]->irqNumber == irq_line)
			return list->device_list[i];
		i++;
	} while (i < list->device_count);
	return NULL;
}

uint32_t myGPIOK_list_device_count(myGPIOK_list_t *list) {
	return list->device_count;
}


