/**
 * @file myGPIO.h
 * @author Salvatore Barone <salvator.barone@gmail.com>
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
 */

#ifndef MYGPIO_DRIVER_HEADER_H
#define MYGPIO_DRIVER_HEADER_H

#include <inttypes.h>

/**
 * @addtogroup myGPIO
 * @{
 * @addtogroup bare-metal
 * @{
 *
 * @brief device-driver OO-like bare-metal per device myGPIO
 */

/**
 * @brief Indirizzo fisico/virtuale del device
 */
typedef volatile uint32_t* myGPIO_t;

#define MYGPIO_PIN0   0x1U         //!< maschera di selezione del pin 0 
#define MYGPIO_PIN1   0x2U         //!< maschera di selezione del pin 1 
#define MYGPIO_PIN2   0x4U         //!< maschera di selezione del pin 2 
#define MYGPIO_PIN3   0x8U         //!< maschera di selezione del pin 3 
#define MYGPIO_PIN4   0x10U        //!< maschera di selezione del pin 4 
#define MYGPIO_PIN5   0x20U        //!< maschera di selezione del pin 5 
#define MYGPIO_PIN6   0x40U        //!< maschera di selezione del pin 6 
#define MYGPIO_PIN7   0x80U        //!< maschera di selezione del pin 7 
#define MYGPIO_PIN8   0x100U       //!< maschera di selezione del pin 8 
#define MYGPIO_PIN9   0x200U       //!< maschera di selezione del pin 9 
#define MYGPIO_PIN10  0x400U       //!< maschera di selezione del pin 10 
#define MYGPIO_PIN11  0x800U       //!< maschera di selezione del pin 11 
#define MYGPIO_PIN12  0x1000U      //!< maschera di selezione del pin 12 
#define MYGPIO_PIN13  0x2000U      //!< maschera di selezione del pin 13 
#define MYGPIO_PIN14  0x4000U      //!< maschera di selezione del pin 14 
#define MYGPIO_PIN15  0x8000U      //!< maschera di selezione del pin 15 
#define MYGPIO_PIN16  0x10000U     //!< maschera di selezione del pin 16 
#define MYGPIO_PIN17  0x20000U     //!< maschera di selezione del pin 17 
#define MYGPIO_PIN18  0x40000U     //!< maschera di selezione del pin 18 
#define MYGPIO_PIN19  0x80000U     //!< maschera di selezione del pin 19 
#define MYGPIO_PIN20  0x100000U    //!< maschera di selezione del pin 20 
#define MYGPIO_PIN21  0x200000U    //!< maschera di selezione del pin 21 
#define MYGPIO_PIN22  0x400000U    //!< maschera di selezione del pin 22 
#define MYGPIO_PIN23  0x800000U    //!< maschera di selezione del pin 23 
#define MYGPIO_PIN24  0x1000000U   //!< maschera di selezione del pin 24 
#define MYGPIO_PIN25  0x2000000U   //!< maschera di selezione del pin 25 
#define MYGPIO_PIN26  0x4000000U   //!< maschera di selezione del pin 26 
#define MYGPIO_PIN27  0x8000000U   //!< maschera di selezione del pin 27 
#define MYGPIO_PIN28  0x10000000U  //!< maschera di selezione del pin 28 
#define MYGPIO_PIN29  0x20000000U  //!< maschera di selezione del pin 29 
#define MYGPIO_PIN30  0x40000000U  //!< maschera di selezione del pin 30 
#define MYGPIO_PIN31  0x80000000U  //!< maschera di selezione del pin 31 

#define MYGPIO_PIN(i) ((uint32_t)(1<<(i)))

#define MYGPIO_MODE_READ  0U //!< modalità lettura
#define MYGPIO_MODE_WRITE 1U //!< modalità scrittura
#define MYGPIO_PIN_RESET  0U
#define MYGPIO_PIN_SET    1U

void     myGPIO_Init                    (myGPIO_t *gpio, uint32_t base_address);
void     myGPIO_SetMode                 (myGPIO_t gpio, uint32_t mask,          uint32_t mode);
void     myGPIO_SetValue                (myGPIO_t gpio, uint32_t mask,          uint32_t value);
void     myGPIO_Toggle                  (myGPIO_t gpio, uint32_t mask);
uint32_t myGPIO_GetValue                (myGPIO_t gpio, uint32_t mask);
uint32_t myGPIO_GetRead                 (myGPIO_t gpio);
void     myGPIO_GlobalInterruptEnable   (myGPIO_t gpio);
void     myGPIO_GlobalInterruptDisable  (myGPIO_t gpio);
uint32_t myGPIO_IsGlobalInterruptEnabled(myGPIO_t gpio);
uint32_t myGPIO_PendingInterrupt        (myGPIO_t gpio);
void     myGPIO_PinInterruptEnable      (myGPIO_t gpio, uint32_t mask);
void     myGPIO_PinInterruptDisable     (myGPIO_t gpio, uint32_t mask);
uint32_t myGPIO_EnabledPinInterrupt     (myGPIO_t gpio);
uint32_t myGPIO_PendingPinInterrupt     (myGPIO_t gpio);
void     myGPIO_PinInterruptAck         (myGPIO_t gpio, uint32_t mask);

/**
 * @}
 * @}
 */

#endif
