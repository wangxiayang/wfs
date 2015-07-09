#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/xattr.h>
#include <stdlib.h>

static char *rpath_prefix = "/home/xywang/fuse-store";

static unsigned int full_length(const char *path)
{
  	return strlen(rpath_prefix) + strlen(path);
}

#define REAL_PATH(path) \
	char real_path[full_length(path) + 1];\
	strcpy(real_path, rpath_prefix);\
	strcpy(real_path + strlen(rpath_prefix), path);

static int wfs_getattr(const char *path, struct stat *stbuf)
{
	printf("get_attr path=%s\n", path);
	
	memset(stbuf, 0, sizeof(struct stat));

	REAL_PATH(path)

	int res = lstat(real_path, stbuf);
	int errv = errno;
	if (res == 0) {
		return res;
	} else {
		printf("\33[0;31mFailed to get attr: %s\n\33[m", strerror(errv));
		return -errv;
	}
}

static int wfs_readlink(const char *path, char *buf, size_t size)
{
	printf("readlink path=%s\n", path);
	
	REAL_PATH(path)

	ssize_t res = readlink(real_path, buf, size);
	int errv = errno;
	if (res != -1) {
		return 0;
	} else {
		printf("\33[0;31m[Failed to read link: %s\n\33[m", strerror(errv));
		return -errv;
	}
}

static int wfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
	printf("readdir path=%s offset=%jd\n", path, (intmax_t)offset);

	char real_path[full_length(path) + 1];
	strcpy(real_path, rpath_prefix);
	strcpy(real_path + strlen(rpath_prefix), path);

	if (offset != 0) {
	  	fprintf(stderr, "\33[0;31m[WARNING]\33[moffset is not zero\n");
	}

	DIR *dir = (DIR *)fi->fh;
	if(!dir) {
		fprintf(stderr, "fi->fh is empty %s\n", real_path);
		return -EBADF;
	}

	struct dirent *entry;
	while((entry = readdir(dir)) != NULL) {
		filler(buf, entry->d_name, NULL, 0);
	}

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
	printf("read path=%s size=%zu offset=%jd\n", path, size, (intmax_t)offset);

	REAL_PATH(path)

  	int fd = fi->fh;
	if (!fd) {
	  	fprintf(stderr, "no file discriptor for path=%s\n", real_path);
	  	return 0;
	}

	//if (offset != 0) {
	//  	fprintf(stderr, "\33[0;31m[WARNING]\33[moffset is not 0 %jd\n", (intmax_t)offset);
	//}

	off_t seekres = lseek(fd, offset, SEEK_SET);
	int seekerrv = errno;
	if (seekres < 0) {
	  	fprintf(stderr, "failed in lseek\n");
	  	return -seekerrv;
	}

	ssize_t res = read(fd, buf, size);
	int errv = errno;
	if (res < 0) {
		return -errv;
	} else {
	  	return res;
	}
}

static int wfs_mknod(const char *path, mode_t mode, dev_t dev)
{
	printf("\33[0;31mmknod\33[m path=%s\n", path);
	return 0;
}

static int wfs_mkdir(const char *path, mode_t mode)
{
	printf("mkdir path=%s\n", path);

	REAL_PATH(path)

	int res = mkdir(real_path, mode);
	int errv = errno;

	if (res == 0) {
	  	return res;
	} else {
	  	return -errv;
	}
}

static int wfs_unlink(const char *path)
{
	//printf("\33[0;31munlink\33[m path=%s\n", path);
	printf("unlink path=%s\n", path);
	
	REAL_PATH(path)

	int res = unlink(real_path);
	int errv = errno;
	if (res == 0) {
		return res;
	} else {
		return -errv;
	}
}

static int wfs_rmdir(const char *path)
{
	printf("rmdir path=%s\n", path);

	REAL_PATH(path)

	int res = rmdir(real_path);
	int errv = errno;
	if (res == 0) {
	  	return res;
	} else {
	  	return -errv;
	}
}

/* oldpath is a plain string - no need to map to real_path */
static int wfs_symlink(const char *oldpath, const char *newpath)
{
	printf("\33[0;33msymlink\33[m str=%s new=%s\n", oldpath, newpath);

	REAL_PATH(newpath)

	int res = symlink(oldpath, real_path);
	int errv = errno;
	if (res == 0) {
	  	return res;
	} else {
		printf("\33[0;31mFailed to create symlink: %s\n\33[m", strerror(errv));
	  	return -errv;
	}
}

static int wfs_rename(const char *oldpath, const char *newpath)
{
	printf("rename old=%s new=%s\n", oldpath, newpath);

	char real_oldpath[full_length(oldpath) + 1];
	strcpy(real_oldpath, rpath_prefix);
	strcpy(real_oldpath + strlen(rpath_prefix), oldpath);

	char real_newpath[full_length(newpath) + 1];
	strcpy(real_newpath, rpath_prefix);
	strcpy(real_newpath + strlen(rpath_prefix), newpath);

	int res = rename(real_oldpath, real_newpath);
	int errv = errno;
	if (res == 0) {
	  	return res;
	} else {
	  	return -errv;
	}
}

static int wfs_link(const char *oldpath, const char *newpath)
{
	printf("link old=%s new=%s\n", oldpath, newpath);

	char real_oldpath[full_length(oldpath) + 1];
	strcpy(real_oldpath, rpath_prefix);
	strcpy(real_oldpath + strlen(rpath_prefix), oldpath);

	char real_newpath[full_length(newpath) + 1];
	strcpy(real_newpath, rpath_prefix);
	strcpy(real_newpath + strlen(rpath_prefix), newpath);

	int res = link(real_oldpath, real_newpath);
	int errv = errno;
	if (res == 0) {
	  	return res;
	} else {
	  	return -errv;
	}
}

static int wfs_chmod(const char *path, mode_t mode)
{
	//printf("\33[0;33mchmod\33[m path=%s\n", path);
	printf("chmod path=%s\n", path);
	
	REAL_PATH(path)

	int res = chmod(real_path, mode);
	int errv = errno;
	if (res == 0) {
		return res;
	} else {
		return -errv;
	}
}

static int wfs_chown(const char *path, uid_t uid, gid_t gid)
{
	//printf("\33[0;31mchown\33[m path=%s\n", path);
	printf("chown path=%s\n", path);
	
	REAL_PATH(path)

	int res = chown(real_path, uid, gid);
	int errv = errno;
	if (res == 0) {
	  	return res;
	} else {
	  	return -errv;
	}
}

static int wfs_truncate(const char *path, off_t offset)
{
	//printf("\33[0;31mtruncate\33[m path=%s\n", path);
	printf("truncate path=%s offset=%jd\n", path, (intmax_t)offset);

	REAL_PATH(path)

	int res = truncate(real_path, offset);
	int errv = errno;
	if (res == 0) {
		return res;
	} else {
		return -errv;
	}
}

static int wfs_write(const char *path, const char *buf, size_t size, off_t offset,
	struct fuse_file_info *fi)
{
	printf("write path=%s size=%zu offset=%jd\n", path, size, (intmax_t)offset);

	//if (offset != 0) {
	//	fprintf(stderr, "\33[0;31m[WARNING]\33[moffset is not zero %jd\n", (intmax_t)offset);
	//}

	int fd = (int)fi->fh;
	if (!fd) {
		fprintf(stderr, "fd is invalid\n");
		return -EBADF;
	}

	off_t seekres = lseek(fd, offset, SEEK_SET);
	int seekerrv = errno;
	if (seekres < 0) {
	  	fprintf(stderr, "lseek failed\n");
		return seekerrv;
	}

	ssize_t res = write(fd, buf, size);
	int errv = errno;
	if (res == -1) {
		return -errv;
	} else {
		return res;
	}
}

static int wfs_statfs(const char *path, struct statvfs *vfs)
{
	printf("statfs path=%s\n", path);

	REAL_PATH(path)

	int res = statvfs(real_path, vfs);
	int errv = errno;
	if (res == 0) {
	  	return res;
	} else {
	  	return -errv;
	}
}

static int wfs_flush(const char *path, struct fuse_file_info *fi)
{
	printf("\33[0;33mflush\33[m path=%s\n", path);
	
	return 0;
//	FILE *fd = (FILE *)fi->fh;
//	if (!fd) {
//		fprintf(stderr, "invalid fd\n");
//		return -EBADF;
//	}
//
//	int res = fflush(fd);
//	int errv = errno;
//	if (res == 0) {
//		return res;
//	} else {
//		return -errv;
//	}
}

static int wfs_release(const char *path, struct fuse_file_info *fi)
{
	printf("release path=%s\n", path);

	int fd = fi->fh;
	if (!fd) {
		fprintf(stderr, "invalid fd\n");
		return -EBADF;
	}

	int res = close(fd);
	int errv = errno;
	if (res == 0) {
		return res;
	} else {
		return -errv;
	}
}

static int wfs_fsync(const char *path, int flags, struct fuse_file_info *fi)
{
	//printf("\33[0;31mfsync\33[m path=%s\n", path);
	printf("fsync path=%s\n", path);

	int fd = fi->fh;
	if (!fd) {
		fprintf(stderr, "invalid fd\n");
		return -EBADF;
	}

	int res = fsync(fd);
	int errv = errno;
	if (res == 0) {
		return res;
	} else {
		return -errv;
	}
}

static int wfs_setxattr(const char *path, const char *name, const char *value, size_t size, int flags)
{
	//printf("\33[0;31msetxattr\33[m path=%s name=%s value=%s\n", path, name, value);
	printf("setxattr path=%s name=%s\n", path, name);

	REAL_PATH(path)

	int res = setxattr(real_path, name, value, size, flags);
	int errv = errno;
	if (res == 0) {
		return res;
	} else {
		return -errv;
	}
}

static int wfs_getxattr(const char *path, const char *name, char *value, size_t size)
{
	//printf("\33[0;33mgetxattr\33[m path=%s name=%s value=%s\n", path, name, value);
	printf("getxattr path=%s name=%s\n", path, name);

	REAL_PATH(path)

	ssize_t res = getxattr(real_path, name, value, size);
	int errv = errno;
	if (res == -1) {
		return -errv;
	} else {
		return res;
	}
}

static int wfs_listxattr(const char *path, char *buf, size_t size)
{
	printf("\33[0;31mlistxattr\33[m path=%s\n", path);
	return 0;
}

static int wfs_removexattr(const char *path, const char *name)
{
	//printf("\33[0;33mremovexattr\33[m path=%s name=%s\n", path, name);
	printf("removexattr path=%s name=%s\n", path, name);

	REAL_PATH(path)

	int res = removexattr(real_path, name);
	int errv = errno;
	if (res == 0) {
		return res;
	} else {
		return -errv;
	}
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
	printf("\33[0;31mfsyncdir\33[m path=%s flags=%d\n", path, flags);
	return 0;
}

static void *wfs_init(struct fuse_conn_info *conn)
{
  	printf("\33[0;31minit\33[m\n");

	return 0;
}

static void wfs_destroy(void *args)
{
  	printf("\33[0;31mdestroy\33[m\n");
}

static int wfs_access(const char *path, int mode)
{
  	printf("access path=%s\n", path);

	REAL_PATH(path)

	int res = access(real_path, mode);
	int errv = errno;
	if (res == 0) {
	  	return res;
	} else {
	  	return -errv;
	}
}

static int wfs_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
	//printf("\33[0;33mcreate\33[m path=%s\n", path);
	printf("create path=%s\n", path);

	REAL_PATH(path)

	int res = creat(real_path, mode);
	int errv = errno;
	if (res == -1) {
		return -errv;
	} else {
		fi->fh = res;
		return 0;
	}
}

static int wfs_ftruncate(const char *path, off_t offset, struct fuse_file_info *fi)
{
	//printf("\33[0;31mftruncate\33[m path=%s\n", path);
	printf("ftruncate path=%s offset=%jd\n", path, (intmax_t)offset);

	int fd = fi->fh;
	if (!fd) {
		fprintf(stderr, "invalid fd\n");
		return -EBADF;
	}

	int res = ftruncate(fd, offset);
	int errv = errno;
	if (res == 0) {
		return res;
	} else {
		return -errv;
	}
}

static int wfs_fgetattr(const char *path, struct stat *stat, struct fuse_file_info *fi)
{
	//printf("\33[0;33mfgetattr\33[m path=%s\n", path);
	printf("fgetattr path=%s\n", path);

	REAL_PATH(path)

	int fd = fi->fh;
	if (!fd) {
		fprintf(stderr, "fd is invalid\n");
		return -EBADF;
	}

	int res = fstat(fd, stat);
	int errv = errno;
	if (res == 0) {
		return res;
	}
	else {
		return -errv;
	}
}

static int wfs_lock(const char *path, struct fuse_file_info *fi, int cmd, struct flock *locks)
{
	printf("\33[0;33mlock\33[m path=%s\n", path);
	return 0;
}

static int wfs_utimens(const char *path, const struct timespec tv[2])
{
	printf("\33[0;31mutimes\33[m path=%s\n", path);
	return 0;
}

static int wfs_bmap(const char *path, size_t blocksize, uint64_t *idx)
{
  	printf("\33[0;31mbmap path=%s blocksize=%zu\33[m\n", path, blocksize);
	return 0;
}

static int wfs_ioctl(const char *path, int cmd, void *args,
	struct fuse_file_info *fi, unsigned int flags, void *data)
{
  	printf("\33[0;31mioctl path=%s\33[m\n", path);
	return 0;
}

static int wfs_poll(const char *path, struct fuse_file_info *fi,
	struct fuse_pollhandle *ph, unsigned *reventsp)
{
  	printf("\33[0;31mpoll path=%s\n", path);
	return 0;
}

//write_buf
//read_buf
//flock
//fallocate


static struct fuse_operations wfs_oper = {
  	.getattr = wfs_getattr,
	.readlink = wfs_readlink,
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

	.init = wfs_init,
	.destroy = wfs_destroy,
	.access = wfs_access,
	.ioctl = wfs_ioctl,
	.poll = wfs_poll,
	.bmap = wfs_bmap,
};

int main(int argc, char *argv[])
{
  	//setvbuf(stdout, NULL, _IONBF, 0);
	return fuse_main(argc, argv, &wfs_oper, NULL);
}
