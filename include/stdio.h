//zcr copied from chapter9
/* EXTERN */
//#define	EXTERN	extern	/* EXTERN is defined as extern except in global.c */

/* string */
#define	STR_DEFAULT_LEN	1024

#define	O_CREAT		1
#define	O_RDWR		2

#define SEEK_SET	1
#define SEEK_CUR	2
#define SEEK_END	3

#define	MAX_PATH	128

int	open(const char *pathname, int flags);
int	close(int fd);
int	read(int fd, void *buf, int count);
int	write(int fd, const void *buf, int count);
//~zcr

