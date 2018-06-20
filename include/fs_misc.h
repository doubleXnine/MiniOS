/**
 * fs_misc.h
 * This file contains miscellaneous defines and declarations associated with filesystem.
 * The code is added by zcr, and the file is added by xw. 18/6/17
 */

#ifndef	FS_MISC_H
#define	FS_MISC_H

/* APIs of file operation */
#define	MAX_PATH	128
#define	O_CREAT		1
#define	O_RDWR		2
#define SEEK_SET	1
#define SEEK_CUR	2
#define SEEK_END	3

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
PUBLIC int sys_lseek(void *uesp);
//~xw
PUBLIC int sys_unlink(void *uesp);	//added by xw, 18/6/19

/**
 * MESSAGE mechanism is borrowed from MINIX
 */
struct mess1 {
	int m1i1;
	int m1i2;
	int m1i3;
	int m1i4;
};
struct mess2 {
	void* m2p1;
	void* m2p2;
	void* m2p3;
	void* m2p4;
};
struct mess3 {
	int	m3i1;
	int	m3i2;
	int	m3i3;
	int	m3i4;
	u64	m3l1;
	u64	m3l2;
	void*	m3p1;
	void*	m3p2;
};
typedef struct {
	int source;
	int type;
	union {
		struct mess1 m1;
		struct mess2 m2;
		struct mess3 m3;
	} u;
} MESSAGE;

/**
 * @enum msgtype
 * @brief MESSAGE types
 */
enum msgtype {
	/* 
	 * when hard interrupt occurs, a msg (with type==HARD_INT) will
	 * be sent to some tasks
	 */
	HARD_INT = 1,

	/* SYS task */
	GET_TICKS,

	/// zcr added from ch9/e/include/const.h
	/* FS */
	OPEN, CLOSE, READ, WRITE, LSEEK, STAT, UNLINK,
	
	/* message type for drivers */
	DEV_OPEN = 1001,
	DEV_CLOSE,
	DEV_READ,
	DEV_WRITE,
	DEV_IOCTL
};

/* data of filesystem */
EXTERN	u8* fsbuf;
EXTERN	MESSAGE	fs_msg;
EXTERN	PROCESS* pcaller;
EXTERN	struct inode* root_inode;
EXTERN	struct file_desc f_desc_table[NR_FILE_DESC];
EXTERN	struct inode inode_table[NR_INODE];
EXTERN	struct super_block super_block[NR_SUPER_BLOCK];

/* declaration of functions in fs.c */
PUBLIC int do_open();
PUBLIC int do_close(int fd);
PUBLIC int do_stat();
PUBLIC int search_file(char * path);

PUBLIC void put_inode(struct inode * pinode);
PUBLIC void sync_inode(struct inode * p);
PUBLIC struct inode* get_inode(int dev, int num);
PUBLIC struct super_block*	get_super_block(int dev);

#endif /* FS_MISC_H */
