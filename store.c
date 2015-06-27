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

#define REAL_PATH(oripath) \
	char real_path[full_length(path) + 1];\
	strcpy(real_path, rpath_prefix);\
	strcpy(real_path + strlen(rpath_prefix), path);

static int wfs_getattr(const char *path, struct stat *stbuf)
{
	printf("get_attr path=%s\n", path);
	
	memset(stbuf, 0, sizeof(struct stat));

	char real_path[full_length(path) + 1];
	strcpy(real_path, rpath_prefix);
	strcpy(real_path + strlen(rpath_prefix), path);

	return stat(real_path, stbuf);
}

static int wfs_readlink(const char *path, char *buf, size_t size)
{
	printf("\33[readlink path=%s\n", path);
	return 0;
}

static int wfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
	printf("readdir path=%s offset=%u\n", path, offset);

	char real_path[full_length(path) + 1];
	strcpy(real_path, rpath_prefix);
	strcpy(real_path + strlen(rpath_prefix), path);

	printf("readdir realpath=%s\n", real_path);

	DIR *dir = (DIR *)fi->fh;
	if(!dir) {
		fprintf(stderr, "fi->fh is empty %s\n", real_path);
		return -EBADF;
	}

	struct dirent *entry;
	while((entry = readdir(dir)) != NULL) {
		filler(buf, entry->d_name, NULL, 0);
	}
	//closedir(dir);

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

static int wfs_getdir(const char *path, fuse_dirh_t fd, fuse_dirfil_t fdf)
{
	printf("getdir path=%s\n", path);
	return 0;
}

static int wfs_mknod(const char *path, mode_t mode, dev_t dev)
{
	printf("mknod path=%s\n", path);
	return 0;
}

static int wfs_mkdir(const char *path, mode_t mode)
{
	printf("mkdir path=%s\n", path);
	return 0;
}

static int wfs_unlink(const char *path)
{
	printf("unlink path=%s\n", path);
	return 0;
}

static int wfs_rmdir(const char *path)
{
	printf("rmdir path=%s\n", path);
	return 0;
}

static int wfs_symlink(const char *oldpath, const char *newpath)
{
	printf("symlink old=%s new=%s\n", oldpath, newpath);
	return 0;
}

static int wfs_rename(const char *oldpath, const char *newpath)
{
	printf("rename old=%s new=%s\n", oldpath, newpath);
	return 0;
}

static int wfs_link(const char *oldpath, const char *newpath)
{
	printf("link old=%s new=%s\n", oldpath, newpath);
	return 0;
}

static int wfs_chmod(const char *path, mode_t mode)
{
	printf("chmod path=%s\n", path);
	return 0;
}

static int wfs_chown(const char *path, uid_t uid, gid_t gid)
{
	printf("chown path=%s\n", path);
	return 0;
}

static int wfs_truncate(const char *path, off_t offset)
{
	printf("truncate path=%s\n", path);
	return 0;
}

static int wfs_write(const char *path, const char *buf, size_t size, off_t offset,
	struct fuse_file_info *fi)
{
	printf("write path=%s\n", path);
	return 0;
}

static int wfs_statfs(const char *path, struct statvfs *vfs)
{
	printf("statfs path=%s\n", path);
	return 0;
}

static int wfs_flush(const char *path, struct fuse_file_info *fi)
{
	printf("flush path=%s\n", path);
	return 0;
}

static int wfs_release(const char *path, struct fuse_file_info *fi)
{
	printf("release path=%s\n", path);

	FILE *fd = (FILE *)fi->fh;
	if (!fd) {
		fprintf(stderr, "invalid fd\n");
		return -EBADF;
	}

	close(fd);
	return 0;
}

static int wfs_fsync(const char *path, int flags, struct fuse_file_info *fi)
{
	printf("fsync path=%s\n", path);
	return 0;
}

static int wfs_setxattr(const char *path, const char *name, const char *value, size_t size, int flags)
{
	printf("setxattr path=%s name=%s value=%s\n", path, name, value);
	return 0;
}

static int wfs_getxattr(const char *path, const char *name, char *value, size_t size)
{
	printf("getxattr path=%s name=%s value=%s\n", path, name, value);
	return 0;
}

static int wfs_listxattr(const char *path, char *buf, size_t size)
{
	printf("listxattr path=%s\n", path);
	return 0;
}

static int wfs_removexattr(const char *path, const char *name)
{
	printf("removexattr path=%s name=%s\n", path, name);
	return 0;
}

static int wfs_opendir(const char *path, struct fuse_file_info *fi)
{
	printf("opendir path=%s\n", path);

	REAL_PATH(path)

	DIR *dir = opendir(real_path);
	if(!dir) {
		fprintf(stderr, "invalid dir path %s\n", real_path);
		return -EBADF;
	}

	fi->fh = (uint64_t)dir;
	return 0;
}

static int wfs_releasedir(const char *path, struct fuse_file_info *fi)
{
	printf("releasedir path=%s\n", path);

	//REAL_PATH(path)

	DIR *dir = (DIR *)fi->fh;
	if (!dir) {
		fprintf(stderr, "invalid fi->fh\n");
		return -EBADF;
	}

	closedir(dir);

	return 0;
}

static int wfs_fsyncdir(const char *path, int flags, struct fuse_file_info *fi)
{
	printf("fsyncdir path=%s flags=%d\n", path, flags);
	return 0;
}

//init
//destroy
//access

static int wfs_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
	printf("create path=%s\n", path);
	return 0;
}

static int wfs_ftruncate(const char *path, off_t offset, struct fuse_file_info *fi)
{
	printf("ftruncate path=%s\n", path);
	return 0;
}

static int wfs_fgetattr(const char *path, struct stat *stat, struct fuse_file_info *fi)
{
	printf("fgetattr path=%s\n", path);
	return 0;
}

static int wfs_lock(const char *path, struct fuse_file_info *fi, int cmd, struct flock *f)
{
	printf("lock path=%s\n", path);
	return 0;
}

static int wfs_utimens(const char *path, const struct timespec tv[2])
{
	printf("utimes path=%s\n", path);
	return 0;
}

//bmap
//ioctl
//poll
//write_buf
//read_buf
//flock
//fallocate


static struct fuse_operations wfs_oper = {
  	.getattr = wfs_getattr,
	.readlink = wfs_readlink,
	.getdir = wfs_getdir,
	.mknod = wfs_mknod,
	.mkdir = wfs_mkdir,
	.unlink = wfs_unlink,
	.rmdir = wfs_rmdir,
	.symlink = wfs_symlink,
	.rename = wfs_rename,
	.link = wfs_link,
	.chmod = wfs_chmod,
	.chown = wfs_chown,
	.truncate = wfs_truncate,
	.readdir = wfs_readdir,
	.open = wfs_open,
	.read = wfs_read,
	.write = wfs_write,
	.statfs = wfs_statfs,
	.flush = wfs_flush,
	.release = wfs_release,
	.fsync = wfs_fsync,
	.setxattr = wfs_setxattr,
	.getxattr = wfs_getxattr,
	.listxattr = wfs_listxattr,
	.removexattr = wfs_removexattr,
	.opendir = wfs_opendir,
	.releasedir = wfs_releasedir,
	.fsyncdir = wfs_fsyncdir,
	.create = wfs_create,
	.ftruncate = wfs_ftruncate,
	.fgetattr = wfs_fgetattr,
	.lock = wfs_lock,
	.utimens = wfs_utimens,
};

int main(int argc, char *argv[])
{
  	setvbuf(stdout, NULL, _IONBF, 0);
	return fuse_main(argc, argv, &wfs_oper, NULL);
}
