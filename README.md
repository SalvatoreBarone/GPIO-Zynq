# Zynq7000DriverPack
Vari Device Driver per la famiglia di MPSoC Xilinx Zynq-7000

L'implementazione si trova nella cartella Src. All'interno di essa e' presente:
- l'implementazione VHDL di una periferica GPIO, con supporto agli interrupt, nella directory Src/GPIO/VHDL;
- i driver in C della suddetta periferica, nella directory Src/GPIO;
- un board support package, comprensivo di driver per la gestione di button, switch e led, per la board Digilent Zynq nella directory Src/Zybo;
- un driver per display lcd alfanuerico Hitachi HD44780, che fa uso della periferica GPIO di cui sopra e dei relativi driver.

Il progetto e' in continua espansione ed e' aggiornato molto frequentemente.
