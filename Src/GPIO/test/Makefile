CC = gcc
CC_OPZ = -Wall -Wextra
COMPILE = $(CC) $(CC_OPZ) $(DEBUG_OPZ) -c $< -o $@
LINK = $(CC) $(LINK_OPZ) $^ -o $@
SRC=../

test : test.o gpio.o
	$(LINK)

test.o : test.c gpio.o
	$(COMPILE) -I..

gpio.o : $(SRC)gpio.c $(SRC)gpio.h
	$(COMPILE)

clean :
	rm -rf *.o test
