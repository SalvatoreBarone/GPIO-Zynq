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
Src/
├── Examples
│   ├── bsp_example.c			Esempio d'uso del driver bare-metal myGPIO, BSP e driver per display lcd Hitachi HD44780
│   ├── interrupt_bare.c		Esempio d'uso del driver bare-metal myGPIO con interruzioni
│   ├── Makefile
│   ├── mygpiok.c				Programma userspace che fa uso del driver myGPIO in kernel-mode
│   ├── noDriver.c				Programma userspace che interagisce con device myGPIO attraverso /dev/mem
│   ├── readAll.c
│   ├── uio.c					Programma userspace che interagisce con device myGPIO attraverso driver UIO
│   ├── uio-int.c				Programma userspace che interagisce con device myGPIO attraverso driver UIO con interruzioni
│   ├── xil_gpio.c
│   └── xil_gpio.h
└── myGPIO
    ├── bare-metal				Driver bare-metal per device myGPIO
    │   ├── HD44780				Driver bare-metal per display lcd Hitachi HD44780, usa il driver myGPIO
    │   │   ├── hd44780.c
    │   │   └── hd44780.h
    │   ├── myGPIO.c
    │   ├── myGPIO.h
    │   └── ZyboBSP				BSP per Digilent Zybo che usa driver myGPIO
    │       ├── ZyboButton.c
    │       ├── ZyboButton.h
    │       ├── Zybo.h
    │       ├── ZyboLed.c
    │       ├── ZyboLed.h
    │       ├── ZyboSwitch.c
    │       └── ZyboSwitch.h
    ├── linux-driver			Driver Linux (modulo kernel) per device myGPIO
    │   ├── configure.sh
    │   ├── Makefile
    │   ├── myGPIOK_list.c
    │   ├── myGPIOK_list.h
    │   ├── myGPIOK_main.c
    │   ├── myGPIOK_t.c
    │   └── myGPIOK_t.h
    └── VHDL					Implementazione VHDL del device myGPIO
        ├── GPIOarray.vhd
        ├── GPIOsingle.vhd
        ├── myGPIO_AXI.vhd
        └── myGPIO.vhd

Doc/
│	device.pdf					Documentazione pdf dell'implementazione VHDL del device myGPIO
│	driver.pdf					Documentazione pdf dei driver in C, sia sia bare-metal che Linux, con esempi
├── C 							Documentazione dei driver in C, sia bare-metal che Linux, con esempi, in html e latex
│   ├── html
│   │   └── index.html			Indice della documentazione (HTML)
│   └── latex			
├── schemes						Schemi a blocchi usati nella documentazione
└── vhdl						Documentazione interna dell'implementazione VHDL del device myGPIO
    ├── html
    │   └── index.html			Indice della documentazione (HTML)
    └── latex
