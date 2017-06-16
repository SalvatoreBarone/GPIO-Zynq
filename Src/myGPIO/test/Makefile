CC = gcc
CC_OPZ = -Wall -Wextra
COMPILE = $(CC) $(CC_OPZ) $(DEBUG_OPZ) -c $< -o $@
LINK = $(CC) $(LINK_OPZ) $^ -o $@
SRC=../

test : test.o myGPIO.o
	$(LINK)

test.o : test.c myGPIO.o
	$(COMPILE) -I..

myGPIO.o : $(SRC)myGPIO.c $(SRC)myGPIO.h
	$(COMPILE)

clean :
	rm -rf *.o test
