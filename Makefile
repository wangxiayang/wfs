default: store
	gcc -Wall -c common.c

store: common.o
	gcc -Wall $^ store.c `pkg-config fuse --cflags --libs` -o $@

clean:
	-fusermount -u test
	rm -f store hello *.o

mount:
	./store test -f
