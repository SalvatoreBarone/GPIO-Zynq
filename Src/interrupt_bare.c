/**
 * @file interrupt_bare.c
 * @author Salvatore Barone <salvator.barone@gmail.com>
 * @date 23 06 2017
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
#include "xparameters.h"
#include "xscugic.h"
#include "myGPIO.h"
#include <assert.h>

typedef struct {
  myGPIO_t user_input; /* button/switches GPIO */
  myGPIO_t leds;       /* GPIO connesso ai led */
} callback_args_t;

void myGPIO_isr_callback(void* args) {
  callback_args_t * c_args = (callback_args_t*) args;
  /* Disabilita le interruzioni (globali) del device GPIO connesso ai button */
  myGPIO_GlobalInterruptDisable(c_args->user_input);
  /* Disabilita le interruzioni (per pin) del device GPIO connesso ai button */
  uint32_t enabledInterrut = myGPIO_EnabledPinInterrupt(c_args->user_input);
  myGPIO_PinInterruptDisable(c_args->user_input, enabledInterrut);
  /* Acquisisce la maschera delle interruzioni non servite */
  uint32_t pendingInterrupt = myGPIO_PendingPinInterrupt(c_args->user_input);

  /* Acquisisce la maschera dei bit asseriti */
  uint32_t value = myGPIO_GetRead(c_args->user_input);
  /* Resetta lo stato di tutti i led */
  myGPIO_SetValue(c_args->leds, MYGPIO_PIN0 | MYGPIO_PIN1 | MYGPIO_PIN2 | MYGPIO_PIN3, MYGPIO_PIN_RESET);
  /* Illumina i led corrispondenti */
  myGPIO_SetValue(c_args->leds, value, MYGPIO_PIN_SET);

  /* Resetta i flag delle interruzioni pendenti, ormai servite */
  myGPIO_PinInterruptAck(c_args->user_input, pendingInterrupt);
  /* Riabilita le interruzioni (per pin) */
  myGPIO_PinInterruptEnable(c_args->user_input, enabledInterrut);
  /* Riabilita le interruzioni (globali) */
  myGPIO_GlobalInterruptEnable(c_args->user_input);
}

int main() {
  XScuGic gic;
  XScuGic_Config *gic_config;
  myGPIO_t led_gpio = NULL;
  myGPIO_t btn_gpio = NULL;
  myGPIO_t swc_gpio = NULL;

  /********************************************************************************************************************/
  /* Configurazione dei device GPIO */
  /* Inizializzazione dei device */
  myGPIO_Init(&led_gpio, XPAR_MYGPIO_0_S00_AXI_BASEADDR);
  myGPIO_Init(&btn_gpio, XPAR_MYGPIO_1_S00_AXI_BASEADDR);
  myGPIO_Init(&swc_gpio, XPAR_MYGPIO_2_S00_AXI_BASEADDR);
  /* Impostazione della modalità di funzionamento */
  myGPIO_SetMode(led_gpio, MYGPIO_PIN0 | MYGPIO_PIN1 | MYGPIO_PIN2 | MYGPIO_PIN3, MYGPIO_MODE_WRITE);
  myGPIO_SetMode(btn_gpio, MYGPIO_PIN0 | MYGPIO_PIN1 | MYGPIO_PIN2 | MYGPIO_PIN3, MYGPIO_MODE_READ);
  myGPIO_SetMode(swc_gpio, MYGPIO_PIN0 | MYGPIO_PIN1 | MYGPIO_PIN2 | MYGPIO_PIN3, MYGPIO_MODE_READ);
  /* Inizializzazione del valore */
  myGPIO_SetValue(led_gpio, MYGPIO_PIN0 | MYGPIO_PIN1 | MYGPIO_PIN2 | MYGPIO_PIN3, MYGPIO_PIN_RESET);
  myGPIO_SetValue(btn_gpio, MYGPIO_PIN0 | MYGPIO_PIN1 | MYGPIO_PIN2 | MYGPIO_PIN3, MYGPIO_PIN_RESET);
  myGPIO_SetValue(swc_gpio, MYGPIO_PIN0 | MYGPIO_PIN1 | MYGPIO_PIN2 | MYGPIO_PIN3, MYGPIO_PIN_RESET);
  /* Abilitazione delle interruzioni */
  /* Nota: se il sottosistema delle interruzioni non è stato configurato, le interruzioni provenienti dai device
   * vengono ignorate. */
  myGPIO_GlobalInterruptEnable(btn_gpio);
  myGPIO_GlobalInterruptEnable(swc_gpio);
  myGPIO_PinInterruptEnable(btn_gpio, MYGPIO_PIN0 | MYGPIO_PIN1 | MYGPIO_PIN2 | MYGPIO_PIN3);
  myGPIO_PinInterruptEnable(swc_gpio, MYGPIO_PIN0 | MYGPIO_PIN1 | MYGPIO_PIN2 | MYGPIO_PIN3);

  /********************************************************************************************************************/
  /* Configurazione del sottosistema delle interruzioni */
  /* Inizializza il sottosistema delle interruzioni*/
  Xil_ExceptionInit();
  
  /* Recupera la struttura dati di configurazione del GIC dalla tabella di look-up */
  assert(NULL != (gic_config = XScuGic_LookupConfig(XPAR_SCUGIC_0_DEVICE_ID)));

  /* Inizializzazione del GIC */
  /*   1. Imposta l'indirizzo base del device */
  assert(XST_SUCCESS == XScuGic_CfgInitialize(&gic, gic_config, gic_config->CpuBaseAddress));
  /*   2. Registra la ISR principale */
  Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,(Xil_ExceptionHandler)XScuGic_InterruptHandler, &gic);
  /*   3. Registra le callback per i device GPIO connessi a button e switches */
  callback_args_t btn_args = {btn_gpio, led_gpio}, swc_args = {swc_gpio, led_gpio}; 
  assert(XST_SUCCESS == XScuGic_Connect(&gic, XPAR_FABRIC_MYGPIO_1_INTERRUPT_INTR, (Xil_InterruptHandler)myGPIO_isr_callback, (void*)&btn_args));
  assert(XST_SUCCESS == XScuGic_Connect(&gic, XPAR_FABRIC_MYGPIO_2_INTERRUPT_INTR, (Xil_InterruptHandler)myGPIO_isr_callback, (void*)&swc_args));
  /*   4. abilita le linee di interruzioni a cui sono associati i device GPIO connessi a button e switches */
  XScuGic_Enable(&gic, XPAR_FABRIC_MYGPIO_1_INTERRUPT_INTR);
  XScuGic_Enable(&gic, XPAR_FABRIC_MYGPIO_2_INTERRUPT_INTR);
  
  /* Abilitazione degli interrupt del processing-system */
  Xil_ExceptionEnable();

  for (;;);
  return 0;
}


