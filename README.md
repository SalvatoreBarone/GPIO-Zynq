# Zynq7000DriverPack
Esempi didattici di Device Driver per la famiglia di MPSoC Xilinx Zynq-7000

L'implementazione si trova nella cartella <b>Src</b>. All'interno di essa e' presente:
- l'implementazione VHDL di una periferica GPIO, con supporto agli interrupt, nella directory Src/myGPIO/VHDL;
- i driver in C della suddetta periferica, che ne permettono l'utilizzo in sistemi privi si SO, nella directory Src/myGPIO/bare-metal
- un board support package, comprensivo di driver per la gestione di button, switch e led, per la board Digilent Zynq nella
  directory Src/myGPIO/bare-metal/Zybo, da usare in sistemi privi si SO ;
- un driver per display lcd alfanuerico Hitachi HD44780, che fa uso della periferica GPIO di cui sopra e dei relativi driver
  nella cartella Src/myGPIO/bare-metal/HD44780
- un device-driver kernel-mode per Linux, nella cartella Src/myGPIO/linux-driver

La documentazione si trova nella cartella <b>Doc</b>. All'interno di essa sono presenti:
- la cartella "VHDL", contenente la documentazione dell'implementazione VHDL della periferica myGPIO; al suo interno e'
  disponibile la documentazione in html o pdf;
- la cartella "C", contenente la documentazione dell'implementazione C del driver per la periferica myGPIO, del BSP per
  la board Digilent Zynq, e, soprattutto, degli esempi di device-driver per Linux, scritti in modo didattizo. La
  documentazione e' disponibile in html o pdf ed e' comprensiva di esempi.

<b>Il progetto e' in continua evoluzione ed e' aggiornato molto frequentemente.</b>

# Contenuto 
Src/<br>
├── Examples<br>
│   ├── bsp_example.c: Esempio d'uso del driver bare-metal myGPIO, BSP e driver per display lcd Hitachi HD44780;<br>
│   ├── interrupt_bare.c: Esempio d'uso del driver bare-metal myGPIO con interruzioni;<br>
│   ├── Makefile: Makefile che consente di compilare tutti gli esempi che seguono;<br>
│   ├── mygpiok.c: Programma userspace che fa uso del driver myGPIO in kernel-mode;<br>
│   ├── noDriver.c: Programma userspace che interagisce con device myGPIO attraverso /dev/mem<br>
│   ├── readAll.c: Programma userspace che legge un blocco di registri di memoria consecutivi attraverso /dev/mem<br>
│   ├── uio.c: Programma userspace che interagisce con device myGPIO attraverso driver UIO<br>
│   ├── uio-int.c: Programma userspace che interagisce con device myGPIO attraverso driver UIO con interruzioni<br>
│   ├── xil_gpio.c: Modulo che implementa un driver base per interagire con un device Xilinx GPIO (sorgente);<br>
│   └── xil_gpio.h: Modulo che implementa un driver base per interagire con un device Xilinx GPIO (header);<br>
└── myGPIO<br>
    ├── bare-metal<br>
    │   ├── HD44780: Driver bare-metal per display lcd Hitachi HD44780, usa il driver myGPIO<br>
    │   │   ├── hd44780.c: sorgente<br>
    │   │   └── hd44780.h: header<br>
    │   ├── myGPIO.c: Driver bare-metal per device myGPIO (sorgente)<br>
    │   ├── myGPIO.h: Driver bare-metal per device myGPIO (header)<br>
    │   └── ZyboBSP: BSP per Digilent Zybo che usa driver myGPIO<br>
    │       ├── Zybo.h<br>
    │       ├── ZyboButton.c<br>
    │       ├── ZyboButton.h<br>
    │       ├── ZyboLed.c<br>
    │       ├── ZyboLed.h<br>
    │       ├── ZyboSwitch.c<br>
    │       └── ZyboSwitch.h<br>
    ├── linux-driver: Driver Linux (modulo kernel) per device myGPIO<br>
    │   ├── configure.sh: script di configurazione per la compilazione, va modificato opportunamente;<br>
    │   ├── Makefile: permette la compilazione del modulo kernel;<br>
    │   ├── myGPIOK_list.c: implementazione della struttura dati myGPIOK_list_t (mantiene un riferimento agli oggetti myGPIOK_t gestiti)<br>
    │   ├── myGPIOK_list.h: definizione della struttura dati myGPIOK_list_t (mantiene un riferimento agli oggetti myGPIOK_t gestiti)<br>
    │   ├── myGPIOK_main.c: implementazione delle funzioni principali del driver kernel;<br>
    │   ├── myGPIOK_t.c: implementazione della struttura myGPIOK_t, che rappresenta un device myGPIO a livello kernel<br>
    │   └── myGPIOK_t.h: definizione della struttura myGPIOK_t, che rappresenta un device myGPIO a livello kernel<br>
    └── VHDL: Implementazione VHDL del device myGPIO<br>
        ├── GPIOsingle.vhd: gpio singolo;<br>
        ├── GPIOarray.vhd: vettore di gpio singoli, di dimensione variabile<br>
        ├── myGPIO_AXI.vhd: implementazione della logica AXI4-Lite<br>
        └── myGPIO.vhd: interfaccia AXI4-Lite del device myGPIO<br>
<br>
Doc/<br>
│	device.pdf: Documentazione pdf dell'implementazione VHDL del device myGPIO<br>
│	driver.pdf: Documentazione pdf dei driver in C, sia sia bare-metal che Linux, con esempi<br>
├── C: Documentazione dei driver in C, sia bare-metal che Linux, con esempi, in html e latex<br>
│   ├── html<br>
│   │   └── index.html: Indice della documentazione (HTML)<br>
│   └── latex<br>			
├── schemes: Schemi a blocchi usati nella documentazione<br>
└── vhdl: Documentazione interna dell'implementazione VHDL del device myGPIO<br>
    ├── html<br>
    │   └── index.html: Indice della documentazione (HTML)<br>
    └── latex<br>
