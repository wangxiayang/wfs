CC=gcc
SOURCES=store.c common.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLES=store

all: store

%.o: %.c
	${CC} -Wall -c $^ -o $@ `pkg-config fuse --cflags`

$(EXECUTABLES): common.o store.o
	gcc -Wall -o $@ $^ `pkg-config fuse --libs` -lconfig

clean:
	-fusermount -u test
	rm -f ${EXECUTABLES} hello ${OBJECTS}

mount:
	./store test -f
