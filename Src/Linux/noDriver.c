/**
 * @file noDriver.c
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
 * @example noDriver.c
 * Il file noDriver.c contiene un programma di esempio per l'interfacciamento con una periferica myGPIO.
 * L'esempio mostra come possa, un programma userspace in esecuzione su sistema operativo Linux,
 * interfacciarsi con un device myGPIO agendo direttamente sui registri di memoria, senza mediazione di
 * altri driver, usando il device-file /dev/mem.
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
  printf("noDriver -a gpio_phisycal_address -w|m <hex-value> -r\n");
  printf("\t-m <hex-value>: scrive nel registro \"mode\"\n");
  printf("\t-w <hex-value>: scrive nel registro \"write\"\n");
  printf("\t-r: legge il valore del registro \"read\"\n");
  printf("I parametri possono anche essere usati assieme.\n");
}

/**
 * @brief Effettua il parsing dei parametri passati al programma
 *
 * @param [in]   argc
 * @param [in]   argv
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
 * @param [in] op_mode        sarà impostato ad 1 se l'utente intende effettuare scrittuara su mode
 * @param [in] mode_value     conterrà il valore che l'utente intende scrivere nel registro mode
 * @param [in] op_write       sarà impostato ad 1 se l'utente intende effettuare scrittuara su write
 * @param [in] write_value    conterrà il valore che l'utente intende scrivere nel registro write
 * @param [in] op_read        sarà impostato ad 1 se l'utente intende effettuare lettura da read
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
    read_value = myGPIO_GetRead(gpio);
    printf("Lettura dat registro read: %08x\n", read_value);
  }
}


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

  /** <h4>Apertura di /dev/mem</h4>
   * In questo specifico esempio l'interfacciamento avviene da user-space, agendo direttamente sui registri
   * di memoria, senza mediazione di altri driver, usando il device /dev/mem.
   * Questo presuppone che si sia nelle condizioni di poter calcolare dell'indirizzo di memoria virtuale
   * del device. <br>
   * L'accesso al device /dev/mem viene ottenuto mediante la system-call open():
   * @code
   * #include <sys/stat.h>
   * #include <fcntl.h>
   * int open(const char *path, int oflag, ...  );
   * @endcode
   * la quale restituisce il descrittore del file /dev/mem, usato nel seguito per effettuare le operazioni
   * di I/O. I valori del parametro oflag specificano il modo in cui il file /dev/mem viene aperto. In questo
   * caso viene usato O_RDWR, il quale garantisce accesso in lettura ed in scrittura. Altri valori sono O_RDONLY,
   * il quale garantisce accesso in sola lettura, ed O_WRONLY, che, invece, garantisce accesso in sola scrittura.
   */
  int descriptor = open ("/dev/mem", O_RDWR);
  if (descriptor < 1) {
    perror(argv[0]);
    return -1;
  }

  /** <h4>Calcolo dell'indirizzo virtuale del device.</h4>
   * Linux implementa la segregazione della memoria. Vale a dire che un processo può accedere solo
   * agli indirizzo di memoria (virtuali) appartenenti al suo address-space.
   * Se è necessario effettuare un accesso ad un indirizzo specifico, bisogna effettuare il mapping
   * di quell'indirizzo nell'address space del processo.
   * Linux implementa la paginazione della memoria, quindi l'indirizzo del quale si desidera effettuare
   * il mapping, apparterrà ad una specifica pagina di memoria. Per sapere a quale pagina  appartenga
   * l'idirizzo, è necessario conoscere quale sia la dimensione delle pagine di memoria. Tipicamente
   * la dimensione delle pagine è una potenza del due.
   * Si supponga che l'indirizzo di cui si vuole fare il mapping è <b>0x43C002F0</b> e che la dimensione delle
   * pagine sia 16KB.
   * Scrivendo la dimensione delle pagine in esadecimale <br>
   *                   <center><b>0x00002000</b></center><br>
   * sottraendo 1 <br>
   *                   <center><b>0x00001FFF</b></center><br>
   * negando <br>
   *                   <center><b>0xFFFFE000</b></center><br>
   * si ottiene una maschera che, posta in and con un indirizzo, restituisce l'indirizzo della pagina di
   * memoria a cui l'indirizzo appartiene. In questo caso <br>
   *             <center><b>0x43C002F0 & 0xFFFFE000 = 0x43C00000</b></center><br>
   * L'indirizzo della pagina potrà essere usato per il mapping, ma per accedere allo specifico indirizzo
   * è necessario calcolarne l'offset, sottraengogli l'indirizzo della pagina. In questo modo, dopo aver
   * effettuato il mapping, si potrà accedere allo stesso a partire dall'indirizzo virtuale della pagina
   * stessa.
   *
   *
   */
  uint32_t page_size = sysconf(_SC_PAGESIZE);    // dimensione della pagina
  uint32_t page_mask = ~(page_size-1);           // maschera di conversione indirizzo -> indirizzo pagina
  uint32_t page_addr = gpio_addr & page_mask;    // indirizzo della "pagina fisica" a cui è mappato il device
  uint32_t offset    = gpio_addr - page_addr;    // offset del device rispetto all'indirizzo della pagina
  /** <h4>Conversione dell'indirizzo fisico in indirizzo virtuale</h4>
   * La "conversione" dell'indirizzo fisico del device in indirizzo virtuale appartenente allo spazio di
   * indirizzamento del processo viene effettuato tramite la chiamata alla funzione mmap(), la quale stabilisce
   * un mapping tra lo spazio di indirizzamento di un processo ed un file, una porzione di memoria condivisa o
   * un qualsiasi altro memory-object, restituendo un indirizzo virtuale valido, attraverso il quale è possibile
   * accedere al blocco di memoria fisico.
   * @code
   *    #include <sys/mman.h>
   *    void *mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off);
   * @endcode
   * Per semplicità supponiamo che la chiamata alla funzione sia la seguente:
   *               <center>pa=mmap(addr, len, prot, flags, fildes, off);</center>
   * la semantica dei diversi parametri è:
   *   - pa: indirizzo virtuale dell'address-space locale del processo, a cui viene eseguito il map; se il mapping
   *     ha successo viene restituito qualcosa di diverso da MAP_FAILED;
   *   - addr:
   *   - len: lunghezza, in byte, del blocco mappato; in questo caso viene usato il valore restituito da
   *     sysconf(_SC_PAGESIZE);
   *   - prot: specifica i permessi di accesso al blocco di memoria del quale si sta facendo il mapping;
   *     - PROT_READ indica che il blocco può essere letto;
   *     - PROT_WRITE indica che il blocco può essere scritto;
   *     - PROT_NONE sta ad indicare che il blocco non può essere acceduto;
   *       .
   *   - flags:fornisce informazioni aggiuntive circa la gestione del blocco di dati di cui si sta facendo il
   *     mapping; il valore del flag può essere uno dei seguenti:
   *       - MAP_SHARED: modifiche al blocco sono condivise con chiunque altri lo stia usando;
   *       - MAP_PRIVATE: le modifiche sono primate;
   *    .
   *   - filedes: descrittore del file /dev/mem
   *   - off: indirizzo fisico del blocco che si intente mappare; è necessario che sia allineato alla dimensione
   *     della pagina di memoria, così come restituito dalla funzione sysconf(_SC_PAGESIZE);
   *
   * In questo caso la chiamata a mmap avviene con i seguenti parametri:
   * @code
   * uint32_t page_size = sysconf(_SC_PAGESIZE);    // dimensione della pagina
   * uint32_t page_mask = ~(page_size-1);      // maschera di conversione indirizzo -> indirizzo pagina
   * uint32_t page_addr = gpio_addr & page_mask;    // indirizzo della "pagina fisica" a cui è mappato il device
   * uint32_t offset = gpio_addr - page_addr;    // offset del device rispetto all'indirizzo della pagina
   * void* vrt_page_addr = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, descriptor, page_addr) + offset;
   * @endcode
   */
  void* vrt_page_addr = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, descriptor, page_addr);
  if (vrt_page_addr == MAP_FAILED) {
    printf("Mapping indirizzo fisico - indirizzo virtuale FALLITO!\n");
    return -1;
  }
  void* vrt_gpio_addr = vrt_page_addr + offset;  // indirizzo virtuale del device gpio
  /** <h4>Operazioni sul device</h4>
   * Una volta effettuato il mapping, le operazioni preventivate con l'invocazione del programma vengono effettuate
   * dalla funzione gpio_op(). Si rimanda alla sua documentazione per i dettagli sulle operazioni effettuate().
   */
  gpio_op(vrt_gpio_addr, op_mode, mode_value, op_write, write_value, op_read);

  munmap(vrt_page_addr, page_size);
  close(descriptor);

  return 0;
}


