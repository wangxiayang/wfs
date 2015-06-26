#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>

static char *rpath_prefix = "/home/xywang/fuse-store";

static unsigned int full_length(const char *path)
{
  	return strlen(rpath_prefix) + strlen(path);
}

static int wfs_getattr(const char *path, struct stat *stbuf)
{
	printf("get_attr path=%s\n", path);
	
	memset(stbuf, 0, sizeof(struct stat));

	char real_path[full_length(path) + 1];
	strcpy(real_path, rpath_prefix);
	strcpy(real_path + strlen(rpath_prefix), path);

	return stat(real_path, stbuf);
}

static int wfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
	printf("readdir path=%s offset=%u\n", path, offset);

	char real_path[full_length(path) + 1];
	strcpy(real_path, rpath_prefix);
	strcpy(real_path + strlen(rpath_prefix), path);

	printf("readdir realpath=%s\n", real_path);

	DIR *dir;
	if(!(dir = opendir(real_path))) {
		fprintf(stderr, "invalid dir path %s\n", real_path);
		return -EBADF;
	}

	struct dirent *entry;
	while((entry = readdir(dir)) != NULL) {
		filler(buf, entry->d_name, NULL, 0);
	}
	closedir(dir);

	//filler(buf, ".", NULL, 0);
	//filler(buf, "..", NULL, 0);
	//filler(buf, "hello", NULL, 0);

	return 0;
}

static struct fuse_operations wfs_oper = {
  	.getattr = wfs_getattr,
	.readdir = wfs_readdir,
};

int main(int argc, char *argv[])
{
  	setvbuf(stdout, NULL, _IONBF, 0);
	return fuse_main(argc, argv, &wfs_oper, NULL);
}
