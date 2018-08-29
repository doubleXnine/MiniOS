/**
 * fs.h
 * This file contains APIs of filesystem, it's used inside the kernel.
 * There is a seperate header file for user program's use. 
 * This file is added by xw. 18/6/17
 */

#ifndef	FS_H
#define	FS_H

/* APIs of file operation */
#define	O_CREAT		1
#define	O_RDWR		2
#define SEEK_SET	1
#define SEEK_CUR	2
#define SEEK_END	3

#define	MAX_PATH	128
#define	MAX_FILENAME_LEN	12

PUBLIC int open(const char *pathname, int flags);
PUBLIC int close(int fd);
PUBLIC int read(int fd, void *buf, int count);
PUBLIC int write(int fd, const void *buf, int count);
PUBLIC int lseek(int fd, int offset, int whence);
PUBLIC int unlink(const char *pathname);

//added by xw, 18/6/18
PUBLIC int sys_open(void *uesp);
PUBLIC int sys_close(void *uesp);
PUBLIC int sys_read(void *uesp);
PUBLIC int sys_write(void *uesp);
PUBLIC int sys_lseek(void *uesp);	//~xw
PUBLIC int sys_unlink(void *uesp);	//added by xw, 18/6/19

PUBLIC void init_fs();

#endif /* FS_H */
