MYGPIO_SRC = ../../../
XIL_GPIO_SRC = ../../
CC = gcc
CC_OPZ = -Wall -Wextra -I$(MYGPIO_SRC) -I$(XIL_GPIO_SRC)
DEFINE = -D__DEBUG__ -D__XIL_GPIO__
COMPILE = $(CC) $(CC_OPZ) $(DEFINE) -c $< -o $@
LINK = $(CC) $(LINK_OPZ) $^ -o $@
BUILD_N := $(shell date +"%s")


.PHONI: clean

gpio : gpio.o myGPIO.o xil_gpio.o
	$(LINK)

gpio.o : gpio.c 
	@echo "BUILD " $(BUILD_N)
	$(COMPILE) -DBUILD=$(BUILD_N)

myGPIO.o : $(MYGPIO_SRC)myGPIO.c 
	$(COMPILE)
	
xil_gpio.o : $(XIL_GPIO_SRC)xil_gpio.c 
	$(COMPILE)

clean :
	rm -f *.o gpio