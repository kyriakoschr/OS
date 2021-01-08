OBJS = main.o accounts.o hasht.o list.o
SOURCE = main.cpp accounts.cpp hasht.cpp list.cpp
HEADER = accounts.h hasht.h list.h
OUT = stucs
CC = g++
FLAGS = -g3 -c
$(OUT): $(OBJS)
	$(CC) -g $(OBJS) -o $@

main.o: main.cpp hasht.h
	$(CC) $(FLAGS) main.cpp
accounts.o: accounts.cpp accounts.h list.h
	$(CC) $(FLAGS) accounts.cpp
hasht.o: hasht.cpp hasht.h accounts.h
	$(CC) $(FLAGS) hasht.cpp
list.o: list.cpp list.h accounts.h
	$(CC) $(FLAGS) list.cpp

clean :
	rm -f $(OBJS) $(OUT)

count :
	wc $(SOURCE) $(HEADER)
