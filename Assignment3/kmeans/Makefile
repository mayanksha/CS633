CC=mpicc
CFLAGS=
OUT=src.x

all: build

build: src.o vector.o
	$(CC) $(CFLAGS) -o $(OUT) src.c vector.c

debug: CFLAGS+=-DDEBUG CFLAGS+=-DPRINT_POINTS
debug: build

src.o: src.c vector.h
	$(CC) $(CFLAGS) -c src.c

vector.o: vector.c vector.h
	$(CC) $(CFLAGS) -c vector.c

clean:
	rm -f *.o *.x

