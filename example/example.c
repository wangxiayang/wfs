#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "common.h"

int test1() {
	int res = symlink("todo.txt", "/home/xywang/fuse/s");
	int errv = errno;
	if (res == 0) {
		return 0;
	} else {
		printf("Error: %s\n", strerror(errv));
		return 1;
	}
}

int test2() {
	printf("Before\n");
	red_printf("Hello %s %s!\n", "world", "young man");
	printf("Hehe\n");
	return 0;
}


int main() {
	test2();
	return 0;
}
