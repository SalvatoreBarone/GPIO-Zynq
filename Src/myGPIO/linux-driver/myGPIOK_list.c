/**
 * @file myGPIOK_list.c
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
 * @addtogroup DeviceList
 * @{
 */
#include "myGPIOK_list.h"
#include <linux/slab.h>

/**
 * @brief Inizializza una struttura dati myGPIOK_list_t
 * @param [in] list		 puntatore a myGPIOK_list_t, lista da inizializzare
 * @param [in] list_size numero massimo di device che la struttra dati potra' gestire
 * @retval -ENOMEM nel caso in cui la struttura non possa essere allocata in memoria
 * @retval 0 se non si manifestano errori
 */
int myGPIOK_list_Init(myGPIOK_list_t *list, uint32_t list_size) {
	uint32_t i;
	list->list_size = list_size;
	list->device_count = 0;
	list->device_list = kmalloc(list->list_size * sizeof(myGPIOK_t*), GFP_KERNEL);

	if (list->device_list == NULL)
		return -ENOMEM;


	for (i=0; i<list->list_size; i++)
		list->device_list[i] = NULL;

	return 0;
}

/**
 * @brief Dealloca gli oggetti internamente gestiti da un oggetto myGPIOK_list_t, liberando la memoria
 * @param [in] list		 puntatore a myGPIOK_list_t, lista da distruggere
 */
void myGPIOK_list_Destroy(myGPIOK_list_t* list) {
	kfree(list->device_list);
}

/**
 * @brief Aggiunge un riferimento ad un oggetto myGPIOK_t alla lista
 * @param [in] list 	puntatore a myGPIOK_list_t, lista a cui aggiungere l'oggetto
 * @param [in] device	puntatore a myGPIOK_t, oggetto da aggiungere alla lista
 * @retval -1 se lo spazio si e' esaurito
 * @retval 0 se non si manifesta nessun errore
 *
 * @warning La funzione si limita ad aggiungere l'oggetto myGPIOK_t alla lista, senza effettuare alcun tipo
 * di controllo su di esso. Non viene verificato, ad esempio, se il device che si intende aggiungere sia
 * effettivamente gia' presente in lista o se si tratti di un puntatore nullo.
 */
int myGPIOK_list_add(myGPIOK_list_t *list, myGPIOK_t *device) {
	if (list->device_count >= list->list_size)
		return -1;

	list->device_list[list->device_count] = device;
	list->device_count++;
	return 0;
}

/**
 * @brief Ricerca un oggetto myGPIOK_t all'interno della lista
 * @param [in] list 	puntatore a myGPIOK_list_t, lista in cui effettuare la ricerca
 * @param [in] op		puntatore a struct platform_device, argomento con cui viene invocata probe() o remove(), associato ad un oggetto myGPIOK_t
 * @return indirizzo dell'oggetto myGPIOK_t, se e' presente nella lista, NULL altrimenti
 */
myGPIOK_t* myGPIOK_list_find_by_op(myGPIOK_list_t *list, struct platform_device *op) {
	uint32_t i = 0;
	do {
		if (list->device_list[i]->op == op)
			return list->device_list[i];
		i++;
	} while (i < list->device_count);
	return NULL;
}

/**
 * @brief Ricerca un oggetto myGPIOK_t all'interno della lista
 * @param [in] list 	puntatore a myGPIOK_list_t, lista in cui effettuare la ricerca
 * @param [in] dev		major/minor number associato al device, parametro con cui viene invocata la open() o la release()
 * @return indirizzo dell'oggetto myGPIOK_t, se e' presente nella lista, NULL altrimenti
 */
myGPIOK_t* myGPIOK_list_find_by_minor(myGPIOK_list_t *list, dev_t dev) {
	uint32_t i = 0;
	do {
		if (list->device_list[i]->Mm == dev)
			return list->device_list[i];
		i++;
	} while (i < list->device_count);
	return NULL;
}

/**
 * @brief Ricerca un oggetto myGPIOK_t all'interno della lista
 * @param [in] list		puntatore a myGPIOK_list_t, lista in cui effettuare la ricerca
 * @param [in[ irq_line	linea di interruzione alla quale il device e' connesso, parametro di invocazione della ISR
 * @return indirizzo dell'oggetto myGPIOK_t, se e' presente nella lista, NULL altrimenti
 */
myGPIOK_t* myGPIOK_list_find_irq_line(myGPIOK_list_t *list, int irq_line) {
	uint32_t i = 0;
	do {
		if (list->device_list[i]->irqNumber == irq_line)
			return list->device_list[i];
		i++;
	} while (i < list->device_count);
	return NULL;
}

/**
 * @brief Restituisce il numero di device correntemente inseriti nella lista
 * @param [in] list puntatore a myGPIOK_list_t, lista di cui si intende conoscere il numero di oggetti myGPIOK_t contenuti
 * @return numero di device correntemente inseriti nella lista
 */
uint32_t myGPIOK_list_device_count(myGPIOK_list_t *list) {
	return list->device_count;
}

/**
 * @}
 * @}
 * @}
 */

