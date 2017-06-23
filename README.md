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

# To-Do
1. Aggiunta delle funzionalita' che permettano di usare la stessa istanza del driver kernel-mode con istanze myGPIO diverse
 
