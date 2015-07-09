#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

int main() {
	int res = symlink("todo.txt", "/home/xywang/fuse/s");
	int errv = errno;
	if (res == 0) {
		return 0;
	} else {
		printf("Error: %s\n", strerror(errv));
		return 1;
	}
}
