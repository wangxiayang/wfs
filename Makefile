default:
	gcc -Wall store.c `pkg-config fuse --cflags --libs` -o store

clean:
	fusermount -u test
	rm store hello

mount:
	./store test -f

hello:
	gcc -Wall hello.c `pkg-config fuse --cflags --libs` -o hello
