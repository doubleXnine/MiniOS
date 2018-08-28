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

/* data of filesystem */
EXTERN	u8* fsbuf;
//EXTERN	MESSAGE	fs_msg;		//deleted by xw, 18/8/27
//EXTERN	PROCESS* pcaller;	//deleted by xw, 18/8/27
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
PUBLIC struct inode* get_inode_sched(int dev, int num);	//modified by xw, 18/8/28
PUBLIC struct super_block*	get_super_block(int dev);

#endif /* FS_MISC_H */
