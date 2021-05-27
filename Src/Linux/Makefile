.PHONI: clean all dirs 

all: sbagliato noDriver uio uio-int mygpiok
	rm *.o

clean:
	rm -rf *.o sbagliato noDriver uio uio-int mygpiok

noDriver: noDriver.o myGPIO.o
sbagliato: sbagliato.o myGPIO.o
uio : uio.o myGPIO.o
uio-int : uio-int.o myGPIO.o 
mygpiok : mygpiok.o myGPIO.o
sbagliato.o : sbagliato.c
noDriver.o : noDriver.c
uio.o : uio.c
uio-int.o : uio-int.c
mygpiok.o : mygpiok.c 
myGPIO.o : ../myGPIO.c 


