CC=g++

CFLAGS=-Wall -Wextra -g3 -pthread

.PHONY: clean distclean

all: restaurant doorman customer

restaurant: restaurant.o tables.h bar.h
	$(CC) $(CFLAGS) -o restaurant restaurant.o tables.o bar.o

doorman: doorman.o tables.h bar.h
	$(CC) $(CFLAGS) -o doorman doorman.o tables.o bar.o

customer: customer.o tables.h bar.h
	$(CC) $(CFLAGS) -o customer customer.o tables.o bar.o

restaurant.o: restaurant.cpp tables.cpp bar.cpp
	$(CC) -c restaurant.cpp tables.cpp bar.cpp

doorman.o: doorman.cpp tables.cpp bar.cpp
	$(CC) -c doorman.cpp tables.cpp bar.cpp

customer.o: customer.cpp tables.cpp bar.cpp
	$(CC) -c customer.cpp tables.cpp bar.cpp
clean:
	rm -f ./*.o

distclean: clean
	rm -f customer doorman restaurant
	./script.sh
