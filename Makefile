default:
	gcc -Wall -c common.c
	gcc -Wall store.c `pkg-config fuse --cflags --libs` -o store

clean:
	-fusermount -u test
	rm -f store hello *.o

mount:
	./store test -f
