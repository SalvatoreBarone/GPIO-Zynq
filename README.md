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
│   ├── bsp_example.c			Esempio d'uso del driver bare-metal myGPIO, BSP e driver per display lcd Hitachi HD44780<br>
│   ├── interrupt_bare.c		Esempio d'uso del driver bare-metal myGPIO con interruzioni<br>
│   ├── Makefile<br>
│   ├── mygpiok.c				Programma userspace che fa uso del driver myGPIO in kernel-mode<br>
│   ├── noDriver.c				Programma userspace che interagisce con device myGPIO attraverso /dev/mem<br>
│   ├── readAll.c<br>
│   ├── uio.c					Programma userspace che interagisce con device myGPIO attraverso driver UIO<br>
│   ├── uio-int.c				Programma userspace che interagisce con device myGPIO attraverso driver UIO con interruzioni<br>
│   ├── xil_gpio.c<br>
│   └── xil_gpio.h<br>
└── myGPIO<br>
    ├── bare-metal				Driver bare-metal per device myGPIO<br>
    │   ├── HD44780				Driver bare-metal per display lcd Hitachi HD44780, usa il driver myGPIO<br>
    │   │   ├── hd44780.c<br>
    │   │   └── hd44780.h<br>
    │   ├── myGPIO.c<br>
    │   ├── myGPIO.h<br>
    │   └── ZyboBSP				BSP per Digilent Zybo che usa driver myGPIO<br>
    │       ├── ZyboButton.c<br>
    │       ├── ZyboButton.h<br>
    │       ├── Zybo.h<br>
    │       ├── ZyboLed.c<br>
    │       ├── ZyboLed.h<br>
    │       ├── ZyboSwitch.c<br>
    │       └── ZyboSwitch.h<br>
    ├── linux-driver			Driver Linux (modulo kernel) per device myGPIO<br>
    │   ├── configure.sh<br>
    │   ├── Makefile<br>
    │   ├── myGPIOK_list.c<br>
    │   ├── myGPIOK_list.h<br>
    │   ├── myGPIOK_main.c<br>
    │   ├── myGPIOK_t.c<br>
    │   └── myGPIOK_t.h<br>
    └── VHDL					Implementazione VHDL del device myGPIO<br>
        ├── GPIOarray.vhd<br>
        ├── GPIOsingle.vhd<br>
        ├── myGPIO_AXI.vhd<br>
        └── myGPIO.vhd<br>
<br>
Doc/<br>
│	device.pdf					Documentazione pdf dell'implementazione VHDL del device myGPIO<br>
│	driver.pdf					Documentazione pdf dei driver in C, sia sia bare-metal che Linux, con esempi<br>
├── C 							Documentazione dei driver in C, sia bare-metal che Linux, con esempi, in html e latex<br>
│   ├── html<br>
│   │   └── index.html			Indice della documentazione (HTML)<br>
│   └── latex<br>			
├── schemes						Schemi a blocchi usati nella documentazione<br>
└── vhdl						Documentazione interna dell'implementazione VHDL del device myGPIO<br>
    ├── html<br>
    │   └── index.html			Indice della documentazione (HTML)<br>
    └── latex<br>
