/**
 * @file sbagliato.c
 * @author Salvatore Barone <salvator.barone@gmail.com>
 * @date 12 06 2017
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
 * @example sbagliato.c
 * Il file sbagliato.c contiene un programma di esempio per l'interfacciamento con una periferica myGPIO.
 * L'esempio mostra come NON possa, un programma userspace in esecuzione su sistema operativo Linux,
 * interfacciarsi con un device myGPIO agendo direttamente sui registri di memoria usando l'indirizzo 
 * fisico.
 */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "myGPIO.h"

/**
 * @brief Stampa un messaggio che fornisce indicazioni sull'utilizzo del programma
 */
void howto(void) {
  printf("Uso:\n");
  printf("sbagliato -a gpio_phisycal_address -w|m <hex-value> -r\n");
  printf("\t-m <hex-value>: scrive nel registro \"mode\"\n");
  printf("\t-w <hex-value>: scrive nel registro \"write\"\n");
  printf("\t-r: legge il valore del registro \"read\"\n");
  printf("I parametri possono anche essere usati assieme.\n");
}

/**
 * @brief Effettua il parsing dei parametri passati al programma
 *
 * @param [in]  argc
 * @param [in]  argv
 * @param [out] gpio_address  conterrà l'indirizzo di memoria del device gpio
 * @param [out] op_mode       sarà impostato ad 1 se l'utente intende effettuare scrittuara su mode
 * @param [out] mode_value    conterrà il valore che l'utente intende scrivere nel registro mode
 * @param [out] op_write      sarà impostato ad 1 se l'utente intende effettuare scrittuara su write
 * @param [out] write_value   conterrà il valore che l'utente intende scrivere nel registro write
 * @param [out] op_read       sarà impostato ad 1 se l'utente intende effettuare lettura da read
 *
 * @retval 0 se il parsing ha successo
 * @retval -1 se si verifica un errore
 *
 * @details
 */
int parse_args(  
    int                 argc,
    char       ** const argv,
    uint32_t    * const gpio_address,
    uint8_t     * const op_mode,
    uint32_t    * const mode_value,
    uint8_t     * const op_write,
    uint32_t    * const write_value,
    uint8_t     * const op_read)
{
  int par;
  /** <h4>Parsing dei parametri del programma.</h4>
   * Il parsing viene effettuato usando la funzione getopt().
   * @code
   * #include <unistd.h>
   * int getopt(int argc, char * const argv[], const char *optstring);
   * @endcode
   * Essa prende in input i parametri argc ed argv passati alla funzione main() quando il programma viene invocato.
   * Quando una delle stringhe che compongono argv comincia con il carattere '-', getopt() la considera una opzione.
   * Il carattere immediatamente successivo il '-' identifica la particolare opzione.
   * La funzione può essere chiamata ripetutamente, fino a quando non restituisce -1, ad indicare che sono stati
   * analizzati tutti i parametri passati al programma.
   * Quando getopt() trova un'opzione, restituisce quel carattere ed aggiorna la variabile globale optind, che punta
   * al prossimo parametro contenuto in argv.
   * La stringa optstring indica quali sono le opzioni considerate. Se una opzione è seguita da ':' vuol dire che
   * essa è seguita da un argomento. Tale argomento può essere ottenuto mediante la variabile globale optarg.
   *
   * <h4>Parametri riconosciuti</h4>
   * La funzione riconosce i parametri:
   *  - 'à : seguito dall'indirizzo fisico della periferica con la quale interagire, il quale può essere indicato
   *          in esadecimale;
   *  - 'w' : operazione di scrittura, seguito dal valore che si intende scrivere, in esadecimale; la scrittura verrà
   *          effettuata sul registro WRITE;
   *  - 'm' : impostazione modalità, seguito dalla modalità col quale impostare il device; la scrittura verrà
   *          effettuata sul registro MODE;
   *  - 'r' : operazione di lettura, primo di argomento; la lettura viene effettuata dal registro READ ed è non
   *          bloccante, nel senso che viene semplicemente letto il contenuto del registro.
   */
  while((par = getopt(argc, argv, "a:w:m:r")) != -1) {
    switch (par) {
    case 'a' :
      *gpio_address = strtoul(optarg, NULL, 0);
      break;
    case 'w' :
      *write_value = strtoul(optarg, NULL, 0);
      *op_write = 1;
      break;
    case 'm' :
      *mode_value = strtoul(optarg, NULL, 0);
      *op_mode = 1;
      break;
    case 'r' :
      *op_read = 1;
      break;
    default :
      printf("%c: parametro sconosciuto.\n", par);
      howto();
      return -1;
    }
  }
  return 0;
}


/**
 * @brief Effettua operazioni su un device
 *
 * @param [in] vrt_gpio_addr  indirizzo di memoria del device gpio
 * @param [in] op_mode      sarà impostato ad 1 se l'utente intende effettuare scrittuara su mode
 * @param [in] mode_value    conterrà il valore che l'utente intende scrivere nel registro mode
 * @param [in] op_write      sarà impostato ad 1 se l'utente intende effettuare scrittuara su write
 * @param [in] write_value    conterrà il valore che l'utente intende scrivere nel registro write
 * @param [in] op_read      sarà impostato ad 1 se l'utente intende effettuare lettura da read
 *
 * @details
 * La funzione viene invocata dopo che sia stato eseguito il parsing dei parametri passati al programma quando
 * esso viene invocato. è stata scritta per funzionare sia con il GPIO Xilinx che con il GPIO custom myGPIO.
 * è possibile utilizzare il primo definendo la macro __XIL_GPIO__. Effettua, sul device, le operazioni
 * impostate, in accordo con i parametri passati al programma alla sua invocazione.
 */
void gpio_op (  
        void * const vrt_gpio_addr,
        uint8_t      op_mode,
        uint32_t     mode_value,
        uint8_t      op_write,
        uint32_t     write_value,
        uint8_t      op_read)
{
  printf("Indirizzo gpio: %08x\n", (uint32_t)vrt_gpio_addr);
  myGPIO_t gpio;
  myGPIO_Init(&gpio, (uint32_t)vrt_gpio_addr);

  /** <h4>Impostazione della modalità di funzionamento</h4>
   * Nel caso in cui si stia operando su un device GPIO Xilinx, le operazioni di impostazione della modalità di
   * funzionamento del GPIO vengono effettuate scrivendo direttamente sul registro MODE del device. In caso contrario
   * si è preferito utilizzare la funzioni MYGPIO_PIN_SETMode() (Si veda il modulo myGPIO). Funzionalmente non c'è
   * differenza.
   */
  if (op_mode == 1) {
    myGPIO_SetMode(gpio, mode_value, MYGPIO_MODE_WRITE);
    myGPIO_SetMode(gpio, ~mode_value, MYGPIO_MODE_READ);
    printf("Scrittura sul registro mode: %08x\n", mode_value);
  }
  /** <h4>Operazione di scrittura</h4>
   * Nel caso in cui si stia operando su un device GPIO Xilinx, le operazioni di scrittura del valore dei pin
   * del device GPIO vengono effettuate scrivendo direttamente sul registro WRITE del device. In caso contrario
   * si è preferito utilizzare la funzioni MYGPIO_PIN_SETValue() (Si veda il modulo myGPIO). Funzionalmente non c'è
   * differenza.
   */
  if (op_write == 1) {
    myGPIO_SetValue(gpio, write_value, MYGPIO_PIN_SET);
    myGPIO_SetValue(gpio, ~write_value, MYGPIO_PIN_RESET);
    printf("Scrittura sul registro write: %08x\n", write_value);
  }
  /** <h4>Operazione di lettura</h4>
   * Nel caso in cui si stia operando su un device GPIO Xilinx, le operazioni di lettura del valore dei pin
   * del device GPIO vengono effettuate leggendo direttamente dal registro READ del device. In caso contrario
   * si è preferito utilizzare la funzioni myGPIO_getRead() (Si veda il modulo myGPIO). Funzionalmente non c'è
   * differenza. La lettura è non bloccante: viene semplicemente letto il valore contenuto nel registro
   * perché tale modalità di interazione non permette l'implementazione di un meccanismo di lettura basato su
   * interruzioni.
   */
  if (op_read == 1) {
    uint32_t read_value = 0;
    read_value = myGPIO_GetRead(&gpio);
    printf("Lettura dat registro read: %08x\n", read_value);
  }
}

/**
 * @brief funzione main().
 *
 * @details
 */
int main(int argc, char** argv) {
  uint32_t  gpio_addr   = 0;    // indirizzo di memoria del device gpio
  uint8_t   op_mode     = 0;    // impostato ad 1 se l'utente intende effettuare scrittuara su mode
  uint32_t  mode_value  = 0;    // valore che l'utente intende scrivere nel registro mode
  uint8_t   op_write    = 0;    // impostato ad 1 se l'utente intende effettuare scrittuara su write
  uint32_t  write_value = 0;    // valore che l'utente intende scrivere nel registro write
  uint8_t   op_read     = 0;    // impostato ad 1 se l'utente intende effettuare lettura da read

  /** <h4>Parsing dei parametri di invocazione</h4>
   * Il parsing dei parametri passati al programma all'atto della sua invocazione viene effettuato dalla funzione
   * parse_args(). Si rimanda alla sua documentazione per i dettagli sui parametri riconosciuti.
   */
  if (parse_args(argc, argv, &gpio_addr, &op_mode, &mode_value, &op_write, &write_value, &op_read) == -1)
    return -1;
  /**
   * Se non viene specificato l'indirizzo fisico del device al quale accedere è impossibile continuare.
   * Per questo motivo, in questo caso, il programma viene terminato.
   */
  if (gpio_addr == 0) {
    printf("è necessario specificare l'indirizzo di memoria del device.\n");
    howto();
    return -1;
  }

  gpio_op(gpio_addr, op_mode, mode_value, op_write, write_value, op_read);

  return 0;
}


