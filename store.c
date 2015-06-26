#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>

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

static int wfs_open(const char *path, struct fuse_file_info *fi)
{
  	printf("open path=%s flag=%d\n", path, fi->flags);

	char real_path[full_length(path) + 1];
	strcpy(real_path, rpath_prefix);
	strcpy(real_path + strlen(rpath_prefix), path);

  	int fd = open(real_path, fi->flags);
	if(fd < 0)
	  	return -ENOENT;

	fi->fh = fd;
	return 0;
}

static int wfs_read(const char *path,
	char *buf,
	size_t size,
	off_t offset,
	struct fuse_file_info *fi)
{
	printf("read path=%s\n", path);

	char real_path[full_length(path) + 1];
	strcpy(real_path, rpath_prefix);
	strcpy(real_path + strlen(rpath_prefix), path);

  	uint64_t fd = fi->fh;
	if (!fd) {
	  	fprintf(stderr, "no file discriptor for path=%s\n", path);
	  	return 0;
	}

	return read(fd, buf, size);
}

static struct fuse_operations wfs_oper = {
  	.getattr = wfs_getattr,
	.readdir = wfs_readdir,
	.open = wfs_open,
	.read = wfs_read,
};

int main(int argc, char *argv[])
{
  	setvbuf(stdout, NULL, _IONBF, 0);
	return fuse_main(argc, argv, &wfs_oper, NULL);
}
