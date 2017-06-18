# Zynq7000DriverPack
Vari Device Driver per la famiglia di MPSoC Xilinx Zynq-7000

<b>Il progetto e' in continua espansione ed e' aggiornato molto frequentemente.</b>

L'implementazione si trova nella cartella <b>Src</b>. All'interno di essa e' presente:
- l'implementazione VHDL di una periferica GPIO, con supporto agli interrupt, nella directory Src/myGPIO/VHDL;
- i driver in C della suddetta periferica, nella directory Src/myGPIO;
- un board support package, comprensivo di driver per la gestione di button, switch e led, per la board Digilent Zynq nella
  directory Src/Zybo;
- un driver per display lcd alfanuerico Hitachi HD44780, che fa uso della periferica GPIO di cui sopra e dei relativi driver.
- diversi device-driver per Kernel Linux, nella cartella Src/myGPIO/linuxdriver

La documentazione si trova nella cartella <b>Doc</b>. All'interno di essa sono presenti:
- la cartella vhdl, contenente la documentazione dell'implementazione VHDL della periferica myGPIO; al suo interno e'
  disponibile la documentazione in html o pdf;
- la cartella driver, contenente la documentazione dell'implementazione C del driver per la periferica myGPIO, del BSP per
  la board Digilent Zynq, e, soprattutto, dei driver per Kernel Linux, scritti minuziosamente ed in modo didattizo. La
  documentazione e' disponibile in html o pdf; la documentazione e' comprensiva di esempi.


TODO
In Src/myGPIO/linux-driver/kernel_module/kernel_module/myGPIOK.c:
aggiungere il supporto per scheduling/queueing in lettura

implementare e documentare
 - static irqreturn_t myGPIOK_irq_handler(int irq, struct pt_regs * regs);
 - static unsigned int myGPIOK_poll (struct file *file, struct poll_table_struct *wait);
 - static ssize_t myGPIOK_read (struct file *file, char *buf, size_t count, loff_t *ppos);

