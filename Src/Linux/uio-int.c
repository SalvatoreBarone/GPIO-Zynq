/**
 * @file uio-int.c
 * @author Salvatore Barone <salvator.barone@gmail.com>
 * @date 14 06 2017
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
 * @example uio-int.c
 * Il file uio-int.c √® un programma di esempio per l'interfacciamento con una periferica myGPIO. L'esempio
 * mostra come possa, un programma userspace in esecuzione su sistema operativo Linux, interfacciarsi
 * con un device myGPIO, interagendo con esso attraverso il driver UIO. Questo specifico esempio mostra
 * come utilizzare gli interrupt, gestiti dal driver UIO, per effettuare la lettura dopo che il device
 * myGPIO abbia generato interruzione.
 *
 * @warning Se nel device tree source non viene indicato
 * <center>compatible = "generic-uio";</center>
 * tra i driver compatibili con il device, il driver UIO non viene correttamente istanziato ed il
 * programma non funzioner√†.
 */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "myGPIO.h"
#include "xil_gpio.h"

/**
 * @brief Stampa un messaggio che fornisce indicazioni sull'utilizzo del programma
 */
void howto(void) {
  printf("Uso:\n");
  printf("uio -d /dev/uioX -w|m <hex-value> -r\n");
  printf("\t-m <hex-value>: scrive nel registro \"mode\"\n");
  printf("\t-w <hex-value>: scrive nel registro \"write\"\n");
  printf("\t-r: legge il valore del registro \"read\"\n");
  printf("I parametri possono anche essere usati assieme.\n");
}


/**
 * @brief Effettua il parsing dei parametri passati al programma
 * @param [in]   argc
 * @param [in]   argv
 * @param [out] uio_file    file uio da usare
 * @param [out] op_mode      sar√† impostato ad 1 se l'utente intende effettuare scrittuara su mode
 * @param [out] mode_value    conterr√† il valore che l'utente intende scrivere nel registro mode
 * @param [out] op_write    sar√† impostato ad 1 se l'utente intende effettuare scrittuara su write
 * @param [out] write_value    conterr√† il valore che l'utente intende scrivere nel registro write
 * @param [out] op_read      sar√† impostato ad 1 se l'utente intende effettuare lettura da read
 *
 * @retval 0 se il parsing ha successo
 * @retval -1 se si verifica un errore
 *
 * @details
 */
int parse_args(  int     argc,
        char    **argv,
        char    **uio,
        uint8_t    *op_mode,
        uint32_t  *mode_value,
        uint8_t    *op_write,
        uint32_t  *write_value,
        uint8_t    *op_read)
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
 * La funzione pu√≤ essere chiamata ripetutamente, fino a quando non restituisce -1, ad indicare che sono stati
 * analizzati tutti i parametri passati al programma.
 * Quando getopt() trova un'opzione, restituisce quel carattere ed aggiorna la variabile globale optind, che punta
 * al prossimo parametro contenuto in argv.
 * La stringa optstring indica quali sono le opzioni considerate. Se una opzione √® seguita da ':' vuol dire che
 * essa √® seguita da un argomento. Tale argomento pu√≤ essere ottenuto mediante la variabile globale optarg.
 *
 * <h4>Parametri riconosciuti</h4>
 * La funzione riconosce i parametri:
 *  - 'd' : seguito dal percordo del device /dev/uioX col quale interagire
 *  - 'w' : operazione di scrittura, seguito dal valore che si intende scrivere, in esadecimale; la scrittura verr√†
 *          effettuata sul registro WRITE;
 *  - 'm' : impostazione modalit√†, seguito dalla modalit√† col quale impostare il device; la scrittura verr√†
 *          effettuata sul registro MODE;
 *  - 'r' : operazione di lettura, primo di argomento; la lettura viene effettuata dal registro READ ed √® non
 *          bloccante, nel senso che viene semplicemente letto il contenuto del registro.
 */
  while((par = getopt(argc, argv, "d:w:m:r")) != -1) {
    switch (par) {
    case 'd' :
      *uio = optarg;
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
 * @param [in] uio_descriptor  descrittore del file /dev/uioX usato
 * @param [in] op_mode      sar√† impostato ad 1 se l'utente intende effettuare scrittuara su mode
 * @param [in] mode_value    conterr√† il valore che l'utente intende scrivere nel registro mode
 * @param [in] op_write      sar√† impostato ad 1 se l'utente intende effettuare scrittuara su write
 * @param [in] write_value    conterr√† il valore che l'utente intende scrivere nel registro write
 * @param [in] op_read      sar√† impostato ad 1 se l'utente intende effettuare lettura da read
 *
 * @details
 * La funzione viene invocata dopo che sia stato eseguito il parsing dei parametri passati al programma quando
 * esso viene invocato. √® stata scritta per funzionare sia con il GPIO Xilinx che con il GPIO custom myGPIO.
 * √® possibile utilizzare il primo definendo la macro __XIL_GPIO__. Effettua, sul device, le operazioni
 * impostate, in accordo con i parametri passati al programma alla sua invocazione.
 */
void gpio_op (
        void * const vrt_gpio_addr,
        int          uio_descriptor,
        uint8_t      op_mode,
        uint32_t     mode_value,
        uint8_t      op_write,
        uint32_t     write_value,
        uint8_t      op_read)
{
  printf("Indirizzo gpio: %08x\n", (uint32_t)vrt_gpio_addr);
  myGPIO_t gpio;
  myGPIO_Init(&gpio, (uint32_t) vrt_gpio_addr);

  /** <h4>Impostazione della modalit√† di funzionamento</h4>
   * Nel caso in cui si stia operando su un device GPIO Xilinx, le operazioni di impostazione della modalit√† di
   * funzionamento del GPIO vengono effettuate scrivendo direttamente sul registro MODE del device. In caso contrario
   * si √® preferito utilizzare la funzioni MYGPIO_PIN_SETMode() (Si veda il modulo myGPIO). Funzionalmente non c'√®
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
   * si √® preferito utilizzare la funzioni MYGPIO_PIN_SETValue() (Si veda il modulo myGPIO). Funzionalmente non c'√®
   * differenza.
   */
  if (op_write == 1) {
    myGPIO_SetValue(gpio, write_value, MYGPIO_PIN_SET);
    myGPIO_SetValue(gpio, ~write_value, MYGPIO_PIN_RESET);
    printf("Scrittura sul registro write: %08x\n", write_value);
  }
  /** <h4>Operazione di lettura con interrupt</h4>
   * Nel caso in cui si stia operando su un device GPIO Xilinx, le operazioni di lettura del valore dei pin
   * del device GPIO vengono effettuate leggendo direttamente dal registro READ del device. In caso contrario
   * si √® preferito utilizzare la funzioni myGPIO_getRead() (Si veda il modulo myGPIO). Funzionalmente non c'√®
   * differenza. <b>La lettura avviene usando il meccanismo delle interruzioni</b>
   *
   * NOTA: la parte di codice per il GPIO Xilinx √® stata scritta per hardware configurato in modo che il channel
   * 1 del gpio fosse connessi esclusivamente ai led,mentre switch e button fossero connessi al channel 2 dello
   * stesso GPIO. Il channel 1 ha dimensione 4 bit, mentre il channel 2 √® da 8 bit.
   */
  if (op_read == 1) {
    uint32_t read_value = 0;
    // interrupt enable (interni alla periferica)
    myGPIO_GlobalInterruptEnable(gpio);
    myGPIO_PinInterruptEnable(gpio, MYGPIO_PIN0|MYGPIO_PIN1|MYGPIO_PIN2|MYGPIO_PIN3);

    /**<h4>Attesa dell'arrivo di una interruzione</h4>
     * Gli interrupt sono gestiti effettuando una lettura bloccante su /dev/uioX. Una read() su /dev/uioX
     * fa in modo che il processo venga sospeso ed inserito nella cosa dei processi in attesa di un evento
     * su quel file. Appena l'interrupt si manifesta, il processo viene posto nella cosa dei processi
     * pronti. La funzione read() consente di ottenere anche il numero totale di interrupt manifestatisi
     * su quella particolare periferica.
     * La read() restituisce il numero di interrupt che si sono manifestati.
     * Quando un device possiede pi√Ļ di una sorgente di interrupt interna, ma non possiede maschere IRQ
     * differenti o registri di stato differenti, potrebbe essere impossibile, per un programma in
     * userspace, determinare quale sia la sorgente di interrupt se l'handler implementato nel kernel
     * le disabilita scrivendo nei registri.
     */
    printf("Attesa dell'interruzione\n");
    uint32_t interrupt_count = 1;
    if (read(uio_descriptor, &interrupt_count, sizeof(uint32_t)) != sizeof(uint32_t)) {
      printf("Read error!\n");
      return;
    }
    /**<h4>Servizio dell'interruzione<h4>
     * Al ritorno dalla read() √® possibile servire l'interruzione. Si noti che il codice ivi eseguito tutto
     * √® fourch√© una ISR. La vera ISR viene chiamata dal sistema operativo ed √® definita all'interno del
     * driver UIO.
     * Dopo aver disabilitato gli interrupt della periferica, viene letto il valore del registro READ e
     * stampato il valore che esso conteneva.
     */
    printf("Interrupt count: %08x\n", interrupt_count);
    // disabilitazione interrupt (interni alla periferica)
    myGPIO_GlobalInterruptDisable(gpio);
    myGPIO_PinInterruptDisable(gpio, MYGPIO_PIN0|MYGPIO_PIN1|MYGPIO_PIN2|MYGPIO_PIN3);

    // "servizio" dell'interruzione.
    // lettura del registro
    read_value = myGPIO_GetRead(gpio);
    printf("Lettura dat registro read: %08x\n", read_value);
    /**<br>
     * In questo caso √® stato ritenuto opportuno, a titolo di esempio, mostrare come sia possibile bloccare
     * il programma, dopo aver "servito" l'interruzione scatenata alla pressione di un tasto, fino a quando
     * il tasto (o i tasti) premuti non siano riportati alla posizione di riposo.
     * Lo stato del registro READ della periferica viene ripetutamente letto all'interno di un hot-loop, fino
     * a quando non assume valore nullo. In tal caso si ha la certezza che i button o gli switch, in questo caso,
     * siano stati riportati alla posizione di riposo.
     * Si tenga presente che il device GPIO Xilinx generata una interruzione sia alla pressione che al rilascio di
     * uno dei button o di uno degli switch
     */
    while(myGPIO_GetRead(gpio) != 0);
    /**<br>
     * Dopo che button e switch siano stati riportati alla posizione di riposo, viene inviato l'ack al device, per
     * segnalargli che l'interrupt √® stato servito.
     */
    // invio dell'ack alla periferica
    myGPIO_PinInterruptAck(gpio, myGPIO_PendingPinInterrupt(gpio));
    /**<h4>Riabilitare gli interrupt UIO</h4>
     * Per lasciare inalterati i registri della periferica il kernel deve disabilitare completamente le
     * interruzioni per la linea di interrupt cui la periferica √® connessa, in modo che il programma userspace
     * possa determinare la causa scatenante l'interruzione.
     * Una volta terminate le operazioni, per√≤, il programma userspace non pu√≤ riabilitare le interruzioni,
     * motivo per cui il driver implementa anche una funzione write().
     * La funzione write(), chiamata su /dev/uioX, consente di riabilitare le interruzioni per quella
     * specifica periferica, scrivendo 1.
     */
    uint32_t reenable = 1;
    if (write(uio_descriptor, (void*)&reenable, sizeof(uint32_t)) != sizeof(uint32_t)) {
      printf("Write error!\n");
      return;
    }
  }
}

/**
 * @brief funzione main().
 *
 * @details
 */
int main(int argc, char** argv) {
  char* uio_file = 0;      // nome del file uio
  uint8_t op_mode = 0;    // impostato ad 1 se l'utente intende effettuare scrittuara su mode
  uint32_t mode_value;    // valore che l'utente intende scrivere nel registro mode
  uint8_t op_write = 0;    // impostato ad 1 se l'utente intende effettuare scrittuara su write
  uint32_t write_value;    // valore che l'utente intende scrivere nel registro write
  uint8_t op_read = 0;    // impostato ad 1 se l'utente intende effettuare lettura da read

/** <h4>Parsing dei parametri di invocazione</h4>
 * Il parsing dei parametri passati al programma all'atto della sua invocazione viene effettuato dalla funzione
 * parse_args(). Si rimanda alla sua documentazione per i dettagli sui parametri riconosciuti.
 */
  if (parse_args(argc, argv, &uio_file, &op_mode, &mode_value, &op_write, &write_value, &op_read) == -1)
    return -1;
/**
 * Se non viene specificato il device UIO col quale interagire √® impossibile continuare.
 * Per questo motivo, in questo caso, il programma viene terminato.
 */
  if (uio_file == 0) {
    printf("√® necessario specificare l'indirizzo di memoria del device.\n");
    howto();
    return -1;
  }
/** <h4>Accesso ad un device /dev/uioX</h4>
 * Il driver generic-UIO √® il driver generico per eccellenza. Ad ogni periferica compatibile con
 * UIO √® associato un file diverso in /dev/uioX attraverso il quale √® possibile raggiungere il device.
 * Tale file sar√† /dev/uio0 per il primo device, /dev/uio1 per il secondo, /dev/uio2 per il terzo e cos√¨ via.
 * on for subsequent devices. Tale file pu√≤ essere usato per accedere allo spazio degli indirizzi
 * del device usando mmap().
 *
 * In questo caso, rispetto all'esempio noDriver, accedere al device √® estremamente pi√Ļ semplice.
 * Se il device √® compatibile con il driver UIO, √® possibile "aprire" un file in /dev/uioX,
 * effettuare il mapping, connettendo il device allo spazio di indirizzamento del processo, senza la
 * necessit√† di conoscere l'indirizzo fisico della periferica col quale di intende comunicare.
 *
 * L'accesso al device /dev/uioX viene ottenuto mediante la system-call open():
 * @code
 * #include <sys/stat.h>
 * #include <fcntl.h>
 * int open(const char *path, int oflag, ...  );
 * @endcode
 * la quale restituisce il descrittore del file /dev/uioX, usato nel seguito per effettuare le operazioni
 * di I/O. I valori del parametro oflag specificano il modo in cui il file /dev/uioX viene aperto. In questo
 * caso viene usato O_RDWR, il quale garantisce accesso in lettura ed in scrittura. Altri valori sono O_RDONLY,
 * il quale garantisce accesso in sola lettura, ed O_WRONLY, che, invece, garantisce accesso in sola scrittura.
 */
  int descriptor = open (uio_file, O_RDWR);
  if (descriptor < 1) {
    perror(argv[0]);
    return -1;
  }
/** <h4>Mapping un device /dev/uioX</h4>
 * La "conversione" dell'indirizzo fisico del device in indirizzo virtuale appartenente allo spazio di
 * indirizzamento del processo viene effettuato tramite la chiamata alla funzione mmap(), la quale stabilisce
 * un mapping tra lo spazio di indirizzamento di un processo ed un file, una porzione di memoria condivisa o
 * un qualsiasi altro memory-object, restituendo un indirizzo virtuale valido, attraverso il quale √® possibile
 * accedere al blocco di memoria fisico.
 * @code
 *    #include <sys/mman.h>
 *    void *mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off);
 * @endcode
 * Per semplicit√† supponiamo che la chiamata alla funzione sia la seguente:
 *               <center>pa=mmap(addr, len, prot, flags, fildes, off);</center>
 * la semantica dei diversi parametri √®:
 *   - pa: indirizzo virtuale dell'address-space locale del processo, a cui viene eseguito il map; se il mapping
 *     ha successo viene restituito qualcosa di diverso da MAP_FAILED;
 *   - addr:
 *   - len: lunghezza, in byte, del blocco mappato; in questo caso viene usato il valore restituito da
 *     sysconf(_SC_PAGESIZE);
 *   - prot: specifica i permessi di accesso al blocco di memoria del quale si sta facendo il mapping;
 *     - PROT_READ indica che il blocco pu√≤ essere letto;
 *     - PROT_WRITE indica che il blocco pu√≤ essere scritto;
 *     - PROT_NONE sta ad indicare che il blocco non pu√≤ essere acceduto;
 *       .
 *   - flags:fornisce informazioni aggiuntive circa la gestione del blocco di dati di cui si sta facendo il
 *     mapping; il valore del flag pu√≤ essere uno dei seguenti:
 *       - MAP_SHARED: modifiche al blocco sono condivise con chiunque altri lo stia usando;
 *       - MAP_PRIVATE: le modifiche sono primate;
 *    .
 *   - filedes: descrittore del file /dev/mem
 *   - off: indirizzo fisico del blocco che si intente mappare; √® necessario che sia allineato alla dimensione
 *     della pagina di memoria, cos√¨ come restituito dalla funzione sysconf(_SC_PAGESIZE);
 *
 * In questo caso la chiamata a mmap avviene con i seguenti parametri:
 * @code
 * uint32_t page_size = sysconf(_SC_PAGESIZE);    // dimensione della pagina
 * void* vrt_gpio_addr = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, descriptor, 0);
 * @endcode
 *
 * Rispetto al "driver" nodriver, la chiamata differisce per un solo perticolare: essendo descriptor il
 * descrittore di uioX, e l'offset specificato nullo, la funzione restituisce direttamente l'indirizzo virtuale
 * del device nello spazio di indirizzamento del processo.
 */
  uint32_t page_size = sysconf(_SC_PAGESIZE);    // dimensione della pagina
  void* vrt_gpio_addr = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, descriptor, 0);
  if (vrt_gpio_addr == MAP_FAILED) {
    printf("Mapping indirizzo fisico - indirizzo virtuale FALLITO!\n");
    return -1;
  }
/** <h4>Operazioni sul device</h4>
 * Una volta effettuato il mapping, le operazioni preventivate con l'invocazione del programma vengono effettuate
 * dalla funzione gpio_op(). Si rimanda alla sua documentazione per i dettagli sulle operazioni effettuate().
 */
  gpio_op(vrt_gpio_addr, descriptor, op_mode, mode_value, op_write, write_value, op_read);

  munmap(vrt_gpio_addr, page_size);
  close(descriptor);

  return 0;
}


