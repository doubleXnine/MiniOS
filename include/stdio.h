/********************************************
*用户库函数声明		add by visual 2016.5.16
*************************************************/


/*syscall.asm*/
int get_ticks();
int get_pid();					
void* kmalloc(int size);			
void* kmalloc_4k();			
void* malloc(int size);			
void* malloc_4k();				
int free(void *arg);				
int free_4k(void* AdddrLin);	
int fork();			
int pthread(void *arg);	
void udisp_int(int arg);
void udisp_str(char* arg);

//added by xw
/* file system */
#define	MAX_FILENAME_LEN	12
#define	MAX_PATH	128
#define	O_CREAT		1
#define	O_RDWR		2
#define SEEK_SET	1
#define SEEK_CUR	2
#define SEEK_END	3

int open(const char *pathname, int flags);		//added by xw, 18/6/19
int close(int fd);								//added by xw, 18/6/19
int read(int fd, void *buf, int count);			//added by xw, 18/6/19
int write(int fd, const void *buf, int count);	//added by xw, 18/6/19
int lseek(int fd, int offset, int whence);		//added by xw, 18/6/19
int unlink(const char *pathname);				//added by xw, 18/6/19
//~xw

/*string.asm*/
void* memcpy(void* p_dst, void*  p_src, int size);//void* memcpy(void* es:p_dst, void* ds:p_src, int size);
void memset(void* p_dst, char ch, int size);
char* strcpy(char* p_dst, char* p_src);
int strlen(char* p_str);	//added by xw, 18/6/19
