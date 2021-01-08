CC=g++

CFLAGS=-Wall -Wextra -g3

.PHONY: clean distclean

all: circle semicircle ring square ellipse shapes

shapes: shapes.o myRecordDef.h
	$(CC) $(CFLAGS) -o shapes shapes.o

circle: circle.o myRecordDef.h
	$(CC) $(CFLAGS) -o circle circle.o

semicircle: semicircle.o myRecordDef.h
	$(CC) $(CFLAGS) -o semicircle semicircle.o

ring: ring.o myRecordDef.h
	$(CC) $(CFLAGS) -o ring ring.o

square: square.o myRecordDef.h
	$(CC) $(CFLAGS) -o square square.o

ellipse: ellipse.o myRecordDef.h
	$(CC) $(CFLAGS) -o ellipse ellipse.o

circle.o: circle.cpp
	$(CC) -c circle.cpp

semicircle.o: semicircle.cpp
	$(CC) -c semicircle.cpp

ring.o: ring.cpp
	$(CC) -c ring.cpp

square.o: square.cpp
	$(CC) -c square.cpp

ellipse.o: ellipse.cpp
	$(CC) -c ellipse.cpp

shapes.o: shapes.cpp
	$(CC) -c shapes.cpp

clean:
	rm -f ./*.o

distclean: clean
	rm -f circle ring square ellipse semicircle shapes
