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
 *
 * @addtogroup myGPIO
 * @{
 * @addtogroup Linux-Driver
 * @{
 * @addtogroup myGPIO_device_list
 * @{
 */

#ifndef __MYGPIOK_DEVICE_LIST__
#define __MYGPIOK_DEVICE_LIST__

#include "myGPIOK_t.h"

typedef struct {
	myGPIOK_t **device_list;
	struct platform_device **op_list;
	uint32_t list_size;
	uint32_t device_count;
} myGPIOK_list_t;


extern int myGPIOK_list_Init(myGPIOK_list_t *list, uint32_t list_size);

extern void myGPIOK_list_Destroy(myGPIOK_list_t* list);

extern int myGPIOK_list_add(myGPIOK_list_t *list, struct platform_device* op, myGPIOK_t *device);

extern myGPIOK_t* myGPIOK_list_find_by_op(myGPIOK_list_t *list, struct platform_device *op);

extern myGPIOK_t* myGPIOK_list_find_by_minor(myGPIOK_list_t *list, dev_t dev);

extern myGPIOK_t* myGPIOK_list_find_irq_line(myGPIOK_list_t *list, int irq_line);

extern uint32_t myGPIOK_list_device_count(myGPIOK_list_t *list);

#endif

/**
 * @}
 * @}
 * @}
 */
