MYGPIO_SRC = ../myGPIO/bare-metal/
XIL_GPIO_SRC = ./
CC_OPZ = -Wall -Wextra -I$(MYGPIO_SRC) -I$(XIL_GPIO_SRC)
DEFINE = -D__DEBUG__
COMPILE = ${CC} $(CC_OPZ) $(DEFINE) -c $< -o $@
LINK_OPZ =
LINK = ${CC} $(LINK_OPZ) $^ -o $@
BUILD_N := $(shell date +"%s")


.PHONI: clean all

all: noDriver uio uio-int mygpiok
	
clean:
	rm -rf *.o noDriver uio uio-int mygpiok

noDriver : noDriver.o myGPIO.o
	$(LINK)

noDriver.o : noDriver.c
	@echo "BUILD " $(BUILD_N)
	$(COMPILE) -DBUILD=$(BUILD_N)

uio : uio.o myGPIO.o
	$(LINK)

uio.o : uio.c
	@echo "BUILD " $(BUILD_N)
	$(COMPILE) -DBUILD=$(BUILD_N)

uio-int : uio-int.o myGPIO.o xil_gpio.o
	$(LINK)

uio-int.o : uio-int.c
	@echo "BUILD " $(BUILD_N)
	$(COMPILE) -DBUILD=$(BUILD_N)

mygpiok : mygpiok.o myGPIO.o xil_gpio.o
	$(LINK)

mygpiok.o : mygpiok.c 
	@echo "BUILD " $(BUILD_N)
	$(COMPILE) -DBUILD=$(BUILD_N)

myGPIO.o : $(MYGPIO_SRC)myGPIO.c 
	$(COMPILE)

