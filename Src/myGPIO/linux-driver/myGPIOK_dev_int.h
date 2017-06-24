/**
 * @file myGPIOK_dev_int.h
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
 */
#ifndef __MYGPIOK_DEVICE_INTERRUPT__
#define __MYGPIOK_DEVICE_INTERRUPT__

#define myGPIOK_GIES_OFFSET		0x0CU	//!< @brief Offset, rispetto all'indirizzo base, del registro "gies" per il device myGPIO
#define myGPIOK_PIE_OFFSET		0x10U	//!< @brief Offset, rispetto all'indirizzo base, del registro "pie" per il device myGPIO
#define myGPIOK_IRQ_OFFSET		0x14U	//!< @brief Offset, rispetto all'indirizzo base, del registro "irq" per il device myGPIO
#define myGPIOK_IACK_OFFSET		0x18U	//!< @brief Offset, rispetto all'indirizzo base, del registro "iack" per il device myGPIO

/**
 * @brief Abilita gli interrupt globali;
 * @param [in] baseAddress indirizzo virtuale del device
 */
extern void myGPIOK_GlobalInterruptEnable(void* baseAddress);

/**
 * @brief Disabilita gli interrupt globali;
 * @param [in] baseAddress indirizzo virtuale del device
 */
extern void myGPIOK_GlobalInterruptDisable(void* baseAddress);

/**
 * @brief Abilita gli interrupt per i singoli pin del device.
 * @param [in] baseAddress indirizzo virtuale del device
 * @param [in] mask maschera di selezione degli interrupt da abilitare; quelli non selezionati non
 * vengono abilitati;
 */
extern void myGPIOK_PinInterruptEnable(void* baseAddress, unsigned mask);

/**
 * @brief Disabilita gli interrupt per i singoli pin del device.
 * @param [in] baseAddress indirizzo virtuale del device
 * @param [in] mask maschera di selezione degli interrupt da disabilitare; quelli non selezionati non
 * vengono disabilitati;
 */
extern void myGPIOK_PinInterruptDisable(void* baseAddress, unsigned mask);

/**
 * @brief Consente di ottenere una maschera che indichi quali interrupt non siano stati ancora serviti;
 * @param [in] baseAddress indirizzo virtuale del device
 * @return maschera che riporta i pin per i quali gli interrupt non sono stati ancora serviti;
 */
extern unsigned myGPIOK_PendingPinInterrupt(void* baseAddress);

/**
 * @brief Invia al device notifica di servizio di un interrupt;
 * @param [in] baseAddress indirizzo virtuale del device
 * @param [in] mask mask maschera di selezione degli interrupt da notificare; quelli non selezionati non
 * vengono notificati;
 */
extern void myGPIOK_PinInterruptAck(void* baseAddress, unsigned mask);

/**
 * @cond
 * Funzioni e definizioni di servizio per GPIO Xilinx
 * Non verranno documentate.
 */
#ifdef __XGPIO__
#define XGPIO_GIE_OFFSET	0x11C
#define XGPIO_ISR_OFFSET	0x120
#define XGPIO_IER_OFFSET	0x128
#define XGPIO_GIE			0x80000000
#define XGPIO_CH1_IE		0x00000001
#define XGPIO_CH2_IE		0x00000002
#define XGPIO_GIDS			0x00000000
#define XGPIO_CH1_IDS		0x00000000
#define XGPIO_CH2_IDS		0x00000000
#define XGPIO_CH1_ACK		0x01
#define XGPIO_CH2_ACK		0x02

#define XGPIO_USED_INT		XGPIO_CH2_IE

extern void XGpio_Global_Interrupt(void* baseAddress, unsigned mask);

extern void XGpio_Channel_Interrupt(void* baseAddress, unsigned mask);

extern void XGpio_Ack_Interrupt(void* baseAddress, unsigned channel);

#endif
/**
 * @endcond
 */



#endif
/**
 * @}
 * @}
 */

