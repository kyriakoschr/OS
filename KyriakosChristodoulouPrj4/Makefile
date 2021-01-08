CC=g++

CFLAGS= -g3

.PHONY: clean distclean swpclean

mydiz: main.o block.h BlockHandler.h BlockHandler.o
	$(CC) $(CFLAGS) -o mydiz main.o BlockHandler.o

main.o: main.cpp
	$(CC) -c -g3 main.cpp

BlockHandler.o: BlockHandler.cpp
	$(CC) -c -g3 BlockHandler.cpp

clean:
	rm -f ./*.o

swpclean:
	rm -f ./*.swp

distclean: clean swpclean
	rm -f ./mydiz
